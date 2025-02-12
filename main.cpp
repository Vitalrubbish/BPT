#include <iostream>
#include "BPT.h"
BPT <Data> bpt("file");
int main() {
    int operation_cnt;
    std::cin >> operation_cnt;
    while (operation_cnt--) {
        std::string operation_name;
        std::cin >> operation_name;
        if (operation_name == "insert") {
            std::string key;
            int value;
            std::cin >> key >> value;
            Data new_data{key, value};
            bpt.addData(new_data);
        }

        if (operation_name == "delete") {
            std::string key;
            int value;
            std::cin >> key >> value;
            Data new_data{key, value};
            bpt.removeData(new_data);
        }

        if (operation_name == "find") {
            std::string key;
            std::cin >> key;
            bpt.findData(key);
        }
    }
    return 0;
}