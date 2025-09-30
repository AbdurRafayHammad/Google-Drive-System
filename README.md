Google Drive File System
Introduction

The Google Drive File System is a console-based C++ application that simulates a cloud storage platform. It uses core data structures and object-oriented programming principles to replicate essential features such as file versioning, compression, user authentication, and file sharing — all within a terminal interface.

This project demonstrates how cloud storage backends can be efficiently implemented with fundamental data structures, providing secure user management, optimized storage, and real-time file operations.

Data Structures Used

Tree — Folder Structure
Implements a hierarchical directory system with support for DFS and BFS traversal.

Hash Table — File Metadata Lookup
Stores metadata (size, type, date, owner) for O(1) search, insertion, and updates.

Stack — Recycle Bin
Provides LIFO-based recovery of deleted files, including time-based auto-deletion.

Queue — Recent Files (LRU)
Maintains a FIFO cache for recently accessed files to simulate LRU caching.

Graph — User Authentication & File Sharing
Tracks users as nodes with directed edges representing file-sharing relationships.

Doubly Linked List — Version History
Stores and manages file versions, enabling rollback and update tracking.

Functionalities
1. Basic Functionalities
Folder/File Management: Create, navigate, and delete folders/files.
File CRUD Operations: Create, Read, Update (store versions), Delete (send to Recycle Bin).
Recycle Bin: Recover most recently deleted files via stack.
Recent Files: Track recent file access using queue-based LRU.
User Authentication: Sign up, login, logout with timestamps. Supports password recovery.
2. Intermediate Functionalities
File Search: Combines tree traversal and hash lookups.
Graph-based File Sharing: Share files among users with BFS/DFS traversal.
File Versioning: Track/rollback file changes using doubly linked lists.
Robust Error Handling: Input validation, duplication checks, and exceptions.
3. Advanced Functionalities

Access Control & Permissions:

Admin: Full control (read/write/execute)
Editor: Read and Write
Viewer: Read-only
(Defined via directed edges in the graph.)

Compression Algorithms:

Run-Length Encoding (RLE): e.g., AAAAABBBCC → 5A3B2C
Dictionary Compression: Replaces repeated words with codes

Cloud Sync Simulation: Background syncing with queue-based mechanisms.

Scalability & Optimization: Planned AVL tree support, memory management via simulated garbage collection.

Requirements
C++ Compiler (g++, MSVC, MinGW)
Console terminal (Windows or Linux)
No third-party libraries required
Concepts Demonstrated
OOP: Inheritance, Encapsulation, Polymorphism
File I/O: Persistent storage using fstream
Data Structures: Stack, Queue, Linked List, Hash Table, Tree, Graph
Algorithms: BFS, DFS, Hashing, Compression (RLE/Dictionary)
Access Control: Role-based permissions
CLI UX: Color-coded output, animations, user prompts
About

Developed as a term project for the Data Structures & Algorithms course, this system shows how complex cloud file storage platforms can be simulated using simple yet powerful data structures. It bridges theory and practice, making it an excellent learning resource for understanding how data structures power real-world systems.
