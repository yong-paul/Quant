#pragma once
#include <string>
#include <memory>
#include <unordered_map>
#include <deque>
#include <mutex>

template<typename T>
class DataCache {
public:
    // 添加数据
    void addData(const std::string& key, const T& data, size_t maxSize = 1000);
    
    // 获取最新数据
    T getLatestData(const std::string& key) const;
    
    // 获取历史数据
    std::vector<T> getHistoryData(const std::string& key, size_t count = 100) const;
    
    // 清除指定key的数据
    void clearData(const std::string& key);
    
    // 清除所有数据
    void clearAll();
    
    // 获取数据数量
    size_t getDataCount(const std::string& key) const;
    
private:
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::deque<T>> dataMap_;
}; 