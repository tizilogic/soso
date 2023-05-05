#pragma once

#include <sht/sht.h>
#include <stdint.h>
#include <stdlib.h>

#define SOSO_EMPTY_CARD 255
// TODO: Find a way to determine the maximum possible number of moves per game state
#define SOSO_MOVES_AVAILABLE_CAP 16
#define SOSO_BLACK 0
#define SOSO_RED 1
#define SOSO_KING 12

typedef enum soso_pile {
	SOSO_EMPTY = 0,
	SOSO_STOCK_WASTE,
	SOSO_TABLEAU1,
	SOSO_TABLEAU2,
	SOSO_TABLEAU3,
	SOSO_TABLEAU4,
	SOSO_TABLEAU5,
	SOSO_TABLEAU6,
	SOSO_TABLEAU7,
	SOSO_FOUNDATIONC,
	SOSO_FOUNDATIOND,
	SOSO_FOUNDATIONS,
	SOSO_FOUNDATIONH,
} soso_pile_t;

typedef struct soso_deck {
	int8_t cards[52];
} soso_deck_t;

typedef struct soso_game {
	int8_t stock[24];
	int8_t stock_cursor;
	int8_t stock_count;
	int8_t tableau[7][13];
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
	soso_move_t moves_available[SOSO_MOVES_AVAILABLE_CAP];
	int moves_cap, moves_top;
	int moves_available_top;
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
