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
/// \brief Defines the LoggerSeverity enum which essentially represents the
/// level for logging.
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

/// \brief The internal LogMessage which contains a timestamp, the source
/// location, the logger severity and the actual message.
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

/// \brief Represents a simple singleton Logger class design for writing to
/// stdout and files.
class Logger final
{
  public:
    /// \brief The default constructor.
    Logger() = default;
    /// \brief Constructor.
    /// \param _filename The filename where the log file will be written and
    /// saved to. \param _console Whether to output the log to the console or
    /// not.
    Logger(std::string const& _filename, bool _console = true);

    /// \brief Creates an instance for the logger or returns the current
    /// instance if one has already been created.
    static Logger& instance() noexcept;

    /// \brief Helper function for logging to console.
    template <typename FormatString, typename... Args>
    void log(source_location const& _loc, LoggerSeverity _severity,
             FormatString const& _fmt, Args&&... _args)
    {
        if (shouldLog(_severity) && console_ && fileStream_.is_open()) {
            _log(_loc, _severity, _fmt, std::forward<Args>(_args)...);
        }
    }

    /// \brief Helper function for logging to console.
    template <typename... Args>
    void print(std::string_view _fmt, Args&&... _args)
    {
        if (console_) {
            fmt::print(_fmt, std::forward<Args>(_args)...);
        }
    }

    /// \brief Helper function for logging to console.
    template <typename... Args>
    void print(FILE* _file, std::string_view _fmt, Args&&... _args)
    {
        if (console_) {
            fmt::print(_file, _fmt, std::forward<Args>(_args)...);
        }
    }

    /// \brief Returns the severity for the logger.
    LoggerSeverity severity() const noexcept;

    /// \brief Sets the severity for the logger.
    /// \param _severity The severity to set.
    void severity(LoggerSeverity _severity) noexcept;

    /// \brief Returns the filename for the logger.
    std::string const& filename() const noexcept;

    /// \brief Opens a filestream with a given filename.
    /// \param _filename The file to open.
    void open(std::string&& _filename);

    /// \brief Returns true if the output should be written to the console,
    /// otherwise returns false.
    bool hasConsoleSink() const noexcept;

  private:
    /// \brief Returns true if the given log severity is active.
    /// \param _severity The given log severity to check.
    constexpr bool shouldLog(LoggerSeverity _severity) const noexcept
    {
        return _severity >= severity_;
    }

    /// \brief Returns the color by a given log severity.
    /// \param _severity The given log severity.
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

    /// \brief Helper function for logging to console.
    void log(details::LogMessage const& _msg, LoggerSeverity _severity);

    /// \brief Helper function for logging to console.
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

    /// The file to save the log file.
    std::string filename_;
    /// Whether to write to the console or not.
    bool console_{true};
    /// The filestream.
    std::ofstream fileStream_;
    /// The logger severity.
    LoggerSeverity severity_{LoggerSeverity::Info};
};
} // namespace amadeus

/// \def Internal macro for invoking the log function with the singleton and a
/// given log severity.
#define LOGGER_CALL(level, ...)                                                \
    (Logger::instance()).log(source_location::current(), level, __VA_ARGS__)

/// \def Macro for invoking the log function with the INFO severity.
#define LOG_INFO(...) LOGGER_CALL(LoggerSeverity::Info, __VA_ARGS__)

/// \def Macro for invoking the log function with the DEBUG severity.
#define LOG_DEBUG(...) LOGGER_CALL(LoggerSeverity::Debug, __VA_ARGS__)

/// \def Macro for invoking the log function with the WARN severity.
#define LOG_WARN(...) LOGGER_CALL(LoggerSeverity::Warn, __VA_ARGS__)

/// \def Macro for invoking the log function with the ERROR severity.
#define LOG_ERROR(...) LOGGER_CALL(LoggerSeverity::Error, __VA_ARGS__)

/// \def Macro for invoking the log function with the FATAL severity.
#define LOG_FATAL(...) LOGGER_CALL(LoggerSeverity::Fatal, __VA_ARGS__)

#endif // !WEBSOCKET_SERVER_LOGGER_HH
