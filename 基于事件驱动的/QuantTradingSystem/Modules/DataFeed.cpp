#include "DataFeed.h"
#include <iostream>
#include <thread>

class ExchangeDataFeed : public DataFeed {
public:
    void init(const std::string& configPath) override {
        // 初始化交易所API连接
        std::cout << "Exchange API initialized with config: " << configPath << std::endl;
    }
    
    void start() override {
        // 启动数据接收线程
        dataThread = std::thread([this]() {
            while (running) {
                // 模拟从交易所接收数据
                std::string rawData = "{\"symbol\":\"BTC/USDT\",\"price\":50000.0}";
                
                if (rawDataCallback) {
                    rawDataCallback(rawData);
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        });
    }
    
    void stop() override {
        running = false;
        if (dataThread.joinable()) {
            dataThread.join();
        }
    }
    
private:
    std::thread dataThread;
    bool running = true;
};

// 创建数据源实例
std::shared_ptr<DataFeed> createDataFeed() {
    return std::make_shared<ExchangeDataFeed>();
}