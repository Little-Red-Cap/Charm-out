module;
#include <cstdint>
export module out.domain;


export namespace out {

    enum class level : std::uint8_t { off, error, warn, info, debug, trace };

    // 编译期日志级别：✅ 唯一允许的宏用途：选择编译期常量（不参与逻辑）完全零开销的日志过滤
    inline constexpr level build_level =
    #if defined(LOG_LEVEL_TRACE)
      level::trace;
#elif defined(LOG_LEVEL_DEBUG)
          level::debug;
#elif defined(LOG_LEVEL_INFO)
              level::info;
#elif defined(LOG_LEVEL_WARN)
                  level::warn;
#elif defined(LOG_LEVEL_ERROR)
                      level::error;
#else
                          level::off;
#endif

    // 域/标签：用于编译期过滤不同模块的日志，先做轻量版（只做编译期筛选的“类型标签”）
    struct default_domain {};

    // template <class T>
    // struct domain_t { using type = T; };

    // 域过滤器：编译期启用/禁用特定域
    template <class Domain>
    inline constexpr bool domain_enabled = true;

    // 用法示例：
    // template <> inline constexpr bool domain_enabled<my_module> = false;

}
