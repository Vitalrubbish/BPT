#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "./map/map.hpp"
#include "./list/list.hpp"
#include <string>
#include <fstream>
const int max_size_ = 24;
template <typename T>
struct CacheManager {
    struct LRUNode {
        T data{};
        int t{};
    };
    sjtu::map<size_t, int> timeList;
    sjtu::map<int, LRUNode> cachePool;
    std::size_t timeStamp = 0;

    CacheManager() = default;

    ~CacheManager() = default;

    void recordAccess(const int& index, const T& node) {
        ++timeStamp;
        if (!cachePool.count(index)) {
            cachePool[index] = LRUNode{node};
        }
        cachePool[index].t = timeStamp;
        timeList[timeStamp] = index;
    }

    [[nodiscard]] std::size_t size() const {
        return cachePool.size();
    }
};
#endif //CACHEMANAGER_H