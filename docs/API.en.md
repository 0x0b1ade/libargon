# libargon API Reference

| [中文](API.zh.md) |
| ----------------- |

## Types

### `Argon`

Parser context holding all option definitions and parser state.

| Field     | Type                     | Required | Description                                                          |
| --------- | ------------------------ | :------: | -------------------------------------------------------------------- |
| `options` | `ArgonOption *`          | **yes**  | `ARGON_OPTION_SENTINEL`-terminated array of global options           |
| `subs`    | `ArgonSub *`             |    no    | `ARGON_SUB_SENTINEL`-terminated array of subcommands; `NULL` if none |
| `flags`   | `ArgonFlags`             |    no    | Bitmask of behavioural flags                                         |
| `errmsg`  | `char[ARGON_ERRMSG_LEN]` |   out    | Buffer populated with a diagnostic on error                          |

The `options` array holds the global options. Each scope — global and per-subcommand — may contain up to `ARGON_OPTION_MAX_COUNT` options; up to `ARGON_SUBCOMMAND_MAX_COUNT` subcommands may be defined. Entries beyond these limits are silently ignored. A single parser context thus supports at most `ARGON_OPTION_MAX_COUNT` $\times$ `ARGON_SUBCOMMAND_MAX_COUNT` options, which is sufficient for most use cases.

See also:

- [`ArgonOption`](#argonoption)
- [`ArgonFlags`](#argonflags)
- [`ARGON_ERRMSG_LEN`](#constants)
- [`ARGON_OPTION_MAX_COUNT`](#constants)
- [`ARGON_SUBCOMMAND_MAX_COUNT`](#constants)

---

### `ArgonOption`

Describes one command-line option. If both `fullname` and `alias` are zero, the option is treated as a **positional argument**.

| Field          | Type               | Required | Description                                                 |
| -------------- | ------------------ | :------: | ----------------------------------------------------------- |
| `fullname`     | `const char *`     |    no    | Long name (e.g. `"output"` for `--output`); `NULL` for none |
| `alias`        | `char`             |    no    | Short name (e.g. `'o'` for `-o`); `'\0'` for none           |
| `type`         | `ArgonOptionType`  | **yes**  | Expected value type                                         |
| `desc`         | `const char *`     |    no    | Human-readable description for help output                  |
| `target`       | `void *`           | **yes**  | Pointer to user variable receiving the parsed value         |
| `enum_plugin`  | `ArgonEnumPlugin`  |    no    | Enum validation (valid for `STRREF` types)                  |
| `array_plugin` | `ArgonArrayPlugin` |    no    | Multi-value collection (set `max_len` > 0)                  |

The option array must be `ARGON_OPTION_SENTINEL`-terminated as a sentinel.

See also:

- [`ArgonEnumPlugin`](#argonenumplugin)
- [`ArgonArrayPlugin`](#argonarrayplugin)
- [`ArgonOptionType`](#argonoptiontype)

---

### `ArgonSub`

Describes one subcommand.

| Field     | Type            | Description                                                                                    |
| --------- | --------------- | ---------------------------------------------------------------------------------------------- |
| `name`    | `const char *`  | Subcommand name (e.g. `"build"`)                                                               |
| `alias`   | `char`          | Short alias (e.g. `'b'`); `'\0'` for none                                                      |
| `desc`    | `const char *`  | Human-readable description for help output                                                     |
| `options` | `ArgonOption *` | `ARGON_OPTION_SENTINEL`-terminated option array; may point to the same array as global options |

The subcommand array must be `ARGON_SUB_SENTINEL`-terminated as a sentinel.

See also:

- [`ArgonOption`](#argonoption)

---

### `ArgonResult`

Operation result code.

| Constant                   | Description                                  |
| -------------------------- | -------------------------------------------- |
| `ARGON_OK`                 | Success                                      |
| `ARGON_ERR_UNKNOWN_OPTION` | Unrecognized option or unexpected positional |
| `ARGON_ERR_MISSING_VALUE`  | Option requires a value but none provided    |
| `ARGON_ERR_INVALID_VALUE`  | Value cannot be parsed for the option type   |
| `ARGON_ERR_UNKNOWN_SUB`    | Unrecognized subcommand name                 |

---

### `ArgonOptionType`

Value type assigned to an option.

| Constant                  | Target type    | Description                                                           |
| ------------------------- | -------------- | --------------------------------------------------------------------- |
| `ARGON_OPTYPE_BOOL`       | `bool`         | Accepts `true`/`false`/`0`/`1`                                        |
| `ARGON_OPTYPE_INT`        | `int`          | Decimal integer                                                       |
| `ARGON_OPTYPE_FLOAT`      | `double`       | Decimal floating-point                                                |
| `ARGON_OPTYPE_STRREF`     | `const char *` | Pointer into `argv` or matching enum string; supports enum validation |
| `ARGON_OPTYPE_STRREF_MUT` | `char *`       | Mutable pointer into `argv`; enum validation not supported            |

Writing a `BOOL` option name without a value is equivalent to providing `TRUE`. `--overwrite` and `--overwrite TRUE` are interchangeable.

Enums are typically string constants; since the matched enum pointer is needed for efficient address comparison, only immutable string types support enum validation.

---

### `ArgonFlags`

Bitmask of behavioural flags for the parser context (`Argon.flags`).

| Constant                    | Description                                                                           |
| --------------------------- | ------------------------------------------------------------------------------------- |
| `ARGON_COMBINED_MATCHING`   | Enable combined short options: `-abc` = `-a -b -c` (only the last may be non-boolean) |
| `ARGON_CASE_INSENSITIVE`    | Case-insensitive matching for option names and subcommand names                       |
| `ARGON_ALLOW_SPECIAL_FLOAT` | Allow `inf`/`nan` as valid float values                                               |

---

### `ArgonEnumPlugin`

Constrains an `ARGON_OPTYPE_STRREF` option to a predefined set of values. The set must be `NULL`-terminated.

| Field            | Type            | Description                                 |
| ---------------- | --------------- | ------------------------------------------- |
| `enums`          | `const char **` | Enum set (capped at `ARGON_ENUM_MAX_COUNT`) |
| `case_sensitive` | `bool`          | If `false`, matching ignores case           |

```c
const char *POSSIBLE_USER_ENUMS[] = {"png", "jpg", "bmp", NULL};
```

When a match is found, the returned pointer points into the `enums` array (not `argv`), so the matched enum value can be determined by `==` identity comparison.

See also:

- [`ARGON_ENUM_MAX_COUNT`](#constants)

---

### `ArgonArrayPlugin`

Enables multi-value collection for an option.

| Field           | Type      | Description                                                |
| --------------- | --------- | ---------------------------------------------------------- |
| `max_len`       | `uint8_t` | Maximum number of values (capped at `ARGON_ARRAY_MAX_LEN`) |
| `force_filling` | `bool`    | If `true`, require exactly `max_len` values or fail        |

See also:

- [`ARGON_ARRAY_MAX_LEN`](#constants)

## Interface

### `argon_parse`

```c
ArgonResult argon_parse(Argon *ctx, char *const argv[]);
```

Walk `argv[1..]` until the `NULL` sentinel.

1. Global options from `ctx->options` are matched first.
2. If `ctx->subs` is non-`NULL` and a subcommand name is matched, subsequent arguments are matched against that subcommand's option set only.
3. Parsed values are written into the `target` pointers of matching `ArgonOption` definitions.

On failure, `ctx->errmsg` receives a diagnostic.

**Returns** `ARGON_OK` on success, or an error code.

---

### `ARGON_ARGV(...)`

Constructs a `NULL`-terminated `char *[]` on the stack from variadic arguments. `argv[0]` is automatically set to `""` (ignored by the parser). Controlled by `ARGON_PEDANTIC`.

```c
char *argv[] = ARGON_ARGV("--format", "jpg");
argon_parse(&ctx, ARGON_ARGV("--verbose", "-o", "out.txt"));
```

---

### `argon_print_table`

```c
ArgonResult argon_print_table(Argon *ctx, FILE *ostream);
```

Write a formatted option table listing all named global options to `ostream`. Positional options are omitted. If `ctx->subs` is set, lists every subcommand together with its full option table.

---

### `argon_print_sub_table`

```c
ArgonResult argon_print_sub_table(const ArgonSub *ctx, FILE *ostream);
```

Write a formatted option table for the options of a specific subcommand.

---

### `argon_active_sub`

```c
const ArgonSub *argon_active_sub(const Argon *ctx);
```

Return the subcommand matched during parsing, or `NULL` if none was matched.

---

### `argon_option_is_parsed`

```c
bool argon_option_is_parsed(const ArgonOption *opt);
```

Return `true` if the option's target was written to during parsing (i.e., the user supplied this option on the command line). Since `target` variables often have initial values, this method accurately determines whether a given option was actually parsed.

---

## Constants

| Name                         | Configurable | Description                                                                                                 |
| ---------------------------- | :----------: | ----------------------------------------------------------------------------------------------------------- |
| `ARGON_IMPLS`                |      ✓       | Define to include the library implementation; exactly one source file per translation unit should define it |
| `ARGON_PEDANTIC`             |      ✓       | Disables all non-strictly-conforming C features                                                             |
| `ARGON_VERSION_MAJOR`        |      —       | Major version number                                                                                        |
| `ARGON_VERSION_MINOR`        |      —       | Minor version number                                                                                        |
| `ARGON_VERSION_PATCH`        |      —       | Patch version number                                                                                        |
| `ARGON_ERRMSG_LEN`           |      ✓       | Error message buffer length                                                                                 |
| `ARGON_OPTNAME_MAX_LEN`      |      ✓       | Maximum long option name length                                                                             |
| `ARGON_ENUM_MAX_LEN`         |      ✓       | Maximum enum string length                                                                                  |
| `ARGON_ENUM_MAX_COUNT`       |      —       | Maximum number of enum values per option                                                                    |
| `ARGON_ARRAY_MAX_LEN`        |      —       | Maximum number of array values per option                                                                   |
| `ARGON_OPTION_MAX_COUNT`     |      —       | Maximum number of options per option set                                                                    |
| `ARGON_SUBCOMMAND_MAX_COUNT` |      —       | Maximum number of subcommands                                                                               |
