# libargon API 参考

| [English](API.en.md) |
| -------------------- |

## 类型

### `Argon`

解析器上下文，持有所有选项定义和解析状态。

| 字段      | 类型                     |  必填  | 说明                                  |
| --------- | ------------------------ | :----: | ------------------------------------- |
| `options` | `ArgonOption *`          | **是** | `ARGON_OPTION_SENTINEL` 终止的全局选项数组              |
| `subs`    | `ArgonSub *`             |   否   | `ARGON_SUB_SENTINEL` 终止的子程序数组；无则为 `NULL` |
| `flags`   | `ArgonFlags`             |   否   | 行为标志位掩码                        |
| `errmsg`  | `char[ARGON_ERRMSG_LEN]` |  出参  | 发生错误时填入诊断信息                |

`options` 字段指向的选项数组被视为全局选项。全局选项和每个字程序内部的选项最多 `ARGON_OPTION_MAX_COUNT` 个；子程序最多 `ARGON_SUBCOMMAND_MAX_COUNT` 个。超出的部分会被直接无视。因此单个解析器上下文最多支持 `ARGON_OPTION_MAX_COUNT` $\times$ `ARGON_SUBCOMMAND_MAX_COUNT` 个选项，大多数情况下已足够使用。

另见：

- [`ArgonOption`](#argonoption)
- [`ArgonFlags`](#argonflags)
- [`ARGON_ERRMSG_LEN`](#常量)
- [`ARGON_OPTION_MAX_COUNT`](#常量)
- [`ARGON_SUBCOMMAND_MAX_COUNT`](#常量)

---

### `ArgonOption`

描述一个命令行选项。若 `fullname` 和 `alias` 同时为零值，则该选项被视为**位置参数**。

| 字段           | 类型               |  必填  | 说明                                                 |
| -------------- | ------------------ | :----: | ---------------------------------------------------- |
| `fullname`     | `const char *`     |   否   | 全写名（如 `"output"` 对应 `--output`）；无则 `NULL` |
| `alias`        | `char`             |   否   | 短写名（如 `'o'` 对应 `-o`）；无则 `'\0'`            |
| `type`         | `ArgonOptionType`  | **是** | 期望的值类型                                         |
| `desc`         | `const char *`     |   否   | 帮助输出中的人类可读描述                             |
| `target`       | `void *`           | **是** | 指向接收解析值的用户变量的指针                       |
| `enum_plugin`  | `ArgonEnumPlugin`  |   否   | 枚举校验（仅对 `STRREF` 类型有效）                   |
| `array_plugin` | `ArgonArrayPlugin` |   否   | 多值收集（设置 `max_len` > 0）                       |

选项数组必须以 `ARGON_OPTION_SENTINEL` 终止作为哨兵。

另见：

- [`ArgonEnumPlugin`](#argonenumplugin)
- [`ArgonArrayPlugin`](#argonarrayplugin)
- [`ArgonOptionType`](#argonoptiontype)

---

### `ArgonSub`

描述一个子程序。

| 字段      | 类型            | 说明                                                   |
| --------- | --------------- | ------------------------------------------------------ |
| `name`    | `const char *`  | 子程序名（如 `"build"`）                               |
| `alias`   | `char`          | 短别名（如 `'b'`）；无则 `'\0'`                        |
| `desc`    | `const char *`  | 帮助输出中的人类可读描述                               |
| `options` | `ArgonOption *` | `ARGON_OPTION_SENTINEL` 终止的选项数组；可直接指向全局选项数组以实现共享 |

子程序数组必须以 `ARGON_SUB_SENTINEL` 终止作为哨兵。

另见：

- [`ArgonOption`](#argonoption)

---

### `ArgonResult`

操作返回的结果码。

| 名称                       | 说明                         |
| -------------------------- | ---------------------------- |
| `ARGON_OK`                 | 成功                         |
| `ARGON_ERR_UNKNOWN_OPTION` | 无法识别的选项或意外位置参数 |
| `ARGON_ERR_MISSING_VALUE`  | 选项需要值但未提供           |
| `ARGON_ERR_INVALID_VALUE`  | 值无法按选项类型解析         |
| `ARGON_ERR_UNKNOWN_SUB`    | 无法识别的子程序名           |

---

### `ArgonOptionType`

选项的值类型。

| 名称                      | 目标类型       | 说明                                               |
| ------------------------- | -------------- | -------------------------------------------------- |
| `ARGON_OPTYPE_BOOL`       | `bool`         | 接受 `true`/`false`/`0`/`1`                        |
| `ARGON_OPTYPE_INT`        | `int`          | 十进制整数                                         |
| `ARGON_OPTYPE_FLOAT`      | `double`       | 十进制浮点数                                       |
| `ARGON_OPTYPE_STRREF`     | `const char *` | 指向 `argv` 或匹配的枚举字符串的指针；支持枚举校验 |
| `ARGON_OPTYPE_STRREF_MUT` | `char *`       | 指向 `argv` 的可变指针；不支持枚举校验             |

若仅书写 `ARGON_OPTYPE_BOOL` 类型选项的选项名，则自动视为填写了真值。`--overwrite` 和 `--overwrite TRUE` 是等价的。

枚举往往是字符串常量，考虑到需要返回匹配到的枚举指针以进行高效的地址比较，只有不可变的字符串类型选项支持枚举校验。

---

### `ArgonFlags`

解析器上下文 (`Argon.flags`) 的行为标志位掩码。

| 名称                        | 说明                                                                    |
| --------------------------- | ----------------------------------------------------------------------- |
| `ARGON_COMBINED_MATCHING`   | 启用组合短选项：`-abc` = `-a -b -c`（仅最后一个参数支持非布尔类型选项） |
| `ARGON_CASE_INSENSITIVE`    | 选项名、子程序名忽略大小写匹配                                          |
| `ARGON_ALLOW_SPECIAL_FLOAT` | 允许 `inf`/`nan` 等作为合法的浮点值                                     |

---

### `ArgonEnumPlugin`

将 `ARGON_OPTYPE_STRREF` 选项的值约束到预定义的集合。该集合需要以 `NULL` 结尾。

| 字段             | 类型            | 说明                                      |
| ---------------- | --------------- | ----------------------------------------- |
| `enums`          | `const char **` | 枚举集合（上限为 `ARGON_ENUM_MAX_COUNT`） |
| `case_sensitive` | `bool`          | 若为 `false`，匹配时忽略大小写            |

```c
const char *POSSIBLE_USER_ENUMS[] = {"png", "jpg", "bmp", NULL};
```

匹配成功时，返回的指针指向 `enums` 数组内元素（而非 `argv`），因此可通过 `==` 做恒等比较判断命中了哪个枚举值。

另见：

- [`ARGON_ENUM_MAX_COUNT`](#常量)

---

### `ArgonArrayPlugin`

为单个选项启用多值收集。

| 字段            | 类型      | 说明                                               |
| --------------- | --------- | -------------------------------------------------- |
| `max_len`       | `uint8_t` | 最大值的数量（上限为 `ARGON_ARRAY_MAX_LEN`）       |
| `force_filling` | `bool`    | 若为 `true`，必须提供恰好 `max_len` 个值，否则失败 |

另见：

- [`ARGON_ARRAY_MAX_LEN`](#常量)

## 接口

### `argon_parse`

```c
ArgonResult argon_parse(Argon *ctx, char *const argv[]);
```

遍历 `argv[1..]` 直至 `NULL` 哨兵。

1. 首先匹配 `ctx->options` 中的全局选项。
2. 若 `ctx->subs` 非 `NULL` 且当前匹配到了子程序名，后续参数仅匹配该子程序的选项集。
3. 解析的值写入匹配的 `ArgonOption` 定义的 `target` 指针。

失败时 `ctx->errmsg` 写入诊断信息。

**返回值** 成功返回 `ARGON_OK`，否则返回错误码。

---

### `ARGON_ARGV(...)`

通过可变参数在栈上构造一个 `NULL` 终止的 `char *[]`。`argv[0]` 自动设为 `""`（解析器忽略）。

```c
char *argv[] = ARGON_ARGV("--format", "jpg");
argon_parse(&ctx, ARGON_ARGV("--verbose", "-o", "out.txt"));
```

---

### `argon_print_table`

```c
ArgonResult argon_print_table(Argon *ctx, FILE *ostream);
```

向 `ostream` 输出一个格式化的选项表，列出所有命名全局选项。位置参数被省略。若设置了 `ctx->subs`，同时列出每个子程序及其完整选项表。

---

### `argon_print_sub_table`

```c
ArgonResult argon_print_sub_table(const ArgonSub *ctx, FILE *ostream);
```

为指定子程序输出格式化的选项表。

---

### `argon_active_sub`

```c
const ArgonSub *argon_active_sub(const Argon *ctx);
```

返回解析过程中匹配到的子程序，未匹配则返回 `NULL`.

---

### `argon_option_is_parsed`

```c
bool argon_option_is_parsed(const ArgonOption *opt);
```

若该选项的目标变量在解析过程中被写入（即用户在命令行中提供了该选项），返回 `true`. `target` 指针指向的数据在解析前往往具有初始化值，该方法可准确判断对应的选项究竟是否被解析。

---

## 常量

| 名称                         | 可修改 | 说明                     |
| ---------------------------- | :----: | ------------------------ |
| `ARGON_VERSION_MAJOR`        |   —    | 主版本号                 |
| `ARGON_VERSION_MINOR`        |   —    | 次版本号                 |
| `ARGON_VERSION_PATCH`        |   —    | 修订版本号               |
| `ARGON_ERRMSG_LEN`           |   ✓    | 错误信息缓冲区长度       |
| `ARGON_OPTNAME_MAX_LEN`      |   ✓    | 选项全写名最大长度       |
| `ARGON_ENUM_MAX_LEN`         |   ✓    | 枚举字符串最大长度       |
| `ARGON_ENUM_MAX_COUNT`       |   —    | 单个选项枚举值的最大数量 |
| `ARGON_ARRAY_MAX_LEN`        |   —    | 数组选项值的最大数量     |
| `ARGON_OPTION_MAX_COUNT`     |   —    | 单个选项集的最大选项数   |
| `ARGON_SUBCOMMAND_MAX_COUNT` |   —    | 子程序的最大数量         |
