#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "exceptions.h"

template<class T>
class linked_list {
private:
    struct node {
        T data;
        node* next;
        node(const T& val) : data(val), next(nullptr) {}
    };

    node* head_;
    node* tail_;
    int size_;

public:
    linked_list() : head_(nullptr), tail_(nullptr), size_(0) {}

    linked_list(const T* items, int count) : head_(nullptr), tail_(nullptr), size_(0) {
        for (int i = 0; i < count; i++) {
            append(items[i]);
        }
    }

    linked_list(const linked_list& other) : head_(nullptr), tail_(nullptr), size_(0) {
        node* curr = other.head_;
        while (curr != nullptr) {
            append(curr->data);
            curr = curr->next;
        }
    }

    linked_list& operator=(const linked_list& other) {
        if (this != &other) {
            node* curr = head_;
            while (curr != nullptr) {
                node* next = curr->next;
                delete curr;
                curr = next;
            }
            head_ = tail_ = nullptr;
            size_ = 0;

            curr = other.head_;
            while (curr != nullptr) {
                append(curr->data);
                curr = curr->next;
            }
        }
        return *this;
    }

    ~linked_list() {
        node* curr = head_;
        while (curr != nullptr) {
            node* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    T get_first() const {
        if (head_ == nullptr) {
            throw empty_collection_exception("list is empty");
        }
        return head_->data;
    }

    T get_last() const {
        if (tail_ == nullptr) {
            throw empty_collection_exception("list is empty");
        }
        return tail_->data;
    }

    T get(int index) const {
        if (index < 0 || index >= size_) {
            throw index_out_of_range_exception("index out of range");
        }
        node* curr = head_;
        for (int i = 0; i < index; i++) {
            curr = curr->next;
        }
        return curr->data;
    }

    linked_list* get_sub_list(int start_index, int end_index) const {
        if (start_index < 0 || end_index >= size_ || start_index > end_index) {
            throw index_out_of_range_exception("invalid indices");
        }
        linked_list* result = new linked_list();
        node* curr = head_;
        for (int i = 0; i < start_index; i++) {
            curr = curr->next;
        }
        for (int i = start_index; i <= end_index; i++) {
            result->append(curr->data);
            curr = curr->next;
        }
        return result;
    }

    int get_length() const {
        return size_;
    }

    void append(const T& item) {
        node* new_node = new node(item);
        if (head_ == nullptr) {
            head_ = tail_ = new_node;
        }
        else {
            tail_->next = new_node;
            tail_ = new_node;
        }
        size_++;
    }

    void prepend(const T& item) {
        node* new_node = new node(item);
        if (head_ == nullptr) {
            head_ = tail_ = new_node;
        }
        else {
            new_node->next = head_;
            head_ = new_node;
        }
        size_++;
    }

    void insert_at(const T& item, int index) {
        if (index < 0 || index > size_) {
            throw index_out_of_range_exception("index out of range");
        }
        if (index == 0) {
            prepend(item);
            return;
        }
        if (index == size_) {
            append(item);
            return;
        }
        node* curr = head_;
        for (int i = 0; i < index - 1; i++) {
            curr = curr->next;
        }
        node* new_node = new node(item);
        new_node->next = curr->next;
        curr->next = new_node;
        size_++;
    }

    linked_list* concat(const linked_list* other) const {
        linked_list* result = new linked_list(*this);
        node* curr = other->head_;
        while (curr != nullptr) {
            result->append(curr->data);
            curr = curr->next;
        }
        return result;
    }

    node* get_head() const {
        return head_;
    }
};

#endif
