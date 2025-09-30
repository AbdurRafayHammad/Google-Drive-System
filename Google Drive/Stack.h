#pragma once
#include <ctime>
#include "HashTable.h"
struct StackNode {
    File_Metadata* file;
    time_t deletetion_time;
    StackNode* next;    
    StackNode(File_Metadata* file) : file(file), deletetion_time(time(nullptr)), next(nullptr) {}
};
class Stack {
private:
    StackNode* top;
    int auto_delete_threshold;
public:
    Stack(int threshold = 3600) : top(nullptr), auto_delete_threshold(threshold) {} // Default: 1 hour

    ~Stack() {
        while (top) {
            StackNode* temp = top;
            top = top->next;
            delete temp->file;
            delete temp;      
        }
    }
    void push(File_Metadata* file) {
        auto_delete_old_entries(); // Auto-delete old entries before adding a new one
        StackNode* newNode = new StackNode(file);
        newNode->next = top;
        top = newNode;
    }
    File_Metadata* pop() {
        if (!top) return nullptr;
        StackNode* temp = top;
        top = top->next;
        File_Metadata* file = temp->file;
        delete temp; 
        return file;
    }
    File_Metadata* peek() const {
        return top ? top->file : nullptr;
    }
    bool isEmpty() const {
        return top == nullptr;
    }
    void auto_delete_old_entries() {
        time_t curr_time = time(nullptr);
        while (top && difftime(curr_time, top->deletetion_time) > auto_delete_threshold) {
            StackNode* temp = top;
            top = top->next;
            delete temp->file;
            delete temp;      
        }
    }
};