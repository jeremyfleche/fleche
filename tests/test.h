#pragma once

#include <cstdio>

inline constexpr const char* RED = "\033[31m";
inline constexpr const char* GREEN = "\033[32m";
inline constexpr const char* DIM = "\033[2m";
inline constexpr const char* BOLD = "\033[1m";
inline constexpr const char* RESET = "\033[0m";

inline int test_checks = 0;
inline int test_failures = 0;

// Failure messages of the current suite, printed by run_suite() after its status line.
inline constexpr int MAX_MESSAGES = 32;
inline char test_messages[MAX_MESSAGES][256];
inline int test_message_count = 0;

#define CHECK(cond)                                                                                                    \
	do {                                                                                                               \
		++test_checks;                                                                                                 \
		if (!(cond)) {                                                                                                 \
			++test_failures;                                                                                           \
			if (test_message_count < MAX_MESSAGES)                                                                     \
				std::snprintf(test_messages[test_message_count++], 256, "%s:%d: %s", __FILE__, __LINE__, #cond);       \
		}                                                                                                              \
	} while (0)

void test_types();
void test_attacks();