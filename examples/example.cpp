#include <string_view>

import out.api;

// Custom domain
struct network_domain {};
template <> inline constexpr auto out::domain_enabled<network_domain> = true;
struct noisy_domain {};
template <> inline constexpr auto out::domain_enabled<noisy_domain> = false;

extern "C" void example()
{
    out::port::console_sink console;

    // ------------------------------------------------------------
    // Minimal: Hello world
    // ------------------------------------------------------------
    out::println<"Hello world!">(console);
    out::info<"Default console works too.">();

    // ------------------------------------------------------------
    // Format basics: {}, escaping, width/zero-pad, hex
    // ------------------------------------------------------------
    out::info<"Enable output via LOG_LEVEL_INFO/DEBUG/... at build time.">(console);

    out::debug<"Value: {}\t Hex: {:04x}\t Hex: {:04X}">(console, 42, 0xABCD, 0xABCD);
    out::info<"Escaped braces: {{}}">(console);
    out::info<"Width pad: '0x{:08X}'">(console, 0x12AB);

    // ------------------------------------------------------------
    // Common types: zero/negative/char/string_view
    // ------------------------------------------------------------
    out::info<"Zero: '{}'">(console, 0);
    out::info<"Negative: '{}'">(console, -123);
    out::info<"Chars: '{}' '{}'">( console, 'A', 'z' );
    out::info<"String: '{}'">(console, std::string_view{"hello"});

    // TODO: 增加bool类型支持
    // out::info<"Bool: '{}' and '{}'">(console, true, false);

    // TODO: 增加自定义类型扩展

    out::info<"==========================">(console);

    // ------------------------------------------------------------
    // Optional features (pitfalls)
    //
    // Binary: {:b}/{:B}
    //     Requires: -DOUT_ENABLE_BINARY
    //
    // Float: {:f}/{:e}/{:g} and precision {:.2f}
    //     Requires: -DOUT_ENABLE_FLOAT
    //
    // If you forget to enable them, you should get a clear compile-time message.
    // ------------------------------------------------------------
    {
        // 二进制输出    需编译选项开启对应宏 OUT_ENABLE_BINARY
        auto vb = 0b11001010;
        out::debug<"Binary demo: value={}, bin={:b}">(console, vb, vb);
        // 浮点数输出    需编译选项开启对应宏 OUT_ENABLE_FLOAT
        auto vf = 3.1415926f;
        out::debug<"Float demo: f={:f}">(console, vf);
        out::debug<"Float precision: f={:.2f}">(console, vf);
    }

    out::println<"==========================">(console);

    // ------------------------------------------------------------
    // ANSI: default (no color) -> injected -> disabled
    // ------------------------------------------------------------
    // ANSI tokens (compile-time enable/disable).
    out::warn<"{}WARN{} default color reset">(console, out::fg(out::color::yellow), out::reset);
    out::info<"{}INFO{} back to normal">(console, out::fg(out::color::green), out::reset);
    // enable
    auto console_ansi = out::ansi_with<true>(console);
    out::error<"{}{}{}{}">(console_ansi, out::fg(out::color::red), out::bold, "CRITICAL ERROR", out::reset);
    out::warn<"{}WARN{} default color reset">(console_ansi, out::fg(out::color::yellow), out::reset);
    out::info<"{}INFO{} back to normal">(console_ansi, out::fg(out::color::green), out::reset);
    // disable
    auto console_plain = out::ansi_with<false>(console);
    out::info<"{}INFO{} ansi off">(console_plain, out::fg(out::color::green), out::reset);

    out::println<"==========================">(console_ansi);

    // 使用语法糖缩短命名
    {
        using namespace out;

        auto text = "The text line";
        info<"{} is normal">(console_ansi, text);
        info<"{}{} add italic">(console_ansi, italic, text);
        info<"{}{} add bold">(console_ansi, bold, text);
        info<"{}{} add underline">(console_ansi, underline, text);
        info<"{}{} add Foreground">(console_ansi, !color::red, text);
        info<"{}{} add Background color">(console_ansi, ~color::blue, text);
        info<"{}{} add dim intensity">(console_ansi, dim, text);
        info<"{}{} cancel dim and bold">(console_ansi, "\x1b[22m", text);
        info<"{}{} is reset">(console_ansi, reset, text);

        println<"==========================">(console_ansi);
    }


    // ------------------------------------------------------------
    // Lazy evaluation: callable runs only when the level is enabled.
    // ------------------------------------------------------------
    out::trace<"Expensive: {}">(console, out::lazy([] { return 99; }));
    auto lazy_v = out::lazy([] { return 123; });
    out::trace<"Lazy lvalue: {}">(console, lazy_v);

    // ------------------------------------------------------------
    // Timestamp
    // ------------------------------------------------------------
    auto ts = out::port::now_ms();
    out::info<"Timestamp(ms): {} Event occurred">(console, ts);

    // ------------------------------------------------------------
    // Domain filtering
    // ------------------------------------------------------------
    out::buffer_sink<256> cap;
    // 这里被过滤，不会打印到 cap buffer
    out::emit<out::level::info, noisy_domain, "[noisy] {}">(cap, "SHOULD NOT APPEAR");
    out::print<"{}">(console, cap.view());  // 输出为空

    out::emit<out::level::info, network_domain,   "[net] {}">(cap, "Connected");
    out::print<"{}">(console, cap.view());

    // ------------------------------------------------------------
    // Sinks: line-buffered + fixed buffer
    // ------------------------------------------------------------
    out::line_buffered_sink line_buf{console}; // flushes on '\n'.
    out::println<"Buffered line {}">(line_buf, 1);

    out::buffer_sink<256> buf;
    out::println<"Buf: {}">(buf, 123);
    out::println<"More: {}">(buf, "OK");
    out::println<"{}">(console, buf.view());

    // TODO: 添加更多错误示例
    // out::debug<"{}\t{}">(uart, 42); // 参数数量不匹配
    // out::debug<"{">(uart, 42); // 括号不闭合
}
