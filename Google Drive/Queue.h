#pragma once
#include "Stack.h" // For FileMetadata
#include<iostream>
using namespace std;
struct Qnode {
    File_Metadata* file; // Pointer to the file metadata
    Qnode* next;    // Pointer to the next node in the queue

    Qnode(File_Metadata* file) : file(file), next(nullptr) {}
};
class Queue {
private:
    Qnode* front; // Pointer to the front of the queue
    Qnode* rear;  // Pointer to the rear of the queue
    int size;         // Current size of the queue
    int capacity;     // Maximum capacity of the queue

public:
    Queue(int capacity) : front(nullptr), rear(nullptr), size(0), capacity(capacity) {}
    ~Queue() {
        while (front) {
            Qnode* temp = front;
            front = front->next;
            delete temp->file; // Free the file metadata
            delete temp;       // Free the queue node
        }
    }
    // Add a file to the queue
    void enQ(File_Metadata* file) {
        cout << "Enqueuing file: " << file->name << endl; // Debug message
        // If the queue is full, remove the oldest file
        if (size == capacity) {
            cout << "Queue is full. Dequeuing oldest file." << endl;
            deQ();
        }
        Qnode* newNode = new Qnode(file);
        if (!rear) {
            front = rear = newNode;
        }
        else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }
    // Remove the oldest file from the queue
    void deQ() {
        if (!front) return; // Queue is empty

        Qnode* temp = front;
        front = front->next;
        if (!front) rear = nullptr; // If the queue becomes empty
        delete temp->file;          // Free the file metadata
        delete temp;                // Free the queue node
        size--;
    }
    // Display the contents of the queue
    void display() const {
        Qnode* current = front;
        if (!current) {
            cout << "Queue is empty." << endl;
            return;
        }
        while (current) {
            cout << "File: " << current->file->name << ", Type: " << current->file->type << endl;
            current = current->next;
        }
    }
    // Check if the queue is empty
    bool isEmpty() const {
        return size == 0;
    }
};
