// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

#include <string.h>

static void
test_unknown_option(void)
{
    TEST("unknown option error");
    bool        val    = false;
    ArgonOption opts[] = {
        {
            .fullname = "known",
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &val,
        },
        ARGON_OPTION_SENTINEL,
    };
    Argon       argon = {.options = opts};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--unknown"));
    ASSERT(r == ARGON_ERR_UNKNOWN_OPTION, "should fail with UNKNOWN_OPTION");
    ASSERT(strlen(argon.errmsg) > 0, "errmsg should be populated");
    PASS();
}

static void
test_missing_value(void)
{
    TEST("missing value error");
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
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("--count"));
    ASSERT(r == ARGON_ERR_MISSING_VALUE, "should fail with MISSING_VALUE");
    ASSERT(strlen(argon.errmsg) > 0, "errmsg should be populated");
    PASS();
}

static void
test_invalid_value(void)
{
    TEST("invalid value error");
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
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("--count", "notanumber"));
    ASSERT(r == ARGON_ERR_INVALID_VALUE, "should fail with INVALID_VALUE");
    PASS();
}

int
main(void)
{
    test_unknown_option();
    test_missing_value();
    test_invalid_value();
    (void) fprintf(stderr, "  %d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
