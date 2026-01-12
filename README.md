<div align="center">
    <h1>
        <a href="https://github.com/Little-Red-Cap/Charm-out">Charm-out</a>
    </h1>
    <p> <strong> <em>
        适用于裸机的零分配、轻量级 C++ 格式化与日志库 
        <br>
        Tiny, zero-allocation C++ formatting & logging for bare-metal
    </em> </strong> </p>
    <img src="https://img.shields.io/badge/License-MIT-green" alt="" />
    <br>
    <a href="https://github.com/Little-Red-Cap/Charm-out/actions/workflows/cmake-multi-platform.yml">
        <img src="https://github.com/Little-Red-Cap/Charm-out/actions/workflows/cmake-multi-platform.yml/badge.svg" alt="cmake-multi-platform" />
    </a>
</div>

[中文](README.md) | [English](doc/README_en.md)

> 一款专为 **无操作系统 / 无堆内存 / 无异常** 的微控制器环境设计的现代 `printf` 替代方案。  
> **零成本抽象**、**类型安全**、**编译期优化**、**基于模块**、可裁剪，且易于移植。
---

## 环境要求

- 支持 **C++ Modules**（C++20+）以及足够 C++23 特性的编译器（具体取决于你的工具链配置）。
- 标准库支持 `<expected>`、`<span>`、`<to_chars>`。
- 专为 **独立环境 / 嵌入式** 设计，但也提供了 Windows 移植示例。

---

## 快速上手

- [代码示例](examples/example.cpp)
- [Windows 工程示例](examples/windows)
- [STM32 工程示例](examples/stm32f103c8)

---

## 功能列表

| 功能 / 特性                  | 支持情况 |    配置     | 使用                  | 说明                                                        |
|--------------------------|:----:|:---------:|---------------------|-----------------------------------------------------------|
| 格式化 占位符                  |  ✅   |     -     | `{{` / `}}` 转义      |                                                           |
| 格式化 二进制                  |  ✅   | 可选/默认关闭⚙️ | `:b` / `:B`         |                                                           |
| 格式化 整数                   |  ✅   |    可用✅    | `:x` / `:X`         |                                                           |
| 格式化 浮点                   |  ✅   | 可选/默认关闭⚙️ | `:f/:F/:e/:E/:g/:G` | 启用后支持 float 固定小数格式（默认6位小数，可指定精度 {:.f}）                    |
| 格式化 布尔                   |  🚫  |   计划中📅   |                     | 布尔类型目前会格式化为数字0/1（即将改进为true/false文本）                       |
| 格式化 字符/字符串               |  ✅   |    可用✅    |                     | 字符和字符串字面量/std::string_view 输出，基本的宽度和零填充控制                 |
| 格式化 自定义类型                |  🚫  |   计划中📅   |                     | 计划提供 out::formatter<T> 或类似机制，让用户注册新类型的格式化逻辑。              |
| 格式化 宽度和零填充               |  ✅   |    可用✅    | `:08X`、`:5d` 风格     |                                                           |
| 格式化 控制                   |  🚫  |     -     |                     | 对齐`<`、`>`、`^`<br> 自定义填充字符- `+` <br> 空格符号标志，`#` <br>前缀`0x` |
| 日志 等级过滤                  |  ✅   | 可选/默认关闭⚙️ |                     |                                                           |
| 日志 域过滤                   |  ✅   |   可选⚙️    |                     |                                                           |
| 转义序列ANSI 颜色/样式标记         |  ✅   | 可选/默认关闭⚙️ |                     | 可编译期注入：`ansi_with<true>(sink)`                            |
| println换行符配置             |  🚫  |   计划中📅   |                     | 当前默认\r\n                                                  |
| 惰性求值包装器                  |  ✅   |     -     |                     | `out::lazy(...)`                                          |
| 时间戳                      |  ⚙️  |   可选⚙️    |                     | 当前由port对接                                                 |
| 无异常                      |  ✅   |     -     |                     | 错误处理将始终通过返回值进行（基于 `std::expected` 的错误处理）                  |
| 无动态内存                    |  ✅   |     -     |                     | 内存管理交由调用者或使用栈/静态分配。                                       |
| 编译期格式解析                  |  ✅   |     -     |                     |                                                           |
| 运行时格式解析                  |  🚫  |     -     |                     | 对于必须在运行时确定格式的情况，建议在应用层做好格式拼装或使用其他库。                       |
| 自动线程安全                   |  🚫  |     -     |                     | 由使用者根据具体环境自行确保。如需在多任务环境使用，同步机制应在应用层处理。                    |
| 完整的 std::format/{fmt} 兼容 |  🚫  |     -     |                     | 我们选择保留一个精简且可定制的核心，而非成为一个庞大的通用格式化库。                        |
| 平台无关                     |  ✅   |     -     |                     | `Sink` 概念：任何实现了 `write(bytes)` 的类型                        |

---
