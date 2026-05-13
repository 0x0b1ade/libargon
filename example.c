// SPDX-License-Identifier: MIT
// Copyright (c) 2026 锯条

#define ARGON_IMPLS
#include "argon.h"

#include <stdio.h>
#include <string.h>

#define ARRAYLEN(a) (sizeof(a) / sizeof(*(a)))

int
main(int argc, char *argv[])
{
    // Global options
    (void) argc;
    bool help = false;

    // Reset subcommand options
    const char *resize_input  = NULL;
    const char *resize_output = NULL;
    int         width         = 0;
    int         height        = 0;

    ArgonOption resize_opts[] = {
        {
            .fullname = "width",
            .alias    = 'w',
            .type     = ARGON_OPTYPE_INT,
            .target   = &width,
            .desc     = "Target width",
        },
        {
            .fullname = "height",
            .alias    = 'h',
            .type     = ARGON_OPTYPE_INT,
            .target   = &height,
            .desc     = "Target height",
        },
        {
            .type   = ARGON_OPTYPE_STRREF,
            .target = (void *) &resize_input,
            .desc   = "input file",
        },
        {
            .type   = ARGON_OPTYPE_STRREF,
            .target = (void *) &resize_output,
            .desc   = "output file",
        },
        {0},
    };

    // Convert subcommand options
    const char *format      = NULL;
    int         quality     = 90;
    double      scale       = 1.0;
    bool        overwrite   = false;
    const char *conv_input  = NULL;
    const char *conv_output = NULL;

    const char *fmt_enums[] = {"png", "jpg", "bmp", "webp", NULL};

    ArgonOption convert_opts[] = {
        {
            .fullname    = "format",
            .alias       = 'f',
            .type        = ARGON_OPTYPE_STRREF,
            .target      = (void *) &format,
            .desc        = "Output format",
            .enum_plugin = {.enums = fmt_enums, .case_sensitive = false},
        },
        {
            .fullname = "quality",
            .alias    = 'q',
            .type     = ARGON_OPTYPE_INT,
            .target   = &quality,
            .desc     = "Quality 1-100 (default: 90)",
        },
        {
            .fullname = "scale",
            .alias    = 's',
            .type     = ARGON_OPTYPE_FLOAT,
            .target   = &scale,
            .desc     = "Scale factor (default: 1.0)",
        },
        {
            .fullname = "overwrite",
            .alias    = 0,
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &overwrite,
            .desc     = "Overwrite existing files",
        },
        {
            .type   = ARGON_OPTYPE_STRREF,
            .target = (void *) &conv_input,
            .desc   = "input file",
        },
        {
            .type   = ARGON_OPTYPE_STRREF,
            .target = (void *) &conv_output,
            .desc   = "output file",
        },
        {0},
    };

    // Info subcommand options
    const char *info_input = NULL;
    bool        metadata   = false;

    ArgonOption info_opts[] = {
        {
            .fullname = "metadata",
            .alias    = 'm',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &metadata,
            .desc     = "Show only metadata",
        },
        {
            .type   = ARGON_OPTYPE_STRREF,
            .target = (void *) &info_input,
            .desc   = "input file",
        },
        {0},
    };

    // Global option array
    ArgonOption global_opts[] = {
        {
            .fullname = "help",
            .alias    = 'h',
            .type     = ARGON_OPTYPE_BOOL,
            .target   = &help,
            .desc     = "Show help",
        },
        {0},
    };

    static const char SUB_CONVERT[] = "convert", SUB_RESIZE[] = "resize",
                      SUB_INFO[] = "info";

    ArgonSub subcommands[] = {
        {
            .name    = SUB_CONVERT,
            .alias   = 'c',
            .desc    = "Convert image format",
            .options = convert_opts,
        },
        {
            .name    = SUB_RESIZE,
            .alias   = 'r',
            .desc    = "Resize image dimensions",
            .options = resize_opts,
        },
        {
            .name    = SUB_INFO,
            .alias   = 'i',
            .desc    = "Show image information",
            .options = info_opts,
        },
        {0},
    };

    Argon argon = {
        .options = global_opts,
        .subs    = subcommands,
        .flags   = ARGON_COMBINED_MATCHING | ARGON_CASE_INSENSITIVE,
    };

    ArgonResult r = argon_parse(&argon, argv);
    if (r != ARGON_OK)
    {
        (void) fprintf(stderr, "Error: %s\n", argon.errmsg);
        (void) fprintf(stderr, "Try 'imgtoolhelp' for usage.\n");
        return 1;
    }

    const ArgonSub *cmd = argon_active_sub(&argon);

    if (help || !cmd)
    {
        printf("Usage: imgtool [OPTIONS] <subcommand> [ARGS...]\n\n");
        argon_print_table(&argon, stdout);
        return 0;
    }

    // Subcommand dispatch
    if (strncmp(cmd->name, SUB_CONVERT, ARRAYLEN(SUB_CONVERT) - 1) == 0)
    {
        printf("=== Convert ===\n");
        printf("  input:     %s\n", conv_input ? conv_input : "(not set)");
        printf("  output:    %s\n", conv_output ? conv_output : "(not set)");
        printf("  format:    %s\n", format ? format : "(default)");
        printf("  quality:   %d\n", quality);
        printf("  scale:     %.2f\n", scale);
        printf("  overwrite: %s\n", overwrite ? "yes" : "no");
    }
    else if (strncmp(cmd->name, SUB_RESIZE, ARRAYLEN(SUB_RESIZE)) == 0)
    {
        printf("=== Resize ===\n");
        printf("  input:  %s\n", resize_input ? resize_input : "(not set)");
        printf("  output: %s\n", resize_output ? resize_output : "(not set)");
        printf("  size:   %dx%d\n", width, height);
    }
    else if (strncmp(cmd->name, SUB_INFO, ARRAYLEN(SUB_INFO)) == 0)
    {
        printf("=== Info ===\n");
        printf("  input:    %s\n", info_input ? info_input : "(not set)");
        printf("  metadata: %s\n", metadata ? "yes" : "no");
    }

    return 0;
}
