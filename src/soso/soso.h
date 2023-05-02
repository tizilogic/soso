#pragma once

#include <sht/sht.h>
#include <stdint.h>
#include <stdlib.h>

#define SOSO_EMPTY_CARD 255

typedef struct soso_deck {
	int8_t cards[52];
} soso_deck_t;

typedef struct soso_game {
	int8_t stock[24];
	int8_t waste[24];
	int8_t tableau[7][13];
	int8_t foundation[4][13];
	int8_t stock_top;
	int8_t waste_top;
	int8_t tableau_top[7];
	int8_t tableau_up[7];
	int8_t foundation_top[4];
} soso_game_t;

void soso_shuffle(soso_deck_t *deck, uint64_t seed);
void soso_deal(soso_game_t *game, soso_deck_t *deck);

typedef struct soso_move {
	int8_t from;
	int8_t to;
	int8_t count;
	int8_t flip;
} soso_move_t;

typedef struct soso_ctx {
	int draw_count;
	sht_t *visited;
	soso_move_t *moves;
	int moves_cap, moves_top;
	int moves_total;
	void *(*alloc)(size_t);
	void *(*realloc)(void *, size_t);
	void (*free)(void *);
} soso_ctx_t;

void soso_ctx_init(soso_ctx_t *ctx, int draw_count, void *(*custom_alloc)(size_t),
                   void *(*custom_realloc)(void *, size_t), void (*custom_free)(void *));
void soso_ctx_destroy(soso_ctx_t *ctx);
void soso_solve(soso_ctx_t *ctx, soso_game_t *game);

void soso_random_seed(uint64_t seed);
uint64_t soso_random_get(void);
