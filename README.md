<div align="center">

# [Charm-out](https://github.com/Little-Red-Cap/Charm-out)

***适用于裸机的零分配、轻量级现代 C++ 格式化与日志库***
<br>
***Tiny, zero-allocation C++ formatting & logging for bare-metal***

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg?style=flat-square)](https://en.cppreference.com/w/cpp/23)
[![C++20 Modules](https://img.shields.io/badge/Modules-C%2B%2B20-blue.svg?style=flat-square)](https://en.cppreference.com/w/cpp/language/modules)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=flat-square)](LICENSE)

[快速开始](#-快速开始) · [文档](doc/) · [示例](examples/) · [基准测试](#-性能数据)
</div>

[English](doc/README_en.md) | [简体中文](README.md)

---

```cpp
import out.api;

int main() {
    out::println<"Hello, {}!">("World");
    out::info<"Value: {}, Hex: {:04x}">(42, 0xABCD);
}
```

---

## ✨ 核心特性

<table>
<tr>
<td width="50%">

### 🚀 零成本抽象
- 编译期格式化解析
- 未启用的日志完全消失
- 域过滤编译期决定
- 可选功能按需编译

</td>
<td width="50%">

### 🛡️ 类型安全
- 编译期类型检查
- 参数数量验证
- 无隐式转换陷阱
- 格式字符串验证

</td>
</tr>
<tr>
<td width="50%">

### 🔒 嵌入式友好
- 无异常（`std::expected`）
- 无堆分配
- 无虚函数
- C++20 Modules

</td>
<td width="50%">

### 🎨 功能丰富
- 灵活的 Sink 抽象
- ANSI 终端颜色
- 懒求值
- 行缓冲

</td>
</tr>
</table>

---

## 📦 快速开始

- [代码示例](examples/example.cpp)
- [Windows 工程示例](examples/windows)
- [STM32 工程示例](examples/stm32f103c8)

### 最小示例

```cpp
import out.api;
import out.port;

int main() {
    out::port::console_sink console;
    
    out::info<"Hello, {}!">("World");
    // 输出: Hello, World!
    
    out::debug<"Value: {}, Hex: {:04x}">(42, 0xABCD);
    // 输出: Value: 42, Hex: abcd
}
```

### 编译选项

| 选项 | 作用 | 默认 |
|------|------|------|
| `-DLOG_LEVEL_DEBUG` | 启用 debug 及以上级别 | OFF |
| `-DOUT_ENABLE_BINARY` | 启用二进制输出 | OFF |
| `-DOUT_ENABLE_FLOAT` | 启用浮点数支持 | OFF |


---

## 📚 详细示例

### 格式化输出

```cpp
// 基础类型
out::info<"Integer: {}">(42);
out::info<"Negative: {}">(-123);
out::info<"Char: '{}'">("A");
out::info<"String: {}">(std::string_view{"Hello"});

// 宽度和填充
out::info<"Padded: {:08d}">(42);        // 00000042
out::info<"Hex: 0x{:04X}">(0xAB);       // 0x00AB

// 二进制（需要 -DOUT_ENABLE_BINARY）
out::info<"Binary: {:b}">(0b11001010);  // 11001010

// 浮点数（需要 -DOUT_ENABLE_FLOAT）
out::info<"Float: {:.2f}">(3.14159);    // 3.14

// 转义括号
out::info<"Escaped: {{}}">();           // Escaped: {}
```

### 日志级别控制

```cpp
// 编译时设置：-DLOG_LEVEL_INFO
out::error<"Critical: {}">(code);   // ✅ 总是编译
out::warn<"Warning: {}">(msg);      // ✅ 编译
out::info<"Status: {}">(status);    // ✅ 编译
out::debug<"Debug: {}">(value);     // ❌ 不编译（零开销）
out::trace<"Trace: {}">(detail);    // ❌ 不编译（零开销）
```

### ANSI 颜色

```cpp
import out.ansi;

auto console = out::port::console_sink{};
auto uart_color = out::ansi_with<true>(console);

// 颜色输出
out::error<"{}{}{}{}">(uart_color,
    out::fg(out::color::red),
    out::bold,
    "ERROR!",
    out::reset
);

// 语法糖
using namespace out;
info<"{}Status: {}OK{}">(uart_color,
    !color::green,  // 前景色
    bold,
    reset
);
```

### 域过滤

```cpp
// 定义域
struct network_domain {};
struct storage_domain {};

// 启用/禁用域（编译期决定）
template <> inline constexpr bool out::domain_enabled<network_domain> = true;
template <> inline constexpr bool out::domain_enabled<storage_domain> = false;

// 使用
out::emit<out::level::info, network_domain, "Connected to {}">(uart, "192.168.1.1");
out::emit<out::level::info, storage_domain, "File saved">(uart);  // 零开销过滤
```

### 懒求值

```cpp
// 避免不必要的计算
out::trace<"Result: {}">(uart, 
    out::lazy([](){ return expensive_computation(); })
);
// 如果 trace 级别未启用，lambda 不会被调用
```

---

## 📊 功能对比表

### 支持的格式化类型

| 类型 | 格式符 | 示例 | 编译选项 |
|------|--------|------|----------|
| 整数（十进制） | `{}`, `{:d}` | `42` | 默认启用 |
| 整数（十六进制小写） | `{:x}` | `ab` | 默认启用 |
| 整数（十六进制大写） | `{:X}` | `AB` | 默认启用 |
| 整数（二进制） | `{:b}`, `{:B}` | `1010` | `-DOUT_ENABLE_BINARY` |
| 浮点数（定点） | `{:f}`, `{:.2f}` | `3.14` | `-DOUT_ENABLE_FLOAT` |
| 浮点数（科学计数） | `{:e}`, `{:E}` | `1.23e+02` | `-DOUT_ENABLE_FLOAT` |
| 浮点数（自动） | `{:g}`, `{:G}` | `123.45` | `-DOUT_ENABLE_FLOAT` |
| 字符 | `{}` | `'A'` | 默认启用 |
| 字符串 | `{}` | `"hello"` | 默认启用 |
| 枚举 | `{}`, `{:x}` | `42` | 默认启用 |

### 支持的格式化选项

| 选项 | 语法 | 说明 | 示例 |
|------|------|------|------|
| 宽度 | `{:N}` | 最小宽度（空格填充） | `{:8}` → `"      42"` |
| 零填充 | `{:0N}` | 零填充 | `{:08x}` → `"0000002a"` |
| 精度 | `{:.N}` | 浮点数小数位数 | `{:.2f}` → `"3.14"` |
| 大小写 | `{:X}`, `{:E}` | 大写输出 | `{:X}` → `"AB"` |


---

## 🔧 平台移植

### 移植接口

只需实现 `out.port.cppm` 中的三个函数：

```cpp
// out.port.xxx.cpp
module out.port;

namespace out::port {
    // 1. 控制台输出
    result<std::size_t> console_sink::write(bytes b) noexcept {
        HAL_UART_Transmit(&huart1, (uint8_t*)b.data(), b.size(), 1000);
        return ok(b.size());
    }
    
    // 2. 串口输出（可选）
    result<std::size_t> uart_sink::write(bytes b) const noexcept {
        auto* huart = static_cast<UART_HandleTypeDef*>(handle);
        HAL_UART_Transmit(huart, (uint8_t*)b.data(), b.size(), 1000);
        return ok(b.size());
    }
    
    // 3. 时间戳（可选）
    tick_t now_ms() noexcept {
        return HAL_GetTick();
    }
}
```

### 平台示例

<details>
<summary><b>PC (stdio)</b></summary>

```cpp
module;
#include <chrono>
#include <expected>

module out.port;
import out.core;


namespace out::port {

    result<std::size_t> console_sink::write(const bytes b) noexcept {
        auto n = std::fwrite(b.data(), 1, b.size(), stdout);
        std::fflush(stdout);
        if (n != b.size()) return std::unexpected(errc::io_error);
        return ok(n);
    }

    result<std::size_t> uart_sink::write(const bytes b) const noexcept {
        (void)handle;
        return console_sink{}.write(b);
    }

    tick_t now_ms() noexcept {
        using namespace std::chrono;
        auto ms = duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
        return static_cast<tick_t>(ms);
    }

}

```
</details>

<details>
<summary><b>STM32 (HAL)</b></summary>

```cpp
module out.port;
import out.core;

extern "C" {
    #include "stm32f4xx_hal.h"
    extern UART_HandleTypeDef huart1;
}

namespace out::port {
    result<std::size_t> console_sink::write(bytes b) noexcept {
        HAL_UART_Transmit(&huart1, 
            reinterpret_cast<const uint8_t*>(b.data()), 
            b.size(), HAL_MAX_DELAY);
        return ok(b.size());
    }
    
    tick_t now_ms() noexcept {
        return HAL_GetTick();
    }
}
```
</details>

<details>
<summary><b>ESP32 (IDF)</b></summary>

```cpp
module out.port;
import out.core;

extern "C" {
    #include "esp_log.h"
    #include "esp_timer.h"
}

namespace out::port {
    result<std::size_t> console_sink::write(bytes b) noexcept {
        printf("%.*s", (int)b.size(), (const char*)b.data());
        return ok(b.size());
    }
    
    tick_t now_ms() noexcept {
        return esp_timer_get_time() / 1000;
    }
}
```
</details>

<details>
<summary><b>nRF52 (SDK)</b></summary>

```cpp
module out.port;
import out.core;

extern "C" {
    #include "nrf_log.h"
    #include "app_timer.h"
}

namespace out::port {
    result<std::size_t> console_sink::write(bytes b) noexcept {
        NRF_LOG_RAW_INFO("%.*s", b.size(), b.data());
        return ok(b.size());
    }
    
    tick_t now_ms() noexcept {
        return app_timer_cnt_get() / 32.768;
    }
}
```
</details>

---

## 📈 性能数据

### 代码大小对比（Cortex-M3, -Os）
该部分尚未测试

### 运行时性能（STM32F103, 72MHz）
该部分尚未测试

---

## 🗂️ 模块结构

```
out/
├── modules/               # 核心库（C++20 Modules）
│   ├── out.core.cppm      # 基础类型（result, errc, bytes）
│   ├── out.sink.cppm      # Sink 概念 + 通用 Sink 实现
│   ├── out.format.cppm    # 编译期格式化引擎
│   ├── out.print.cppm     # print/println 接口
│   ├── out.domain.cppm    # 日志级别与域管理
│   ├── out.ansi.cppm      # ANSI 颜色支持
│   ├── out.api.cppm       # 高层 API（info/debug/error...）
│   └── out.port.cppm      # 移植层接口声明
│
├── examples/              # 示例代码
│   ├── example.cpp        # 跨平台示例实现
│   ├── windows/           # Windows 示例
│   └── stm32f103c8/       # STM32 示例
│
├── doc/                   # 文档
│
└── CMakeLists.txt         # 构建系统
```

---

<div align="center">

问题反馈：[GitHub Issues](https://github.com/Little-Red-Cap/Charm-out/issues)
<br>
**⭐ 如果这个项目对你有帮助，请给个 Star！**

[回到顶部](#charm-out)

</div>

