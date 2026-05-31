// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

static void
test_double_dash(void)
{
    TEST("double-dash terminator");
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
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--", "--count", "42"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(val == 0, "--count should not be parsed after --");
    PASS();
}

static void
test_print_table(void)
{
    TEST("print_table (no crash)");
    bool        val    = false;
    ArgonOption opts[] = {
        {
            .fullname = "verbose",
            .alias    = 'v',
            .type     = ARGON_OPTYPE_BOOL,
            .desc     = "Enable verbose output.",
            .target   = &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_print_table(&argon, stdout);
    ASSERT(r == ARGON_OK, "print_table should return OK");
    PASS();
}

int
main(void)
{
    test_double_dash();
    test_print_table();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
