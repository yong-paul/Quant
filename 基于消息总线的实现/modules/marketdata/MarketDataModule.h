#pragma once
#include "../IModule.h"
#include "../MessageBus.h"
#include <memory>
#include "../../QuoteSource.h"

// 行情数据消息
struct MarketDataMessage : public Message {
    std::string symbol;
    double price;
    int volume;
    
    MarketDataMessage() : Message{"MarketData"} {}
};

// 行情数据模块
class MarketDataModule : public IModule {
    std::shared_ptr<MessageBus> bus;
    std::shared_ptr<IQuoteSource> quoteSource;
    
public:
    void setQuoteSource(std::shared_ptr<IQuoteSource> source) {
        quoteSource = source;
    }
    
    void initialize(std::shared_ptr<MessageBus> bus) override {
        this->bus = bus;
    }
    
    void start() override {
        if (quoteSource) {
            quoteSource->connect();
            
            std::thread([this]() {
                while (true) {
                    MarketDataMessage msg = quoteSource->getMarketData("AAPL");
                    bus->publish(msg);
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }).detach();
        } else {
            // 默认模拟行情数据
            std::thread([this]() {
                while (true) {
                    MarketDataMessage msg;
                    msg.symbol = "AAPL";
                    msg.price = 100.0 + (rand() % 100) / 10.0;
                    msg.volume = 100 + rand() % 1000;
                    bus->publish(msg);
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }).detach();
        }
    }
    
    void stop() override {
        if (quoteSource) {
            quoteSource->disconnect();
        }
    }
    
    std::string name() const override {
        return "MarketDataModule";
    }
};

REGISTER_MODULE(MarketDataModule)