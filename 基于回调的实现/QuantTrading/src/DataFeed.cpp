#include "DataFeed.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

class DataFeed : public IDataFeed {
private:
    DataCallback m_callback;
    bool m_running = false;

public:
    void subscribe(const std::vector<std::string>& symbols) override {
        std::cout << "Subscribed to symbols: ";
        for (const auto& symbol : symbols) {
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
    }

    void unsubscribe(const std::vector<std::string>& symbols) override {
        std::cout << "Unsubscribed from symbols: ";
        for (const auto& symbol : symbols) {
            std::cout << symbol << " ";
        }
        std::cout << std::endl;
    }

    void registerCallback(DataCallback callback) override {
        m_callback = callback;
    }

    void start() override {
        m_running = true;
        std::cout << "Data feed started" << std::endl;
        
        // 模拟数据生成线程
        std::thread([this]() {
            while (m_running) {
                // 模拟生成市场数据
                MarketData data;
                data.symbol = "AAPL";
                data.price = 100.0 + (rand() % 100) / 10.0;
                
                // 触发回调
                if (m_callback) {
                    m_callback(data);
                }
                
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }).detach();
    }

    void stop() override {
        m_running = false;
        std::cout << "Data feed stopped" << std::endl;
    }
};