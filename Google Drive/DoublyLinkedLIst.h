#pragma once
#include <string>
#include <iostream>
struct VersionNode {
    int version_No;          // Version number
    std::string content;        // File content for this version
    VersionNode* prev;          // Pointer to the previous version
    VersionNode* next;          // Pointer to the next version

    VersionNode(int versionNumber, const std::string& content)
        : version_No(versionNumber), content(content), prev(nullptr), next(nullptr) {
    }
};
class DoublyLinkedList {
private:
    VersionNode* head;          // Pointer to the first version
    VersionNode* tail;          // Pointer to the most recent version
    int version_count;           // Total number of versions
public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), version_count(0) {}
    ~DoublyLinkedList() {
        while (head) {
            VersionNode* temp = head;
            head = head->next;
            delete temp;
        }
    }
    // Add a new version to the list
    void add_version(const std::string& content) {
        version_count++;
        VersionNode* newNode = new VersionNode(version_count, content);
        if (!head) {
            head = tail = newNode;
        }
        else {
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
    }
    // Rollback to a specific version
    void rollback_to_version(int versionNumber) {
        VersionNode* current = tail;
        while (current && current->version_No != versionNumber) {
            VersionNode* temp = current;
            current = current->prev;
            delete temp; // Delete newer versions
        }
        if (current) {
            current->next = nullptr;
            tail = current;
            version_count = versionNumber;
        }
        else {
            std::cout << "Version " << versionNumber << " not found.\n";
        }
    }
    // Display the version history
    void display_history() const {
        VersionNode* current = head;
        while (current) {
            std::cout << "Version " << current->version_No << ": " << current->content << "\n";
            current = current->next;
        }
    }
    // Get the content of the most recent version
    std::string get_Current_Version_Content() const {
        return tail ? tail->content : "No versions available.";
    }
};