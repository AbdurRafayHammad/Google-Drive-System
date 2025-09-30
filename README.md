Google Drive File System The Google Drive File System is a console-based application that simulates a powerful cloud storage platform. Built in C++, this system uses core data structures and OOP principles to deliver functionalities like file versioning, compression, user authentication, and file sharing — all within a terminal interface.
Introduction This project aims to replicate essential features of cloud storage systems (like Google Drive) using efficient C++ data structures. It emphasizes optimized storage, secure user management, version tracking, and real-time file operations, providing a robust backend for virtual file handling.
Data Structures Used Tree — Folder Structure Hierarchical directory structure using a tree, supporting both depth-first and breadth-first traversal.
Hash Table — File Metadata Lookup File metadata (size, type, date, owner) is stored in a hash table for O(1) search, insertion, and update.
Stack — Recycle Bin Implements LIFO strategy for deleted files, allowing efficient recovery of the most recently deleted items.
Queue — Recent Files (LRU) Maintains recently accessed files in a FIFO order to simulate Least Recently Used (LRU) caching behavior.
Graph — User Authentication & File Sharing A graph tracks users as nodes and file-sharing relationships as directed edges, enabling BFS/DFS traversal for access control and sharing.
Doubly Linked List — Version History Each file maintains its version history in a doubly linked list, supporting rollback and update tracking.
Functionalities by Level 2.1 Basic Functionalities Folder/File Management Create, navigate, delete folders and files using tree + hash table.
File CRUD Operations Create, Read, Update (store version), and Delete (send to Recycle Bin).
Recycle Bin Stack-based mechanism to recover most recently deleted files. Includes time-based auto-deletion.
Recent Files Queue-based access tracking system for fast retrieval of most-used files.
User Authentication Sign up, login, logout with time stamps. Supports password recovery via security questions.
2.2 Intermediate Functionalities File Search Combines tree traversal with hash-based lookup.
Graph-based File Sharing Share files among users using directed edges. Visualize access via BFS/DFS traversal.
File Versioning Add/rollback versions of a file using doubly linked list for tracking changes.
Robust Error Handling Input validation, duplication checks, access rights, and exceptions for safe operations.
2.3 Advanced Functionalities Access Control & Permissions Role-based access (Admin, Editor, Viewer) with: Admin: Full control (read/write/execute) Editor: Read and Write Viewer: Read-only Directed edges define permission levels.
Compression Algorithms Run-Length Encoding (RLE): AAAAABBBCC → 5A3B2C Dictionary Compression: Replaces repeated words with codes
Cloud Sync Queue Background sync using queue mechanism to simulate cloud storage syncing.
Scalability & Optimization Uses balanced trees like AVL (planned) Simulates garbage collection to manage memory
Requirements C++ Compiler (g++, MSVC, MinGW) Console terminal (Windows or Linux) No third-party libraries required
Concepts Demonstrated OOP: Inheritance, Encapsulation, Polymorphism File I/O: fstream for user persistence and metadata Data Structures: Stack, Queue, Linked List, Hash Table, Tree, Graph Algorithms: BFS, DFS, Hashing, Compression (RLE/Dictionary) Role-based Access Control CLI UX: Color-coded output, loading animation, prompts
About Developed as a term project for the Data Structures & Algorithms course, this simulation demonstrates how complex systems like cloud file storage can be built using simple yet powerful core data structures. It serves as a great learning resource for understanding how theoretical structures apply to real-world problems.
