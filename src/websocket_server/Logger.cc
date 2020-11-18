#include "websocket_server/Logger.hh"

#include <chrono>
#include <ostream>

using namespace amadeus;
using namespace std::string_view_literals;

namespace amadeus {
namespace details {
template <typename T>
std::string timeFormat(std::chrono::time_point<T> time)
{
    using namespace std::chrono;

    auto curr_time = T::to_time_t(time);
    char sRep[100];
    strftime(sRep, sizeof(sRep), "%Y-%m-%d %H:%M:%S", localtime(&curr_time));

    auto since_epoch = time.time_since_epoch();
    auto const s = duration_cast<seconds>(since_epoch);
    since_epoch -= s;
    auto const milli = duration_cast<milliseconds>(since_epoch);

    return fmt::format("{}:{}", sRep, milli.count());
}

LogMessage::LogMessage(ClockType::time_point _logTime, source_location _loc,
                       LoggerSeverity _severity, std::string_view const& _msg)
    : time_(_logTime)
    , severity_(_severity)
    , source_(_loc)
    , message_(_msg)
{
}

LogMessage::LogMessage(source_location _loc, LoggerSeverity _severity,
                       std::string_view const& _msg)
    : LogMessage(ClockType::now(), _loc, _severity, _msg)
{
}

LogMessage::LogMessage(LoggerSeverity _severity, std::string_view const& _msg)
    : LogMessage(ClockType::now(), source_location::current(), _severity, _msg)
{
}
} // namespace details
} // namespace amadeus

#ifdef _WIN32
static void setColorMode()
{
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
}
#endif

Logger::Logger(std::string const& _filename, bool _console /*= true*/)
    : filename_(_filename)
    , console_(_console)
    , fileStream_(_filename)
{
#ifdef _WIN32
    setColorMode();
#endif
}

Logger& Logger::instance() noexcept
{
    static Logger instance{};
    return instance;
}

LoggerSeverity Logger::severity() const noexcept
{
    return severity_;
}

void Logger::severity(LoggerSeverity _severity) noexcept
{
    severity_ = _severity;
}

std::string const& Logger::filename() const noexcept
{
    return filename_;
}

void Logger::open(std::string&& _filename)
{
    filename_ = std::move(_filename);
    fileStream_.open(filename_);
}

bool Logger::hasConsoleSink() const noexcept
{
    return console_;
}

constexpr auto severityName(LoggerSeverity _severity) noexcept
    -> std::string_view
{
    switch (_severity) {
    case LoggerSeverity::Info:
        return "INFO"sv;
    case LoggerSeverity::Debug:
        return "DEBUG"sv;
    case LoggerSeverity::Warn:
        return "WARN"sv;
    case LoggerSeverity::Error:
        return "ERROR"sv;
    case LoggerSeverity::Fatal:
        return "FATAL"sv;
    default:
        break;
    }
    return "unknown"sv;
}

void Logger::log(details::LogMessage const& _msg, LoggerSeverity _severity)
{
    auto const color = colorBySeverity(_severity);
    // Example: [INFO] <-- With dedicated color
    auto const severityColor = fmt::format(fmt::emphasis::bold | fmt::fg(color),
                                           severityName(_severity));

    // ignore file path
    std::string_view fileName{_msg.source().file_name()};
#ifdef _WIN32
    char constexpr Delimiter{'\\'};
#else
    char constexpr Delimiter{'/'};
#endif
    auto const filePos = fileName.find_last_of(Delimiter) + 1;
    fileName = fileName.substr(filePos);

    auto const formatted =
        fmt::format("[{}] [{}] {} {}:{} {}", details::timeFormat(_msg.time()),
                    severityColor, _msg.source().function_name(), fileName,
                    _msg.source().line(), _msg.message());

    // Write to file stream
    auto const severityNoColor = fmt::format(severityName(_severity));

    fileStream_ << fmt::format("[{}] [{}] {} {}:{} {}",
                               details::timeFormat(_msg.time()),
                               severityNoColor, _msg.source().function_name(),
                               fileName, _msg.source().line(), _msg.message());
    fileStream_.flush();

    // Write to console
    std::fwrite(formatted.c_str(), sizeof(char), formatted.size(), stdout);
    std::fflush(stdout);
}