// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#ifndef ARGON_TEST_H
#define ARGON_TEST_H

#include <stdbool.h>

static int tests_run  = 0;
static int tests_pass = 0;
static int tests_fail = 0;

#define TEST(name)                                                                       \
    do                                                                                   \
    {                                                                                    \
        tests_run++;                                                                     \
        (void) fprintf(stderr, "  %-50s ... ", name);                                    \
    } while (0)

#define PASS()                                                                           \
    do                                                                                   \
    {                                                                                    \
        tests_pass++;                                                                    \
        (void) fprintf(stderr, "PASS\n");                                                \
    } while (0)

#define FAIL(msg)                                                                        \
    do                                                                                   \
    {                                                                                    \
        tests_fail++;                                                                    \
        (void) fprintf(stderr, "FAIL: %s\n", msg);                                       \
    } while (0)

#define ASSERT(cond, msg)                                                                \
    do                                                                                   \
    {                                                                                    \
        if (!(cond))                                                                     \
        {                                                                                \
            FAIL(msg);                                                                   \
            return;                                                                      \
        }                                                                                \
    } while (0)

#endif
