// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

static void
test_optional_provided(void)
{
    TEST("optional flag (provided)");
    int         val    = 0;
    ArgonOption opts[] = {
        {
            .fullname = "count",
            .type     = ARGON_OPTYPE_INT,
            .target   = &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--count", "42"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == 42, "value not written");
    ASSERT((opts[0].flags_ & ARGON_OPTFLAG_WRITTEN_) != 0,
           "written flag not set");
    PASS();
}

static void
test_optional_not_provided(void)
{
    TEST("optional flag (not provided)");
    int         val    = 99;
    ArgonOption opts[] = {
        {
            .fullname = "count",
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
test_written_flag_array(void)
{
    TEST("written flag (array)");
    int         arr[2] = {0};
    ArgonOption opts[] = {
        {
            .fullname     = "coords",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 2, .force_filling = true},
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--coords", "10", "20"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(arr[0] == 10 && arr[1] == 20, "array values wrong");
    ASSERT((opts[0].flags_ & ARGON_OPTFLAG_WRITTEN_) != 0,
           "written flag not set on array");
    PASS();
}

static void
test_is_parsed(void)
{
    TEST("argon_option_nonnull");
    int         val    = 0;
    ArgonOption opts[] = {
        {
            .fullname = "count",
            .type     = ARGON_OPTYPE_INT,
            .target   = &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--count", "42"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(argon_option_is_parsed(&opts[0]),
           "should return true after parsing");
    PASS();
}

int
main(void)
{
    test_optional_provided();
    test_optional_not_provided();
    test_written_flag_array();
    test_is_parsed();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
