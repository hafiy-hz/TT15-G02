// my_queue.h
// Custom Queue implementation (no STL allowed)
// Written by: Member 3
//
// Used by the Runner to read raw lines from the .asm file before
// parsing them into Instruction objects (one-by-one, FIFO order).

#ifndef MY_QUEUE_H
#define MY_QUEUE_H

#include <iostream>
using namespace std;

class MyQueue {
private:
    string* data;     // dynamic array holding queue items
    int capacity;      // max number of items the queue can hold
    int frontIndex;    // index of the front item
    int rearIndex;     // index of the next free slot
    int count;         // current number of items in the queue

public:
    // Constructor: allocate array with given capacity
    MyQueue(int cap) {
        capacity = cap;
        data = new string[capacity];
        frontIndex = 0;
        rearIndex = 0;
        count = 0;
    }

    // Destructor: free the dynamically allocated array
    ~MyQueue() {
        delete[] data;
    }

    // Returns true if the queue has no items
    bool isEmpty() {
        return count == 0;
    }

    // Returns true if the queue is at capacity
    bool isFull() {
        return count == capacity;
    }

    // Add an item to the back of the queue
    void enqueue(string item) {
        if (isFull()) {
            cout << "Error: Queue is full, cannot enqueue." << endl;
            return;
        }
        data[rearIndex] = item;
        rearIndex = (rearIndex + 1) % capacity; // wrap around
        count++;
    }

    // Remove and return the item at the front of the queue
    string dequeue() {
        if (isEmpty()) {
            cout << "Error: Queue is empty, cannot dequeue." << endl;
            return "";
        }
        string item = data[frontIndex];
        frontIndex = (frontIndex + 1) % capacity; // wrap around
        count--;
        return item;
    }

    // Look at the front item without removing it
    string peek() {
        if (isEmpty()) {
            cout << "Error: Queue is empty, cannot peek." << endl;
            return "";
        }
        return data[frontIndex];
    }

    // Return current number of items
    int size() {
        return count;
    }
};

#endif