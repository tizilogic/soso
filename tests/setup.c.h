#pragma once

#include "test_helper.h"
#include <soso/soso.h>

#include <stdbool.h>
#include <stdio.h>

static bool setup_verify_shuffle(void) {
	soso_deck_t deck;
	soso_shuffle(&deck, 42);
	bool illegal_card = false;
	bool shuffled = false;
	for (int i = 0; i < 52; ++i) {
		if (deck.cards[i] > 51) illegal_card = true;
		if (deck.cards[i] != i) shuffled = true;
	}
	if (illegal_card == false && shuffled == true) return true;
	check_eq(illegal_card, false);
	check_eq(shuffled, true);
	return false;
}

static bool setup_verify_deal(void) {
	soso_deck_t deck;
	for (uint8_t i = 0; i < 52; ++i) deck.cards[i] = i;
	soso_game_t game;
	soso_deal(&game, &deck);
	uint8_t card = 0;
	for (int i = 0; i < 7; ++i) {
		if (game.tableau_top[i] != i + 1) test_fail();
		if (game.tableau_up[i] != i) test_fail();
		if (i < 4)
			if (game.foundation_top[i] != 0) test_fail();
		for (int j = i; j < 7; ++j)
			if (game.tableau[j][i] != card++) test_fail();
	}
	for (int i = 0; i < 24; ++i)
		if (game.stock[23 - i] != card++) test_fail();
	if (game.stock_count != 24) test_fail();
	if (game.stock_cursor != 23) test_fail();
	return true;
}

static bool setup_run_tests(void) {
	printf("Running Setup tests:\n");
	int count = 0;
	int fail = 0;
	run_test(setup_verify_shuffle, &count, &fail);
	run_test(setup_verify_deal, &count, &fail);
	printf("Finished %d tests, %d failures\n", count, fail);
	return fail == 0;
}
