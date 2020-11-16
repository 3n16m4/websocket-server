#ifndef WEBSOCKET_SERVER_LOGGER_HH
#define WEBSOCKET_SERVER_LOGGER_HH

#include "websocket_server/utils/source_location.hh"

#include <fmt/color.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>

#include <fstream>
#include <string>
#include <string_view>
#include <fstream>
#include <chrono>

namespace amadeus {
enum class LoggerSeverity
{
    Info,
    Debug,
    Warn,
    Error,
    Fatal,
    Off
};

namespace details {
using ClockType = std::chrono::system_clock;

class LogMessage final
{
  public:
    LogMessage(ClockType::time_point _logTime, source_location _loc,
               LoggerSeverity _severity, std::string_view const& _msg);

    LogMessage(source_location _loc, LoggerSeverity _severity,
               std::string_view const& msg);

    LogMessage(LoggerSeverity _severity, std::string_view const& _msg);

    [[nodiscard]] LoggerSeverity severity() const noexcept
    {
        return severity_;
    }

    [[nodiscard]] ClockType::time_point time() const noexcept
    {
        return time_;
    }

    [[nodiscard]] source_location source() const noexcept
    {
        return source_;
    }

    [[nodiscard]] std::string_view message() const noexcept
    {
        return message_;
    }

  private:
    ClockType::time_point time_;
    LoggerSeverity severity_{LoggerSeverity::Off};
    source_location source_;
    std::string_view message_;
};
} // namespace details

class Logger final
{
  public:
    Logger() = default;
    explicit Logger(std::string const& _filename, bool _console = true);

    static Logger& instance() noexcept;

    template <typename FormatString, typename... Args>
    void log(source_location const& _loc, LoggerSeverity _severity,
             FormatString const& _fmt, Args&&... _args)
    {
        if (shouldLog(_severity) && console_ && fileStream_.is_open()) {
            _log(_loc, _severity, _fmt, std::forward<Args>(_args)...);
        }
    }

    template <typename... Args>
    void print(std::string_view _fmt, Args&&... _args)
    {
        if (console_) {
            fmt::print(_fmt, std::forward<Args>(_args)...);
        }
    }

    template <typename... Args>
    void print(FILE* _file, std::string_view _fmt, Args&&... _args)
    {
        if (console_) {
            fmt::print(_file, _fmt, std::forward<Args>(_args)...);
        }
    }

    LoggerSeverity severity() const noexcept;

    void severity(LoggerSeverity _severity) noexcept;

    std::string const& filename() const noexcept;

    void open(std::string&& _filename);

    bool hasConsoleSink() const noexcept;

  private:
    constexpr bool shouldLog(LoggerSeverity _severity) const noexcept
    {
        return _severity >= severity_;
    }

    constexpr fmt::color colorBySeverity(LoggerSeverity _severity) noexcept
    {
        switch (_severity) {
        case LoggerSeverity::Info:
            return fmt::color::green;
        case LoggerSeverity::Debug:
            return fmt::color::aqua;
        case LoggerSeverity::Warn:
            return fmt::color::gold;
        case LoggerSeverity::Error:
            return fmt::color::pink;
        case LoggerSeverity::Fatal:
            return fmt::color::red;
        case LoggerSeverity::Off:
            return fmt::color::black;
        }
        return fmt::color::white;
    }

    void log(details::LogMessage const& _msg, LoggerSeverity _severity);

    template <typename FormatString, typename... Args>
    void _log(source_location _loc, LoggerSeverity _severity,
              FormatString const& _fmt, Args&&... _args)
    {
        fmt::memory_buffer buf;
        fmt::format_to(buf, _fmt, std::forward<Args>(_args)...);
        details::LogMessage msg(_loc, _severity,
                                std::string_view(buf.data(), buf.size()));
        log(msg, _severity);
    }

    static void setColorMode()
    {
#ifdef _WIN32
        if (HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            hOut != INVALID_HANDLE_VALUE) {
            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode)) {
                return;
            }

            dwMode |= 0x0004;
            if (!SetConsoleMode(hOut, dwMode)) {
                return;
            }
        }
#endif
    }

    std::string filename_;
    bool console_{true};
    std::ofstream fileStream_;
    LoggerSeverity severity_{LoggerSeverity::Info};
};
} // namespace amadeus

#define LOGGER_CALL(level, ...)                                                \
    (Logger::instance()).log(source_location::current(), level, __VA_ARGS__)

#define LOG_INFO(...) LOGGER_CALL(LoggerSeverity::Info, __VA_ARGS__)

#define LOG_DEBUG(...) LOGGER_CALL(LoggerSeverity::Debug, __VA_ARGS__)

#define LOG_WARN(...) LOGGER_CALL(LoggerSeverity::Warn, __VA_ARGS__)

#define LOG_ERROR(...) LOGGER_CALL(LoggerSeverity::Error, __VA_ARGS__)

#define LOG_FATAL(...) LOGGER_CALL(LoggerSeverity::Fatal, __VA_ARGS__)

#endif // !WEBSOCKET_SERVER_LOGGER_HH