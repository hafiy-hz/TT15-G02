// test_my_queue.cpp
// Standalone test for MyQueue - run this to confirm it works
// before integrating into the main project.

#include "../src/my_queue.h"
#include <iostream>
using namespace std;

int main() {
    MyQueue q(5); // small capacity to easily test full/empty/wraparound

    cout << "Testing isEmpty on new queue: " << (q.isEmpty() ? "true" : "false") << endl;

    cout << "\nEnqueueing 5 sample asm lines..." << endl;
    q.enqueue("MOV R1, 5");
    q.enqueue("ADD R1, 6");
    q.enqueue("MOV R3, R1");
    q.enqueue("MUL R3, 4");
    q.enqueue("STORE 20, R3");

    cout << "Size after enqueueing 5 items: " << q.size() << endl;
    cout << "isFull: " << (q.isFull() ? "true" : "false") << endl;

    cout << "\nTrying to enqueue a 6th item (should print error):" << endl;
    q.enqueue("DISPLAY R3");

    cout << "\nPeek front (should be 'MOV R1, 5'): " << q.peek() << endl;

    cout << "\nDequeueing all items in order:" << endl;
    while (!q.isEmpty()) {
        cout << "Dequeued: " << q.dequeue() << endl;
    }

    cout << "\nTrying to dequeue from empty queue (should print error):" << endl;
    q.dequeue();

    cout << "\nTesting wraparound: enqueue/dequeue repeatedly..." << endl;
    q.enqueue("A");
    q.enqueue("B");
    cout << "Dequeued: " << q.dequeue() << endl;
    q.enqueue("C");
    q.enqueue("D");
    q.enqueue("E"); // this should wrap around the internal array
    cout << "Size: " << q.size() << endl;
    while (!q.isEmpty()) {
        cout << "Dequeued: " << q.dequeue() << endl;
    }

    cout << "\nAll tests completed." << endl;
    return 0;
}