module;
#include <array>
#include <cstdint>
#include <expected>
#include <string_view>
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
    constexpr decltype(auto) eval(const lazy_t<F>& lz) { return lz.f(); }

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

    namespace detail {
        template <class S>
        struct sink_ref {
            S* base{};
            result<std::size_t> write(bytes b) const noexcept { return base->write(b); }
        };

        template <class S>
        constexpr S* base_ptr(S& s) noexcept { return std::addressof(s); }

        template <class S>
        constexpr S* base_ptr(sink_ref<S>& s) noexcept { return s.base; }

        template <class S, bool Enabled>
        constexpr S* base_ptr(ansi::ansi_sink_ref<S, Enabled>& s) noexcept { return s.base; }
    }

    enum class newline : std::uint8_t { none, lf, crlf };

    struct style_cmd {
        enum class kind : std::uint8_t { seq, fg, bg };
        kind k{};
        std::string_view seq{};
        int code = 0;
    };

    constexpr style_cmd make_style(reset_t) noexcept { return {style_cmd::kind::seq, "\x1b[0m", 0}; }
    constexpr style_cmd make_style(bold_t) noexcept { return {style_cmd::kind::seq, "\x1b[1m", 0}; }
    constexpr style_cmd make_style(dim_t) noexcept { return {style_cmd::kind::seq, "\x1b[2m", 0}; }
    constexpr style_cmd make_style(italic_t) noexcept { return {style_cmd::kind::seq, "\x1b[3m", 0}; }
    constexpr style_cmd make_style(underline_t) noexcept { return {style_cmd::kind::seq, "\x1b[4m", 0}; }
    constexpr style_cmd make_style(ansi::fg_t v) noexcept {
        return {style_cmd::kind::fg, {}, ansi::fg_code(v.c)};
    }
    constexpr style_cmd make_style(ansi::bg_t v) noexcept {
        return {style_cmd::kind::bg, {}, ansi::bg_code(v.c)};
    }
    template <class T>
    constexpr style_cmd make_style(const T&) noexcept {
        static_assert(out::dependent_false_v<T>, "unsupported style token");
        return {};
    }

    template <level L, class Domain, class Sink>
    struct logger {
        Sink sink;
        std::array<style_cmd, 8> styles{};
        std::uint8_t style_count = 0;
        bool auto_reset_enabled = true;
        bool with_timestamp = false;
        bool with_level = true;
        bool with_domain = false;
        newline nl = newline::crlf;

        explicit constexpr logger(Sink s) noexcept : sink(std::move(s)) {}

        template <class NewSink>
        constexpr auto with_sink(NewSink ns) const noexcept {
            logger<L, Domain, NewSink> out{std::move(ns)};
            copy_opts_to(out);
            return out;
        }

        template <bool Enabled = true>
        constexpr auto ansi() const noexcept {
            auto* base = detail::base_ptr(sink);
            using base_t = std::remove_reference_t<decltype(*base)>;
            return with_sink(ansi::ansi_sink_ref<base_t, Enabled>{base});
        }

        template <class NewDomain>
        constexpr auto domain() const noexcept {
            logger<L, NewDomain, Sink> out{sink};
            copy_opts_to(out);
            return out;
        }

        constexpr logger& auto_reset(bool on) noexcept { auto_reset_enabled = on; return *this; }
        constexpr logger& no_reset() noexcept { auto_reset_enabled = false; return *this; }
        constexpr logger& reset_on() noexcept { auto_reset_enabled = true; return *this; }
        constexpr logger& timestamp() noexcept { with_timestamp = true; return *this; }
        constexpr logger& level_prefix(bool on = true) noexcept { with_level = on; return *this; }
        constexpr logger& domain_prefix(bool on = true) noexcept { with_domain = on; return *this; }
        constexpr logger& set_newline(newline n) noexcept { nl = n; return *this; }

        template <class... Tokens>
        constexpr logger& style(Tokens&&... tokens) noexcept {
            (push_style(make_style(std::forward<Tokens>(tokens))), ...);
            return *this;
        }

        template <fixed_string Fmt, class... Args>
        inline result<std::size_t> print(Args&&... args) noexcept {
            return emit<false, Fmt>(std::forward<Args>(args)...);
        }

        template <fixed_string Fmt, class... Args>
        inline result<std::size_t> println(Args&&... args) noexcept {
            return emit<true, Fmt>(std::forward<Args>(args)...);
        }

    private:
        template <class Other>
        constexpr void copy_opts_to(Other& out) const noexcept {
            out.styles = styles;
            out.style_count = style_count;
            out.auto_reset_enabled = auto_reset_enabled;
            out.with_timestamp = with_timestamp;
            out.with_level = with_level;
            out.with_domain = with_domain;
            out.nl = nl;
        }

        static constexpr char level_tag() noexcept {
            if constexpr (L == level::error) return 'E';
            else if constexpr (L == level::warn) return 'W';
            else if constexpr (L == level::info) return 'I';
            else if constexpr (L == level::debug) return 'D';
            else if constexpr (L == level::trace) return 'T';
            else return ' ';
        }

        constexpr void push_style(style_cmd cmd) noexcept {
            if (style_count >= styles.size()) return;
            styles[style_count++] = cmd;
        }

        template <class S>
        inline result<std::size_t> write_style(S& s, const style_cmd& cmd) noexcept {
            if constexpr (ansi::AnsiSink<S>) {
                if (cmd.k == style_cmd::kind::seq) {
                    return s.write_ansi(cmd.seq);
                }
                return ansi::write_ansi_code(s, cmd.code);
            } else {
                return ok<std::size_t>(0u);
            }
        }

        template <bool WithNewline, fixed_string Fmt, class... Args>
        inline result<std::size_t> emit(Args&&... args) noexcept {
            if constexpr (build_level >= L && L != level::off && domain_enabled<Domain>) {
                std::size_t total = 0;

                if (with_timestamp) {
                    auto rts = out::print<"[{}] ">(sink, port::now_ms());
                    if (!rts) return std::unexpected(rts.error());
                    total += *rts;
                }

                if (with_level) {
                    char buf[4] = {'[', level_tag(), ']', ' '};
                    auto rp = write(sink, std::string_view{buf, sizeof(buf)});
                    if (!rp) return std::unexpected(rp.error());
                    total += *rp;
                }

                if (with_domain) {
                    if constexpr (domain_name<Domain>.size() != 0) {
                        auto r1 = write(sink, "[");
                        if (!r1) return std::unexpected(r1.error());
                        total += *r1;
                        auto r2 = write(sink, domain_name<Domain>);
                        if (!r2) return std::unexpected(r2.error());
                        total += *r2;
                        auto r3 = write(sink, "] ");
                        if (!r3) return std::unexpected(r3.error());
                        total += *r3;
                    }
                }

                for (std::uint8_t i = 0; i < style_count; ++i) {
                    auto rs = write_style(sink, styles[i]);
                    if (!rs) return std::unexpected(rs.error());
                    total += *rs;
                }

                auto r = vprint<Fmt>(sink, eval(std::forward<Args>(args))...);
                if (!r) return std::unexpected(r.error());
                total += *r;

                if (auto_reset_enabled && style_count > 0) {
                    auto rr = write_style(sink, make_style(reset_t{}));
                    if (!rr) return std::unexpected(rr.error());
                    total += *rr;
                }

                if constexpr (WithNewline) {
                    if (nl != newline::none) {
                        std::string_view nl_sv = (nl == newline::crlf) ? "\r\n" : "\n";
                        auto rn = write(sink, nl_sv);
                        if (!rn) return std::unexpected(rn.error());
                        total += *rn;

                        auto* base = detail::base_ptr(sink);
                        using base_t = std::remove_reference_t<decltype(*base)>;
                        if constexpr (Flushable<base_t>) {
                            auto rf = base->flush();
                            if (!rf) return std::unexpected(rf.error());
                            total += *rf;
                        }
                    }
                }

                return ok(total);
            } else {
                return ok(0u);
            }
        }
    };

    template <level L, class Domain = default_domain>
    inline auto log() noexcept {
        return logger<L, Domain, detail::sink_ref<port::console_sink>>{
            detail::sink_ref<port::console_sink>{&port::default_console()}
        };
    }

    template <level L, class Domain = default_domain, class S>
    inline auto log(S& s) noexcept {
        return logger<L, Domain, detail::sink_ref<S>>{detail::sink_ref<S>{&s}};
    }

    template <level L, class Domain = default_domain>
    inline auto logc() noexcept {
        return log<L, Domain>().template ansi<true>();
    }

    template <level L, class Domain = default_domain, class S>
    inline auto logc(S& s) noexcept {
        return log<L, Domain>(s).template ansi<true>();
    }

}
