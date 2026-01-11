module;
#include <cstdint>
#include <span>
#include <expected>
export module out.core;


export namespace out {

    enum class errc : std::uint8_t {
        ok = 0,
        io_error,
        // io_fault,
        // would_block,
        buffer_overflow,
        invalid_format,
        not_supported,
    };

    template <class T>
    using result = std::expected<T, errc>;

    using bytes = std::span<const std::byte>;
    using cbytes = std::span<const char>;

    // 小工具：把任意 trivially copyable 的对象视作字节序列（可选用）
    template <class T>
    constexpr bytes as_bytes(std::span<const T> s) noexcept {
        static_assert(std::is_trivially_copyable_v<T>);
        return {reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
    }

    // 建议统一用 ok(...) 构造成功值，避免各种编译器在 expected 转换构造上出幺蛾子
    // 小工具：显式构造成功值，避免不同编译器对 expected 的隐式转换差异
    template <class T>
    constexpr result<std::remove_cvref_t<T>> ok(T&& v) noexcept {
        return result<std::remove_cvref_t<T>>{std::in_place, std::forward<T>(v)};
    }
}
