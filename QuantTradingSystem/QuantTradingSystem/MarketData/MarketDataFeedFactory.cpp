#include "IMarketDataFeed.h"
#include "CTPMarketDataFeed.h"
#include <stdexcept>

std::shared_ptr<IMarketDataFeed> MarketDataFeedFactory::createMarketDataFeed(const std::string& provider) {
    if (provider == "CTP") {
        return std::make_shared<CTPMarketDataFeed>();
    }
    // 在这里添加其他数据源的支持
    // else if (provider == "XTP") {
    //     return std::make_shared<XTPMarketDataFeed>();
    // }
    
    throw std::runtime_error("Unsupported market data provider: " + provider);
} 