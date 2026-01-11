module;
#include <charconv>
#include <cstdint>
#include <expected>
#include <string_view>

export module out.ansi;

import out.core;
import out.sink;
import out.format; // fmt_spec + write_one protocol

namespace out {
    struct reset_t {};
    struct bold_t {};
    struct dim_t {};
    struct italic_t {};
    struct underline_t {};
}

export namespace out::ansi {
    // 颜色/指令 token：依然是“值类型”，可被 format/print 拼接
    enum class color : std::uint8_t { default_, black, red, green, yellow, blue, magenta, cyan, white };

    inline constexpr reset_t reset{};
    inline constexpr bold_t  bold{};
    inline constexpr dim_t dim{};
    inline constexpr italic_t italic{};
    inline constexpr underline_t underline{};

    struct fg_t { color c; };
    struct bg_t { color c; };
    constexpr fg_t fg(color c) noexcept { return {c}; }
    constexpr bg_t bg(color c) noexcept { return {c}; }

    // 注入能力：ansi_sink_ref —— 不复制 Base，只持有指针（最轻）
    // Enabled 为 false 时，所有 ANSI token 写入直接编译期变成 0（无 runtime 分支）
    template <class Base, bool Enabled = true>
    struct ansi_sink_ref {
        Base* base{};

        // 让它本身也满足 Sink：转发普通写入
        result<std::size_t> write(bytes b) noexcept { return base->write(b); }

        // ANSI 专用写入：能力点（只有 wrapper 有）
        result<std::size_t> write_ansi(std::string_view sv) noexcept {
            if constexpr (Enabled) return out::write(*base, sv);
            else return out::ok<std::size_t>(0u);
        }
    };

    template <class S>
    using sink_ref_t = ansi_sink_ref<std::remove_reference_t<S>>;

    // 语法糖：ansi(sink) => 注入 ANSI 能力（默认启用）
    template <class S>
    constexpr auto enable(S& s) noexcept -> ansi_sink_ref<S, true> { return { &s }; }

    // 语法糖：ansi<false>(sink) => 注入但禁用 ANSI（编译期静默）
    template <bool Enabled, class S>
    constexpr auto with(S& s) noexcept -> ansi_sink_ref<S, Enabled> { return { &s }; }

    // 仅内部使用的实现细节
    namespace detail {
        // 用 concept 限定：只有带 write_ansi 的 sink 才吃 ANSI token
        template <class S>
        concept AnsiSink = requires(S& s, std::string_view sv) {
            { s.write_ansi(sv) } -> std::same_as<result<std::size_t>>;
        };

        // 生成序列：完全无堆，单次写入完整 ESC[...]m
        constexpr int fg_code(color c) noexcept {
            switch (c) {
            case color::black: return 30;
            case color::red: return 31;
            case color::green: return 32;
            case color::yellow: return 33;
            case color::blue: return 34;
            case color::magenta: return 35;
            case color::cyan: return 36;
            case color::white: return 37;
            default: return 39;
            }
        }
        constexpr int bg_code(color c) noexcept { return (fg_code(c) - 30) + 40; }

        template <AnsiSink S>
        inline result<std::size_t> write_code(S& s, int code) noexcept {
            char  buf[16];
            char* p = buf;

            *p++ = '\x1b';
            *p++ = '[';

            auto [ptr, ec] = std::to_chars(p, buf + sizeof(buf), code);
            if (ec != std::errc{}) return std::unexpected(errc::buffer_overflow);

            *ptr++ = 'm';
            return s.write_ansi(std::string_view{buf, static_cast<std::size_t>(ptr - buf)});
        }
    } // namespace detail

    // write_one 仅对具备 write_ansi 的 sink 生效
    template <detail::AnsiSink S>
    inline result<std::size_t> write_one(S& s, reset_t, fmt_spec) noexcept { return s.write_ansi("\x1b[0m"); }

    template <detail::AnsiSink S>
    inline result<std::size_t> write_one(S& s, bold_t, fmt_spec) noexcept { return s.write_ansi("\x1b[1m"); }

    template <detail::AnsiSink S>
    inline result<std::size_t> write_one(S& s, dim_t, fmt_spec) noexcept { return s.write_ansi("\x1b[2m"); }

    template <detail::AnsiSink S>
    inline result<std::size_t> write_one(S& s, italic_t, fmt_spec) noexcept { return s.write_ansi("\x1b[3m"); }

    template <detail::AnsiSink S>
    inline result<std::size_t> write_one(S& s, underline_t, fmt_spec) noexcept { return s.write_ansi("\x1b[4m"); }

    template <detail::AnsiSink S>
    inline result<std::size_t> write_one(S& s, fg_t v, fmt_spec) noexcept { return detail::write_code(s, detail::fg_code(v.c)); }

    template <detail::AnsiSink S>
    inline result<std::size_t> write_one(S& s, bg_t v, fmt_spec) noexcept { return detail::write_code(s, detail::bg_code(v.c)); }
} // namespace out::ansi

// ===== 语法糖：把常用符号再导出到 out 命名空间 =====
export namespace out {
    using ansi::color;
    using ansi::reset;
    using ansi::bold;
    using ansi::dim;
    using ansi::italic;
    using ansi::underline;
    using ansi::fg;
    using ansi::bg;

    template <bool Enabled = true, class S>
    constexpr auto ansi_with(S& s) noexcept { return ansi::with<Enabled>(s); }

    constexpr ansi::fg_t operator!(color c) noexcept { return {c}; }  // !color::red => 前景色
    constexpr ansi::bg_t operator~(color c) noexcept { return {c}; }  // ~color::red => 背景色
}
