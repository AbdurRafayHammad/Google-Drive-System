#pragma once
#ifndef GOOGLEDRIVE_H
#define GOOGLEDRIVE_H

#include "HashTable.h"
#include <iostream>
using namespace std;

File_Metadata::File_Metadata(std::string name, std::string type, int size, std::string date, std::string owner) {
    this->name = name;
    this->type = type;
    this->size = size;
    this->date = date;
    this->owner = owner;
    this->next = nullptr;
}

// Only defined in this .cpp file
int hashfunc(const string& key, int tableSize) {
    int hash = 0;
    for (char ch : key)
        hash = (hash * 31 + ch) % tableSize;
    return hash;
}

Hash_table::Hash_table() : capacity(TABLE_SIZE) {  
    table = new File_Metadata*[TABLE_SIZE];  
    for (int i = 0; i < TABLE_SIZE; ++i)  
        table[i] = nullptr;  
}

Hash_table::~Hash_table() {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        File_Metadata* entry = table[i];
        while (entry) {
            File_Metadata* temp = entry;
            entry = entry->next;
            delete temp;
        }
    }
    delete[] table; // Free the table array
}

void Hash_table::insert(const std::string& name, const std::string& type,
    int size, const std::string& date, const std::string& owner) {
    int index = hashfunc(name, TABLE_SIZE);
    File_Metadata* newEntry = new File_Metadata(name, type, size, date, owner);
    newEntry->next = table[index];
    table[index] = newEntry;
}

File_Metadata* Hash_table::search(const std::string& name) {
    int index = hashfunc(name, TABLE_SIZE);
    File_Metadata* entry = table[index];
    while (entry) {
        if (entry->name == name)
            return entry;
        entry = entry->next;
    }
    return nullptr;
}

void Hash_table::remove(const std::string& name) {
    int index = hashfunc(name, TABLE_SIZE);
    File_Metadata* entry = table[index];
    File_Metadata* prev = nullptr;
    while (entry) {
        if (entry->name == name) {
            if (prev) prev->next = entry->next;
            else table[index] = entry->next;
            delete entry;
            return;
        }
        prev = entry;
        entry = entry->next;
    }
    std::cout << "File '" << name << "' not found.\n";
}

#endif // GOOGLEDRIVE_H