#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER
#include <pthread.h>
#include <stdlib.h>
#include <cstdlib>
#include <stdio.h> 
using namespace std;

class BufferDuplet
{
private:
    bool clientTask;
    int fileDesc;
public:
    BufferDuplet(int, bool);
    ~BufferDuplet();
    void setTask(bool);
    void setFileDesc(int);
    bool getTask();
    int getFD();
    bool isWorkerTask();
    bool isClientTask();
};

template <typename DataType>
class CircularBuffer
{
private:
    int counter;
    int startPointer;
    int endPointer;
    int size;
    DataType *bufferEntries;
    pthread_mutex_t buffer_mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
public:
    CircularBuffer(int);
    ~CircularBuffer();
    bool isEmpty();
    bool isFull();
    DataType pop();
    void push(DataType);
    void insert(DataType);
    DataType remove();
    void incrementCounter();
    void decrementCounter();
    void incrementStartPointer();
    void incrementEndPointer();
    int getStartPointer();
    int getEndPointer();
};

#endif