#pragma once
#ifndef GOOGLEDRIVE_H
#define GOOGLEDRIVE_H
#include <string>
#include <queue>
#include "Queue.h"
#include "Stack.h"
#include "Graph.h"
#include "DoublyLinkedList.h"
#include "HashTable.h"
struct Node {
    std::string name;
    bool is_Folder;
    Node* first_child;
    Node* next_sibling;
    Node(const std::string& name, bool isFolder);
};

class Google_Drive {
private:
    Node* root;
    Stack recycle_bin; // Stack for recently deleted files
    Queue recent_files; // Queue for recently accessed files
    Graph user_graph;   // Graph for user connections and file sharing
    Hash_table file_hash_table;
    // Helper recursive functions
    void display_tree_helper(Node* node, int depth);
    void delete_Tree(Node* node);
    Node* search_node_helper(Node* node, const std::string& name);

public:
    Google_Drive();               // Constructor
    ~Google_Drive();              // Destructor
    void add_child(const std::string& parentName, const std::string& name, bool isFolder);
    void display_tree();
    Node* search_node(const std::string& name);
    void rename_node(const std::string& currentName, const std::string& newName);
    void delete_node(const std::string& name);
    void restore_file();
    Node* get_root();
    Stack& get_recycle_bin();
    Queue& get_recent_files(); // Getter for the recent files queue
    Graph& get_user_graph();   // Getter for the user graph
    void search_file(const std::string& fileName);
    
};
#endif // GOOGLEDRIVE_H