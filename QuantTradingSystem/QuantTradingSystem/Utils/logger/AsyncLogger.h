#pragma once

#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>
#include <fstream>
#include <memory>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <direct.h>
#include <io.h>
#include <vector>
#include <algorithm>

namespace QuantTrading {

// 日志级别枚举
enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR,
    FATAL
};

// 日志消息结构
struct LogMessage {
    LogLevel level;
    std::string timestamp;
    std::string message;
    std::string file;
    int line;
    std::string function;
};

class AsyncLogger {
public:
    static AsyncLogger& getInstance() {
        static AsyncLogger instance;
        return instance;
    }

    // 初始化日志系统
    void init(const std::string& logDir = "logs", 
              const std::string& logPrefix = "quant_trading",
              LogLevel minLevel = LogLevel::DEBUG,
              size_t maxFileSize = 10 * 1024 * 1024,  // 10MB
              size_t maxFiles = 5);

    // 停止日志系统
    void stop();

    // 日志记录接口
    void log(LogLevel level, const std::string& message, 
             const std::string& file, int line, const std::string& function);

private:
    AsyncLogger();
    ~AsyncLogger();

    // 禁止拷贝和赋值
    AsyncLogger(const AsyncLogger&) = delete;
    AsyncLogger& operator=(const AsyncLogger&) = delete;

    // 日志处理线程函数
    void processLogs();

    // 获取日志级别字符串
    std::string getLevelString(LogLevel level);

    // 获取当前时间戳字符串
    std::string getCurrentTimestamp();

    // 检查并轮转日志文件
    void checkAndRotateLogFile();

    // 创建目录
    bool createDirectory(const std::string& path);

    // 获取目录中的文件列表
    std::vector<std::string> getDirectoryFiles(const std::string& path, const std::string& extension);

    // 删除文件
    bool deleteFile(const std::string& path);

    // 成员变量
    std::string logDir_;
    std::string logPrefix_;
    LogLevel minLevel_;
    size_t maxFileSize_;
    size_t maxFiles_;
    std::string currentLogFile_;
    std::ofstream logFile_;
    std::queue<LogMessage> logQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;
    std::thread logThread_;
    std::atomic<bool> running_;
    std::atomic<size_t> currentFileSize_;
};

} // namespace QuantTrading

// 日志宏定义
#define LOG_DEBUG(msg) QuantTrading::AsyncLogger::getInstance().log(QuantTrading::LogLevel::DEBUG, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_INFO(msg) QuantTrading::AsyncLogger::getInstance().log(QuantTrading::LogLevel::INFO, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_WARNING(msg) QuantTrading::AsyncLogger::getInstance().log(QuantTrading::LogLevel::WARNING, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_ERROR(msg) QuantTrading::AsyncLogger::getInstance().log(QuantTrading::LogLevel::ERROR, msg, __FILE__, __LINE__, __FUNCTION__)
#define LOG_FATAL(msg) QuantTrading::AsyncLogger::getInstance().log(QuantTrading::LogLevel::FATAL, msg, __FILE__, __LINE__, __FUNCTION__) 