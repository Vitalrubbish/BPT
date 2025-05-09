#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "./list/list.hpp"
#include "./vector/vector.hpp"
#include <shared_mutex>
#include <list>
#include "./HashTable.h"
const int max_size_ = 100;
template <typename T>
struct CacheManager {
    struct LRUNode {
        bool dirty;
        int index;
    };

    HashTable<T> hashTable;
    HashTable<typename std::list<LRUNode>::iterator> position;
    std::list<LRUNode> lis;

    CacheManager() = default;

    [[nodiscard]] size_t size() {
        return lis.size();
    }

    bool checkExist(const int& index) {
        return hashTable.check(index);
    }

    T get(const int& index) {
        typename std::list<LRUNode>::iterator pos = position.lw(index);
        LRUNode newNode{pos -> dirty, index};
        lis.erase(pos);
        position.erase(index);
        lis.push_back(newNode);
        auto pos_ = lis.end();
        --pos_;
        position.sw(index, pos_);
        return hashTable.lw(index);
    }

    void put(const int& index, const T& val) {
        LRUNode newNode{true, index};
        if (checkExist(index)) {
            typename std::list<LRUNode>::iterator pos = position.lw(index);
            lis.erase(pos);
            position.erase(index);
            hashTable.erase(index);
        }
        else {
            newNode.dirty = false;
        }
        hashTable.sw(index, val);
        lis.push_back(newNode);
        auto pos = lis.end();
        --pos;
        position.sw(index, pos);
    }
};
#endif //CACHEMANAGER_H
