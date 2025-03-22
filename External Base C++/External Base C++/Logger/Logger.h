
class Logger {
public:
    enum class LogLevel { LOG_INFO, LOG_WARNING, LOG_ERROR };

    static Logger& getInstance();

    template <typename... Args>
    void log(LogLevel level, Args&&... args) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::ostringstream oss;
        (oss << ... << args);

        std::string logMessage = "[" + getCurrentTimestamp() + "] [" + getLogLevelString(level) + "] " + oss.str() + "\n";

        std::cout << logMessage;
        logfile_ << logMessage;
        logfile_.flush();
    }

private:
    std::ofstream logfile_;
    std::mutex mutex_;

    Logger();
    ~Logger();

    std::string getLogLevelString(LogLevel level);
    std::string getCurrentTimestamp();
};

#define LOG_INFO(...) Logger::getInstance().log(Logger::LogLevel::LOG_INFO, __VA_ARGS__)
#define LOG_WARNING(...) Logger::getInstance().log(Logger::LogLevel::LOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...) Logger::getInstance().log(Logger::LogLevel::LOG_ERROR, __VA_ARGS__)
