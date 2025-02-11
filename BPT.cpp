#include "BPT.h"

bool operator== (const Data &obj1, const Data &obj2) {
    std::string_view key1(obj1.key, obj1.key_len);
    std::string_view key2(obj2.key, obj2.key_len);
    return key1 == key2 && obj1.value == obj2.value;
}

bool operator< (const Data &obj1, const Data &obj2) {
    std::string_view key1(obj1.key, obj1.key_len);
    std::string_view key2(obj2.key, obj2.key_len);
    if (key1 != key2) {
        return key1 < key2;
    }
    return obj1.value < obj2.value;
}

bool operator> (const Data &obj1, const Data &obj2) {
    std::string_view key1(obj1.key, obj1.key_len);
    std::string_view key2(obj2.key, obj2.key_len);
    if (key1 != key2) {
        return key1 > key2;
    }
    return obj1.value > obj2.value;
}

bool operator<= (const Data &obj1, const Data &obj2) {
    std::string_view key1(obj1.key, obj1.key_len);
    std::string_view key2(obj2.key, obj2.key_len);
    if (key1 != key2) {
        return key1 < key2;
    }
    return obj1.value <= obj2.value;
}

bool operator>= (const Data &obj1, const Data &obj2) {
    std::string_view key1(obj1.key, obj1.key_len);
    std::string_view key2(obj2.key, obj2.key_len);
    if (key1 != key2) {
        return key1 > key2;
    }
    return obj1.value >= obj2.value;
}


template<class T>
void BPT<T>::readNode(const int &index_) {
    if (index_ == -1) {
        std::cout << "Error occurred" << '\n';
    }
    node_file.seekp(index_ * sizeof(Node<T>));
    node_file.read(reinterpret_cast<char*>(&cur), sizeof(Node<T>));
}

template<class T>
void BPT<T>::writeNode(Node<T> node, const int &index_) {
    node_file.seekp(index_ * sizeof(Node<T>));
    node_file.write(reinterpret_cast<char*>(&node), sizeof(Node<T>));
}

template<class T>
void BPT<T>::splitNode() {
    int mid = node_size / 2;

    Node<T> new_node{};
    new_node.index = new_id;
    new_node.is_leaf = cur.is_leaf;
    new_node.father = cur.father;
    ++new_id;

    if (cur.is_leaf) {
        if (cur.next != -1) {
            Node<T> cur_node = cur;
            readNode(cur.next);
            cur.prev = new_node.index;
            writeNode(cur, cur.index);
            cur = cur_node;
        }
        new_node.next = cur.next;
        new_node.prev = cur.index;
        cur.next = new_node.index;
    }

    Node<T> tmp = cur;
    for (int i = mid; i < tmp.size; ++i) {
        new_node.storage[i - mid] = tmp.storage[i];
        new_node.son[i - mid] = tmp.son[i];
    }
    cur = tmp;

    new_node.size = cur.size - mid;
    cur.size = mid;

    Node<T> original_node = cur;
    if (original_node.index == root) {
        Node<T> new_root{};
        new_root.index = new_id;
        ++new_id;
        root = new_root.index;
        original_node.father = new_node.father = root;
        new_root.storage[0] = original_node.storage[original_node.size - 1];
        new_root.storage[1] = new_node.storage[new_node.size - 1];
        new_root.son[0] = original_node.index;
        new_root.son[1] = new_node.index;
        new_root.is_leaf = false;
        new_root.size = 2;
        cur = new_root;
    }
    else {
        readNode(original_node.father);
        for (int i = 0; i < cur.size; i++) {
            if (cur.son[i] == original_node.index) {
                cur.son[i] = new_node.index;
                break;
            }
        }
        insert(original_node.storage[original_node.size - 1], original_node.index);
    }

    writeNode(original_node, original_node.index);
    writeNode(new_node, new_node.index);
    writeNode(cur, cur.index);
}

template<class T>
void BPT<T>::insert(const T &data, const int &son_index) {
    if (cur.size == 0) {
        cur.storage[0] = data;
        cur.son[0] = son_index;
    }
    else {
        bool flag = false;
        for (int i = cur.size; i >= 1; --i) {
            if (data < cur.storage[i - 1]) {
                cur.storage[i] = cur.storage[i - 1];
                cur.son[i] = cur.son[i - 1];
            }
            else {
                cur.storage[i] = data;
                cur.son[i] = son_index;
                flag = true;
                break;
            }
        }
        if (!flag) {
            cur.storage[0] = data;
            cur.son[0] = son_index;
        }
    }
    ++cur.size;
}

template<class T>
void BPT<T>::remove(const T &data) {
    for (int i = 0; i < cur.size; i++) {
        if (data == cur.storage[i]) {
            for (int j = i; j < cur.size - 1; j++) {
                cur.storage[j] = cur.storage[j + 1];
                cur.son[j] = cur.son[j + 1];
            }
            --cur.size;
        }
    }
}

template<class T>
void BPT<T>::flush(int st) {
    bool flag = false;
    if (st == root) {
        return;
    }
    Node<T> st_node = cur;
    Node<T> son_node = cur;
    readNode(cur.father);
    while (!flag) {
        for (int i = 0; i < cur.size; i++) {
            if (cur.son[i] == son_node.index) {
                if (cur.storage[i] != son_node.storage[son_node.size - 1]) {
                    cur.storage[i] = son_node.storage[son_node.size - 1];
                    writeNode(cur, cur.index);
                    son_node = cur;
                }
                else {
                    flag = true;
                }
                break;
            }
        }
        if (cur.index == root) {
            break;
        }
        readNode(cur.father);
    }
    cur = st_node;
}

template<class T>
bool BPT<T>::borrowFromRight() {
    bool flag = false;
    int depth_count = 0;
    Node<T> cur_node = cur;

    while (true) {
        if (cur.index == root) {
            cur = cur_node;
            return false;
        }
        readNode(cur.father);
        for (int i = 0; i < cur.size; i++) {
            if (cur.storage[i] == cur_node.storage[cur_node.size - 1]) {
                if (i < cur.size - 1) {
                    int p = cur.index;
                    readNode(cur.son[i + 1]);
                    if (cur.father != p) {
                        cur.father = p;
                        writeNode(cur, cur.index);
                    }
                    for (int j = 0; j < depth_count; j++) {
                        p = cur.index;
                        readNode(cur.son[0]);
                        cur.father = p;
                        writeNode(cur, cur.index);
                    }
                    flag = true;
                }
                break;
            }
        }
        if (flag) {
            break;
        }
        ++depth_count;
    }

    if (cur.size > node_size / 2) {
        T tmp = cur.storage[0];
        int right_son = cur.son[0];
        remove(tmp);
        writeNode(cur, cur.index);
        cur = cur_node;
        insert(tmp, right_son);
        writeNode(cur, cur.index);
        flush(cur.index);
        if (right_son != -1) {
            cur_node = cur;
            readNode(right_son);
            cur.father = cur_node.index;
            writeNode(cur, cur.index);
            cur = cur_node;
        }
        return true;
    }
    cur = cur_node;
    return false;
}

template<class T>
bool BPT<T>::borrowFromLeft() {
    bool flag = false;
    int depth_count = 0;
    Node<T> cur_node = cur;
    while (true) {
        if (cur.index == root) {
            cur = cur_node;
            return false;
        }
        readNode(cur.father);
        for (int i = 0; i < cur.size; i++) {
            if (cur.storage[i] == cur_node.storage[cur_node.size - 1]) {
                if (i > 0) {
                    readNode(cur.son[i - 1]);
                    for (int j = 0; j < depth_count; j++) {
                        readNode(cur.son[cur.size - 1]);
                    }
                    flag = true;
                }
                break;
            }
        }
        if (flag) {
            break;
        }
        ++depth_count;
    }

    if (cur.size > node_size / 2) {
        T tmp = cur.storage[cur.size - 1];
        int left_son = cur.son[cur.size - 1];
        remove(tmp);
        writeNode(cur, cur.index);
        flush(cur.index);
        cur = cur_node;
        insert(tmp, left_son);
        writeNode(cur, cur.index);
        if (left_son != -1) {
            cur_node = cur;
            readNode(left_son);
            cur.father = cur_node.index;
            writeNode(cur, cur.index);
            cur = cur_node;
        }
        return true;
    }
    cur = cur_node;
    return false;
}


template<class T>
void BPT<T>::combine() {
    Node<T> cur_node = cur;

    readNode(cur.father);
    for (int i = 0; i < cur.size; i++) {
        if (cur.storage[i] == cur_node.storage[cur_node.size - 1]) {
            if (i == cur.size - 1) {
                cur.storage[i - 1] = cur.storage[i];
                --cur.size;
                writeNode(cur, cur.index);
                readNode(cur.son[i - 1]);
                if (cur.is_leaf) {
                    cur.next = cur_node.next;
                    Node<T> tmp = cur;
                    if (cur_node.next != -1) {
                        readNode(cur_node.next);
                        cur.prev = tmp.index;
                        writeNode(cur, cur.index);
                    }
                    cur = tmp;
                }

                Node<T> father_node = cur;
                for (int j = 0; j < cur_node.size; j++) {
                    father_node.storage[j + father_node.size] = cur_node.storage[j];
                    father_node.son[j + father_node.size] = cur_node.son[j];
                }
                cur = father_node;

                cur.size += cur_node.size;
                writeNode(cur, cur.index);
            }
            else {
                int next_index = cur.son[i + 1];
                cur.storage[i] = cur.storage[i + 1];
                for (int j = i + 1; j < cur.size - 1; j++) {
                    cur.storage[j] = cur.storage[j + 1];
                    cur.son[j] = cur.son[j + 1];
                }
                --cur.size;
                writeNode(cur, cur.index);

                readNode(next_index);
                Node<T> father_node = cur;
                for (int j = 0; j < father_node.size; j++) {
                    cur_node.storage[j + cur_node.size] = father_node.storage[j];
                    cur_node.son[j + cur_node.size] = father_node.son[j];
                }
                cur = father_node;

                cur_node.size += cur.size;
                cur_node.next = cur.next;
                writeNode(cur_node, cur_node.index);
                if (cur.is_leaf && cur.next != -1) {
                    readNode(cur.next);
                    cur.prev = cur_node.index;
                    writeNode(cur, cur.index);
                }
                cur = cur_node;
            }
            break;
        }
    }
}

template<class T>
void BPT<T>::addData(const T &data) {
    int p = root, q = -1;
    while (true) {
        readNode(p);
        if (cur.father != q) {
            cur.father = q;
            writeNode(cur, cur.index);
        }
        if (cur.is_leaf) {
            break;
        }
        for (int i = 0; i < cur.size; i++) {
            if (data <= cur.storage[i] || i == cur.size - 1) {
                q = p;
                p = cur.son[i];
                break;
            }
        }
    }
    for (int i = 0; i < cur.size; i++) {
        if (data == cur.storage[i]) {
            return;
        }
    }

    insert(data, -1);
    writeNode(cur, cur.index);
    if (data == cur.storage[cur.size - 1]) {
        flush(cur.index);
    }
    while (cur.size > node_size) {
        splitNode();
    }

}

template <class T>
void BPT<T>::removeData(const T &data) {
    int p = root, q = -1;
    while (true) {
        readNode(p);
        if (cur.father != q) {
            cur.father = q;
            writeNode(cur, cur.index);
        }
        if (cur.is_leaf) {
            break;
        }
        writeNode(cur, cur.index);
        for (int i = 0; i < cur.size; i++) {
            if (data <= cur.storage[i] || i == cur.size - 1) {
                q = p;
                p = cur.son[i];
                break;
            }
        }
    }

    if (cur.size == 0) {
        return;
    }

    bool f = false;
    if (data == cur.storage[cur.size - 1]) {
        f = true;
    }

    int prev_size = cur.size;
    remove(data);
    if (cur.size != prev_size) {
        writeNode(cur, cur.index);
        if (f) {
            flush(cur.index);
        }
    }
    else {
        return;
    }

    while(true) {
        if (cur.size < node_size / 2 && cur.index != root) {
            bool flag = borrowFromRight();
            if (!flag) {
                flag = borrowFromLeft();
                if (!flag) {
                    combine();
                    readNode(cur.father);
                    if (cur.index == root && !cur.is_leaf && cur.size <= 1) {
                        root = cur.son[0];
                        return;
                    }
                }
            }
        }
        else {
            break;
        }
    }

}

template <class T>
void BPT<T>::findData(const std::string &str) {
    int p = root;
    while (true) {
        readNode(p);
        if (cur.is_leaf) {
            break;
        }
        for (int i = 0; i < cur.size; i++) {
            std::string _str(cur.storage[i].key, cur.storage[i].key_len);
            if (str <= _str || i == cur.size - 1) {
                p = cur.son[i];
                break;
            }
        }
    }

    bool output = false;
    while (true) {
        for (int i = 0; i < cur.size; i++) {
            std::string _str(cur.storage[i].key, cur.storage[i].key_len);
            if (_str == str) {
                std::cout << cur.storage[i].value << " ";
                output = true;
            }
        }
        if (cur.next != -1) {
            readNode(cur.next);
        }
        else {
            break;
        }
    }

    if (!output) {
        std::cout << "null";
    }
    std::cout << "\n";
}

template class BPT<Data>;