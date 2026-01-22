module out.port;

namespace out::port {
    static console_sink* g_default_console = nullptr;

    void set_default_console(console_sink* p) noexcept { g_default_console = p; }

    console_sink& default_console() noexcept {
        if (g_default_console) return *g_default_console;
        static console_sink inst{};
        return inst;
    }

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
