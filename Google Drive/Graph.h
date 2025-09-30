#pragma once
#include <string>
#include <iostream>
#include <fstream> // For file handling
#include <ctime> // For time and date
#include <queue>
#include <stack>
#include <unordered_set>
#include "HashTable.h" // For custom hash table
#include "DoublyLinkedLIst.h" // For linked list to store shared files

class Graph {
private:
    struct User_node {
        std::string user_name;       // User's name
        std::string password;       // User's password
        std::string security_Q; // Security question for password recovery
        std::string security_Ans;   // Answer to the security question
        std::string last_login_time;    // Last login time
        std::string last_logout_time;   // Last logout time
        DoublyLinkedList shared_files; // Linked list of shared files
        std::vector<User_node*> shared_with; // List of users this user has shared files with
        struct SharedWithNode {       // Linked list for users this user has shared files with
            User_node* user;
            SharedWithNode* next;
            SharedWithNode(User_node* user) : user(user), next(nullptr) {}
        };

        SharedWithNode* shared_with_head; // Head of the sharedWith linked list
        User_node* next;                 // Pointer to the next user in the hash table

        User_node(const std::string& name, const std::string& pass = "", const std::string& question = "", const std::string& answer = "")
            : user_name(name), password(pass), security_Q(question), security_Ans(answer), shared_with_head(nullptr), next(nullptr) {}
    };

    User_node** table; // Hash table to store users
    int tab_size;    // Size of the hash table
    const std::string user_file = "users.txt"; // File to store user details

    // Hash function to map user names to indices
    int hashfunc(const std::string& key) const {
        int hash = 0;
        for (char ch : key) {
            hash = (hash * 31 + ch) % tab_size;
        }
        return hash;
    }

    // Get the current time as a string
    std::string get_current_time() const {
        time_t now = time(nullptr);
        char buffer[26];
        ctime_s(buffer, sizeof(buffer), &now);
        return std::string(buffer);
    }

    // Save user details to the file
    void save_user_to_file(const User_node* user) const {
        std::ofstream outFile(user_file, std::ios::app); // Append mode
        if (outFile.is_open()) {
            outFile << user->user_name << ","
                    << user->password << ","
                    << user->security_Q << ","
                    << user->security_Ans << "\n";
            outFile.close();
        } else {
            std::cerr << "Error: Unable to open file for writing.\n";
        }
    }

    // Load all users from the file
    void load_users_from_file() {
        std::ifstream inFile(user_file);
        if (inFile.is_open()) {
            std::string line;
            while (std::getline(inFile, line)) {
                size_t pos1 = line.find(',');
                size_t pos2 = line.find(',', pos1 + 1);
                size_t pos3 = line.find(',', pos2 + 1);

                if (pos1 != std::string::npos && pos2 != std::string::npos && pos3 != std::string::npos) {
                    std::string userName = line.substr(0, pos1);
                    std::string password = line.substr(pos1 + 1, pos2 - pos1 - 1);
                    std::string question = line.substr(pos2 + 1, pos3 - pos2 - 1);
                    std::string answer = line.substr(pos3 + 1);

                    add_user(userName, password, question, answer);
                }
            }
            inFile.close();
        } else {
            std::cerr << "Error: Unable to open file for reading.\n";
        }
    }

public:
    Graph(int size = 10) : tab_size(size) {
        table = new User_node * [tab_size];
        for (int i = 0; i < tab_size; ++i) {
            table[i] = nullptr;
        }
        load_users_from_file(); // Load users from file at startup
    }

    ~Graph() {
        for (int i = 0; i < tab_size; ++i) {
            User_node* current = table[i];
            while (current) {
                User_node* temp = current;
                current = current->next;
                delete temp;
            }
        }
        delete[] table;
    }

    // Find a user node in the hash table
    User_node* find_user(const std::string& user) const {
        int index = hashfunc(user);
        User_node* current = table[index];
        while (current) {
            if (current->user_name == user) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    // Add a user to the graph
    void add_user(const std::string& user, const std::string& pass = "", const std::string& question = "", const std::string& answer = "") {
        if (find_user(user)) {
            return; // User already exists
        }

        int index = hashfunc(user);
        User_node* newUser = new User_node(user, pass, question, answer);
        newUser->next = table[index];
        table[index] = newUser;
    }

    // Sign up a new user
    void signUp(const std::string& user, const std::string& pass, const std::string& question, const std::string& answer) {
        if (find_user(user)) {
            std::cout << "User already exists!\n";
            return;
        }

        add_user(user, pass, question, answer);
        save_user_to_file(find_user(user)); // Save the new user to the file
        std::cout << "User signed up successfully!\n";
    }

    // Log in a user
    void logIn(const std::string& user, const std::string& pass) {
        User_node* user_node = find_user(user);
        if (!user_node) {
            std::cout << "User not found!\n";
            return;
        }

        if (user_node->password != pass) {
            std::cout << "Incorrect password!\n";
            return;
        }

        user_node->last_login_time = get_current_time();
        std::cout << "User logged in successfully at " << user_node->last_login_time;
    }

    // Log out a user
    void logOut(const std::string& user) {
        User_node* user_Node = find_user(user);
        if (!user_Node) {
            std::cout << "User not found!\n";
            return;
        }

        user_Node->last_logout_time = get_current_time();
        std::cout << "User logged out successfully at " << user_Node->last_logout_time;
    }

    // Recover password
    void recover_password(const std::string& user, const std::string& answer) {
        User_node* userNode = find_user(user);
        if (!userNode) {
            std::cout << "User not found!\n";
            return;
        }

        if (userNode->security_Ans != answer) {
            std::cout << "Incorrect answer to the security question!\n";
            return;
        }

        std::cout << "Your password is: " << userNode->password << "\n";
    }

    // Share a file between two users
    void share_file(const std::string& fromUser, const std::string& toUser, const std::string& fileName) {
        User_node* from_node = find_user(fromUser);
        User_node* to_node = find_user(toUser);

        if (!from_node || !to_node) {
            std::cout << "One or both users not found!\n";
            return;
        }

        // Add the file to the shared files list of the sender
        from_node->shared_files.add_version(fileName);

        // Add the recipient to the sender's sharedWith list
        from_node->shared_with.push_back(to_node);

        std::cout << "File '" << fileName << "' shared from " << fromUser << " to " << toUser << ".\n";
    }

    // View files shared by a user
    void view_shared_files(const std::string& user) const {
        User_node* userNode = find_user(user);
        if (!userNode || userNode->shared_files.get_Current_Version_Content().empty()) {
            std::cout << "No files shared by " << user << ".\n";
            return;
        }

        std::cout << "Files shared by " << user << ":\n";
        userNode->shared_files.display_history();
    }

    // Perform BFS to find all users who have access to a specific file
    void BFS(const std::string& startUser, const std::string& fileName) {
        User_node* start_node = find_user(startUser);
        if (!start_node) {
            std::cout << "User not found!\n";
            return;
        }

        std::queue<User_node*> q;
        std::unordered_set<std::string> visited;

        q.push(start_node);
        visited.insert(start_node->user_name);

        std::cout << "BFS Traversal for file '" << fileName << "':\n";

        while (!q.empty()) {
            User_node* current = q.front();
            q.pop();

            // Check if the current user has the file
            if (current->shared_files.get_Current_Version_Content() == fileName) {
                std::cout << current->user_name << " has access to the file.\n";
            }

            // Add all unvisited neighbors to the queue
            for (User_node* neighbor : current->shared_with) {
                if (visited.find(neighbor->user_name) == visited.end()) {
                    q.push(neighbor);
                    visited.insert(neighbor->user_name);
                }
            }
        }
    }

    // Perform DFS to find all users who have access to a specific file
    void DFS(const std::string& startUser, const std::string& fileName) {
        User_node* start_Node = find_user(startUser);
        if (!start_Node) {
            std::cout << "User not found!\n";
            return;
        }

        std::stack<User_node*> s;
        std::unordered_set<std::string> visited;

        s.push(start_Node);
        visited.insert(start_Node->user_name);

        std::cout << "DFS Traversal for file '" << fileName << "':\n";

        while (!s.empty()) {
            User_node* current = s.top();
            s.pop();

            // Check if the current user has the file
            if (current->shared_files.get_Current_Version_Content() == fileName) {
                std::cout << current->user_name << " has access to the file.\n";
            }

            // Add all unvisited neighbors to the stack
            for (User_node* neighbor : current->shared_with) {
                if (visited.find(neighbor->user_name) == visited.end()) {
                    s.push(neighbor);
                    visited.insert(neighbor->user_name);
                }
            }
        }
    }

    // Display all user connections
    void display_connections() const {
        std::cout << "User Connections and Shared Files:\n";
        for (int i = 0; i < tab_size; ++i) {
            User_node* current = table[i];
            while (current) {
                std::cout << current->user_name << ":\n";
                current->shared_files.display_history();
                current = current->next;
            }
        }
    }

    // Display all users and their login/logout times
    void display_users() const {
        for (int i = 0; i < tab_size; ++i) {
            User_node* current = table[i];
            while (current) {
                std::cout << "User: " << current->user_name << "\n";
                std::cout << "Last Login: " << (current->last_login_time.empty() ? "Never" : current->last_login_time) << "\n";
                std::cout << "Last Logout: " << (current->last_logout_time.empty() ? "Never" : current->last_logout_time) << "\n";
                current = current->next;
            }
        }
    }
};