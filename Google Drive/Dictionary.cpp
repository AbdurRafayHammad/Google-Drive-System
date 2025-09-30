#include "Dictionary.h"
// Dictionary implementation
int Dictionary::hash_func(const std::string& word) const {
    int hash = 0;
    for (char c : word) {
        hash = (hash * 31 + c) % table_size;
    }
    return hash;
}
Dictionary::Dictionary(int size) : table_size(size), next_code(0) {
    table = new Entry * [size];
    for (int i = 0; i < size; i++) {
        table[i] = nullptr;
    }
}
Dictionary::~Dictionary() {
    for (int i = 0; i < table_size; i++) {
        Entry* current = table[i];
        while (current) {
            Entry* next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] table;
}
int Dictionary::add(const std::string& word) {
    int index = hash_func(word);
    Entry* current = table[index];
    while (current) {
        if (current->word == word) {
            return current->code;
        }
        current = current->next;
    }
    Entry* newEntry = new Entry(word, next_code);
    newEntry->next = table[index];
    table[index] = newEntry;
    return next_code++;
}
std::string Dictionary::getWord(int code) const {
    for (int i = 0; i < table_size; i++) {
        Entry* current = table[i];
        while (current) {
            if (current->code == code) {
                return current->word;
            }
            current = current->next;
        }
    }
    return "";
}
// DictionaryCompressor implementation
Dictionary_Compressor::Dictionary_Compressor() {}
std::string Dictionary_Compressor::compress(const std::string& input) {
    std::string result;
    std::string word;
    size_t start = 0;
    for (size_t i = 0; i <= input.length(); i++) {
        if (i == input.length() || input[i] == ' ') {
            if (start < i) {
                word = input.substr(start, i - start);
                result += std::to_string(dict.add(word)) + " ";
            }
            start = i + 1;
        }
    }
    return result;
}
std::string Dictionary_Compressor::decompress(const std::string& input) {
    std::string result;
    std::string code;
    size_t start = 0;
    for (size_t i = 0; i <= input.length(); i++) {
        if (i == input.length() || input[i] == ' ') {
            if (start < i) {
                code = input.substr(start, i - start);
                try {
                    int codeNum = std::stoi(code);
                    result += dict.getWord(codeNum) + " ";
                }
                catch (const std::exception& e) {
                    // Invalid code or conversion error
                    result += "[InvalidCode] ";
                }
            }
            start = i + 1;
        }
    }
    // Remove the trailing space, if any
    if (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    return result;
}
// BackGround Sync Class definitions
Background_Sync::Background_Sync() : front(0), rear(-1), size(0), running(true) {
    queue = new SyncItem[max_Q_size];
}
Background_Sync::~Background_Sync() {
    running = false;
    delete[] queue;
}
void Background_Sync::QSync(const string& filename, const string& content) {
    if (size < max_Q_size) {
        rear = (rear + 1) % max_Q_size;
        queue[rear].filename = filename;
        queue[rear].content = content;
        size++;
        cout << "Queued " << filename << " for synchronization\n";
        process_Sync_Q();
    }
}
void Background_Sync::process_Sync_Q() {
    while (size > 0) {
        string filename = queue[front].filename;
        cout << "Syncing " << filename << " to cloud...\n";
        this_thread::sleep_for(chrono::seconds(3));
        cout << "Sync complete for " << filename << "\n";
        front = (front + 1) % max_Q_size;
        size--;
    }
}