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
	check_eq(ctx.moves_available_top, 2);
	soso_move_t m;
	m.from = SOSO_TABLEAU1;
	m.to = SOSO_FOUNDATION1C;
	m.count = 1;
	m.extra = 0;
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
	test_start();
	soso_game_t game;
	memset(&game, 0, sizeof(soso_game_t));
	soso_ctx_t ctx;
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);

	// Draw
	game.stock[0] = mc(0, 12);
	game.stock[1] = mc(0, 11);
	game.stock[2] = mc(0, 10);
	game.stock[3] = mc(0, 9);
	game.stock_count = 4;
	game.stock_cur = 4;

	check_eq(soso_internal_get_waste_card(&game), -1);
	soso_move_t m;
	m.from = SOSO_STOCK_WASTE;
	m.to = SOSO_STOCK_WASTE;
	m.count = 1;
	m.extra = 0;
	soso_internal_make_move(&ctx, &game, m);
	check_eq(soso_internal_get_waste_card(&game), mc(0, 9));
	soso_internal_add_move(&ctx, m, false);
	soso_internal_undo_move(&ctx, &game);
	check_eq(soso_internal_get_waste_card(&game), -1);

	m.count = 5;
	soso_internal_make_move(&ctx, &game, m);
	check_eq(soso_internal_get_waste_card(&game), -1);

	soso_ctx_destroy(&ctx);
	soso_ctx_init(&ctx, 3, 100000, NULL, NULL, NULL);

	game.stock[0] = mc(0, 12);
	game.stock[1] = mc(0, 11);
	game.stock[2] = mc(0, 10);
	game.stock[3] = mc(0, 9);
	game.stock_count = 4;
	game.stock_cur = 4;
	m.count = 2;
	soso_internal_make_move(&ctx, &game, m);
	check_eq(soso_internal_get_waste_card(&game), mc(0, 12));

	// Waste -> Foundation
	game.foundation_top[0] = 12;
	m.to = SOSO_FOUNDATION1C;
	m.count = 1;
	soso_internal_make_move(&ctx, &game, m);
	check_eq(game.foundation_top[0], 13);

	// Waste -> Tableau
	soso_ctx_destroy(&ctx);
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);
	game.stock[0] = mc(0, 12);
	game.stock[1] = mc(0, 11);
	game.stock[2] = mc(0, 10);
	game.stock[3] = mc(0, 9);
	game.stock_count = 4;
	game.stock_cur = 0;

	m.to = SOSO_TABLEAU1;
	soso_internal_make_move(&ctx, &game, m);
	soso_internal_add_move(&ctx, m, false);
	check_eq(game.tableau[0][0], mc(0, 12));
	check_eq(game.tableau_top[0], 1);
	check_eq(game.tableau_up[0], 0);
	check_eq(game.stock_count, 3);
	check_eq(game.stock[0], mc(0, 11));
	soso_internal_undo_move(&ctx, &game);
	check_eq(game.stock_count, 4);
	check_eq(game.stock[0], mc(0, 12));

	// Tableau -> Tableau
	soso_ctx_destroy(&ctx);
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);
	game.tableau[0][0] = mc(1, 1);
	game.tableau[0][1] = mc(1, 12);
	game.tableau[0][2] = mc(2, 11);
	game.tableau[0][3] = mc(3, 10);
	game.tableau_top[0] = 4;
	game.tableau_up[0] = 1;
	m.from = SOSO_TABLEAU1;
	m.to = SOSO_TABLEAU2;
	m.count = 3;
	soso_internal_make_move(&ctx, &game, m);
	soso_internal_add_move(&ctx, m, false);
	check_eq(ctx.moves_top, 1);
	check_eq(game.tableau_top[0], 1);
	check_eq(game.tableau_up[0], 1);
	check_eq(game.tableau[0][0], mc(1, 1));
	check_eq(game.tableau[1][0], mc(1, 12));
	check_eq(game.tableau_top[1], 3);
	check_eq(game.tableau_up[1], 0);
	soso_internal_undo_move(&ctx, &game);
	check_eq(ctx.moves_top, 0);
	check_eq(game.tableau_top[0], 4);
	check_eq(game.tableau_up[0], 1);
	check_eq(game.tableau[0][1], mc(1, 12));

	// Tableau -> Foundation
	soso_ctx_destroy(&ctx);
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);
	game.tableau[0][0] = mc(1, 1);
	game.tableau[0][1] = mc(1, 12);
	game.tableau[0][2] = mc(2, 11);
	game.tableau[0][3] = mc(3, 10);
	game.tableau_top[0] = 4;
	game.tableau_up[0] = 1;
	game.foundation_top[3] = 10;
	m.from = SOSO_TABLEAU1;
	m.to = SOSO_FOUNDATION4H;
	m.count = 1;
	soso_internal_make_move(&ctx, &game, m);
	soso_internal_add_move(&ctx, m, false);
	check_eq(game.foundation_top[3], 11);
	check_eq(game.tableau_top[0], 3);
	soso_internal_undo_move(&ctx, &game);
	check_eq(game.foundation_top[3], 10);
	check_eq(game.tableau_top[0], 4);
	check_eq(game.tableau[0][3], mc(3, 10));

	// Foundation -> Tableau
	soso_ctx_destroy(&ctx);
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);
	game.tableau[0][0] = mc(1, 1);
	game.tableau[0][1] = mc(1, 12);
	game.tableau[0][2] = mc(2, 11);
	game.tableau_top[0] = 3;
	game.tableau_up[0] = 1;
	game.foundation_top[3] = 11;
	m.from = SOSO_FOUNDATION4H;
	m.to = SOSO_TABLEAU1;
	m.count = 1;
	soso_internal_make_move(&ctx, &game, m);
	soso_internal_add_move(&ctx, m, false);
	check_eq(game.foundation_top[3], 10);
	check_eq(game.tableau_top[0], 4);
	check_eq(game.tableau[0][3], mc(3, 10));
	soso_internal_undo_move(&ctx, &game);
	check_eq(game.foundation_top[3], 11);
	check_eq(game.tableau_top[0], 3);

	soso_ctx_destroy(&ctx);
	test_end();
}

static bool sohelper_verify_make_automoves(void) {
	test_start();
	soso_game_t game;
	memset(&game, 0, sizeof(soso_game_t));
	soso_ctx_t ctx;
	soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);

	game.stock[0] = mc(0, 12);
	game.stock[1] = mc(0, 11);
	game.stock[2] = mc(0, 10);
	game.stock[3] = mc(0, 9);
	game.stock_count = 4;
	game.stock_cur = 4;
	game.foundation_top[0] = 10;
	game.tableau[0][0] = mc(1, 1);
	game.tableau_top[0] = 1;
	game.tableau_up[0] = 1;

	soso_make_auto_moves(&ctx, &game);
	check_eq(soso_internal_get_waste_card(&game), mc(0, 10));
	check_eq(game.stock_cur, 2);
	check_eq(game.tableau_up[0], 0);
	check_eq(ctx.moves_top, 2);
	soso_move_t m;
	m.from = SOSO_TABLEAU1;
	m.to = SOSO_TABLEAU1;
	m.count = 1;
	m.extra = SOSO_FLIP | SOSO_AUTO_MOVE;
	check_eq(*(int *)&ctx.moves[0], *(int *)&m);
	m.from = SOSO_STOCK_WASTE;
	m.to = SOSO_STOCK_WASTE;
	m.count = 2;
	m.extra = SOSO_AUTO_MOVE;
	check_eq(*(int *)&ctx.moves[1], *(int *)&m);
	test_end();
}

static bool sohelper_verify_revert_last_move(void) {
	test_start();
	for (int i = 1; i < 71; i += 7) {
		soso_game_t game;
		soso_ctx_t ctx;
		soso_deck_t deck;
		soso_shuffle(&deck, (0xbadfacedbeefc0de ^ (i * 1234567 + (i << 17))) >> (i % 32));
		soso_deal(&game, &deck);
		soso_ctx_init(&ctx, 1, 100000, NULL, NULL, NULL);
		soso_make_auto_moves(&ctx, &game);
		soso_clean_game(&game);
		soso_internal_update_available_moves(&ctx, &game, false);
		uint32_t h = soso_internal_state_hash(&game, &ctx.moves_available[0]);
		uint32_t prev = h;

		soso_internal_make_move(&ctx, &game, ctx.moves_available[0]);
		soso_internal_add_move(&ctx, ctx.moves_available[0], false);
		soso_make_auto_moves(&ctx, &game);
		soso_clean_game(&game);
		soso_internal_update_available_moves(&ctx, &game, false);
		h = soso_internal_state_hash(&game, &ctx.moves_available[0]);
		check_neq(h, prev);

		soso_internal_revert_to_last_move(&ctx, &game);
		soso_make_auto_moves(&ctx, &game);
		soso_clean_game(&game);
		soso_internal_update_available_moves(&ctx, &game, false);
		h = soso_internal_state_hash(&game, &ctx.moves_available[0]);
		check_eq(h, prev);
		soso_ctx_destroy(&ctx);
	}
	test_end();
}

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
	run_test(sohelper_verify_make_undo_move, &count, &fail);
	run_test(sohelper_verify_make_automoves, &count, &fail);
	run_test(sohelper_verify_revert_last_move, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
