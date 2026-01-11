module;
#include <span>
#include <string_view>
#include <expected>
#include <cstring>

export module out.sink;

import out.core;
// TODO: 增加编译期端序转换
export namespace out {

    // Sink 概念：任何实现 write(bytes) 的类型
    template <class S>
    concept Sink = requires(S& s, bytes b) {
        { s.write(b) } -> std::same_as<result<std::size_t>>;
    };

    // 便捷函数：从 string_view 写入
    template <Sink S>
    inline result<std::size_t> write(S& s, std::string_view sv) noexcept {
        auto b = bytes{reinterpret_cast<const std::byte*>(sv.data()), sv.size()};
        return s.write(b);
    }

    // ------------------------------------------------------------------
    // 轻量通用适配器（不再单独提供 out.sinks 模块）

    // 1) 固定缓冲区：便于单元测试/收集输出
    template <std::size_t N>
    struct buffer_sink {
        std::array<char, N> buf{};
        std::size_t pos = 0;

        result<std::size_t> write(bytes b) noexcept {
            if (pos + b.size() > N) return std::unexpected(errc::buffer_overflow);
            std::memcpy(buf.data() + pos, b.data(), b.size());
            pos += b.size();
            return ok(b.size());
        }

        std::string_view view() const noexcept { return {buf.data(), pos}; }
        void clear() noexcept { pos = 0; }
    };

    // 2) 空输出：用于静默/基准
    struct null_sink {
        result<std::size_t> write(bytes b) noexcept { return ok(b.size()); }
    };

    // 3) 行缓冲：累积一行后批量写入（用于 UART/慢速设备）
    template <Sink BaseSink, std::size_t BufSize = 128>
    struct line_buffered_sink {
        BaseSink& base;
        std::array<char, BufSize> buf{};
        std::size_t pos = 0;

        explicit line_buffered_sink(BaseSink& s) : base(s) {}

        result<std::size_t> write(bytes b) noexcept {
            auto data = reinterpret_cast<const char*>(b.data());
            for (std::size_t i = 0; i < b.size(); ++i) {
                if (pos >= BufSize) {
                    auto r = flush();
                    if (!r) return std::unexpected(r.error());
                }

                buf[pos++] = data[i];

                if (data[i] == '\n') {
                    auto r = flush();
                    if (!r) return std::unexpected(r.error());
                }
            }
            return ok(b.size());
        }

        result<std::size_t> flush() noexcept {
            if (pos == 0) return ok<std::size_t>(0u);
            auto b = bytes{reinterpret_cast<const std::byte*>(buf.data()), pos};
            auto r = base.write(b);
            if (r) pos = 0;
            return r;
        }

        ~line_buffered_sink() { (void)flush(); }
    };

}
