// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

static void
test_array_type_mismatch_to_positional(void)
{
    TEST("array type-mismatch -> positional");
    int         arr[3] = {0};
    double      pos    = 0;
    ArgonOption opts[] = {
        {
            .fullname     = "ints",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 3},
        },
        {
            .fullname = NULL,
            .alias    = 0,
            .type     = ARGON_OPTYPE_FLOAT,
            .target   = &pos,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("--ints", "10", "20", "3.4"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(arr[0] == 10 && arr[1] == 20 && arr[2] == 0, "array values wrong");
    ASSERT(pos == 3.4, "positional should get 3.4");
    PASS();
}

static void
test_array_force_type_mismatch_error(void)
{
    TEST("array force-fill type-mismatch error");
    int         arr[3] = {0};
    ArgonOption opts[] = {
        {
            .fullname     = "ints",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 3, .force_filling = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--ints", "10", "3.4"));
    ASSERT(r == ARGON_ERR_INVALID_VALUE, "should fail with INVALID_VALUE");
    PASS();
}

static void
test_array_overflow_to_positional(void)
{
    TEST("array overflow -> positional");
    int         arr[2] = {0};
    int         pos    = 0;
    ArgonOption opts[] = {
        {
            .fullname     = "coords",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 2, .force_filling = true},
        },
        {
            .fullname = NULL,
            .alias    = 0,
            .type     = ARGON_OPTYPE_INT,
            .target   = &pos,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("--coords", "10", "20", "30"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(arr[0] == 10 && arr[1] == 20, "array values wrong");
    ASSERT(pos == 30, "positional should get overflow");
    PASS();
}

static void
test_array_both_incompatible(void)
{
    TEST("array + positional both incompatible");
    int         arr[3] = {0};
    int         pos    = 0;
    ArgonOption opts[] = {
        {
            .fullname     = "ints",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 3, .force_filling = false},
        },
        {
            .fullname = NULL,
            .alias    = 0,
            .type     = ARGON_OPTYPE_INT,
            .target   = &pos,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("--ints", "10", "hello"));
    ASSERT(r == ARGON_ERR_INVALID_VALUE, "should fail");
    PASS();
}

int
main(void)
{
    test_array_type_mismatch_to_positional();
    test_array_force_type_mismatch_error();
    test_array_overflow_to_positional();
    test_array_both_incompatible();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
