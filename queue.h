#ifndef QUEUE_H
#define QUEUE_H

#include "pcb.h"

struct Node {
    PCB data;
    Node* next;

    Node(PCB p) : data(p), next(nullptr) {}
};

class Queue {
private:
    Node* front;
    Node* rear;
    int size;

public:
    Queue() : front(nullptr), rear(nullptr), size(0) {}

    ~Queue() {
        while (front != nullptr) {
            Node* temp = front;
            front = front->next;
            delete temp;
        }
    }

    void enqueue(PCB p) {
        Node* newNode = new Node(p);
        if (rear == nullptr) {
            front = rear = newNode;
        } else {
            rear->next = newNode;
            rear = newNode;
        }
        size++;
    }

    PCB dequeue() {
        if (front == nullptr) {
            return PCB();
        }
        Node* temp = front;
        PCB data = temp->data;
        front = front->next;
        if (front == nullptr) {
            rear = nullptr;
        }
        delete temp;
        size--;
        return data;
    }

    PCB peek() const {
        if (front == nullptr) {
            return PCB();
        }
        return front->data;
    }

    bool isEmpty() const {
        return front == nullptr;
    }

    int getSize() const {
        return size;
    }

    void printAll() const {
        Node* current = front;
        while (current != nullptr) {
            current->data.print();
            current = current->next;
        }
    }

    // Get all PCBs in order
    PCB* getAllInOrder(int& count) const {
        count = size;
        if (size == 0) return nullptr;

        PCB* arr = new PCB[size];
        Node* current = front;
        int i = 0;
        while (current != nullptr) {
            arr[i++] = current->data;
            current = current->next;
        }
        return arr;
    }
};

#endif // QUEUE_H