module;
#include <cstddef>
#include <cstdint>

export module out.port;

import out.core;
import out.sink;

export namespace out::port {

    // 平台输出：控制台/调试口（PC=stdout；嵌入式=SWO/ITM等）
    struct console_sink {
        result<std::size_t> write(bytes b) noexcept;
    };

    // 串口输出（STM32: handle = UART_HandleTypeDef*；PC: 可用作 stdout 模拟）
    struct uart_sink {
        void* handle{};
        result<std::size_t> write(bytes b) const noexcept;
    };

    // 可选时间源：用于时间戳/性能分析，不需要者不必 import
    using tick_t = std::uint64_t;
    tick_t now_ms() noexcept;

}
