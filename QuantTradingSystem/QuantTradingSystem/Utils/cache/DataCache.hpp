#pragma once
#include "DataCache.h"
#include <algorithm>

template<typename T>
void DataCache<T>::addData(const std::string& key, const T& data, size_t maxSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto& queue = dataMap_[key];
    queue.push_back(data);
    
    if (queue.size() > maxSize) {
        queue.pop_front();
    }
}

template<typename T>
T DataCache<T>::getLatestData(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = dataMap_.find(key);
    if (it != dataMap_.end() && !it->second.empty()) {
        return it->second.back();
    }
    
    return T();
}

template<typename T>
std::vector<T> DataCache<T>::getHistoryData(const std::string& key, size_t count) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = dataMap_.find(key);
    if (it == dataMap_.end()) {
        return std::vector<T>();
    }
    
    const auto& queue = it->second;
    count = std::min(count, queue.size());
    
    std::vector<T> result;
    result.reserve(count);
    
    auto start = queue.end() - count;
    std::copy(start, queue.end(), std::back_inserter(result));
    
    return result;
}

template<typename T>
void DataCache<T>::clearData(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex_);
    dataMap_.erase(key);
}

template<typename T>
void DataCache<T>::clearAll() {
    std::lock_guard<std::mutex> lock(mutex_);
    dataMap_.clear();
}

template<typename T>
size_t DataCache<T>::getDataCount(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = dataMap_.find(key);
    if (it != dataMap_.end()) {
        return it->second.size();
    }
    
    return 0;
} 