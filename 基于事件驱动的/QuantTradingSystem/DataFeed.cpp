#include "DataFeed.h"
#include <iostream>

class MockDataFeed : public DataFeed {
public:
    void start() override {
        // 模拟数据源，定期发送测试数据
        while(true) {
            if(rawDataCallback) {
                rawDataCallback("TEST,100.50,1000");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
};

std::shared_ptr<DataFeed> createDataFeed() {
    return std::make_shared<MockDataFeed>();
}