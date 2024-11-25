#include <iostream>
#include <pthread.h>
#include <vector>
#include <chrono>
#include <cstdlib>

using namespace std;

struct ReadWriteLock {
    pthread_mutex_t mutex;
    pthread_cond_t readersOk, writersOk;
    int activeReaders, activeWriters, waitingWriters;

    ReadWriteLock() {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&readersOk, nullptr);
        pthread_cond_init(&writersOk, nullptr);
        activeReaders = activeWriters = waitingWriters = 0;
    }

    
    void readLock() {
        pthread_mutex_lock(&mutex);
        while (activeWriters > 0 || waitingWriters > 0) {
            pthread_cond_wait(&readersOk, &mutex);
        }
        activeReaders++;
        pthread_mutex_unlock(&mutex);
    }

    
    void readUnlock() {
        pthread_mutex_lock(&mutex);
        activeReaders--;
        if (activeReaders == 0) pthread_cond_signal(&writersOk);
        pthread_mutex_unlock(&mutex);
    }

    
    void writeLock() {
        pthread_mutex_lock(&mutex);
        waitingWriters++;
        while (activeReaders > 0 || activeWriters > 0) {
            pthread_cond_wait(&writersOk, &mutex);
        }
        waitingWriters--;
        activeWriters++;
        pthread_mutex_unlock(&mutex);
    }

    
    void writeUnlock() {
        pthread_mutex_lock(&mutex);
        activeWriters--;
        if (waitingWriters > 0) {
            pthread_cond_signal(&writersOk);
        } else {
            pthread_cond_broadcast(&readersOk);
        }
        pthread_mutex_unlock(&mutex);
    }

    ~ReadWriteLock() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&readersOk);
        pthread_cond_destroy(&writersOk);
    }
};


struct Node {
    int data;
    Node* next;
    Node(int value) : data(value), next(nullptr) {}
};


class LinkedList {
    Node* head;
    ReadWriteLock rwLock;

public:
    LinkedList() : head(nullptr) {}


    void insert(int value) {
        rwLock.writeLock();
        Node* newNode = new Node(value);
        newNode->next = head;
        head = newNode;
        rwLock.writeUnlock();
    }

    
    bool find(int value) {
        rwLock.readLock();
        Node* current = head;
        while (current) {
            if (current->data == value) {
                rwLock.readUnlock();
                return true;
            }
            current = current->next;
        }
        rwLock.readUnlock();
        return false;
    }

    
    void printList() {
        rwLock.readLock();
        Node* current = head;
        while (current) {
            cout << current->data << " -> ";
            current = current->next;
        }
        cout << "nullptr" << endl;
        rwLock.readUnlock();
    }
};


void* reader(void* arg) {
    LinkedList* list = static_cast<LinkedList*>(arg);
    for (int i = 0; i < 5; ++i) {
        int value = rand() % 10;
        if (list->find(value)) {
            cout << "Reader: Found " << value << endl;
        } else {
            cout << "Reader: " << value << " not found" << endl;
        }
    }
    return nullptr;
}


void* writer(void* arg) {
    LinkedList* list = static_cast<LinkedList*>(arg);
    for (int i = 0; i < 5; ++i) {
        int value = rand() % 10;
        list->insert(value);
        cout << "Writer: Inserted " << value << endl;
    }
    return nullptr;
}

int main() {
    LinkedList list;
    int numReaders, numWriters;

    cout << "Ingrese el número de lectores: ";
    cin >> numReaders;
    cout << "Ingrese el número de escritores: ";
    cin >> numWriters;

    vector<pthread_t> readers(numReaders);
    vector<pthread_t> writers(numWriters);

    
    for (int i = 0; i < numReaders; ++i) {
        pthread_create(&readers[i], nullptr, reader, &list);
    }

    
    for (int i = 0; i < numWriters; ++i) {
        pthread_create(&writers[i], nullptr, writer, &list);
    }

    
    for (int i = 0; i < numReaders; ++i) {
        pthread_join(readers[i], nullptr);
    }

    
    for (int i = 0; i < numWriters; ++i) {
        pthread_join(writers[i], nullptr);
    }

    cout << "Contenido final de la lista enlazada:" << endl;
    list.printList();

    return 0;
}
