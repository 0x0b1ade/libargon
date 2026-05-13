# libargon

单头文件 C11 命令行参数解析库 / 兼容 C++ / 轻量级、现代化

| [English](README.en.md) |
| ----------------------- |

## 特性

- **单头文件分发** — 移植一个头文件即可工作
- **栈上操作** — 无堆分配操作、无 VLA
- **可忽略大小写匹配** — 全写和缩写均支持设置忽略大小写匹配
- **组合匹配** — 支持将多个短选项组合匹配（如 `-abc [arg]` 会被展开为 `-a -b -c [arg]`）
- **多值匹配** — 选项支持定长或不定长匹配多个值（如 `--size 40 30`）
- **枚举校验** — 将字符串选项约束到预定义白名单
- **位置参数** — 支持匿名选项解析
- **子程序支持** — 隔离全局和子程序选项的解析方式
- **帮助表生成** — 格式化输出选项表，含类型标注、枚举值及数组维度等
- **C++ 兼容** — 不做任何修改即可在 C++ 项目中工作

## 快速开始

[查看示例程序](example.c)

## 编译

需要支持 C11 标准的编译器。无需单独编译，在项目中直接引用头文件即可使用。若需要编译为 静态/动态 库，仓库默认提供 [Xmake](https://xmake.io/) 支持，根目录下的 [xmake.lua](xmake.lua) 具有对应的预设。

```sh
xmake f -m release
xmake build argon
xmake build argon_shared
```

## 基本概念

一个符合 C11 标准的、由宿主环境创建的 argv 类似这样：

```c
const char *POSSIBLE_USER_INPUT = "[exe_path] build target.exe --language c11 ...";
// ↓↓↓
char *argv[] = {"[exe_path]", "build", "target.exe", "--language", "c11", ..., NULL};
```

`Argon` 会假设传入的所有的 argv 都兼容该格式进行工作，忽略 `argv[0]` （但会检测是否为空指针）并始终认为 `argv[argc] == NULL`. 若想解析自定义的参数数组，也需要符合该规则。你可以使用 `Argon` 提供的宏创建合法的参数数组（详见 [API 参考](#api-参考)）。

在该场景下，`build` 被视为子程序名、`target.exe` 为一个匿名位置参数值、`--language` 为一个全写形式的选项名、`c11` 是 `language` 选项对应的参数值。`Argon` 会对这些概念进行抽象。

## API 参考

[查看 API 文档](docs/API.zh.md)
