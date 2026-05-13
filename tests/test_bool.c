// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

static void
test_bool_parse(void)
{
    TEST("bool parse (toggle)");
    bool        val    = false;
    ArgonOption opts[] = {
        {
            .fullname = "flag",
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--flag"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == true, "bool toggle not true");
    PASS();
}

static void
test_bool_explicit(void)
{
    TEST("bool parse (explicit false)");
    bool        val    = true;
    ArgonOption opts[] = {
        {
            .fullname = "flag",
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--flag", "false"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == false, "bool not false");
    PASS();
}

static void
test_written_flag_bool_toggle(void)
{
    TEST("written flag (bool toggle)");
    bool        val    = false;
    ArgonOption opts[] = {
        {
            .fullname = "verbose",
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &val,
        },
        {0},
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--verbose"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == true, "toggle not set");
    ASSERT((opts[0].flags_ & ARGON_OPTFLAG_WRITTEN_) != 0,
           "written flag not set on toggle");
    PASS();
}

int
main(void)
{
    test_bool_parse();
    test_bool_explicit();
    test_written_flag_bool_toggle();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
