module;
#include <utility>
export module out.api;

export import out.ansi;
export import out.core;
export import out.domain;
export import out.format;
export import out.print;
export import out.port;
export import out.sink;

export namespace out {

    // 懒求值包装器（可选，但非常实用）
    template <class F>
    struct lazy_t { F f; };

    template <class F>
    constexpr auto lazy(F&& f) { return lazy_t<std::decay_t<F>>{std::forward<F>(f)}; }

    template <class T>
    constexpr decltype(auto) eval(T&& v) { return std::forward<T>(v); }

    template <class F>
    constexpr decltype(auto) eval(lazy_t<F>& lz) { return lz.f(); }

    template <class F>
    constexpr decltype(auto) eval(lazy_t<F>&& lz) { return lz.f(); }

    // ✅ 统一入口：带 level + domain
    template <level L, class Domain, fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> emit(S& sink, Args&&... args) noexcept {
        if constexpr (build_level >= L && L != level::off && domain_enabled<Domain>) {
            return println<Fmt>(sink, eval(std::forward<Args>(args))...);
        } else {
            return ok(0u);
        }
    }

    // ✅ 门面：默认域 default_domain
    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> error(S& s, Args&&... a) noexcept {
        return emit<level::error, default_domain, Fmt>(s, std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> error(Args&&... a) noexcept {
        return error<Fmt>(port::default_console(), std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> warn(S& s, Args&&... a) noexcept {
        return emit<level::warn, default_domain, Fmt>(s, std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> warn(Args&&... a) noexcept {
        return warn<Fmt>(port::default_console(), std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> info(S& s, Args&&... a) noexcept {
        return emit<level::info, default_domain, Fmt>(s, std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> info(Args&&... a) noexcept {
        return info<Fmt>(port::default_console(), std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> debug(S& s, Args&&... a) noexcept {
        return emit<level::debug, default_domain, Fmt>(s, std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> debug(Args&&... a) noexcept {
        return debug<Fmt>(port::default_console(), std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> trace(S& s, Args&&... a) noexcept {
        return emit<level::trace, default_domain, Fmt>(s, std::forward<Args>(a)...);
    }
    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> trace(Args&&... a) noexcept {
        return trace<Fmt>(port::default_console(), std::forward<Args>(a)...);
    }

}
