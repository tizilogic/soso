#pragma once

#include <assert.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SOSO_EMPTY_CARD -1
// TODO: Find a way to determine the maximum possible number of moves per game state
#define SOSO_MOVES_AVAILABLE_CAP 16
#define SOSO_BLACK 0
#define SOSO_RED 1
#define SOSO_KING 12
#define SOSO_NOEXTRA 0
#define SOSO_FLIP 1
#define SOSO_AUTO_MOVE 2

typedef int_fast8_t soso_int_t;
typedef uint_fast8_t soso_uint_t;

typedef enum soso_pile {
	SOSO_EMPTY_PILE = 0,
	SOSO_STOCK_WASTE,
	SOSO_TABLEAU1,
	SOSO_TABLEAU2,
	SOSO_TABLEAU3,
	SOSO_TABLEAU4,
	SOSO_TABLEAU5,
	SOSO_TABLEAU6,
	SOSO_TABLEAU7,
	SOSO_FOUNDATION1C,
	SOSO_FOUNDATION2D,
	SOSO_FOUNDATION3S,
	SOSO_FOUNDATION4H,
	SOSO_TOTAL_PILES,
} soso_pile_t;

/**
 * Card occupies 6 bits
 * 0-3 --> Value 	[0..12]
 * 4-5 --> Suit		[0..3]
 */
typedef struct soso_deck {
	soso_int_t cards[52];
} soso_deck_t;

typedef struct soso_game {
	soso_int_t stock[24];
	soso_int_t stock_cur;
	soso_int_t stock_count;
	soso_int_t tableau[7][19];
	soso_int_t tableau_top[7];
	soso_int_t tableau_up[7];
	soso_int_t foundation_top[4];
} soso_game_t;

typedef struct soso_move {
	soso_int_t from;
	soso_int_t to;
	soso_int_t count;
	soso_uint_t extra;
} soso_move_t;

typedef struct sht sht_t;
typedef struct soso_ctx {
	int draw_count;
	sht_t *visited;
	soso_move_t *moves;
	soso_move_t moves_available[SOSO_MOVES_AVAILABLE_CAP];
	int moves_cap, moves_top;
	int moves_available_top;
	int automoves_count;
	int moves_total;
	int max_visited;
	void *(*alloc)(size_t);
	void *(*realloc)(void *, size_t);
	void (*free)(void *);
} soso_ctx_t;

void soso_shuffle(soso_deck_t *deck, uint64_t seed);
void soso_deal(soso_game_t *game, soso_deck_t *deck);
// buffer needs to have at least 262 bytes
void soso_export(const soso_game_t *game, char *buffer);
void soso_ctx_init(soso_ctx_t *ctx, int draw_count, int max_visited, void *(*custom_alloc)(size_t),
                   void *(*custom_realloc)(void *, size_t), void (*custom_free)(void *));
void soso_ctx_destroy(soso_ctx_t *ctx);
bool soso_solve(soso_ctx_t *ctx, soso_game_t *game);

void soso_clean_game(soso_game_t *game);
void soso_make_auto_moves(soso_ctx_t *ctx, soso_game_t *game);

void soso_random_seed(uint64_t seed);
uint64_t soso_random_get(void);

#ifdef __cplusplus
}
#endif