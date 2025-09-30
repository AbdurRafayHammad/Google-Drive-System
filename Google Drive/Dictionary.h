#pragma once
#include "googleDrive.h"
#include "HashTable.h"
#include "Graph.h"
#include "Dictionary.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <string>
class Dictionary {
private:
    struct Entry {
        std::string word;
        int code;
        Entry* next;
        Entry(const std::string& w, int c) : word(w), code(c), next(nullptr) {}
    };
    Entry** table;
    int table_size;
    int next_code;
    int hash_func(const std::string& word) const;
public:
    Dictionary(int size = 1000);
    ~Dictionary();
    int add(const std::string& word);
    std::string getWord(int code) const;
};
class Dictionary_Compressor {
private:
    Dictionary dict;
public:
    Dictionary_Compressor();
    std::string compress(const std::string& input);
    std::string decompress(const std::string& input);
};
class Background_Sync {
private:
    struct SyncItem {
        string filename;
        string content;
    };
    static const int max_Q_size = 1000;
    SyncItem* queue;
    int front;
    int rear;
    int size;
    bool running;
    void process_Sync_Q();
public:
    Background_Sync();  
    ~Background_Sync();
    void QSync(const string& filename, const string& content);
};