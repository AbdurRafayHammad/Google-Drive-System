#pragma once
#ifndef HASHTABLE_H
#define HASHTABLE_H
#include <string>
#include "DoublyLinkedLIst.h" // Ensure this is included
#define TABLE_SIZE 10
struct File_Metadata {
    std::string name;
    std::string type;
    int size;
    std::string date;
    std::string owner;
    std::string parent_folder; // New field to store the parent folder
    File_Metadata* next;
    DoublyLinkedList version_history; // Add version history
    File_Metadata(std::string name, std::string type, int size, std::string date, std::string owner, std::string parentFolder)
        : name(name), type(type), size(size), date(date), owner(owner), parent_folder(parentFolder), next(nullptr) {
    }
    File_Metadata(std::string name, std::string type, int size, std::string date, std::string owner);
};
int hashfunc(const std::string& key, int tableSize);
class Hash_table {
private:
    File_Metadata** table; // Pointer to an array of pointers
    int capacity;
public:
    Hash_table(); // Constructor
    ~Hash_table(); // Destructor
    void insert(const std::string& name, const std::string& type,
        int size, const std::string& date, const std::string& owner);
    File_Metadata* search(const std::string& name);
    void remove(const std::string& name);
};
#endif