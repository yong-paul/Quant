#include "AsyncLogger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <windows.h>

namespace QuantTrading {

AsyncLogger::AsyncLogger() 
    : minLevel_(LogLevel::DEBUG)
    , maxFileSize_(10 * 1024 * 1024)
    , maxFiles_(5)
    , running_(false)
    , currentFileSize_(0) {
}

AsyncLogger::~AsyncLogger() {
    stop();
}

void AsyncLogger::init(const std::string& logDir,
                      const std::string& logPrefix,
                      LogLevel minLevel,
                      size_t maxFileSize,
                      size_t maxFiles) {
    logDir_ = logDir;
    logPrefix_ = logPrefix;
    minLevel_ = minLevel;
    maxFileSize_ = maxFileSize;
    maxFiles_ = maxFiles;

    // 创建日志目录
    std::filesystem::create_directories(logDir_);

    // 生成初始日志文件名
    currentLogFile_ = logDir_ + "/" + logPrefix_ + "_" + getCurrentTimestamp() + ".log";
    logFile_.open(currentLogFile_, std::ios::app);
    if (!logFile_.is_open()) {
        throw std::runtime_error("Failed to open log file: " + currentLogFile_);
    }

    // 启动日志处理线程
    running_ = true;
    logThread_ = std::thread(&AsyncLogger::processLogs, this);
}

void AsyncLogger::stop() {
    if (running_) {
        running_ = false;
        queueCondition_.notify_one();
        if (logThread_.joinable()) {
            logThread_.join();
        }
        if (logFile_.is_open()) {
            logFile_.close();
        }
    }
}

void AsyncLogger::log(LogLevel level, const std::string& message,
                     const std::string& file, int line, const std::string& function) {
    if (level < minLevel_) {
        return;
    }

    LogMessage logMsg;
    logMsg.level = level;
    logMsg.timestamp = getCurrentTimestamp();
    logMsg.message = message;
    logMsg.file = file;
    logMsg.line = line;
    logMsg.function = function;

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        logQueue_.push(logMsg);
    }
    queueCondition_.notify_one();
}

void AsyncLogger::processLogs() {
    while (running_) {
        std::unique_lock<std::mutex> lock(queueMutex_);
        queueCondition_.wait(lock, [this] { return !logQueue_.empty() || !running_; });

        while (!logQueue_.empty()) {
            LogMessage msg = logQueue_.front();
            logQueue_.pop();
            lock.unlock();

            // 检查并轮转日志文件
            checkAndRotateLogFile();

            // 格式化日志消息
            std::stringstream ss;
            ss << "[" << msg.timestamp << "] "
               << "[" << getLevelString(msg.level) << "] "
               << "[" << msg.file << ":" << msg.line << "] "
               << "[" << msg.function << "] "
               << msg.message << std::endl;

            // 写入日志文件
            logFile_ << ss.str();
            logFile_.flush();
            currentFileSize_ += ss.str().length();

            lock.lock();
        }
    }
}

std::string AsyncLogger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:               return "UNKNOWN";
    }
}

std::string AsyncLogger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm timeInfo;
    localtime_s(&timeInfo, &time);

    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%Y%m%d_%H%M%S")
       << "_" << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

void AsyncLogger::checkAndRotateLogFile() {
    if (currentFileSize_ >= maxFileSize_) {
        // 关闭当前日志文件
        logFile_.close();

        // 删除最旧的日志文件（如果超过最大文件数）
        std::vector<std::string> logFiles;
        for (const auto& entry : std::filesystem::directory_iterator(logDir_)) {
            if (entry.path().extension() == ".log") {
                logFiles.push_back(entry.path().string());
            }
        }

        if (logFiles.size() >= maxFiles_) {
            std::sort(logFiles.begin(), logFiles.end());
            std::filesystem::remove(logFiles.front());
        }

        // 创建新的日志文件
        currentLogFile_ = logDir_ + "/" + logPrefix_ + "_" + getCurrentTimestamp() + ".log";
        logFile_.open(currentLogFile_, std::ios::app);
        currentFileSize_ = 0;
    }
}

} // namespace QuantTrading 