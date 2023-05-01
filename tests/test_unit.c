#include "setup.c.h"

int main(void) {
	if (setup_run_tests()) return 0;
	return 1;
}
