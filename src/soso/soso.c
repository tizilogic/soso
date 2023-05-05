#include "soso.h"
#include <assert.h>
#include <stdbool.h>
#include <string.h>

#define SOSO_NUM_SHUFFLES 12

static uint64_t random_get_in(uint64_t min, uint64_t max) {
	return soso_random_get() % (max + 1 - min) + min;
}

void soso_shuffle(soso_deck_t *deck, uint64_t seed) {
	soso_random_seed(seed);
	for (int8_t i = 0; i < 52; ++i) {
		deck->cards[i] = i;
	}

	for (int s = 0; s < SOSO_NUM_SHUFFLES; ++s)
		for (int i = 0; i < 51; ++i) {
			int j = random_get_in(i, 51);
			if (i == j) continue;
			int8_t t = deck->cards[j];
			deck->cards[j] = deck->cards[i];
			deck->cards[i] = t;
		}
}

void soso_deal(soso_game_t *game, soso_deck_t *deck) {
	memset(game, SOSO_EMPTY_CARD, sizeof(soso_game_t));
	game->stock_cursor = 23;
	game->stock_count = 24;
	for (int i = 0; i < 7; ++i) {
		game->tableau_top[i] = i + 1;
		game->tableau_up[i] = i;
		if (i < 4) game->foundation_top[i] = 0;
	}

	int next = 0;
	for (int i = 0; i < 7; ++i)
		for (int j = i; j < 7; ++j) game->tableau[j][i] = deck->cards[next++];
	int stock_id = 23;
	while (next < 52) game->stock[stock_id--] = deck->cards[next++];
}

void soso_ctx_init(soso_ctx_t *ctx, int draw_count, void *(*custom_alloc)(size_t),
                   void *(*custom_realloc)(void *, size_t), void (*custom_free)(void *)) {
	ctx->draw_count = draw_count;
	ctx->visited = NULL;
	ctx->moves = NULL;
	ctx->moves_cap = 0;
	ctx->moves_top = 0;
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

static void add_move(soso_ctx_t *ctx, const soso_move_t *m) {
	grow_moves(ctx);
	memcpy(&ctx->moves[ctx->moves_top++], m, sizeof(soso_move_t));
}

static inline int8_t get_waste_card(const soso_game_t *game) {
	if (game->stock_count > 0 && game->stock_count - 1 > game->stock_cursor)
		return game->stock[game->stock_cursor + 1];
	return -1;
}

static inline int8_t cvalue(int8_t card) {
	return card % 13;
}

static inline int8_t csuit(int8_t card) {
	return card / 13;
}

static inline int8_t ccolor(int8_t card) {
	return csuit(card) % 2;
}

static inline bool foundation_valid(int8_t card, const int8_t *foundation_top) {
	return foundation_top[csuit(card)] == cvalue(card);
}

static inline bool tableau_valid(int8_t from, int8_t to) {
	return (cvalue(to) - cvalue(from) == 1) && ccolor(from) != ccolor(to);
}

static void update_waste_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	int8_t card_from, card_to;
	card_from = get_waste_card(game);
	if (card_from > -1) {
		// Waste->Foundation
		if (foundation_valid(card_from, game->foundation_top)) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_FOUNDATIONC + csuit(card_from);
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].flip = 0;
		}
		// Waste->Tableau
		bool king_placed = false;
		for (int i = 0; i < 7; ++i) {
			if (!king_placed && game->tableau_top[i] == 0 && cvalue(card_from) == SOSO_KING) {
				king_placed = true;
				ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].flip = 0;
				continue;
			}
			if (game->tableau_top[i] == 0) continue;
			card_to = game->tableau[i][game->tableau_top[i] - 1];
			if (tableau_valid(card_from, card_to)) {
				ctx->moves_available[ctx->moves_available_top].from = SOSO_STOCK_WASTE;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].flip = 0;
			}
		}
	}
}

static void update_tableau_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	int8_t card_from, card_to;
	for (int i = 0; i < 7; ++i) {
		card_from = game->tableau[i][game->tableau_top[i] - 1];
		// Tableau->Foundation
		if (game->tableau_top[i] > 0 && foundation_valid(card_from, game->foundation_top)) {
			ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
			ctx->moves_available[ctx->moves_available_top].to = SOSO_FOUNDATIONC + csuit(card_from);
			ctx->moves_available[ctx->moves_available_top].count = 1;
			ctx->moves_available[ctx->moves_available_top++].flip = 0;
		}

		// Tableau->Tableau top card
		if (game->tableau_top[i] - game->tableau_up[i] != 1) continue;
		bool king_placed = false;
		for (int j = 0; j < 7; ++j) {
			if (i == j) continue;
			if (!king_placed && game->tableau_top[j] == 0 && cvalue(card_from) == SOSO_KING) {
				king_placed = true;
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].flip = 0;
				continue;
			}

			card_to = game->tableau[j][game->tableau_top[j] - 1];
			if (tableau_valid(card_from, card_to)) {
				ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
				ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
				ctx->moves_available[ctx->moves_available_top].count = 1;
				ctx->moves_available[ctx->moves_available_top++].flip = 0;
			}
		}
	}

	// Tableau talon swap
	for (int i = 0; i < 7; ++i) {
		int8_t talonsize = game->tableau_top[i] - game->tableau_up[i];
		if (game->tableau_top[i] == 0 || talonsize == 1) continue;

		bool cards_closed = game->tableau_up[i] > 0;
		bool king_first = cvalue(game->tableau[i][game->tableau_up[i]]) == SOSO_KING;
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
				ctx->moves_available[ctx->moves_available_top++].flip = 0;
			}

			if (game->tableau_top[j] == 0) continue;

			for (int offset = start; offset < talonsize; ++offset) {
				card_from = game->tableau[i][game->tableau_up[i] + offset];
				card_to = game->tableau[j][game->tableau_top[j] - 1];
				if (tableau_valid(card_from, card_to)) {
					ctx->moves_available[ctx->moves_available_top].from = SOSO_TABLEAU1 + i;
					ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
					ctx->moves_available[ctx->moves_available_top].count = talonsize - offset;
					ctx->moves_available[ctx->moves_available_top++].flip = 0;
				}
			}
		}
	}
}

static void update_foundation_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	for (int i = 0; i < 4; ++i) {
		if (game->foundation_top[i] > 0)
			for (int j = 0; j < 7; ++j)
				if (tableau_valid(i * 13 + (game->foundation_top[i] - 1),
				                  game->tableau[j][game->tableau_top[j] - 1])) {
					ctx->moves_available[ctx->moves_available_top].from = SOSO_FOUNDATIONC + i;
					ctx->moves_available[ctx->moves_available_top].to = SOSO_TABLEAU1 + j;
					ctx->moves_available[ctx->moves_available_top].count = 1;
					ctx->moves_available[ctx->moves_available_top++].flip = 0;
				}
	}
}

static bool update_available_moves(soso_ctx_t *ctx, const soso_game_t *game) {
	ctx->moves_available_top = 0;
	update_waste_moves(ctx, game);
	update_tableau_moves(ctx, game);
	update_foundation_moves(ctx, game);
	return ctx->moves_available_top > 0;
}

static bool draw(soso_game_t *game, int draw_count) {
	if (game->stock_count == 0) return false;
	if (game->stock_cursor == -1)
		game->stock_cursor = game->stock_count - 1;
	else
		game->stock_cursor =
		    (game->stock_cursor - draw_count > -1) ? game->stock_cursor - draw_count : -1;
	return true;
}

static void make_auto_moves(soso_ctx_t *ctx, soso_game_t *game) {
	// Turn flipable cards
	for (int i = 0; i < 7; ++i) {
		if (game->tableau_top > 0 && game->tableau_top[i] == game->tableau_up[i]) {
			--game->tableau_up[i];
			add_move(ctx, &(soso_move_t){.from = i + SOSO_TABLEAU1,
			                             .to = i + SOSO_TABLEAU1,
			                             .count = 1,
			                             .flip = 1});
		}
	}

	// Optionally draw until a move becomes available
	int numdraw = 0;
	for (int i = 0; i < game->stock_count; ++i) {
		if (update_available_moves(ctx, game)) break;
		if (!draw(game, ctx->draw_count)) break;
		++numdraw;
	}
	if (numdraw > 0 && numdraw < game->stock_count)
		add_move(ctx, &(soso_move_t){.count = numdraw,
		                             .from = SOSO_STOCK_WASTE,
		                             .to = SOSO_STOCK_WASTE,
		                             .flip = 0});
}

void soso_solve(soso_ctx_t *ctx, soso_game_t *game) {}
