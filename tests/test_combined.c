// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

#include <string.h>

static void
test_combined_short(void)
{
    TEST("combined short options (-abc)");
    bool        a = false, b = false, c = false;
    ArgonOption opts[] = {
        {
            .fullname = "alpha",
            .alias    = 'a',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &a,
        },
        {
            .fullname = "bravo",
            .alias    = 'b',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &b,
        },
        {
            .fullname = "charlie",
            .alias    = 'c',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &c,
        },
        {0},
    };
    Argon argon = {
        .options = opts,
        .flags   = ARGON_COMBINED_MATCHING,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("-abc"));
    ASSERT(r == ARGON_OK, "combined short parse failed");
    ASSERT(a && b && c, "not all combined flags set");
    PASS();
}

static void
test_combined_short_value(void)
{
    TEST("combined short with value (-abco out.txt)");
    bool        a = false, b = false;
    const char *output = NULL;
    ArgonOption opts[] = {
        {
            .fullname = "alpha",
            .alias    = 'a',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &a,
        },
        {
            .fullname = "bravo",
            .alias    = 'b',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &b,
        },
        {
            .fullname = "output",
            .alias    = 'o',
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &output,
        },
        {0},
    };
    Argon argon = {
        .options = opts,
        .flags   = ARGON_COMBINED_MATCHING,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("-abo", "out.txt"));
    ASSERT(r == ARGON_OK, "combined short with value failed");
    ASSERT(a && b, "bool flags not set");
    ASSERT(output && strcmp(output, "out.txt") == 0, "output value wrong");
    PASS();
}

static void
test_combined_short_bad_middle(void)
{
    TEST("combined short non-bool in middle (error)");
    bool        a = false, b = false;
    const char *output = NULL;
    ArgonOption opts[] = {
        {
            .fullname = "alpha",
            .alias    = 'a',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &a,
        },
        {
            .fullname = "output",
            .alias    = 'o',
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &output,
        },
        {
            .fullname = "bravo",
            .alias    = 'b',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &b,
        },
        {0},
    };
    Argon argon = {
        .options = opts,
        .flags   = ARGON_COMBINED_MATCHING,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("-aob"));
    ASSERT(r == ARGON_ERR_INVALID_VALUE, "should reject non-bool in middle");
    PASS();
}

static void
test_combined_short_array_last(void)
{
    TEST("combined short with array last (-abv 10 20)");
    bool        a = false, b = false;
    int         vals[2] = {0};
    ArgonOption opts[]  = {
        {
            .fullname = "alpha",
            .alias    = 'a',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &a,
        },
        {
            .fullname = "bravo",
            .alias    = 'b',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &b,
        },
        {
            .fullname     = "vec",
            .alias        = 'v',
            .type         = ARGON_OPTYPE_INT,
            .target       = &vals,
            .array_plugin = {.max_len = 2, .force_filling = true},
        },
        {0},
    };
    Argon argon = {
        .options = opts,
        .flags   = ARGON_COMBINED_MATCHING,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("-abv", "10", "20"));
    ASSERT(r == ARGON_OK, "combined short with array failed");
    ASSERT(a && b, "bool flags not set");
    ASSERT(vals[0] == 10 && vals[1] == 20, "array values wrong");
    PASS();
}

static void
test_combined_short_value_int(void)
{
    TEST("combined short with single int last (-abc 42)");
    bool        a = false, b = false;
    int         c      = 0;
    ArgonOption opts[] = {
        {
            .fullname = "alpha",
            .alias    = 'a',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &a,
        },
        {
            .fullname = "bravo",
            .alias    = 'b',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &b,
        },
        {
            .fullname = "count",
            .alias    = 'c',
            .type     = ARGON_OPTYPE_INT,
            .target   = &c,
        },
        {0},
    };
    Argon argon = {
        .options = opts,
        .flags   = ARGON_COMBINED_MATCHING,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("-abc", "42"));
    ASSERT(r == ARGON_OK, "combined short with int value failed");
    ASSERT(a && b, "bool flags not set");
    ASSERT(c == 42, "int value wrong");
    PASS();
}

static void
test_combined_short_value_enum(void)
{
    TEST("combined short with enum last (-abf png)");
    bool        a = false, b = false;
    const char *f       = NULL;
    const char *enums[] = {"png", "jpg", "bmp", NULL};
    ArgonOption opts[]  = {
        {
            .fullname = "alpha",
            .alias    = 'a',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &a,
        },
        {
            .fullname = "bravo",
            .alias    = 'b',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &b,
        },
        {
            .fullname    = "format",
            .alias       = 'f',
            .type        = ARGON_OPTYPE_STRREF,
            .target      = (void *) &f,
            .enum_plugin = {.enums = enums, .case_sensitive = true},
        },
        {0},
    };
    Argon argon = {
        .options = opts,
        .flags   = ARGON_COMBINED_MATCHING,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("-abf", "png"));
    ASSERT(r == ARGON_OK, "combined short with enum value failed");
    ASSERT(a && b, "bool flags not set");
    ASSERT(f && strcmp(f, "png") == 0, "enum value wrong");
    PASS();
}

int
main(void)
{
    test_combined_short();
    test_combined_short_value();
    test_combined_short_bad_middle();
    test_combined_short_array_last();
    test_combined_short_value_int();
    test_combined_short_value_enum();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
