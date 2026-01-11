module out.port;

namespace out::port {
    result<std::size_t> console_sink::write(bytes b) noexcept {
        // TODO: Implement your console output
        return ok(b.size());
    }

    result<std::size_t> uart_sink::write(bytes b) noexcept {
        // TODO: Call your serial port HAL
        return ok(b.size());
    }

    tick_t now_ms() noexcept {
        // TODO: Return your system clock
        return 0;
    }
}
