# 量化交易系统架构设计

## 1. 类关系图 (UML)

```mermaid
classDiagram

class IDataFeed {
  <<interface>>
  +subscribe()
  +unsubscribe()
  +registerCallback()
  +start()
  +stop()
}

class IStrategyEngine {
  <<interface>>
  +registerDataCallback()
  +registerSignalCallback()
  +start()
  +stop()
}

class IRiskManager {
  <<interface>>
  +registerSignalCallback()
  +setRiskCheckCallback()
  +start()
  +stop()
}

class ITradeExecutor {
  <<interface>>
  +registerRiskCheckCallback()
  +registerExecutionCallback()
  +connect()
  +disconnect()
  +execute()
}

class DataFeed {
  +subscribe()
  +unsubscribe()
  +registerCallback()
  +start()
  +stop()
}

class StrategyEngine {
  +registerDataCallback()
  +registerSignalCallback()
  +start()
  +stop()
}

class RiskManager {
  +registerSignalCallback()
  +setRiskCheckCallback()
  +start()
  +stop()
}

class TradeExecutor {
  +registerRiskCheckCallback()
  +registerExecutionCallback()
  +connect()
  +disconnect()
  +execute()
}

IDataFeed <|-- DataFeed
IStrategyEngine <|-- StrategyEngine
IRiskManager <|-- RiskManager
ITradeExecutor <|-- TradeExecutor
```

## 2. 系统框架图

```mermaid
graph TD
  A[数据获取模块] -->|市场数据| B[策略引擎]
  B -->|交易信号| C[风险控制]
  C -->|审核结果| D[交易执行]
  D -->|执行报告| B
```

## 3. 序列图示例

```mermaid
sequenceDiagram
  participant DataFeed
  participant StrategyEngine
  participant RiskManager
  participant TradeExecutor
  
  DataFeed->>StrategyEngine: 市场数据
  StrategyEngine->>RiskManager: 交易信号
  RiskManager->>TradeExecutor: 审核通过
  TradeExecutor-->>StrategyEngine: 执行报告
```

## 4. 主要接口说明

- **IDataFeed**: 数据获取接口
- **IStrategyEngine**: 策略引擎接口
- **IRiskManager**: 风险管理接口
- **ITradeExecutor**: 交易执行接口

## 5. 回调函数说明

1. `onMarketData`: 处理市场数据
2. `onSignal`: 处理交易信号
3. `onRiskCheck`: 风险检查
4. `onExecution`: 执行报告处理