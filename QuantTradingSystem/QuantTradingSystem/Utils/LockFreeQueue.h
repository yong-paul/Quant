#pragma once

#include <atomic>
#include <memory>
#include <array>

namespace Utils {

template<typename T, size_t Capacity>
class LockFreeQueue {
private:
    struct Node {
        T data;
        std::atomic<Node*> next;
        
        Node() : next(nullptr) {}
        explicit Node(const T& item) : data(item), next(nullptr) {}
    };

    // 使用固定大小的内存池
    std::array<Node, Capacity> nodePool;
    std::atomic<size_t> poolIndex{0};

    // 队列头尾指针
    std::atomic<Node*> head;
    std::atomic<Node*> tail;

    // 获取新的节点
    Node* allocateNode() {
        size_t index = poolIndex.fetch_add(1) % Capacity;
        return &nodePool[index];
    }

public:
    LockFreeQueue() {
        // 初始化头尾节点
        Node* dummy = allocateNode();
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        // 清理队列
        while (pop());
    }

    // 尝试将元素推入队列
    bool push(const T& item) {
        Node* newNode = allocateNode();
        newNode->data = item;
        newNode->next.store(nullptr);

        while (true) {
            Node* tailNode = tail.load(std::memory_order_acquire);
            Node* next = tailNode->next.load(std::memory_order_acquire);

            if (tailNode == tail.load(std::memory_order_acquire)) {
                if (next == nullptr) {
                    if (tailNode->next.compare_exchange_weak(next, newNode,
                            std::memory_order_release,
                            std::memory_order_relaxed)) {
                        tail.compare_exchange_strong(tailNode, newNode,
                            std::memory_order_release,
                            std::memory_order_relaxed);
                        return true;
                    }
                } else {
                    tail.compare_exchange_strong(tailNode, next,
                        std::memory_order_release,
                        std::memory_order_relaxed);
                }
            }
        }
    }

    // 尝试从队列中弹出元素
    bool pop(T& item) {
        while (true) {
            Node* headNode = head.load(std::memory_order_acquire);
            Node* tailNode = tail.load(std::memory_order_acquire);
            Node* next = headNode->next.load(std::memory_order_acquire);

            if (headNode == head.load(std::memory_order_acquire)) {
                if (headNode == tailNode) {
                    if (next == nullptr) {
                        return false;
                    }
                    tail.compare_exchange_strong(tailNode, next,
                        std::memory_order_release,
                        std::memory_order_relaxed);
                } else {
                    if (next == nullptr) {
                        continue;
                    }
                    item = next->data;
                    if (head.compare_exchange_weak(headNode, next,
                            std::memory_order_release,
                            std::memory_order_relaxed)) {
                        return true;
                    }
                }
            }
        }
    }

    // 检查队列是否为空
    bool empty() const {
        return head.load(std::memory_order_acquire) == 
               tail.load(std::memory_order_acquire);
    }

    // 获取队列大小（近似值）
    size_t size() const {
        size_t count = 0;
        Node* current = head.load(std::memory_order_acquire);
        while (current != tail.load(std::memory_order_acquire)) {
            current = current->next.load(std::memory_order_acquire);
            ++count;
        }
        return count;
    }
};

} // namespace Utils 