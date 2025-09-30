#include "googleDrive.h"
#include <iostream>
#include "Stack.h"
using namespace std;
Node::Node(const string& name, bool isFolder) {
    this->name = name;
    this->is_Folder = isFolder;
    this->first_child = nullptr;
    this->next_sibling = nullptr;
}
Google_Drive::Google_Drive() : recent_files(5) { // Set the queue capacity to 5
    root = new Node("Root", true);
}
Google_Drive::~Google_Drive() {
    delete_Tree(root);
}
void Google_Drive::delete_Tree(Node* node) {
    if (!node) return;
    delete_Tree(node->first_child);
    delete_Tree(node->next_sibling);
    delete node;
}
Node* Google_Drive::get_root() {
    return root; // Return the root node
}
void Google_Drive::add_child(const string& parentName, const string& name, bool isFolder) {
    // Find the parent node
    Node* parent = search_node(parentName);
    if (!parent || !parent->is_Folder) {
        cout << "Parent folder '" << parentName << "' not found or is not a folder!" << endl;
        return;
    }
    // Create the new node
    Node* newNode = new Node(name, isFolder);
    newNode->first_child = nullptr;
    newNode->next_sibling = nullptr;
    // Add the new node as a child of the parent
    if (!parent->first_child) {
        parent->first_child = newNode;
    }
    else {
        Node* temp = parent->first_child;
        while (temp->next_sibling) {
            temp = temp->next_sibling;
        }
        temp->next_sibling = newNode;
    }
}
void Google_Drive::display_tree() {
    display_tree_helper(root, 0);
}
void Google_Drive::display_tree_helper(Node* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) cout << "  ";
    cout << (node->is_Folder ? "[Folder] " : "[File] ") << node->name << endl;
    display_tree_helper(node->first_child, depth + 1);
    display_tree_helper(node->next_sibling, depth);
}
Node* Google_Drive::search_node_helper(Node* node, const string& name) {
    if (!node) return nullptr;
    // Check the current node
    if (node->name == name) return node;
    // Recursively search in the first child
    Node* found = search_node_helper(node->first_child, name);
    if (found) return found;
    // Recursively search in the next sibling
    return search_node_helper(node->next_sibling, name);
}
void Google_Drive::rename_node(const string& currentName, const string& newName) {
    Node* node = search_node(currentName);
    if (node) {
        node->name = newName;
    }
    else {
        cout << "Node '" << currentName << "' not found!" << endl;
    }
}
void Google_Drive::delete_node(const std::string& name) {
    if (root->name == name) {
        cout << "Cannot delete the root folder!" << endl;
        return;
    }
    Node* parent = nullptr;
    Node* current = root;
    queue<Node*> q;
    q.push(root);
    while (!q.empty()) {
        Node* temp = q.front();
        q.pop();
        Node* child = temp->first_child;
        while (child) {
            if (child->name == name) {
                parent = temp;
                current = child;
                break;
            }
            q.push(child);
            child = child->next_sibling;
        }
        if (parent) break;
    }
    if (!parent) {
        cout << "Node '" << name << "' not found!" << endl;
        return;
    }
    // Push the file metadata onto the stack
    if (!current->is_Folder) {
        recycle_bin.push(new File_Metadata(current->name, "file", 0, "unknown", "unknown", parent->name));
    }
    // Remove the node from the parent's child list
    if (parent->first_child == current) {
        parent->first_child = current->next_sibling;
    }
    else {
        Node* sibling = parent->first_child;
        while (sibling->next_sibling != current) {
            sibling = sibling->next_sibling;
        }
        sibling->next_sibling = current->next_sibling;
    }
    // Delete the subtree rooted at the current node
    delete_Tree(current);
}
void Google_Drive::restore_file() {
    if (recycle_bin.isEmpty()) {
        cout << "Recycle Bin is empty. No files to restore." << endl;
        return;
    }
    // Pop the most recently deleted file from the stack
    File_Metadata* file = recycle_bin.pop();
    if (!file) return;
    // Restore the file to its original folder
    add_child(file->parent_folder, file->name, false);
    cout << "File '" << file->name << "' has been restored to folder '" << file->parent_folder << "'." << endl;
    delete file; // Free the memory for the file metadata
}
Stack& Google_Drive::get_recycle_bin() {
    return recycle_bin;
}
Queue& Google_Drive::get_recent_files() {
    return recent_files;
}
Node* Google_Drive::search_node(const std::string& name) {
    Node* result = search_node_helper(root, name);
    if (result && !result->is_Folder) {
        // Add the file to the recent files queue
        recent_files.enQ(new File_Metadata(result->name, "file", 0, "unknown", "unknown", "unknown"));
    }
    return result;
}
Graph& Google_Drive::get_user_graph() {
    return user_graph;
}
void Google_Drive::search_file(const std::string& fileName) {
    // Step 1: Check in the hash table for O(1) lookup
    File_Metadata* metadata = file_hash_table.search(fileName);
    if (metadata) {
        std::cout << "File found in hash table:\n";
        std::cout << "Name: " << metadata->name << "\n";
        std::cout << "Type: " << metadata->type << "\n";
        std::cout << "Size: " << metadata->size << " KB\n";
        std::cout << "Date: " << metadata->date << "\n";
        std::cout << "Owner: " << metadata->owner << "\n";
        return;
    }
    // Step 2: Perform pre-order traversal to search in the directory tree
    Node* foundNode = search_node_helper(root, fileName);
    if (foundNode) {
        std::cout << "File found in directory tree:\n";
        std::cout << "Name: " << foundNode->name << "\n";
        std::cout << "Type: " << (foundNode->is_Folder ? "Folder" : "File") << "\n";
    }
    else {
        std::cout << "File not found.\n";
    }
}