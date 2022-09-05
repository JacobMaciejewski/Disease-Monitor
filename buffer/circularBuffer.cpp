#include "circularBuffer.h"

template <>
CircularBuffer<BufferDuplet*>::CircularBuffer(int givenSize)
{
    this->counter = 0;
    this->startPointer = 0;
    this->endPointer = 0;
    this->size = givenSize;

    this->bufferEntries = (BufferDuplet**) malloc(givenSize * sizeof(BufferDuplet*));
    //initializing the array containing items of the buffer
    for(int index = 0; index < this->size; index++)
    {
        this->bufferEntries[index] = NULL;
    }

    //initializing the mutex and condition variables
    pthread_mutex_init(&(this->buffer_mutex), NULL);
    pthread_cond_init(&(this->full), NULL);
    pthread_cond_init(&(this->empty), NULL);
}

template <typename DataType>
CircularBuffer<DataType>::~CircularBuffer()
{
    //simply free the duplet containted within the buffer
    for(int index = 0; index < this->size; index++)
    {
        if(this->bufferEntries[index] != NULL)
        {
            delete this->bufferEntries[index];
        }
    }
    free(this->bufferEntries);
}

template <typename DataType>
bool CircularBuffer<DataType>::isEmpty()
{
    return (this->counter == 0);
}

template <typename DataType>
bool CircularBuffer<DataType>::isFull()
{
    return (this->counter == this->size);
}

//returns the next item and does all the necessary incrementations
template <typename DataType>
DataType CircularBuffer<DataType>::remove()
{
    DataType itemToReturn;
    itemToReturn = this->bufferEntries[this->startPointer];
    //setting current position to null
    this->bufferEntries[this->startPointer] = NULL;
    this->incrementStartPointer();
    this->decrementCounter();
    //returning previous content
    return itemToReturn;
}

//adds next item to buffer and does all the necessary incrementations
template <typename DataType>
void CircularBuffer<DataType>::insert(DataType newEntry)
{
    this->bufferEntries[this->endPointer] = newEntry;
    this->incrementEndPointer();
    this->incrementCounter();
    return;
}

//add a new entry to the buffer
template <typename DataType>
void CircularBuffer<DataType>::push(DataType newEntry)
{
    pthread_mutex_lock(&this->buffer_mutex);
    while(this->isFull())
    {
        pthread_cond_wait(&this->full, &this->buffer_mutex);
    }
    //simply add the new entry into the buffer
    this->insert(newEntry);
    //info that item can be popped, as there is one item available in buffer
    if(this->counter == 1)
    {
        pthread_cond_broadcast(&this->empty);
    }
    pthread_mutex_unlock(&this->buffer_mutex);
    return;
}

//remove an entry from the buffer
template <typename DataType>
DataType CircularBuffer<DataType>::pop()
{   
    DataType itemToReturn;

    pthread_mutex_lock(&this->buffer_mutex);
    while(this->isEmpty())
    {
        pthread_cond_wait(&this->empty, &this->buffer_mutex);
    }
    //simply remove an item from the buffer
    itemToReturn = this->remove();
    //info that item can be pushed, only when one position is freed
    if(this->counter == this->size - 1)
    {
        pthread_cond_signal(&this->full);
    }
    pthread_mutex_unlock(&this->buffer_mutex);
    //return the item to the program
    return itemToReturn;
}


template <typename DataType>
void CircularBuffer<DataType>::incrementStartPointer()
{
    this->startPointer = (this->startPointer + 1) % this->size;
    return;
}

template <typename DataType>
void CircularBuffer<DataType>::incrementEndPointer()
{
    this->endPointer = (this->endPointer + 1) % this->size;
    return;
}

template <typename DataType>
int CircularBuffer<DataType>::getStartPointer()
{
    return this->startPointer;
}

template <typename DataType>
int CircularBuffer<DataType>::getEndPointer()
{
    return this->endPointer;
}

template <typename DataType>
void CircularBuffer<DataType>::incrementCounter()
{
    this->counter++;
}

template <typename DataType>
void CircularBuffer<DataType>::decrementCounter()
{
    this->counter--;
}


BufferDuplet::BufferDuplet(int FD, bool isClient)
{
    this->setFileDesc(FD);
    this->setTask(isClient);
}

BufferDuplet::~BufferDuplet()
{

}

void BufferDuplet::setTask(bool isClient)
{
    this->clientTask = isClient;
}


void BufferDuplet::setFileDesc(int FD)
{
    this->fileDesc = FD;
}

bool BufferDuplet::getTask()
{
    return this->clientTask;
}

int BufferDuplet::getFD()
{
    return this->fileDesc;
}

bool BufferDuplet::isWorkerTask()
{
    return (this->getTask() == false);
}
 
bool BufferDuplet::isClientTask()
{
    return this->getTask();
}

template class CircularBuffer<BufferDuplet*>;

