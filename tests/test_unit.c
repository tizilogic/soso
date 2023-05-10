#include "setup.c.h"
#include "sohelper.c.h"
#include "sosolver.c.h"

int main(void) {
	bool failed = false;
	if (!setup_run_tests()) failed = true;
	if (!sohelper_run_tests()) failed = true;
	if (!sosolver_run_tests()) failed = true;

	if (failed) return 1;
	printf("\nAll tests completed successfully\n");
	return 0;
}
