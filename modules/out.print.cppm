module;
#include <expected>
export module out.print;
import out.format;
import out.sink;
import out.core;
import out.port;

export namespace out {

    // ----------------------------
    // Checked APIs (result<T>)
    // ----------------------------
    template <fixed_string Fmt, Sink S, class... Args>
    inline result<std::size_t> try_print(S& sink, Args&&... args) noexcept {
        return vprint<Fmt>(sink, std::forward<Args>(args)...);
    }

    template <fixed_string Fmt, class... Args>
    inline result<std::size_t> try_print(Args&&... args) noexcept {
        return try_print<Fmt>(port::default_console(), std::forward<Args>(args)...);
    }

    // ----------------------------
    // Fire-and-forget APIs (void)
    // ----------------------------
    template <fixed_string Fmt, Sink S, class... Args>
    inline void print(S& sink, Args&&... args) noexcept {
        out::discard(try_print<Fmt>(sink, std::forward<Args>(args)...));
    }

    template <fixed_string Fmt, class... Args>
    inline void print(Args&&... args) noexcept {
        out::discard(try_print<Fmt>(std::forward<Args>(args)...));
    }

}
