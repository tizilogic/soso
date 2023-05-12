#pragma once

#include "test_helper.h"
#include <soso/soso.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool sohelper_verify_card(void) {
	test_start();
	soso_int_t c = soso_internal_make_card(3, 10);
	check_eq(soso_internal_ccolor(c), SOSO_RED);
	check_eq(soso_internal_csuit(c), 3);
	check_eq(soso_internal_cvalue(c), 10);
	check_eq(soso_internal_valid_card(c), true);
	check_eq(soso_internal_valid_card(soso_internal_make_card(3, 13)), false);
	test_end();
}

static bool sohelper_verify_valid(void) {
	test_start();
	soso_game_t game = (soso_game_t){
	    .foundation_top = {0, 2, 4, 6}, .stock = {0, 1, 2, 3, 4}, .stock_count = 5, .stock_cur = 0};
	check_eq(soso_internal_foundation_valid(soso_internal_make_card(0, 0), game.foundation_top),
	         true);
	check_eq(soso_internal_foundation_valid(soso_internal_make_card(1, 2), game.foundation_top),
	         true);
	check_eq(soso_internal_foundation_valid(soso_internal_make_card(2, 4), game.foundation_top),
	         true);
	check_eq(soso_internal_foundation_valid(soso_internal_make_card(3, 6), game.foundation_top),
	         true);
	check_eq(soso_internal_get_waste_card(&game), 0);
	check_eq(soso_internal_tableau_valid(0, soso_internal_make_card(1, 1)), true);
	test_end();
}

static bool sohelper_verify_add_move(void) {
	test_start();
	soso_ctx_t ctx;
	soso_ctx_init(&ctx, 3, 100000, NULL, NULL, NULL);
	soso_move_t m = {
	    .from = SOSO_STOCK_WASTE, .to = SOSO_TABLEAU1, .count = 1, .extra = SOSO_AUTO_MOVE};
	soso_internal_add_move(&ctx, m, true);
	check_eq(ctx.automoves_count, 1);
	check_eq(ctx.moves_top, 1);
	check_eq(*(int *)&ctx.moves[0], *(int *)&m);
	soso_ctx_destroy(&ctx);
	test_end();
}

static bool is_in_available_moves(const soso_ctx_t *ctx, soso_move_t m) {
	for (int i = 0; i < ctx->moves_available_top; ++i)
		if (*(int *)&ctx->moves_available[i] == *(int *)&m) return true;
	return false;
}

static bool sohelper_verify_update_waste_moves(void) {
	test_start();
	soso_deck_t deck;
	soso_game_t game;
	soso_ctx_t ctx;
	soso_shuffle(&deck, 43);
	soso_deal(&game, &deck);
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);
	for (int i = 0; i < 3; ++i) soso_internal_draw(&game, ctx.draw_count);
	soso_internal_update_waste_moves(&ctx, &game);
	check_eq(ctx.moves_available_top, 1);
	soso_move_t m;
	m.from = SOSO_STOCK_WASTE;
	m.to = SOSO_TABLEAU5;
	m.count = 1;
	m.extra = 0;
	check_eq(is_in_available_moves(&ctx, m), true);

	soso_internal_draw(&game, ctx.draw_count);
	ctx.moves_available_top = 0;
	soso_internal_update_waste_moves(&ctx, &game);
	check_eq(ctx.moves_available_top, 1);
	m.to = SOSO_FOUNDATION2D;
	check_eq(is_in_available_moves(&ctx, m), true);
	test_end();
}

#define mc soso_internal_make_card

static bool sohelper_verify_update_tableau_moves(void) {
	test_start();
	soso_game_t game;
	memset(&game, 0, sizeof(soso_game_t));
	soso_ctx_t ctx;
	for (int i = 0; i < 4; ++i) game.foundation_top[i] = 0;
	for (int i = 0; i < 13; ++i) game.tableau[0][i] = mc(i % 4, 12 - i);
	game.tableau_top[0] = 13;
	game.tableau_up[0] = 0;
	game.tableau[1][0] = mc(1, 1);
	game.tableau_top[1] = 1;
	game.tableau_up[1] = 0;
	game.tableau[2][0] = mc(2, 4);
	game.tableau_top[2] = 1;
	game.tableau_up[2] = 0;
	game.tableau[3][0] = mc(1, 0);
	game.tableau[3][1] = mc(2, 12);
	game.tableau_top[3] = 2;
	game.tableau_up[3] = 1;

	ctx.moves_available_top = 0;
	soso_internal_update_tableau_moves(&ctx, &game);
	check_eq(ctx.moves_available_top, 5);
	soso_move_t m;
	m.from = SOSO_TABLEAU1;
	m.to = SOSO_FOUNDATION1C;
	m.count = 1;
	m.extra = 0;
	check_eq(is_in_available_moves(&ctx, m), true);
	m.to = SOSO_TABLEAU2;
	check_eq(is_in_available_moves(&ctx, m), true);
	m.to = SOSO_TABLEAU3;
	m.count = 4;
	check_eq(is_in_available_moves(&ctx, m), true);
	m.to = SOSO_TABLEAU4;
	m.count = 12;
	check_eq(is_in_available_moves(&ctx, m), true);
	m.from = SOSO_TABLEAU4;
	m.to = SOSO_TABLEAU5;
	m.count = 1;
	check_eq(is_in_available_moves(&ctx, m), true);
	test_end();
}

static bool sohelper_verify_update_stock_moves(void) {
	test_start();
	soso_game_t game;
	memset(&game, 0, sizeof(soso_game_t));
	soso_ctx_t ctx;
	ctx.draw_count = 1; // TODO: Test other draw counts
	game.stock[0] = mc(1, 12);
	game.stock[1] = mc(0, 0);
	game.stock[2] = mc(3, 4);
	game.stock_count = 3;
	game.stock_cur = 0;
	ctx.moves_available_top = 0;
	soso_internal_update_stock_moves(&ctx, &game);
	check_eq(ctx.moves_available_top, 1);
	soso_move_t m;
	m.from = SOSO_STOCK_WASTE;
	m.to = SOSO_STOCK_WASTE;
	m.count = 3;
	m.extra = 0;
	check_eq(is_in_available_moves(&ctx, m), true);
	test_end();
}

static bool sohelper_verify_update_foundation_moves(void) {
	test_start();
	soso_game_t game;
	memset(&game, 0, sizeof(soso_game_t));
	soso_ctx_t ctx;
	ctx.draw_count = 1;
	game.foundation_top[0] = 13;
	game.foundation_top[1] = 1;
	game.foundation_top[2] = 3;
	game.tableau[0][0] = mc(1, 3);
	game.tableau_top[0] = 1;
	ctx.moves_available_top = 0;
	soso_internal_update_foundation_moves(&ctx, &game);
	check_eq(ctx.moves_available_top, 2);
	soso_move_t m;
	m.from = SOSO_FOUNDATION1C;
	m.to = SOSO_TABLEAU2;
	m.count = 1;
	m.extra = 0;
	check_eq(is_in_available_moves(&ctx, m), true);
	test_end();
}

static bool sohelper_verify_make_undo_move(void) {
	// Need to test:
	// Draw
	// Waste -> Foundation
	// Waste -> Tableau
	// Tableau -> Tableau
	// Tableau -> Foundation
	// Foundation -> Tableau
	return true;
}

static bool sohelper_verify_make_automoves(void) {}
static bool sohelper_verify_revert_last_move(void) {}

static bool sohelper_run_tests(void) {
	printf("\nRunning Solitaire Helper tests:\n");
	int count = 0;
	int fail = 0;
	run_test(sohelper_verify_card, &count, &fail);
	run_test(sohelper_verify_valid, &count, &fail);
	run_test(sohelper_verify_add_move, &count, &fail);
	run_test(sohelper_verify_update_waste_moves, &count, &fail);
	run_test(sohelper_verify_update_tableau_moves, &count, &fail);
	run_test(sohelper_verify_update_stock_moves, &count, &fail);
	run_test(sohelper_verify_update_foundation_moves, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
