#include "soso.h"

#include <sht/murmur3.h>

#include <assert.h>
#include <string.h>

#define SOSO_NUM_SHUFFLES 12
#define SOSO_HASH_SEED 0xdeadc0de

static uint64_t random_get_in(uint64_t min, uint64_t max) {
	return soso_random_get() % (max + 1 - min) + min;
}

void soso_shuffle(soso_deck_t *deck, uint64_t seed) {
	soso_random_seed(seed);
	for (soso_int_t i = 0; i < 52; ++i) {
		deck->cards[i] = i;
	}

	for (int s = 0; s < SOSO_NUM_SHUFFLES; ++s)
		for (int i = 0; i < 51; ++i) {
			int j = random_get_in(i, 51);
			if (i == j) continue;
			soso_int_t t = deck->cards[j];
			deck->cards[j] = deck->cards[i];
			deck->cards[i] = t;
		}
}

void soso_deal(soso_game_t *game, soso_deck_t *deck) {
	memset(game, SOSO_EMPTY_CARD, sizeof(soso_game_t));
	game->stock_cur = 24;
	game->stock_count = 24;
	for (int i = 0; i < 7; ++i) {
		game->tableau_top[i] = i + 1;
		game->tableau_up[i] = i;
		if (i < 4) game->foundation_top[i] = 0;
	}

	int next = 51;
	for (int i = 0; i < 7; ++i)
		for (int j = i; j < 7; ++j) game->tableau[j][i] = deck->cards[next--];
	int stock_id = 23;
	while (next > -1) game->stock[stock_id--] = deck->cards[next--];
}

void soso_ctx_init(soso_ctx_t *ctx, int draw_count, int max_visited, void *(*custom_alloc)(size_t),
                   void *(*custom_realloc)(void *, size_t), void (*custom_free)(void *)) {
	ctx->draw_count = draw_count;
	ctx->max_visited = max_visited;
	ctx->visited = NULL;
	ctx->moves = NULL;
	ctx->moves_cap = 0;
	ctx->moves_top = 0;
	ctx->automoves_count = 0;
	ctx->moves_total = 0;
	if (custom_alloc != NULL) {
		ctx->alloc = custom_alloc;
		ctx->realloc = custom_realloc;
		ctx->free = custom_free;
	}
	else {
		ctx->alloc = malloc;
		ctx->realloc = realloc;
		ctx->free = free;
	}
}

void soso_ctx_destroy(soso_ctx_t *ctx) {
	if (ctx->moves != NULL) ctx->free(ctx->moves);
	if (ctx->visited != NULL) sht_destroy(ctx->visited);
}

static void grow_moves(soso_ctx_t *ctx) {
	if (ctx->moves_top >= ctx->moves_cap) {
		if (ctx->moves_cap == 0) {
			ctx->moves = (soso_move_t *)ctx->alloc(sizeof(soso_move_t) * 2);
			ctx->moves_cap = 2;
		}
		else {
			ctx->moves =
			    (soso_move_t *)ctx->realloc(ctx->moves, sizeof(soso_move_t) * ctx->moves_cap * 2);
			ctx->moves_cap *= 2;
		}
		assert(ctx->moves != NULL);
	}
}

void soso_internal_add_move(soso_ctx_t *ctx, const soso_move_t m, bool is_auto) {
	grow_moves(ctx);
	ctx->moves[ctx->moves_top++] = m;
	++ctx->moves_total;
	if (is_auto) ++ctx->automoves_count;
}

void soso_internal_update_waste_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	soso_int_t card_from = soso_internal_get_waste_card(game);
	if (card_from == -1) return;

	// Waste->Foundation
	if (soso_internal_foundation_valid(card_from, game->foundation_top)) {
		ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
		ctx->moves_available[ctx->moves_available_top].to =
		    SOSO_FOUNDATION1C + soso_internal_csuit(card_from);
		ctx->moves_available[ctx->moves_available_top].count = 1;
		ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
	}
	// Waste->Tableau
	bool king_placed = false;
	for (int i = 0; i < 7; ++i) {
		if (!king_placed && game->tableau_top[i] == 0 &&
		    soso_internal_cvalue(card_from) == SOSO_KING) {
			king_placed = true;
			ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + i;
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			continue;
		}
		if (game->tableau_top[i] == 0) continue;
		soso_int_t card_to = game->tableau[i][game->tableau_top[i] - 1];
		if (soso_internal_tableau_valid(card_from, card_to)) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + i;
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
		}
	}
}

void soso_internal_update_tableau_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	soso_int_t card_from, card_to;
	for (int i = 0; i < 7; ++i) {
		card_from = game->tableau[i][game->tableau_top[i] - 1];
		// Tableau->Foundation
		if (game->tableau_top[i] > 0 &&
		    soso_internal_foundation_valid(card_from, game->foundation_top)) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
			ctx->moves_available[ctx->moves_available_top].to =
			    SOSO_FOUNDATION1C + soso_internal_csuit(card_from);
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
		}

		// Tableau->Tableau top card
		if (game->tableau_top[i] - game->tableau_up[i] != 1) continue;
		bool king_placed = false;
		for (int j = 0; j < 7; ++j) {
			if (i == j) continue;
			if (!king_placed && game->tableau_top[j] == 0 &&
			    soso_internal_cvalue(card_from) == SOSO_KING) {
				king_placed = true;
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
				continue;
			}

			card_to = game->tableau[j][game->tableau_top[j] - 1];
			if (soso_internal_tableau_valid(card_from, card_to)) {
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			}
		}
	}

	// Tableau talon swap
	for (int i = 0; i < 7; ++i) {
		soso_int_t talonsize = game->tableau_top[i] - game->tableau_up[i];
		if (game->tableau_top[i] == 0 || talonsize == 1) continue;

		bool cards_closed = game->tableau_up[i] > 0;
		bool king_first = soso_internal_cvalue(game->tableau[i][game->tableau_up[i]]) == SOSO_KING;
		int start = (!cards_closed && king_first) ? 1 : 0;

		bool king_placed = false;
		for (int j = 0; j < 7; ++j) {
			if (i == j) continue;

			// King talon
			if (!king_placed && game->tableau_top[j] == 0 && king_first) {
				king_placed = true;
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = talonsize;
				ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			}

			if (game->tableau_top[j] == 0) continue;

			for (int offset = start; offset < talonsize; ++offset) {
				card_from = game->tableau[i][game->tableau_up[i] + offset];
				card_to = game->tableau[j][game->tableau_top[j] - 1];
				if (soso_internal_tableau_valid(card_from, card_to)) {
					ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
					ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
					ctx->moves_available[ctx->moves_available_top].count = talonsize - offset;
					ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
				}
			}
		}
	}
}

void soso_internal_update_stock_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	if (game->stock_count == 0) return;

	soso_int_t current_cursor = game->stock_cur;
	soso_ctx_t pseudo_ctx = (soso_ctx_t){.moves_available_top = 0, .draw_count = ctx->draw_count};
	soso_game_t game_copy;
	memcpy(&game_copy, game, sizeof(soso_game_t));
	int numdraw = 0;
	int maxdraw = soso_internal_max_draws(ctx, game);
	for (int i = 0; i < maxdraw; ++i) {
		if (!soso_internal_draw(&game_copy, ctx->draw_count)) break;
		++numdraw;
		if (soso_internal_update_available_moves(&pseudo_ctx, &game_copy, true)) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].count = numdraw;
			ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
			break;
		}
	}
}

void soso_internal_update_foundation_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	for (int i = 0; i < 4; ++i) {
		if (game->foundation_top[i] > 0)
			for (int j = 0; j < 7; ++j)
				if (soso_internal_tableau_valid(i * 13 + (game->foundation_top[i] - 1),
				                                game->tableau[j][game->tableau_top[j] - 1])) {
					ctx->moves_available[ctx->moves_available_top].from = SOSO_FOUNDATION1C + i;
					ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
					ctx->moves_available[ctx->moves_available_top].count = 1;
					ctx->moves_available[ctx->moves_available_top++].extra = SOSO_NOEXTRA;
				}
	}
}

bool soso_internal_update_available_moves(soso_ctx_t *ctx, const soso_game_t *game, bool no_stock) {
	ctx->moves_available_top = 0;
	soso_internal_update_waste_moves(ctx, game);
	soso_internal_update_tableau_moves(ctx, game);
	soso_internal_update_foundation_moves(ctx, game);
	if (!no_stock) soso_internal_update_stock_moves(ctx, game);
	return ctx->moves_available_top > 0;
}

bool soso_internal_draw(soso_game_t *game, int draw_count) {
	if (game->stock_count == 0) return false;
	if (game->stock_cur == 0) {
		game->stock_cur = game->stock_count;
		for (int i = game->stock_count; i < 24; ++i) game->stock[i] = SOSO_EMPTY_CARD;
	}
	else
		game->stock_cur = (game->stock_cur - draw_count > 0) ? game->stock_cur - draw_count : 0;
	return true;
}

static void make_auto_moves(soso_ctx_t *ctx, soso_game_t *game) {
	// Turn flipable cards
	for (int i = 0; i < 7; ++i) {
		if (game->tableau_top > 0 && game->tableau_top[i] == game->tableau_up[i]) {
			--game->tableau_up[i];
			soso_internal_add_move(ctx,
			                       (soso_move_t){.from = i + SOSO_TABLEAU1,
			                                     .to = i + SOSO_TABLEAU1,
			                                     .count = 1,
			                                     .extra = SOSO_FLIP | SOSO_AUTO_MOVE},
			                       true);
		}
	}

	// Optionally draw until a move becomes available
	int numdraw = 0;
	int maxdraw = soso_internal_max_draws(ctx, game);
	for (int i = 0; i < maxdraw; ++i) {
		if (soso_internal_update_available_moves(ctx, game, true)) break;
		if (!soso_internal_draw(game, ctx->draw_count)) break;
		++numdraw;
	}
	if (numdraw > 0 && numdraw < maxdraw)
		soso_internal_add_move(ctx,
		                       (soso_move_t){.count = numdraw,
		                                     .from = SOSO_STOCK_WASTE,
		                                     .to = SOSO_STOCK_WASTE,
		                                     .extra = SOSO_AUTO_MOVE},
		                       true);
}

static void clean_game(soso_game_t *game) {
	for (int i = game->stock_count; i < 24; ++i) game->stock[i] = SOSO_EMPTY_CARD;
	for (int i = 0; i < 7; ++i)
		for (int j = game->tableau_top[i]; j < 13; ++j) game->tableau[i][j] = SOSO_EMPTY_CARD;
}

static inline uint32_t state_hash(const soso_game_t *game, const soso_move_t *move) {
	uint32_t game_hash;
	MurmurHash3_x86_32(game, sizeof(soso_game_t), 42, &game_hash);
	return game_hash ^ *(const uint32_t *)move;
}

static void undo_move(soso_ctx_t *ctx, soso_game_t *game) {
	assert(ctx->moves_top > 0);
	soso_move_t m = ctx->moves[ctx->moves_top - 1];
	if ((m.extra & SOSO_AUTO_MOVE) > 0) --ctx->automoves_count;
	if (m.from == SOSO_STOCK_WASTE) {
		if (m.to == SOSO_STOCK_WASTE) {
			// Undo Draw
		}
		else if (m.to >= SOSO_TABLEAU1 && m.to <= SOSO_TABLEAU7) {
			// Undo Waste -> Tableau
		}
		else if (m.to >= SOSO_FOUNDATION1C && m.to <= SOSO_FOUNDATION4H) {
			// Undo Waste -> Foundation
		}
	}
	else if (m.from >= SOSO_TABLEAU1 && m.from <= SOSO_TABLEAU7) {
		// Undo Tableau -> ?
	}
	else if (m.from >= SOSO_FOUNDATION1C && m.from <= SOSO_FOUNDATION4H) {
		// Undo Foundation -> ?
	}
	else {
		assert(0);
	}
}

static void revert_to_last_move(soso_ctx_t *ctx, soso_game_t *game) {
	for (;;) {
		if (ctx->moves_top == 0) break;
		undo_move(ctx, game);
		if ((ctx->moves[ctx->moves_top].extra & SOSO_AUTO_MOVE) == 0) break;
	}
}

static void make_move(soso_ctx_t *ctx, soso_game_t *game, soso_move_t m) {}

static bool game_solved(const soso_game_t *game) {
	soso_int_t total = 0;
	for (int i = 0; i < 4; ++i) total += game->foundation_top[i];
	return total == 52;
}

bool soso_solve(soso_ctx_t *ctx, soso_game_t *game) {
	ctx->visited =
	    sht_init_alloc(sizeof(uint32_t), ctx->max_visited, SOSO_HASH_SEED, ctx->alloc, ctx->free);
	int states_visited = 0;
	while (states_visited < ctx->max_visited) {
		make_auto_moves(ctx, game);
		soso_internal_update_available_moves(ctx, game, false);
		if (ctx->moves_available_top == 0 && ctx->moves_top - ctx->automoves_count == 0) break;
		bool move_made = false;
		clean_game(game);  // to get consistent state hash
		for (int i = 0; i < ctx->moves_available_top; ++i) {
			uint32_t h = state_hash(game, &ctx->moves_available[i]);
			if (sht_get(ctx->visited, &h, sizeof(uint32_t)) != NULL) continue;
			move_made = true;
			soso_internal_add_move(ctx, ctx->moves_available[i], false);
			make_move(ctx, game, ctx->moves_available[i]);
			break;
		}
		if (!move_made) {
			revert_to_last_move(ctx, game);
			uint32_t h = state_hash(game, &ctx->moves[ctx->moves_top]);
			sht_set(ctx->visited, &h, sizeof(uint32_t), &h);
			continue;
		}
		if (game_solved(game)) return true;
	}
	return false;
}
