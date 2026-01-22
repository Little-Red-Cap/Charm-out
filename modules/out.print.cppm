module;
#include <expected>
export module out.print;
import out.format;
import out.sink;
import out.core;
import out.port;

export namespace out {

    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> print(S& sink, Args&&... args) noexcept {
        return vprint<Fmt>(sink, std::forward<Args>(args)...);
    }

    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> print(Args&&... args) noexcept {
        return print<Fmt>(port::default_console(), std::forward<Args>(args)...);
    }

    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> println(S& sink, Args&&... args) noexcept {
        auto r = print<Fmt>(sink, std::forward<Args>(args)...);
        if (!r) return r;
        // Note: newline policy is handled by logger, not by print/println.
        auto rn = write(sink, "\r\n");
        if (!rn) return std::unexpected(rn.error());
        return *r + *rn;
    }

    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> println(Args&&... args) noexcept {
        return println<Fmt>(port::default_console(), std::forward<Args>(args)...);
    }

}
