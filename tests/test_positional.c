// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

#include <string.h>

static void
test_positional_int(void)
{
    TEST("positional int");
    int         val    = 0;
    ArgonOption opts[] = {
        {
            .fullname = NULL,
            .alias    = 0,
            .type     = ARGON_OPTYPE_INT,
            .target   = &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("42"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == 42, "positional value wrong");
    PASS();
}

static void
test_positional_string(void)
{
    TEST("positional string");
    const char *val    = NULL;
    ArgonOption opts[] = {
        {
            .fullname = NULL,
            .alias    = 0,
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("input.txt"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val && strcmp(val, "input.txt") == 0, "string value wrong");
    PASS();
}

static void
test_positional_optional_missing(void)
{
    TEST("positional optional (missing)");
    int         val    = 99;
    ArgonOption opts[] = {
        {
            .fullname = NULL,
            .alias    = 0,
            .type     = ARGON_OPTYPE_INT,
            .target   = &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV(NULL));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == 99, "default value changed");
    ASSERT((opts[0].flags_ & ARGON_OPTFLAG_WRITTEN_) == 0,
           "written flag should not be set");
    PASS();
}

static void
test_positional_after_ddash(void)
{
    TEST("positional after --");
    const char *val    = NULL;
    ArgonOption opts[] = {
        {
            .fullname = NULL,
            .alias    = 0,
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--", "--not-an-option"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val && strcmp(val, "--not-an-option") == 0, "should capture after --");
    PASS();
}

static void
test_positional_mixed(void)
{
    TEST("positional mixed with named");
    const char *input  = NULL;
    bool        flag   = false;
    ArgonOption opts[] = {
        {.fullname = NULL,
         .alias    = 0,
         .type     = ARGON_OPTYPE_STRREF,
         .target   = (void *) &input},
        {.fullname = "verbose", .alias = 'v', .type = ARGON_OPTYPE_BOOL, .target = &flag},
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("file.txt", "--verbose"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(input && strcmp(input, "file.txt") == 0, "positional value wrong");
    ASSERT(flag == true, "named flag not set");
    PASS();
}

static void
test_positional_multi(void)
{
    TEST("positional multi");
    int         a = 0, b = 0;
    ArgonOption opts[] = {
        {.fullname = NULL, .alias = 0, .type = ARGON_OPTYPE_INT, .target = &a},
        {.fullname = NULL, .alias = 0, .type = ARGON_OPTYPE_INT, .target = &b},
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("10", "20"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(a == 10 && b == 20, "multi positional values wrong");
    PASS();
}

static void
test_positional_unexpected(void)
{
    TEST("positional unexpected");
    bool        flag   = false;
    ArgonOption opts[] = {
        {
            .fullname = "flag",
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &flag,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("stray"));
    ASSERT(r == ARGON_ERR_UNKNOWN_OPTION, "should fail with UNKNOWN_OPTION");
    PASS();
}

int
main(void)
{
    test_positional_int();
    test_positional_string();
    test_positional_optional_missing();
    test_positional_after_ddash();
    test_positional_mixed();
    test_positional_multi();
    test_positional_unexpected();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
