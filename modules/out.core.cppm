module;
#include <cstdint>
#include <span>
#include <expected>
export module out.core;

// out.core intentionally contains only the minimal, cross-module primitives.
// Do NOT turn this into a general utilities header.

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

    // Utility: view any trivially copyable object as a byte span.
    template <class T>
    constexpr bytes as_bytes(std::span<const T> s) noexcept {
        static_assert(std::is_trivially_copyable_v<T>);
        return {reinterpret_cast<const std::byte*>(s.data()), s.size_bytes()};
    }

    // Utility: explicit success construction to avoid implicit expected conversions.
    // Prefer ok(...) to keep diagnostics consistent across compilers.
    template <class T>
    constexpr result<std::remove_cvref_t<T>> ok(T&& v) noexcept {
        return result<std::remove_cvref_t<T>>{std::in_place, std::forward<T>(v)};
    }

    template <class T>
    constexpr void discard(result<T>&&) noexcept {}

    template <class T>
    constexpr void discard(const result<T>&) noexcept {}
}
