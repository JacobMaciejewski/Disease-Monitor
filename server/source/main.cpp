#include "../headers/mainAux.h"
#define MAX_CLIENTS 20

// arguments given by the user
int queryPortNum;
int statisticsPortNum;
int numThreads;
int bufferSize;

int *clientMainSocket;
int *workersMainSocket;

int totalWorkers;
string **workersData;
//signalizes the end of worker's communication establishment
pthread_cond_t workerDataSet;
pthread_mutex_t worker_data_mutex;
pthread_mutex_t query_print_mutex;


pthread_mutex_t debug_mutex;
bool gotAllWorkers;


//contains each country and the worker it corresponds to
LinkedList<HeapEntry*> *countriesList;
//server thread function
void *answerRequests(void *);

int main(int argc, char *argv[])
{
    pthread_t *threadPool;
    getArguments(argc, argv);
    //initializing the FD buffer
    pthread_mutex_init(&worker_data_mutex, NULL);
    pthread_mutex_init(&query_print_mutex, NULL);
    pthread_mutex_init(&debug_mutex, NULL);
    pthread_cond_init(&workerDataSet, NULL);
    //initializing the FD buffer that will be used by threads
    CircularBuffer<BufferDuplet*> *FD_Buffer = new CircularBuffer<BufferDuplet*>(bufferSize);
    //initially we don't have all workers
    gotAllWorkers = false;
    //allocate memory and bind the two listening sockets
    initializeListeningSockets();

    threadPool = (pthread_t*) malloc(sizeof(pthread_t) * numThreads);
    for(int index = 0; index < numThreads; index++)
    {
        pthread_create(&(threadPool[index]), NULL, answerRequests, FD_Buffer);
    }

    //initialize communication with each worker using server threads
    initializeWorkerCommunication(FD_Buffer);
    //simply makes sure, we have received all workers
    getAllWorkers();
    //keep receiving requests from clients (queries) or communication establishment from workers
    receiveRequests(FD_Buffer);
    // close(*workersMainSocket);
    return 0;
}

//function called by server thread, tries to pop FD from buffer
//is succeeds, executes query
void *answerRequests(void *arg) 
{ 

    CircularBuffer<BufferDuplet*> *FD_Buffer_Ptr;
    BufferDuplet *currentDP;
    FD_Buffer_Ptr = (CircularBuffer<BufferDuplet*>*)(arg);

    int currentFD;

    //waiting for new requests indefinitely
    while(true)
    {
        currentDP = FD_Buffer_Ptr->pop();
        currentFD = currentDP->getFD();

        if(currentDP->isClientTask())
        {
            execClientQuery(currentFD);
            closeRequestSocket(currentFD);
            // check(close(currentFD), "Unable to close client request socket!");
        }else
        {
            execWorkerTask(currentFD);
            closeRequestSocket(currentFD);
            // check(close(currentFD), "Unable to close worker request socket!");
        }  
    }

    //freeing the memory of the given buffer duplet
    delete currentDP;

    return NULL;
} 