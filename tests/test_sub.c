// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"
#include "argon_test.h"

#include <string.h>

static void
test_sub_global_and_sub(void)
{
    TEST("global option before subcommand");
    bool debug = false;
    bool force = false;
    int  jobs  = 1;

    ArgonOption build_opts[] = {
        {
            .fullname = "release",
            .alias    = 'r',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &force,
        },
        {
            .fullname = "jobs",
            .alias    = 'j',
            .type     = ARGON_OPTYPE_INT,
            .target   = &jobs,
        },
        {0},
    };

    ArgonOption global_opts[] = {
        {.fullname = "debug",
         .alias    = 'd',
         .type     = ARGON_OPTYPE_BOOL,
         .target   = &debug},
        {0},
    };

    ArgonSub subs[] = {
        {.name = "build", .alias = 'b', .options = build_opts},
        {0},
    };

    Argon argon = {
        .options = global_opts,
        .subs    = subs,
        .flags   = ARGON_COMBINED_MATCHING,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("-d", "build", "-rj", "4"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(debug, "global opt not parsed");
    ASSERT(force, "sub opt not parsed");
    ASSERT(jobs == 4, "sub opt value wrong");
    ASSERT(argon_active_sub(&argon) == &subs[0], "wrong subcommand");
    PASS();
}

static void
test_sub_alias_match(void)
{
    TEST("subcommand alias match");
    bool        force        = false;
    ArgonOption build_opts[] = {
        {
            .fullname = "release",
            .alias    = 'r',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &force,
        },
        {0},
    };
    ArgonOption global_opts[] = {
        {
            .fullname = "debug",
            .alias    = 'd',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = NULL,
        },
        {0},
    };
    ArgonSub subs[] = {{.name = "build", .alias = 'b', .options = build_opts},
                       {0}};

    Argon       argon = {.options = global_opts, .subs = subs};
    ArgonResult r     = argon_parse(&argon, ARGON_ARGV("b", "-r"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(force, "sub opt not parsed");
    ASSERT(argon_active_sub(&argon) == &subs[0], "wrong subcommand");
    PASS();
}

static void
test_sub_unknown(void)
{
    TEST("unknown subcommand error");
    ArgonOption global_opts[] = {
        {
            .fullname = "debug",
            .alias    = 'd',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = NULL,
        },
        {0},
    };
    ArgonSub    subs[] = {{.name = "build", .options = &(ArgonOption){0}}, {0}};
    Argon       argon  = {.options = global_opts, .subs = subs};
    ArgonResult r      = argon_parse(&argon, ARGON_ARGV("bad"));
    ASSERT(r == ARGON_ERR_UNKNOWN_SUB, "should be unknown subcommand");
    PASS();
}

static void
test_sub_case_insensitive(void)
{
    TEST("subcommand case insensitive");
    ArgonOption global_opts[] = {
        {
            .fullname = "debug",
            .alias    = 'd',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = NULL,
        },
        {0},
    };
    ArgonSub subs[] = {{.name = "build", .options = &(ArgonOption){0}}, {0}};
    Argon    argon  = {
        .options = global_opts,
        .subs    = subs,
        .flags   = ARGON_CASE_INSENSITIVE,
    };
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("BUILD"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(argon_active_sub(&argon) == &subs[0], "wrong subcommand");
    PASS();
}

static void
test_sub_global_after_sub_not_recognized(void)
{
    TEST("global option after subcommand not recognized");
    bool        debug        = false;
    ArgonOption build_opts[] = {
        {
            .fullname = "release",
            .alias    = 'r',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = NULL,
        },
        {0},
    };
    ArgonOption global_opts[] = {
        {.fullname = "debug",
         .alias    = 'd',
         .type     = ARGON_OPTYPE_BOOL,
         .target   = &debug},
        {0},
    };
    ArgonSub    subs[] = {{.name = "build", .options = build_opts}, {0}};
    Argon       argon  = {.options = global_opts, .subs = subs};
    ArgonResult r      = argon_parse(&argon, ARGON_ARGV("build", "-d"));
    ASSERT(r == ARGON_ERR_UNKNOWN_OPTION, "should reject -d after subcommand");
    ASSERT(!debug, "global opt should not be set");
    PASS();
}

static void
test_sub_positional_args(void)
{
    TEST("subcommand positional arguments");
    const char *input  = NULL;
    const char *output = NULL;

    ArgonOption build_opts[] = {
        {
            .fullname = "release",
            .alias    = 'r',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &(bool){0},
        },
        {.type = ARGON_OPTYPE_STRREF, .target = (void *) &input},
        {.type = ARGON_OPTYPE_STRREF, .target = (void *) &output},
        {0},
    };
    ArgonOption global_opts[] = {
        {
            .fullname = "debug",
            .alias    = 'd',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &(bool){0},
        },
        {0},
    };
    ArgonSub    subs[] = {{.name = "build", .options = build_opts}, {0}};
    Argon       argon  = {.options = global_opts, .subs = subs};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("build", "in.txt", "out.txt"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(strcmp(input, "in.txt") == 0, "first positional wrong");
    ASSERT(strcmp(output, "out.txt") == 0, "second positional wrong");
    PASS();
}

static void
test_sub_excess_positional(void)
{
    TEST("excess positional in subcommand");
    ArgonOption build_opts[] = {
        {.type = ARGON_OPTYPE_STRREF, .target = (void *) &(const char *){NULL}},
        {0},
    };
    ArgonOption global_opts[] = {
        {
            .fullname = "debug",
            .alias    = 'd',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &(bool){0},
        },
        {0},
    };
    ArgonSub    subs[] = {{.name = "build", .options = build_opts}, {0}};
    Argon       argon  = {.options = global_opts, .subs = subs};
    ArgonResult r = argon_parse(&argon, ARGON_ARGV("build", "a.txt", "b.txt"));
    ASSERT(r == ARGON_ERR_UNKNOWN_OPTION, "should error on excess positional");
    PASS();
}

static void
test_sub_double_dash(void)
{
    TEST("-- inside subcommand");
    const char *input = NULL;
    const char *opt   = NULL;

    ArgonOption build_opts[] = {
        {
            .fullname = "opt",
            .alias    = 'o',
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &opt,
        },
        {.type = ARGON_OPTYPE_STRREF, .target = (void *) &input},
        {0},
    };
    ArgonOption global_opts[] = {
        {
            .fullname = "debug",
            .alias    = 'd',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &(bool){0},
        },
        {0},
    };
    ArgonSub    subs[] = {{.name = "build", .options = build_opts}, {0}};
    Argon       argon  = {.options = global_opts, .subs = subs};
    ArgonResult r =
        argon_parse(&argon, ARGON_ARGV("build", "-o", "x", "--", "input.txt"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(opt != NULL && strcmp(opt, "x") == 0, "-o not parsed");
    ASSERT(strcmp(input, "input.txt") == 0, "positional after -- wrong");
    PASS();
}

static void
test_sub_value_not_misinterpreted(void)
{
    TEST("option value not mistaken for subcommand");
    const char *target       = NULL;
    ArgonOption value_opts[] = {
        {
            .fullname = "target",
            .alias    = 't',
            .type     = ARGON_OPTYPE_STRREF,
            .target   = (void *) &target,
        },
        {0},
    };
    ArgonSub    subs[] = {{.name = "build", .options = &(ArgonOption){0}}, {0}};
    Argon       argon  = {.options = value_opts, .subs = subs};
    ArgonResult r      = argon_parse(&argon, ARGON_ARGV("-t", "build"));
    ASSERT(r == ARGON_OK, "parse failed");
    ASSERT(strcmp(target, "build") == 0,
           "build should be value, not subcommand");
    ASSERT(!argon_active_sub(&argon), "no subcommand should be active");
    PASS();
}

static void
test_sub_print_table(void)
{
    TEST("print_table with subcommands (no crash)");
    ArgonOption global_opts[] = {
        {
            .fullname = "debug",
            .alias    = 'd',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &(bool){0},
        },
        {0},
    };
    ArgonOption build_opts[] = {
        {
            .fullname = "release",
            .alias    = 'r',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &(bool){0},
        },
        {0},
    };
    ArgonSub subs[] = {
        {.name = "build", .alias = 'b', .desc = "Build", .options = build_opts},
        {0},
    };
    Argon argon = {.options = global_opts, .subs = subs};
    argon_print_table(&argon, stdout);
    PASS();
}

int
main(void)
{
    test_sub_global_and_sub();
    test_sub_alias_match();
    test_sub_unknown();
    test_sub_case_insensitive();
    test_sub_global_after_sub_not_recognized();
    test_sub_positional_args();
    test_sub_excess_positional();
    test_sub_double_dash();
    test_sub_value_not_misinterpreted();
    test_sub_print_table();

    (void) fprintf(stderr, "\n%d/%d passed\n", tests_pass, tests_run);
    return tests_fail > 0 ? 1 : 0;
}
