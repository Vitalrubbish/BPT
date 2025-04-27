#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "./map/map.hpp"
#include "./list/list.hpp"
#include <string>
#include <fstream>
constexpr int max_size_ = 97;

template <typename T>
struct CacheManager {
    struct LRUNode {
        int index = -1;
        T data{};
        int accessTime[5]{};
        int cnt = 0;

        LRUNode() = default;

        LRUNode(const T& data_, const int& k) {
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
        int cur = index % max_size_;
        while (storage[cur].index != -1 && storage[cur].index != -2) {
            cur = (cur + 1) % max_size_;
        }
        storage[cur] = LRUNode{node, k};
        storage[cur].index = index;
        storage[cur].accessTime[0] = timeStamp;
        ++storage[cur].cnt;
        sze++;
    }

    [[nodiscard]] std::size_t size() const {
        return sze;
    }
};
#endif //CACHEMANAGER_H