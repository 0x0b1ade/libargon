// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

#include <string.h>

static void
test_int_parse(void)
{
    TEST("int parse");
    int         val    = 0;
    ArgonOption opts[] = {
        {
            .fullname = "count",
            .type     = ARGON_OPTYPE_INT,
            .target   = &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--count", "42"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == 42, "int value wrong");
    PASS();
}

static void
test_float_parse(void)
{
    TEST("float parse");
    double      val    = 0.0;
    ArgonOption opts[] = {
        {
            .fullname = "scale",
            .type     = ARGON_OPTYPE_FLOAT,
            .target   = &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--scale", "0.6875"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == 0.6875, "float value wrong");
    PASS();
}

static void
test_string_parse(void)
{
    TEST("string parse");
    const char *val    = NULL;
    ArgonOption opts[] = {
        {
            .fullname = "output",
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--output", "file.txt"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val && strcmp(val, "file.txt") == 0, "string value wrong");
    PASS();
}

static void
test_short_alias(void)
{
    TEST("short alias (-o)");
    const char *val    = NULL;
    ArgonOption opts[] = {
        {
            .fullname = "output",
            .alias    = 'o',
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("-o", "out.bin"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val && strcmp(val, "out.bin") == 0, "alias value wrong");
    PASS();
}

static void
test_negative_int(void)
{
    TEST("negative int value");
    int         val    = 0;
    ArgonOption opts[] = {
        {
            .fullname = "count",
            .type     = ARGON_OPTYPE_INT,
            .target   = &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--count", "-1"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == -1, "negative value wrong");
    PASS();
}

int
main(void)
{
    test_int_parse();
    test_float_parse();
    test_string_parse();
    test_short_alias();
    test_negative_int();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
