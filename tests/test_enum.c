// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

#include <string.h>

static void
test_enum_pass(void)
{
    TEST("enum validation (pass)");
    const char *val     = NULL;
    const char *enums[] = {"png", "jpg", "bmp", NULL};
    ArgonOption opts[]  = {
        {
            .fullname    = "format",
            .type        = ARGON_OPTYPE_STRREF,
            .target      = (void *) &val,
            .enum_plugin = {.enums = enums, .case_sensitive = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--format", "png"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val && strcmp(val, "png") == 0, "enum value wrong");
    PASS();
}

static void
test_enum_fail(void)
{
    TEST("enum validation (fail)");
    const char *val     = NULL;
    const char *enums[] = {"png", "jpg", "bmp", NULL};
    ArgonOption opts[]  = {
        {
            .fullname    = "format",
            .type        = ARGON_OPTYPE_STRREF,
            .target      = (void *) &val,
            .enum_plugin = {.enums = enums, .case_sensitive = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--format", "gif"));
    ASSERT(r == ARGON_ERR_INVALID_VALUE, "should fail with INVALID_VALUE");
    PASS();
}

static void
test_enum_case_insensitive(void)
{
    TEST("enum validation (case insensitive)");
    const char *val     = NULL;
    const char *enums[] = {"png", "jpg", "bmp", NULL};
    ArgonOption opts[]  = {
        {
            .fullname    = "format",
            .type        = ARGON_OPTYPE_STRREF,
            .target      = (void *) &val,
            .enum_plugin = {.enums = enums, .case_sensitive = false},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--format", "PNG"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val && strcmp(val, "png") == 0, "should match 'png'");
    PASS();
}

static void
test_enum_array_pass(void)
{
    TEST("enum array (pass)");
    const char *vals[3] = {NULL};
    const char *enums[] = {"png", "jpg", "bmp", NULL};
    ArgonOption opts[]  = {
        {
            .fullname     = "formats",
            .type         = ARGON_OPTYPE_STRREF,
            .target       = (void *) &vals,
            .enum_plugin  = {.enums = enums, .case_sensitive = true},
            .array_plugin = {.max_len = 3, .force_filling = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("--formats", "png", "jpg", "bmp"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(strcmp(vals[0], "png") == 0, "vals[0] wrong");
    ASSERT(strcmp(vals[1], "jpg") == 0, "vals[1] wrong");
    ASSERT(strcmp(vals[2], "bmp") == 0, "vals[2] wrong");
    PASS();
}

static void
test_enum_array_fail(void)
{
    TEST("enum array (fail)");
    const char *vals[3] = {NULL};
    const char *enums[] = {"png", "jpg", "bmp", NULL};
    ArgonOption opts[]  = {
        {
            .fullname     = "formats",
            .type         = ARGON_OPTYPE_STRREF,
            .target       = (void *) &vals,
            .enum_plugin  = {.enums = enums, .case_sensitive = true},
            .array_plugin = {.max_len = 3, .force_filling = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("--formats", "png", "gif", "bmp"));
    ASSERT(r == ARGON_ERR_INVALID_VALUE,
           "should fail with INVALID_VALUE on 'gif'");
    PASS();
}

int
main(void)
{
    test_enum_pass();
    test_enum_fail();
    test_enum_case_insensitive();
    test_enum_array_pass();
    test_enum_array_fail();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
