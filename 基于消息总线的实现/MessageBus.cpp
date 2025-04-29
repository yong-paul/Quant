#include "MessageBus.h"
#include <memory>

class MessageBusImpl : public MessageBus {
    std::unordered_map<std::string, std::vector<MessageHandler>> handlers;

public:
    void subscribe(const std::string& messageType, MessageHandler handler) override {
        handlers[messageType].push_back(handler);
    }

    void publish(const Message& message) override {
        auto it = handlers.find(message.type);
        if (it != handlers.end()) {
            for (auto& handler : it->second) {
                handler(message);
            }
        }
    }
};

std::shared_ptr<MessageBus> MessageBus::create() {
    return std::make_shared<MessageBusImpl>();
}