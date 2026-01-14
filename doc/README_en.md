<div align="center">

# [Charm-out](https://github.com/Little-Red-Cap/Charm-out)

***Tiny, zero-allocation C++ formatting & logging for bare-metal***
<br>
***适用于裸机的零分配、轻量级现代 C++ 格式化与日志库***

[![C++23](https://img.shields.io/badge/C%2B%2B-23-blue.svg?style=flat-square)](https://en.cppreference.com/w/cpp/23)
[![C++20 Modules](https://img.shields.io/badge/Modules-C%2B%2B20-blue.svg?style=flat-square)](https://en.cppreference.com/w/cpp/language/modules)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg?style=flat-square)](../LICENSE)
<br>
[![CLang Build Status](https://github.com/Little-Red-Cap/Charm-out/actions/workflows/build-clang.yml/badge.svg)](https://github.com/Little-Red-Cap/Charm-out/actions)
[![CLang Build Status](https://github.com/Little-Red-Cap/Charm-out/actions/workflows/build-arm-none-eabi.yml/badge.svg)](https://github.com/Little-Red-Cap/Charm-out/actions)

[Quick Start](#-quick-start) · [Docs](../doc/) · [Examples](../examples/) · [Benchmarks](#-performance)
</div>

[English](README_en.md) | [简体中文](../README.md)

---

```cpp
import out.api;

int main() {
    out::println<"Hello, {}!">("World");
    out::info<"Value: {}, Hex: {:04x}">(42, 0xABCD);
}
```

---

## ✨ Highlights

<table>
<tr>
<td width="50%">

### 🚀 Zero-cost abstraction
- Compile-time format parsing
- Disabled logs are compiled out
- Domain filtering decided at compile time
- Optional features built on demand

</td>
<td width="50%">

### 🛡️ Type safety
- Compile-time type checking
- Argument count validation
- Avoid implicit conversion pitfalls
- Format string validation

</td>
</tr>
<tr>
<td width="50%">

### 🔒 Embedded friendly
- No exceptions (`std::expected`)
- No heap allocation
- No virtual functions
- C++20 Modules

</td>
<td width="50%">

### 🎨 Feature rich
- Flexible sink abstraction
- ANSI terminal colors
- Lazy evaluation
- Line buffering

</td>
</tr>
</table>

---

## 📦 Quick Start

- [Example code](../examples/example.cpp)
- [Windows example](../examples/windows)
- [STM32 example](../examples/stm32f103c8)

### Minimal Example

```cpp
import out.api;
import out.port;

int main() {
    out::port::console_sink console;

    out::info<"Hello, {}!">("World");
    // Output: Hello, World!

    out::debug<"Value: {}, Hex: {:04x}">(42, 0xABCD);
    // Output: Value: 42, Hex: abcd
}
```

### Build Options

| Option | Description | Default |
|------|-------------|---------|
| `-DLOG_LEVEL_DEBUG` | Enable debug and above | OFF |
| `-DOUT_ENABLE_BINARY` | Enable binary formatting | OFF |
| `-DOUT_ENABLE_FLOAT` | Enable float formatting | OFF |

---

## 📚 Detailed Examples

### Formatting

```cpp
// Basic types
out::info<"Integer: {}">(42);
out::info<"Negative: {}">(-123);
out::info<"Char: '{}'">('A');
out::info<"String: {}">(std::string_view{"Hello"});

// Width and padding
out::info<"Padded: {:08d}">(42);        // 00000042
out::info<"Hex: 0x{:04X}">(0xAB);       // 0x00AB

// Binary (requires -DOUT_ENABLE_BINARY)
out::info<"Binary: {:b}">(0b11001010);  // 11001010

// Floating point (requires -DOUT_ENABLE_FLOAT)
out::info<"Float: {:.2f}">(3.14159);    // 3.14

// Escaped braces
out::info<"Escaped: {{}}">();           // Escaped: {}
```

### Log Level Control

```cpp
// Compile-time level: -DLOG_LEVEL_INFO
out::error<"Critical: {}">(code);   // ✅ Compiled
out::warn<"Warning: {}">(msg);      // ✅ Compiled
out::info<"Status: {}">(status);    // ✅ Compiled
out::debug<"Debug: {}">(value);     // ❌ Not compiled (zero cost)
out::trace<"Trace: {}">(detail);    // ❌ Not compiled (zero cost)
```

### ANSI Colors

```cpp
import out.ansi;

auto console = out::port::console_sink{};
auto uart_color = out::ansi_with<true>(console);

// Colored output
out::error<"{}{}{}{}">(uart_color,
    out::fg(out::color::red),
    out::bold,
    "ERROR!",
    out::reset
);

// Syntactic sugar
using namespace out;
info<"{}Status: {}OK{}">(uart_color,
    !color::green,  // Foreground color
    bold,
    reset
);
```

### Domain Filtering

```cpp
// Define domains
struct network_domain {};
struct storage_domain {};

// Enable/disable domains (compile-time)
template <> inline constexpr bool out::domain_enabled<network_domain> = true;
template <> inline constexpr bool out::domain_enabled<storage_domain> = false;

// Use
out::emit<out::level::info, network_domain, "Connected to {}">(uart, "192.168.1.1");
out::emit<out::level::info, storage_domain, "File saved">(uart);  // Zero-cost filter
```

### Lazy Evaluation

```cpp
// Avoid unnecessary computation
out::trace<"Result: {}">(uart,
    out::lazy([](){ return expensive_computation(); })
);
// If trace level is disabled, the lambda is never called
```

---

## 📊 Feature Tables

### Supported Formatting Types

| Type | Specifier | Example | Build Option |
|------|-----------|---------|--------------|
| Integer (decimal) | `{}`, `{:d}` | `42` | Enabled by default |
| Integer (hex, lowercase) | `{:x}` | `ab` | Enabled by default |
| Integer (hex, uppercase) | `{:X}` | `AB` | Enabled by default |
| Integer (binary) | `{:b}`, `{:B}` | `1010` | `-DOUT_ENABLE_BINARY` |
| Float (fixed) | `{:f}`, `{:.2f}` | `3.14` | `-DOUT_ENABLE_FLOAT` |
| Float (scientific) | `{:e}`, `{:E}` | `1.23e+02` | `-DOUT_ENABLE_FLOAT` |
| Float (auto) | `{:g}`, `{:G}` | `123.45` | `-DOUT_ENABLE_FLOAT` |
| Char | `{}` | `'A'` | Enabled by default |
| String | `{}` | `"hello"` | Enabled by default |
| Enum | `{}`, `{:x}` | `42` | Enabled by default |

### Supported Formatting Options

| Option | Syntax | Description | Example |
|------|--------|-------------|---------|
| Width | `{:N}` | Minimum width (space padding) | `{:8}` → `"      42"` |
| Zero padding | `{:0N}` | Pad with zeros | `{:08x}` → `"0000002a"` |
| Precision | `{:.N}` | Fractional digits for float | `{:.2f}` → `"3.14"` |
| Uppercase | `{:X}`, `{:E}` | Uppercase output | `{:X}` → `"AB"` |

---

## 🔧 Porting

### Porting Interface

Implement these three functions in `out.port.cppm`:

```cpp
// out.port.xxx.cpp
module out.port;

namespace out::port {
    // 1. Console output
    result<std::size_t> console_sink::write(bytes b) noexcept {
        HAL_UART_Transmit(&huart1, (uint8_t*)b.data(), b.size(), 1000);
        return ok(b.size());
    }

    // 2. UART output (optional)
    result<std::size_t> uart_sink::write(bytes b) const noexcept {
        auto* huart = static_cast<UART_HandleTypeDef*>(handle);
        HAL_UART_Transmit(huart, (uint8_t*)b.data(), b.size(), 1000);
        return ok(b.size());
    }

    // 3. Timestamp (optional)
    tick_t now_ms() noexcept {
        return HAL_GetTick();
    }
}
```

### Platform Examples

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

## 📈 Performance

### Code Size (Cortex-M3, -Os)
Not measured yet.

### Runtime (STM32F103, 72MHz)
Not measured yet.

---

## 🗂️ Module Layout

```
out/
├── modules/               # Core library (C++20 Modules)
│   ├── out.core.cppm      # Basic types (result, errc, bytes)
│   ├── out.sink.cppm      # Sink concept + common sinks
│   ├── out.format.cppm    # Compile-time formatting engine
│   ├── out.print.cppm     # print/println interface
│   ├── out.domain.cppm    # Log levels and domain control
│   ├── out.ansi.cppm      # ANSI color support
│   ├── out.api.cppm       # High-level API (info/debug/error...)
│   └── out.port.cppm      # Porting layer declaration
│
├── examples/              # Example code
│   ├── example.cpp        # Cross-platform example
│   ├── windows/           # Windows example
│   └── stm32f103c8/       # STM32 example
│
├── doc/                   # Documentation
│
└── CMakeLists.txt         # Build system
```

---

<div align="center">

Issues: [GitHub Issues](https://github.com/Little-Red-Cap/Charm-out/issues)
<br>
**⭐ If this project helps you, please give it a star!**

[Back to top](#charm-out)

</div>
