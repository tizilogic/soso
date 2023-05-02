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
	game->waste_top = 0;
	game->stock_top = 24;
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

static bool move_available(const soso_game_t *game) {}

static void draw(soso_game_t *game) {}

static void make_auto_moves(soso_ctx_t *ctx, soso_game_t *game) {
	// Turn flipable cards
	for (int i = 0; i < 7; ++i) {
		if (game->tableau_top > 0 && game->tableau_top[i] == game->tableau_up[i]) {
			--game->tableau_up[i];
			add_move(ctx, &(soso_move_t){.from = i, .to = i, .count = 1, .flip = 1});
		}
	}

	// Optionally draw until a move becomes available
	int snw = game->stock_top + game->waste_top;
	int numdraw = 0;
	for (int i = 0; i < snw; ++i) {
		if (move_available(game)) return;
		draw(game);
		++numdraw;
	}
	if (numdraw > 0 && numdraw < snw)
		add_move(ctx, &(soso_move_t){.count = numdraw, .from = -1, .to = -1, .flip = 0});
}

void soso_solve(soso_ctx_t *ctx, soso_game_t *game) {}
