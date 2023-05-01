#include "soso.h"
#include <stdlib.h>
#include <string.h>

#define SOSO_NUM_SHUFFLES 12

static uint64_t random_get_in(uint64_t min, uint64_t max) {
	return soso_random_get() % (max + 1 - min) + min;
}

void soso_shuffle(soso_deck_t *deck, uint64_t seed) {
	soso_random_seed(seed);
	for (uint8_t i = 0; i < 52; ++i) {
		deck->cards[i] = i;
	}

	for (int s = 0; s < SOSO_NUM_SHUFFLES; ++s)
		for (int i = 0; i < 51; ++i) {
			int j = random_get_in(i, 51);
			if (i == j) continue;
			uint8_t t = deck->cards[j];
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

void soso_ctx_init(soso_ctx_t *ctx, int draw_count) {
    ctx->draw_count = draw_count;
    ctx->visited = NULL;
    ctx->moves = NULL;
    ctx->moves_cap = 0;
    ctx->moves_top = -1;
    ctx->moves_total = 0;
}

void soso_solve(soso_ctx_t *ctx, soso_game_t *game) {}
