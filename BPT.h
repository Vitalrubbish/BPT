#ifndef BPT_H
#define BPT_H
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>

constexpr int node_size = 200;  //into debug mode you can modify node_size to 4

struct Data {
    char key[64]{};
    int key_len = 0;
    int value = 0;

    Data () = default;

    Data (const std::string & _key, int _value): value(_value) {
        strcpy(key, _key.c_str());
        key_len = static_cast<int> (_key.size());
    }

    Data &operator= (const Data &other) {
        if (this == &other) {
            return *this;
        }
        strcpy(key, other.key);
        key_len = other.key_len;
        value = other.value;
        return *this;
    }

    friend bool operator== (const Data & , const Data & );

    friend bool operator< (const Data & , const Data & );

    friend bool operator> (const Data & , const Data & );

    friend bool operator<= (const Data & , const Data & );

    friend bool operator>= (const Data & , const Data & );

};

template <class T>
struct Node {
    int index = -1;
    int size = 0;
    T storage[node_size + 1]{};
    bool is_leaf = true;

    int father = -1;
    int son[node_size + 1]{};
    int prev = -1;
    int next = -1;
};



template <class T>
class BPT {
    int root = -1;
    int head = -1;
    int new_id = 0;

    Node<T> cur{};

    std::fstream basic_file;
    std::fstream node_file;
    std::string basic_file_name;
    std::string node_file_name;

public:
    explicit BPT(const std::string &file_name) {
        basic_file_name = "basic_" + file_name;
        node_file_name = "node_" + file_name;

        node_file.open(node_file_name, std::ios::in|std::ios::out);
        if (!node_file.is_open()) {
            node_file.open(node_file_name, std::ios::out);
            node_file.close();
            node_file.open(node_file_name, std::ios::in|std::ios::out);
        }

        basic_file.open(basic_file_name, std::ios::in|std::ios::out);
        if (!basic_file.is_open()) {
            basic_file.open(basic_file_name, std::ios::out);
            basic_file.close();
            basic_file.open(basic_file_name, std::ios::in|std::ios::out);
            root = head = new_id = 0;
            cur.index = new_id;
            writeNode(cur, cur.index);
            new_id++;
        }
        else {
            basic_file.read(reinterpret_cast<char*> (&root), sizeof(int));
            basic_file.read(reinterpret_cast<char*> (&head), sizeof(int));
            basic_file.read(reinterpret_cast<char*> (&new_id), sizeof(int));
        }
        basic_file.close();
    }

    ~BPT() {
        basic_file.open(basic_file_name, std::ios::in|std::ios::out);
        basic_file.write(reinterpret_cast<char*> (&root), sizeof(int));
        basic_file.write(reinterpret_cast<char*> (&head), sizeof(int));
        basic_file.write(reinterpret_cast<char*> (&new_id), sizeof(int));
        basic_file.close();

        node_file.close();
    }

    void readNode(const int & );

    void writeNode(Node<T> , const int & );

    void splitNode();

    void insert(const T & , const int & );

    void remove(const T & );

    void flush(int );

    bool borrowFromRight();

    bool borrowFromLeft();

    void combine();

    void addData(const T & );

    void removeData(const T & );

    void findData(const std::string & );

};
#endif //BPT_H
