#include "googleDrive.h"
#include "HashTable.h"
#include "Graph.h"
#include "Dictionary.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
using namespace std;
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
// Enums for user roles and permissions
enum class user_role {
    VIEWER,
    EDITOR,
    ADMIN
};
enum class Permission {
    READ = 1,
    WRITE = 2,
    EXECUTE = 4
};
// Helper functions for permissions
bool hasPermission(user_role role, Permission permission) {
    switch (role) {
    case user_role::ADMIN:
        return true;
    case user_role::EDITOR:
        return permission == Permission::READ || permission == Permission::WRITE;
    case user_role::VIEWER:
        return permission == Permission::READ;
    default:
        return false;
    }
}

bool has_write_access(user_role role) {
    return role == user_role::ADMIN || role == user_role::EDITOR;
}

// RLE compression function
string compress_RLE(const string& input) {
    if (input.empty()) return "";  
    string compressed;
    char current = input[0];
    int count = 1; 
    for (size_t i = 1; i < input.length(); i++) {
        if (input[i] == current) {
            count++;
        } else {
            compressed += to_string(count) + current;
            current = input[i];
            count = 1;
        }
    }
    compressed += to_string(count) + current;
    return compressed;
}
// RLE decompression function
string decompress_RLE(const string& input) {
    if (input.empty()) return "";
    
    string decompressed;
    for (size_t i = 0; i < input.length(); i += 2) {
        int count = input[i] - '0';
        char c = input[i + 1];
        decompressed.append(count, c);
    }
    return decompressed;
}
void clrscr() {
    #ifdef _WIN32
        system("cls"); // Windows
    #else
        system("clear"); // Unix/Linux/Mac
    #endif
}
void display_success(const string& message) {
    cout << GREEN << message << RESET << endl;
}
void display_error(const string& message) {
    cout << RED << message << RESET << endl;
}
void display_prompt(const string& message) {
    cout << YELLOW << message << RESET << endl;
}
void show_loading(const string& message) {
    cout << message;
    for (int i = 0; i < 4; ++i) {
        cout << ".";
        this_thread::sleep_for(chrono::milliseconds(500)); // Add a delay
    }
    cout << endl;
}   
void show_Bread_crumb(const string& currentPath) {
    cout << BLUE << "Current Path: " << currentPath << RESET << endl;
}
void display_menu() {
    cout << "=====================================\n";
    cout << "         Google Drive Menu           \n";
    cout << "=====================================\n";
    
    // Show all options but mark restricted ones for Viewers
    cout << "1. Add Folder (Admin/Editor only)\n";
    cout << "2. Add File (Admin/Editor only)\n";
    cout << "3. Rename (Admin/Editor only)\n";
    cout << "4. Delete (Admin/Editor only)\n";
    cout << "5. Search\n";
    cout << "6. Display Structure\n";
    cout << "7. View Recycle Bin\n";
    cout << "8. Restore File (Admin/Editor only)\n";
    cout << "9. Show Help\n";
    cout << "10. View Recent Files\n";
    cout << "11. Add File Version (Admin/Editor only)\n";
    cout << "12. Rollback File Version (Admin/Editor only)\n";
    cout << "13. View File Version History\n";
    cout << "14. Exit\n";
    cout << "15. Create File (Admin/Editor only)\n";
    cout << "16. Read File\n";
    cout << "17. Update File (Admin/Editor only)\n";
    cout << "18. Delete File (Admin/Editor only)\n";
    cout << "19. Traverse Tree (BFS)\n";
    cout << "20. Traverse Tree (DFS)\n";
    cout << "21. Change User Role (Admin only)\n";
    cout << "22. Compress File (Admin/Editor only)\n";
    cout << "23. Decompress File (Admin/Editor only)\n";
    cout << "24. Queue Cloud Sync (Admin/Editor only)\n";
    cout << "=====================================\n";
}
void display_help() {
    clrscr();
    cout << "=====================================\n";
    cout << "              Help Menu              \n";
    cout << "=====================================\n";
    cout << "1. Add Folder: Add a new folder to the structure.\n";
    cout << "2. Add File: Add a new file to a folder with metadata (type, size, date, owner).\n";
    cout << "3. Rename: Rename a file or folder to a new name.\n";
    cout << "4. Delete: Delete a file or folder. Deleted files are moved to the Recycle Bin.\n";
    cout << "5. Search: Search for a file by name and view its metadata.\n";
    cout << "6. Display Structure: Show the folder structure in a tree format.\n";
    cout << "7. View Recycle Bin: View the most recently deleted file in the Recycle Bin.\n";
    cout << "8. Restore File: Restore the most recently deleted file from the Recycle Bin.\n";
    cout << "9. Show Help: Display this help menu with descriptions of all options.\n";
    cout << "10. View Recent Files: Display a list of recently accessed files.\n";
    cout << "11. Add File Version: Add a new version to a file's version history.\n";
    cout << "12. Rollback File Version: Rollback a file to a specific version from its version history.\n";
    cout << "13. View File Version History: View the version history of a file.\n";
    cout << "14. Exit: Exit the program.\n";
    cout << "15. Create File: Create a new file in the folder structure.\n"; // Added new option description
    cout << "16. Read File: Read the content of a file.\n"; // Added new option description
    cout << "17. Update File: Update the content of a file.\n"; // Added new option description
    cout << "18. Delete File: Delete a file and move it to the Recycle Bin.\n"; // Added new option description
    cout << "21. Change User Role: Change the role of a user (Viewer, Editor, Admin).\n";
    cout << "22. Compress File: Compress the content of a file using RLE.\n";
    cout << "23. Decompress File: Decompress the content of a file using RLE.\n";
    cout << "24. Queue Cloud Sync: Queue a file for synchronization to the cloud.\n";
    cout << "=====================================\n";
    cout << "Press Enter to go back to the main menu...";
    cin.ignore();
    cin.get();
}
bool confirm_action(const string& action) {
    char choice;
    cout << YELLOW << "Are you sure you want to " << action << "? (y/n): " << RESET;
    cin >> choice;
    return (choice == 'y' || choice == 'Y');
}
int get_Valid_Choice() {
    int choice;
    while (true) {
        cout << "Enter your choice: ";
        cin >> choice;
        if (cin.fail() || choice < 1 || choice > 24) {
            cin.clear(); // Clear the error flag
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Discard invalid input
            display_error("Invalid choice! Please enter a number between 1 and 24.");
        } else {
            return choice;
        }
    }
}
void display_tree_with_symbols(Node* node, int depth = 0) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) cout << "  ";
    cout << (node->is_Folder ? "[📁] " : "[📄] ") << node->name << endl;
    display_tree_with_symbols(node->first_child, depth + 1);
    display_tree_with_symbols(node->next_sibling, depth);
}
// Update the logIn function to include role selection
bool logIn(Graph& userGraph, string& currentUser, user_role& currentRole) {
    clrscr();
    cout << "Please log in to continue.\n";
    string userName, password;
    cout << "Enter username: ";
    cin >> userName;
    cout << "Enter password: ";
    cin >> password;
    userGraph.logIn(userName, password);
    if (userGraph.find_user(userName)) {
        currentUser = userName;
        
        // Add role selection prompt
        cout << "\nSelect your role:\n";
        cout << "1. Viewer (can only read files)\n";
        cout << "2. Editor (can read and modify files)\n";
        cout << "3. Admin (full access)\n";
        cout << "Enter your choice (1-3): ";
        
        int roleChoice;
        cin >> roleChoice;
        
        switch (roleChoice) {
            case 1:
                currentRole = user_role::VIEWER;
                cout << "Logged in as Viewer\n";
                break;
            case 2:
                currentRole = user_role::EDITOR;
                cout << "Logged in as Editor\n";
                break;
            case 3:
                currentRole = user_role::ADMIN;
                cout << "Logged in as Admin\n";
                break;
            default:
                currentRole = user_role::VIEWER;
                cout << "Invalid choice. Defaulting to Viewer role\n";
        }
        
        display_success("Logged in successfully!");
        return true;
    }
    else {
        display_error("Invalid username or password! Please try again.");
        return false;
    }
}
void signUp(Graph& userGraph) {
    cout << "Welcome to Google Drive!\n";
    cout << "Please sign up to create an account.\n";
    string userName, password, question, answer;
    cout << "Enter username: ";
    cin >> userName;
    cout << "Enter password: ";
    cin >> password;
    cout << "Enter security question: ";
    cin.ignore();
    getline(cin, question);
    cout << "Enter answer to security question: ";
    getline(cin, answer);
    userGraph.signUp(userName, password, question, answer);
    display_success("User signed up successfully!");
}
void display_auth_menu() {
    cout << "=====================================\n";
    cout << "||       Google Drive Login        ||\n";
    cout << "=====================================\n";
    cout << "|| 1. Sign Up                      ||\n";
    cout << "|| 2. Log In                       ||\n";
    cout << "|| 3. Forgot Password              ||\n";
    cout << "|| 4. Exit                         ||\n";
    cout << "=====================================\n";
    cout << "Enter your Choice: ";
}
void forgot_Password(Graph& userGraph) {
    clrscr();
    cout << "Forgot Password\n";
    string userName, answer;
    cout << "Enter your username: ";
    cin >> userName;
    cout << "Enter the answer to your security question: ";
    cin.ignore();
    getline(cin, answer);
    userGraph.recover_password(userName, answer);
}
int main() {
    Google_Drive drive;
    Hash_table hashTable;
    Graph userGraph;
    string currentUser;
    user_role currentRole = user_role::VIEWER; // Default role
    bool isAuthenticated = false;
    Dictionary_Compressor compressor;
    Background_Sync syncManager;
    // Authentication Menu
    while (!isAuthenticated) {
        clrscr();
        display_auth_menu();
        int authChoice;
        cin >> authChoice;  
        switch (authChoice) {
        case 1: // Sign Up
            signUp(userGraph);
            break;
        case 2: // Log In
            isAuthenticated = logIn(userGraph, currentUser, currentRole);
            if (!isAuthenticated) {
                display_prompt("Press Enter to try again...");
                cin.ignore();
                cin.get();
            }
            break;
        case 3: // Forgot Password
            forgot_Password(userGraph);
            display_prompt("Press Enter to return to the menu...");
            cin.ignore();
            cin.get();
            break;
        case 4: // Exit
            cout << "Exiting...\n";
            return 0;
        default:
            display_error("Invalid choice! Please try again.");
        }
    }
    int choice;
    bool exitProgram = false;
    do {
        clrscr();
        display_menu();
        choice = get_Valid_Choice();
        switch (choice) {
        case 1: { // Add Folder
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can add folders.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clrscr();
                string parentName, folderName;
                cout << "Enter parent folder name: ";
                cin >> parentName;
                cout << "Enter folder name: ";
                cin >> folderName;
                drive.add_child(parentName, folderName, true);
                display_prompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 2: { // Add File
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can add files.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clrscr();
                string parentName, fileName, fileType, fileDate, fileOwner;
                int fileSize;
                cout << "Enter parent folder name: ";
                cin >> parentName;
                cout << "Enter file name: ";
                cin >> fileName;
                cout << "Enter file type: ";
                cin >> fileType;
                cout << "Enter file size (in KB): ";
                cin >> fileSize;
                cout << "Enter file creation date: ";
                cin >> fileDate;
                cout << "Enter file owner: ";
                cin >> fileOwner;
                drive.add_child(parentName, fileName, false);
                hashTable.insert(fileName, fileType, fileSize, fileDate, fileOwner);
                display_success("File added successfully!");
                display_prompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 3: { // Rename
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can rename files.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clrscr();
                string oldName, newName;
                cout << "Enter current name: ";
                cin >> oldName;
                cout << "Enter new name: ";
                cin >> newName;
                Node* node = drive.search_node(oldName);
                if (node) {
                    if (!node->is_Folder) {
                        File_Metadata* metadata = hashTable.search(oldName);
                        if (metadata) {
                            hashTable.remove(oldName);
                            hashTable.insert(newName, metadata->type, metadata->size, metadata->date, metadata->owner);
                        }
                    }
                    drive.rename_node(oldName, newName);
                } else {
                    display_error("File not found!");
                }
                display_prompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 4: { // Delete
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can delete files.");
                cin.ignore();
                cin.get();
                break;
            }
            bool goBack = false;
            do {
                clrscr();
                string name;
                cout << "Enter name to delete: ";
                cin >> name;
                Node* node = drive.search_node(name);
                if (node) {
                    if (!node->is_Folder) {
                        hashTable.remove(name);
                    }
                    if (confirm_action("delete this file")) {
                        drive.delete_node(name);
                        display_success("File deleted successfully!");
                    } else {
                        display_prompt("Action canceled.");
                    }
                } else {
                    display_error("File not found!");
                }
                display_prompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 5: { // Search
            bool goBack = false;
            do {
                clrscr();
                std::string fileName;
                std::cout << "Enter the file name to search: ";
                std::cin >> fileName;
                drive.search_file(fileName);
                display_prompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') break;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 6: { // Display Structure
            bool goBack = false;
            do {
                clrscr();
                display_tree_with_symbols(drive.get_root());
                display_prompt("Do you want to go back to the main menu? (y/n): ");
                char backChoice;
                cin >> backChoice;
                if (backChoice == 'y' || backChoice == 'Y') goBack = true;
                else if (backChoice == 'n' || backChoice == 'N') goBack = false;
            } while (!goBack);
            break;
        }
        case 7: { // View Recycle Bin
            if (drive.get_recycle_bin().isEmpty()) {
                display_error("Recycle Bin is empty.");
            } else {
                File_Metadata* file = drive.get_recycle_bin().peek();
                cout << "Most recently deleted file: " << file->name << endl;
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 8: { // Restore File
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can restore files.");
                cin.ignore();
                cin.get();
                break;
            }
            drive.restore_file();
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 9: {
            display_help();
            break;
        }
        case 10: {
            clrscr();
            if (drive.get_recent_files().isEmpty()) {
                display_error("No recent files to display.");
            } else {
                cout << "Recently accessed files:\n";
                drive.get_recent_files().display();
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 11: { // Add File Version
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can add file versions.");
                cin.ignore();
                cin.get();
                break;
            }
            clrscr();
            string fileName, content;            
            try {
                cout << "Enter the file name to add a version: ";
                cin >> fileName;
                if (fileName.empty()) {
                    throw runtime_error("File name cannot be empty!");
                }
                File_Metadata* metadata = hashTable.search(fileName);
                if (!metadata) {
                    throw runtime_error("File not found!");
                }
                if (metadata->owner != currentUser) {
                    bool hasAccess = false;
                    userGraph.BFS(currentUser, fileName);
                    if (!hasAccess) {
                        throw runtime_error("Access denied! You don't have permission to modify this file.");
                    }
                }
                cout << "Enter the content for the new version: ";
                cin.ignore();
                getline(cin, content);
                if (content.empty()) {
                    throw runtime_error("Version content cannot be empty!");
                }
                metadata->version_history.add_version(content);
                display_success("New version added successfully!");

            } catch (const exception& e) {
                display_error(e.what());
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 12: { 
            clrscr();
            string fileName;
            int versionNumber;
            try {
                cout << "Enter the file name to rollback: ";
                cin >> fileName;
                if (fileName.empty()) {
                    throw runtime_error("File name cannot be empty!");
                }
                File_Metadata* metadata = hashTable.search(fileName);
                if (!metadata) {
                    throw runtime_error("File not found!");
                }
                if (metadata->owner != currentUser) {
                    bool hasAccess = false;
                    userGraph.BFS(currentUser, fileName);
                    if (!hasAccess) {
                        throw runtime_error("Access denied! You don't have permission to modify this file.");
                    }
                }
                cout << "\nAvailable versions:\n";
                metadata->version_history.display_history();
                cout << "\nEnter the version number to rollback to: ";
                if (!(cin >> versionNumber)) {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    throw runtime_error("Invalid version number!");
                }
                metadata->version_history.rollback_to_version(versionNumber);
                display_success("File rolled back to version " + to_string(versionNumber) + " successfully!");
            } catch (const exception& e) {
                display_error(e.what());
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 13: {
            clrscr();
            string fileName;
            try {
                cout << "Enter the file name to view version history: ";
                cin >> fileName;
                if (fileName.empty()) {
                    throw runtime_error("File name cannot be empty!");
                }
                File_Metadata* metadata = hashTable.search(fileName);
                if (!metadata) {
                    throw runtime_error("File not found!");
                }
                if (metadata->owner != currentUser) {
                    bool hasAccess = false;
                    userGraph.BFS(currentUser, fileName);
                    if (!hasAccess) {
                        throw runtime_error("Access denied! You don't have permission to view this file's history.");
                    }
                }
                cout << "\nVersion history for file: " << fileName << endl;
                cout << "Owner: " << metadata->owner << endl;
                cout << "Last Modified: " << metadata->date << endl;
                cout << "\nVersions:\n";
                metadata->version_history.display_history();
            } catch (const exception& e) {
                display_error(e.what());
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 14: // Exit
            cout << "Exiting...\n";
            exitProgram = true;
            break;
        case 15: { // Create File
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can create files.");
                cin.ignore();
                cin.get();
                break;
            }
            clrscr();
            string parentName, fileName, fileType, fileDate, fileOwner;
            int fileSize;
            cout << "Enter parent folder name: ";
            cin >> parentName;
            cout << "Enter file name: ";
            cin >> fileName;
            cout << "Enter file type: ";
            cin >> fileType;
            cout << "Enter file size (in KB): ";
            cin >> fileSize;
            cout << "Enter file creation date: ";
            cin >> fileDate;
            cout << "Enter file owner: ";
            cin >> fileOwner;
            drive.add_child(parentName, fileName, false);
            hashTable.insert(fileName, fileType, fileSize, fileDate, fileOwner);
            display_success("File created successfully!");
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 16: { 
            clrscr();
            string fileName;
            cout << "Enter the file name to read: ";
            cin >> fileName;
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "File Details:\n";
                cout << "Name: " << metadata->name << "\n";
                cout << "Type: " << metadata->type << "\n";
                cout << "Size: " << metadata->size << " KB\n";
                cout << "Date: " << metadata->date << "\n";
                cout << "Owner: " << metadata->owner << "\n";
                cout << "Content: " << metadata->version_history.get_Current_Version_Content() << "\n";
            }
            else {
                display_error("File not found!");
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 17: { // Update File
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can update files.");
                cin.ignore();
                cin.get();
                break;
            }
            clrscr();
            string fileName, newContent;
            cout << "Enter the file name to update: ";
            cin >> fileName;
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "Enter the new content for the file: ";
                cin.ignore(); 
                getline(cin, newContent);
                metadata->version_history.add_version(metadata->version_history.get_Current_Version_Content());
                metadata->version_history.add_version(newContent);
                display_success("File updated successfully!");
            } 
            else {
                display_error("File not found!");
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 18: { // Delete File
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can delete files.");
                cin.ignore();
                cin.get();
                break;
            }
            clrscr();
            string fileName;
            cout << "Enter the file name to delete: ";
            cin >> fileName;
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                drive.delete_node(fileName);
                drive.get_recycle_bin().push(metadata);
                display_success("File deleted successfully and moved to Recycle Bin!");
            } 
            else {
                display_error("File not found!");
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 19: {
            clrscr();
            cout << "BFS Tree Traversal for File Access\n";
            cout << "===================================\n";
            string fileName;
            cout << "Enter file name to check access: ";
            cin >> fileName;
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "\nPerforming BFS traversal to find users with access to '" << fileName << "':\n";
                show_loading("Traversing");
                userGraph.BFS(currentUser, fileName);
            }
            else {
                display_error("File not found in the system!");
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 20: {
            clrscr();
            cout << "DFS Tree Traversal for File Access\n";
            cout << "===================================\n";
            string fileName;
            cout << "Enter file name to check access: ";
            cin >> fileName;
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                cout << "\nPerforming DFS traversal to find users with access to '" << fileName << "':\n";
                show_loading("Traversing");
                userGraph.DFS(currentUser, fileName);
            }
            else {
                display_error("File not found in the system!");
            }
            display_prompt("Press Enter to return to the main menu...");
            cin.ignore();
            cin.get();
            break;
        }
        case 21: { // Change User Role
            if (currentRole != user_role::ADMIN) {
                display_error("Only administrators can change user roles!");
                break;
            }
            string username;
            int roleChoice;
            cout << "Enter username: ";
            cin >> username;
            cout << "Select role (1: Viewer, 2: Editor, 3: Admin): ";
            cin >> roleChoice;
            user_role newRole;
            switch (roleChoice) {
                case 1: newRole = user_role::VIEWER; break;
                case 2: newRole = user_role::EDITOR; break;
                case 3: newRole = user_role::ADMIN; break;
                default: newRole = user_role::VIEWER;
            }            
            // Update user role in graph
            display_success("User role updated successfully!");
            break;
        }
        case 22: { // Compress File
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can compress files.");
                cin.ignore();
                cin.get();
                break;
            }
            clrscr();
            string fileName;
            cout << "Enter file name to compress: ";
            cin >> fileName;
            
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                string content = metadata->version_history.get_Current_Version_Content();
                string compressed = compress_RLE(content);
                metadata->version_history.add_version(compressed);
                display_success("File compressed successfully!");
                
                // Queue for cloud sync
                syncManager.QSync(fileName, compressed);
            } else {
                display_error("File not found!");
            }
            break;
        }
        case 23: { // Decompress File
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can decompress files.");
                cin.ignore();
                cin.get();
                break;
            }
            clrscr();
            string fileName;
            cout << "Enter file name to decompress: ";
            cin >> fileName;
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                string content = metadata->version_history.get_Current_Version_Content();
                string decompressed = decompress_RLE(content);
                metadata->version_history.add_version(decompressed);
                display_success("File decompressed successfully!");
            } else {
                display_error("File not found!");
            }
            break;
        }
        case 24: { // Queue Cloud Sync
            if (!has_write_access(currentRole)) {
                display_error("Access denied! Only Administrators and Editors can queue cloud sync.");
                cin.ignore();
                cin.get();
                break;
            }
            clrscr();
            string fileName;
            cout << "Enter file name to sync: ";
            cin >> fileName;
            
            File_Metadata* metadata = hashTable.search(fileName);
            if (metadata) {
                string content = metadata->version_history.get_Current_Version_Content();
                syncManager.QSync(fileName, content);
                display_success("File queued for synchronization!");
            } else {
                display_error("File not found!");
            }
            break;
        }
        default:
            cout << "Invalid choice!\n";
        }
    } while (!exitProgram);
    return 0;
}