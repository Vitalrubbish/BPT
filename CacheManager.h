#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H
#include "./list/list.hpp"
#include "./vector/vector.hpp"
#include "./HashTable.h"
const int max_size_ = 1024;
template <typename T>
struct CacheManager {
    struct LRUNode {
        int t;
        int index;
    };

    int timeStamp = 0;
    HashTable<T> hashTable;
    HashTable<typename sjtu::list<LRUNode>::iterator> position;
    sjtu::list<LRUNode> lis;

    CacheManager() = default;

    [[nodiscard]] size_t size() {
        return lis.size();
    }

    bool checkExist(const int& index) {
        return hashTable.check(index);
    }

    T get(const int& index) {
        ++timeStamp;
        typename sjtu::list<LRUNode>::iterator pos = position.lw(index);
        lis.erase(pos);
        position.erase(index);
        LRUNode newNode{timeStamp, index};
        position.sw(index, lis.push_back(newNode));
        return hashTable.lw(index);
    }

    void put(const int& index, const T& val) {
        ++timeStamp;
        if (checkExist(index)) {
            typename sjtu::list<LRUNode>::iterator pos = position.lw(index);
            lis.erase(pos);
            position.erase(index);
            hashTable.erase(index);
        }
        LRUNode newNode{timeStamp, index};
        hashTable.sw(index, val);
        position.sw(index, lis.push_back(newNode));
    }
};
#endif //CACHEMANAGER_H
