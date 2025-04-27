#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "./list/list.hpp"
constexpr int max_size_ = 10;

template <typename T>
struct CacheManager {
    struct LRUNode {
        int index = -1;
        T data{};
        int accessTime[3]{};
        int cnt = 0;

        LRUNode() = default;

        LRUNode(const T& data_) {
            data = data_;
        }

        ~LRUNode() = default;

    };
    int k{};
    std::size_t timeStamp = 0;
    LRUNode storage[max_size_]{};
    int sze = 0;

    CacheManager() = default;

    explicit CacheManager(const int& k_): k(k_) {}

    ~CacheManager() = default;

    void recordAccess(const int& p) {
        ++timeStamp;
        storage[p].accessTime[storage[p].cnt % k] = timeStamp;
        ++storage[p].cnt;
    }

    void writeInto(const int& index, const T& node) {
        ++timeStamp;
        for (auto & i : storage) {
            if (i.index == -1) {
                i = LRUNode{node};
                i.index = index;
                i.accessTime[0] = timeStamp;
                ++i.cnt;
                sze++;
                return;
            }
        }
    }

    [[nodiscard]] std::size_t size() const {
        return sze;
    }
};
#endif //CACHEMANAGER_H