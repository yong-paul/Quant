#pragma once
#include <string>
#include <chrono>
#include <ctime>
#include <unordered_set>
class TimeUtil {
public:
    // 获取当前时间戳（毫秒）
    static int64_t getCurrentTimestamp();
    
    // 获取当前日期字符串 (YYYYMMDD)
    static std::string getCurrentDate();
    
    // 获取当前时间字符串 (HH:MM:SS.mmm)
    static std::string getCurrentTime();
    
    // 获取当前完整时间字符串 (YYYY-MM-DD HH:MM:SS.mmm)
    static std::string getCurrentDateTime();
    
    // 时间字符串转时间戳
    static int64_t stringToTimestamp(const std::string& timeStr, const std::string& format = "%Y%m%d %H:%M:%S");
    
    // 时间戳转字符串
    static std::string timestampToString(int64_t timestamp, const std::string& format = "%Y-%m-%d %H:%M:%S");
    
    // 交易日相关
    static bool isTradingTime();  // 是否在交易时间
    static bool isMarketOpen();   // 市场是否开市
    static std::string getNextTradingDay(const std::string& date);  // 获取下一个交易日
    
private:
    static const std::string TRADING_DAYS_FILE;  // 交易日历文件路径
    static std::unordered_set<std::string> tradingDays_;  // 交易日缓存
}; 