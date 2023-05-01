#pragma once

#include <sht/sht.h>
#include <stdint.h>

#define SOSO_EMPTY_CARD 255

typedef struct soso_deck {
    uint8_t cards[52];
} soso_deck_t;

typedef struct soso_game {
    uint8_t stock[24];
    uint8_t waste[24];
    uint8_t tableau[7][13];
    uint8_t foundation[4][13];
    uint8_t stock_top;
    uint8_t waste_top;
    uint8_t tableau_top[7];
    uint8_t tableau_up[7];
    uint8_t foundation_top[4];
} soso_game_t;

void soso_shuffle(soso_deck_t *deck, uint64_t seed);
void soso_deal(soso_game_t *game, soso_deck_t *deck);

typedef struct soso_move {
    uint8_t from;
    uint8_t to;
    uint8_t count;
    uint8_t flip;
} soso_move_t;

typedef struct soso_ctx {
    int draw_count;
    sht_t *visited;
    soso_move_t *moves;
    int moves_cap, moves_top;
    int moves_total;
} soso_ctx_t;

void soso_ctx_init(soso_ctx_t *ctx, int draw_count);
void soso_solve(soso_ctx_t *ctx, soso_game_t *game);

void soso_random_seed(uint64_t seed);
uint64_t soso_random_get(void);
