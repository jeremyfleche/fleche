#include "test.h"

#include <cstdio>

// Suites made only of static_assert: if this program compiled, they passed.
static void static_suite(const char* name) {
	std::printf("  %s✓%s %-12s %s(static_assert)%s\n", GREEN, RESET, name, DIM, RESET);
}

// Runs one suite of CHECKs, then prints its status line and its failures.
static void run_suite(const char* name, void (*suite)()) {
	const int checks_before = test_checks;
	const int failures_before = test_failures;
	test_message_count = 0;

	suite();

	const int checks = test_checks - checks_before;
	const int failures = test_failures - failures_before;

	if (failures == 0) {
		std::printf("  %s✓%s %-12s (%d checks)\n", GREEN, RESET, name, checks);
	} else {
		std::printf("  %s✗%s %-12s (%d checks), %s%d failure%s%s\n", RED, RESET, name, checks, RED, failures,
		            failures > 1 ? "s" : "", RESET);
		for (int i = 0; i < test_message_count; ++i)
			std::printf("      %s%s%s\n", RED, test_messages[i], RESET);
	}
}

int main() {
	std::printf("\n");

	run_suite("test_types", test_types);
	static_suite("test_attacks");

	std::printf("\n");
	if (test_failures == 0) {
		std::printf("  %s%sAll tests passed: %d checks.%s\n\n", BOLD, GREEN, test_checks, RESET);
	} else {
		std::printf("  %s%s%d failure%s out of %d checks.%s\n\n", BOLD, RED, test_failures,
		            test_failures > 1 ? "s" : "", test_checks, RESET);
	}
	return test_failures != 0;
}