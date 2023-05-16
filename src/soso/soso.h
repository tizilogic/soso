#pragma once

#include <assert.h>
#include <sht/sht.h>
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

/**
 * @brief Initializes and shuffles a deck with a given seed
 *
 * @param deck
 * @param seed
 */
void soso_shuffle(soso_deck_t *deck, uint64_t seed);

/**
 * @brief Setup a game using a provided deck
 *
 * @param game
 * @param deck
 */
void soso_deal(soso_game_t *game, soso_deck_t *deck);

/**
 * @brief Converts the current state of a game into a string
 *
 * @param game
 * @param buffer Needs to be at least 262 bytes of size
 */
void soso_export(const soso_game_t *game, char *buffer);

/**
 * @brief Initializes a context for solving a game
 *
 * @param ctx
 * @param draw_count integer `>0`
 * @param max_visited number of states to visit before giving up
 * @param custom_alloc optional custom allocator function. `malloc/realloc/free` from the stdlib
 * will be used if this is `NULL`
 * @param custom_realloc if `custom_alloc != NULL` requires a `realloc` equivalent
 * @param custom_free if `custom_alloc != NULL` requires a `free` equivalent
 */
void soso_ctx_init(soso_ctx_t *ctx, int draw_count, int max_visited, void *(*custom_alloc)(size_t),
                   void *(*custom_realloc)(void *, size_t), void (*custom_free)(void *));
void soso_ctx_destroy(soso_ctx_t *ctx);

/**
 * @brief Attempts to solve a game
 * 
 * @param ctx 
 * @param game 
 * @return true found a solution
 * @return false no solution found
 */
bool soso_solve(soso_ctx_t *ctx, soso_game_t *game);

/**
 * @brief Ensures a clean, hashable state
 * 
 * @param game 
 */
void soso_clean_game(soso_game_t *game);

/**
 * @brief Perform card flipping and draw until a move is available
 * 
 * @param ctx 
 * @param game 
 */
void soso_make_auto_moves(soso_ctx_t *ctx, soso_game_t *game);

#ifdef __cplusplus
}
#endif
//--- Internals ---//

void soso_random_seed(uint64_t seed);
uint64_t soso_random_get(void);
uint64_t soso_internal_random_get_in(uint64_t min, uint64_t max);
soso_int_t soso_internal_make_card(soso_int_t suit, soso_int_t value);
soso_int_t soso_internal_get_waste_card(const soso_game_t *game);
soso_int_t soso_internal_cvalue(soso_int_t card);
soso_int_t soso_internal_csuit(soso_int_t card);
soso_int_t soso_internal_ccolor(soso_int_t card);
bool soso_internal_valid_card(soso_int_t card);
bool soso_internal_foundation_valid(soso_int_t card, const soso_int_t *foundation_top);
bool soso_internal_tableau_valid(soso_int_t from, soso_int_t to);
int soso_internal_max_draws(const soso_ctx_t *ctx, const soso_game_t *game);
bool soso_internal_game_solved(const soso_game_t *game);
void soso_internal_revert_to_last_move(soso_ctx_t *ctx, soso_game_t *game);
uint32_t soso_internal_state_hash(const soso_game_t *game, const soso_move_t *move);
void soso_internal_add_move(soso_ctx_t *ctx, const soso_move_t m, bool is_auto);
void soso_internal_update_waste_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_tableau_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_stock_moves(soso_ctx_t *ctx, const soso_game_t *game);
void soso_internal_update_foundation_moves(soso_ctx_t *ctx, const soso_game_t *game);
bool soso_internal_update_available_moves(soso_ctx_t *ctx, const soso_game_t *game, bool no_stock);
bool soso_internal_draw(soso_game_t *game, int draw_count);
bool soso_internal_undo_draw(soso_game_t *game, int draw_count);
void soso_internal_make_move(soso_ctx_t *ctx, soso_game_t *game, soso_move_t m);
void soso_internal_undo_move(soso_ctx_t *ctx, soso_game_t *game);
