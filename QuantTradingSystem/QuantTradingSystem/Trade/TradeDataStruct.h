#pragma once
#include <string>
#include <vector>

// 使用命名空间区分Trade模块中的类型定义
namespace trade {

// 订单方向
enum class OrderDirection {
    Buy,        // 买入
    Sell        // 卖出
};

// 开平标志
enum class OrderOffset {
    Open,       // 开仓
    Close,      // 平仓
    CloseToday, // 平今
    CloseYesterday // 平昨
};

// 价格类型
enum class OrderPriceType {
    Limit,      // 限价单
    Market      // 市价单
};

// 订单状态
enum class OrderStatus {
    Unknown,        // 未知
    Submitting,     // 提交中
    Accepted,       // 已受理
    PartialFilled,  // 部分成交
    Filled,         // 全部成交
    Canceled,       // 已撤单
    Rejected        // 已拒绝
};

// 订单数据结构
struct OrderData {
    std::string orderId;          // 订单编号
    std::string symbol;           // 合约代码
    OrderDirection direction;     // 买卖方向
    OrderOffset offset;           // 开平标志
    OrderPriceType priceType;     // 价格类型
    double price;                 // 价格
    int volume;                   // 数量
    int tradedVolume;             // 已成交数量
    OrderStatus status;           // 订单状态
    std::string statusMsg;        // 状态信息
    std::string insertTime;       // 委托时间
    std::string updateTime;       // 更新时间
    std::string tradingDay;       // 交易日
    std::string accountId;        // 账户编号
    std::string exchangeId;       // 交易所代码
    std::string strategyId;       // 策略ID
    std::string frontId;          // 前置编号 
    std::string sessionId;        // 会话编号
    std::string orderRef;         // 报单引用
};

// 成交数据结构
struct TradeData {
    std::string tradeId;          // 成交编号
    std::string orderId;          // 对应订单编号
    std::string symbol;           // 合约代码
    OrderDirection direction;     // 买卖方向
    OrderOffset offset;           // 开平标志
    double price;                 // 成交价格
    int volume;                   // 成交数量
    std::string tradeTime;        // 成交时间
    std::string tradingDay;       // 交易日
    std::string accountId;        // 账户编号
    std::string exchangeId;       // 交易所代码
};

// 持仓数据结构
struct PositionData {
    std::string symbol;           // 合约代码
    OrderDirection direction;     // 持仓方向
    int totalPosition;            // 总持仓
    int todayPosition;            // 今日持仓
    int yesterdayPosition;        // 昨日持仓
    double openCost;              // 开仓成本
    double positionCost;          // 持仓成本
    double openPrice;             // 开仓均价
    double positionPrice;         // 持仓均价
    double marketValue;           // 市值
    double unrealizedPnl;         // 浮动盈亏
    std::string tradingDay;       // 交易日
    std::string accountId;        // 账户编号
    std::string exchangeId;       // 交易所代码
};

// 账户资金数据结构
struct AccountData {
    std::string accountId;        // 账户编号
    double balance;               // 账户余额
    double available;             // 可用资金
    double frozenMargin;          // 冻结保证金
    double frozenCommission;      // 冻结手续费
    double commission;            // 已支付手续费
    double margin;                // 保证金
    double closeProfit;           // 平仓盈亏
    double positionProfit;        // 持仓盈亏
    double preBalance;            // 上次结算准备金
    double deposit;               // 入金金额
    double withdraw;              // 出金金额
    std::string tradingDay;       // 交易日
    std::string currency;         // 币种
};

} // namespace trade 