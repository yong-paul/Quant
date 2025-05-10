 #include "TimeUtil.h"
#include <iomanip>
#include <sstream>
#include <fstream>
#include "../logger/Logger.h"

const std::string TimeUtil::TRADING_DAYS_FILE = "config/trading_days.txt";
std::unordered_set<std::string> TimeUtil::tradingDays_;

int64_t TimeUtil::getCurrentTimestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

std::string TimeUtil::getCurrentDate() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d");
    return ss.str();
}

std::string TimeUtil::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%H:%M:%S");
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count() % 1000;
    ss << "." << std::setfill('0') << std::setw(3) << ms;
    return ss.str();
}

std::string TimeUtil::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count() % 1000;
    ss << "." << std::setfill('0') << std::setw(3) << ms;
    return ss.str();
}

int64_t TimeUtil::stringToTimestamp(const std::string& timeStr, const std::string& format) {
    std::tm tm = {};
    std::stringstream ss(timeStr);
    ss >> std::get_time(&tm, format.c_str());
    if (ss.fail()) {
        Logger::error("TimeUtil", "Failed to parse time string: {}", timeStr);
        return 0;
    }
    auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        tp.time_since_epoch()
    ).count();
}

std::string TimeUtil::timestampToString(int64_t timestamp, const std::string& format) {
    auto tp = std::chrono::system_clock::time_point(
        std::chrono::milliseconds(timestamp)
    );
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), format.c_str());
    return ss.str();
}

bool TimeUtil::isTradingTime() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto tm = *std::localtime(&time);
    
    // 检查是否是交易日
    if (!isMarketOpen()) {
        return false;
    }
    
    // 检查时间段
    int currentTime = tm.tm_hour * 100 + tm.tm_min;
    return (currentTime >= 930 && currentTime <= 1130) ||  // 上午
           (currentTime >= 1300 && currentTime <= 1500);   // 下午
}

bool TimeUtil::isMarketOpen() {
    if (tradingDays_.empty()) {
        // 加载交易日历
        std::ifstream file(TRADING_DAYS_FILE);
        if (!file) {
            Logger::error("TimeUtil", "Failed to open trading days file: {}", TRADING_DAYS_FILE);
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            tradingDays_.insert(line);
        }
    }
    
    return tradingDays_.find(getCurrentDate()) != tradingDays_.end();
}

std::string TimeUtil::getNextTradingDay(const std::string& date) {
    if (tradingDays_.empty()) {
        isMarketOpen();  // 加载交易日历
    }
    
    auto it = tradingDays_.find(date);
    if (it != tradingDays_.end()) {
        ++it;
        if (it != tradingDays_.end()) {
            return *it;
        }
    }
    
    return "";
}