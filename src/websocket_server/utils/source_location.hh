#ifndef WEBSOCKET_SERVER_SOURCE_LOCATION_HH
#define WEBSOCKET_SERVER_SOURCE_LOCATION_HH

#include <cstdint>

namespace amadeus {
/// \brief Helper class because this project is built with a C++17 compiler and
/// not C++20.
class source_location
{
  public:
    // 14.1.2, source_location creation
    static constexpr source_location
    current(const char* _file = __builtin_FILE(),
            const char* _func = __builtin_FUNCTION(),
            int _line = __builtin_LINE(), int _col = 0) noexcept
    {
        source_location loc;
        loc.file_ = _file;
        loc.func_ = _func;
        loc.line_ = _line;
        loc.col_ = _col;
        return loc;
    }

    // 14.1.3, source_location field access
    [[nodiscard]] constexpr std::int32_t line() const noexcept
    {
        return line_;
    }
    [[nodiscard]] constexpr std::int32_t column() const noexcept
    {
        return col_;
    }
    [[nodiscard]] constexpr const char* file_name() const noexcept
    {
        return file_;
    }
    [[nodiscard]] constexpr const char* function_name() const noexcept
    {
        return func_;
    }

  private:
    const char* file_{"unknown"};
    const char* func_{file_};
    std::int32_t line_{0};
    std::int32_t col_{0};
};
} // namespace amadeus

#endif // !WEBSOCKET_SERVER_SOURCE_LOCATION_HH
