#pragma once

#include <sht/sht.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#define SOSO_EMPTY_CARD 127
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
	soso_int_t tableau[7][13];
	soso_int_t tableau_top[7];
	soso_int_t tableau_up[7];
	soso_int_t foundation_top[4];
} soso_game_t;

void soso_shuffle(soso_deck_t *deck, uint64_t seed);
void soso_deal(soso_game_t *game, soso_deck_t *deck);

typedef struct soso_move {
	soso_int_t from;
	soso_int_t to;
	soso_int_t count;
	soso_uint_t extra;
} soso_move_t;

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

void soso_ctx_init(soso_ctx_t *ctx, int draw_count, int max_visited, void *(*custom_alloc)(size_t),
                   void *(*custom_realloc)(void *, size_t), void (*custom_free)(void *));
void soso_ctx_destroy(soso_ctx_t *ctx);
bool soso_solve(soso_ctx_t *ctx, soso_game_t *game);

void soso_random_seed(uint64_t seed);
uint64_t soso_random_get(void);

//--- Inline ---//

static soso_int_t soso_internal_make_card(soso_int_t suit, soso_int_t value) {
	return (suit << 5) | value;
}

static soso_int_t soso_internal_get_waste_card(const soso_game_t *game) {
	if (game->stock_count > 0 && game->stock_count > game->stock_cur)
		return game->stock[game->stock_cur];
	return -1;
}

static soso_int_t soso_internal_cvalue(soso_int_t card) {
	return card & 0x0f;
}

static soso_int_t soso_internal_csuit(soso_int_t card) {
	return card >> 5;
}

static soso_int_t soso_internal_ccolor(soso_int_t card) {
	return soso_internal_csuit(card) & 1;
}

static bool soso_internal_valid_card(soso_int_t card) {
	soso_uint_t value = card & 0x0f;
	return value < 13;
}

static bool soso_internal_foundation_valid(soso_int_t card, const soso_int_t *foundation_top) {
	return soso_internal_valid_card(card) &&
	       foundation_top[soso_internal_csuit(card)] == soso_internal_cvalue(card);
}

static bool soso_internal_tableau_valid(soso_int_t from, soso_int_t to) {
	return soso_internal_valid_card(from) && soso_internal_valid_card(to) &&
	       (soso_internal_cvalue(to) - soso_internal_cvalue(from) == 1) &&
	       soso_internal_ccolor(from) != soso_internal_ccolor(to);
}

static int soso_internal_max_draws(const soso_ctx_t *ctx, const soso_game_t *game) {
	int maxdraw = game->stock_count + 1;
	if (ctx->draw_count > 1) {
		if ((game->stock_count - game->stock_cur) % ctx->draw_count == 0)
			maxdraw = ((game->stock_count - game->stock_cur) / ctx->draw_count +
			           game->stock_cur / ctx->draw_count) +
			          1;
		else
			maxdraw = (game->stock_cur / ctx->draw_count + game->stock_count / ctx->draw_count) + 1;
	}
	return maxdraw;
}

//--- Internals ---//

void soso_internal_add_move(soso_ctx_t *ctx, const soso_move_t m, bool is_auto);
void soso_internal_update_waste_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_tableau_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_stock_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_foundation_moves(soso_ctx_t *ctx, const soso_game_t *game);
bool soso_internal_update_available_moves(soso_ctx_t *ctx, const soso_game_t *game, bool no_stock);
bool soso_internal_draw(soso_game_t *game, int draw_count);
bool soso_internal_undo_draw(soso_game_t *game, int draw_count);
void make_move(soso_ctx_t *ctx, soso_game_t *game, soso_move_t m);
