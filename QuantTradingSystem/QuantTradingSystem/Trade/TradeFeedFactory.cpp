#include "ITradeFeed.h"
#include "CTPTradeFeed.h"
#include <memory>
#include <stdexcept>

std::shared_ptr<ITradeFeed> TradeFeedFactory::CreateTradeFeed(const std::string& provider) {
    if (provider == "CTP") {
        return std::make_shared<CTPTradeFeed>();
    }
    // 这里可以添加其他交易接口提供商的支持
    
    throw std::runtime_error("Unsupported trade feed provider: " + provider);
} 