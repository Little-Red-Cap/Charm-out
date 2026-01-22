module;
#include <chrono>
#include <expected>

module out.port;
import out.core;


namespace out::port {
    static console_sink* g_default_console = nullptr;

    void set_default_console(console_sink* p) noexcept { g_default_console = p; }

    console_sink& default_console() noexcept {
        if (g_default_console) return *g_default_console;
        static console_sink inst{};
        return inst;
    }

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
