// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

#include <string.h>

static void
test_int_array(void)
{
    TEST("int array parse");
    int         arr[3] = {0};
    ArgonOption opts[] = {
        {
            .fullname     = "coords",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 3, .force_filling = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("--coords", "10", "20", "30"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(arr[0] == 10 && arr[1] == 20 && arr[2] == 30, "array values wrong");
    PASS();
}

static void
test_float_array(void)
{
    TEST("float array parse");
    double      arr[2] = {0};
    ArgonOption opts[] = {
        {
            .fullname     = "scales",
            .type         = ARGON_OPTYPE_FLOAT,
            .target       = &arr,
            .array_plugin = {.max_len = 2, .force_filling = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("--scales", "0.5", "1.5"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(arr[0] == 0.5 && arr[1] == 1.5, "float array values wrong");
    PASS();
}

static void
test_bool_array(void)
{
    TEST("bool array parse");
    bool        arr[3] = {false};
    ArgonOption opts[] = {
        {
            .fullname     = "flags",
            .type         = ARGON_OPTYPE_BOOL,
            .target       = &arr,
            .array_plugin = {.max_len = 3, .force_filling = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("--flags", "true", "false", "1"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(arr[0] == true && arr[1] == false && arr[2] == true,
           "bool array values wrong");
    PASS();
}

static void
test_string_array(void)
{
    TEST("string array parse");
    const char *arr[2] = {NULL};
    ArgonOption opts[] = {
        {
            .fullname     = "inputs",
            .type         = ARGON_OPTYPE_STRREF,
            .target       = &arr,
            .array_plugin = {.max_len = 2, .force_filling = true},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("--inputs", "a.txt", "b.txt"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(strcmp(arr[0], "a.txt") == 0 && strcmp(arr[1], "b.txt") == 0,
           "string array values wrong");
    PASS();
}

static void
test_array_optional(void)
{
    TEST("array optional (fewer values)");
    int         arr[3] = {-1, -1, -1};
    ArgonOption opts[] = {
        {
            .fullname     = "coords",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 3, .force_filling = false},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--coords", "10", "20"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(arr[0] == 10 && arr[1] == 20 && arr[2] == -1,
           "optional array values wrong");
    PASS();
}

static void
test_array_empty_no_flag(void)
{
    TEST("array no values, no flag set");
    int         arr[2] = {-1, -1};
    ArgonOption opts[] = {
        {
            .fullname     = "coords",
            .type         = ARGON_OPTYPE_INT,
            .target       = &arr,
            .array_plugin = {.max_len = 2, .force_filling = false},
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--coords"));
    ASSERT(r == ARGON_OK, "should succeed with zero values");
    ASSERT(arr[0] == -1, "arr[0] should not be written");
    ASSERT(arr[1] == -1, "arr[1] should not be written");
    ASSERT((opts[0].flags_ & ARGON_OPTFLAG_WRITTEN_) == 0,
           "written flag should NOT be set");
    PASS();
}

int
main(void)
{
    test_int_array();
    test_float_array();
    test_bool_array();
    test_string_array();
    test_array_optional();
    test_array_empty_no_flag();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
