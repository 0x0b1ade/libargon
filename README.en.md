# libargon

Single-header C11 command-line argument parsing library / C++ compatible / Lightweight & Modern.

| [中文](README.md) |
| ----------------- |

## Features

- **Single-header distribution** — drop one header into your project
- **Stack-only operation** — no heap allocation, no VLA
- **Case-insensitive matching** — both long and short option names support case-insensitive mode
- **Combined matching** — short options may be combined (e.g. `-abc [arg]` expands to `-a -b -c [arg]`)
- **Multi-value options** — support for fixed-length or variable-length value arrays (e.g. `--size 40 30`)
- **Enum validation** — constrain string options to a whitelist
- **Positional arguments** — bind unnamed arguments to positional options
- **Subcommand support** — isolate global and subcommand option parsing
- **Help table generation** — formatted output of option tables with type annotations, enum values, and array dimensions
- **C++ compatible** — works in C++ projects without modification

## Quick Start

See the [example program](example.c).

## Building

Requires a C11 compiler. No separate compilation is needed — include the header directly in your project. To build a static or shared library, the repository provides [Xmake](https://xmake.io/) support via [xmake.lua](xmake.lua):

```sh
xmake f -m release
xmake build argon
xmake build argon_shared
```

## Concepts

A standard C11 `argv` created by the host environment looks like this:

```c
const char *POSSIBLE_USER_INPUT = "[exe_path] build target.exe --language c11 ...";
// ↓↓↓
char *argv[] = {"[exe_path]", "build", "target.exe", "--language", "c11", ..., NULL};
```

`Argon` expects all `argv` arrays to follow this convention — it ignores `argv[0]` (but checks for a null pointer) and assumes `argv[argc] == NULL`. Custom argument arrays must adhere to the same rule. Use the `ARGON_ARGV` macro to construct valid argument arrays programmatically (see [API Reference](#api-reference)).

In this scenario, `build` is treated as a subcommand name, `target.exe` as an anonymous positional argument value, `--language` as a long-form option name, and `c11` as the parameter value for the `language` option. `Argon` abstracts these concepts.

## API Reference

See the [API documentation](docs/API.en.md).

## License

MIT — see [LICENSE](LICENSE)
