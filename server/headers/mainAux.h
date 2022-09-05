#ifndef MAIN_AUX
#define MAIN_AUX

// including the auxiliarry functions from the previous project
#include "../../auxiliarry/auxiliarry.h"

using namespace std;

// arguments given by the user are globally defined
extern int queryPortNum;
extern int statisticsPortNum;
extern int numThreads;
extern int bufferSize;
extern int *clientMainSocket;
extern int *workersMainSocket;
extern int totalWorkers;
extern string **workersData;
extern LinkedList<HeapEntry*> *countriesList;

//mutexes and main condition variables
extern pthread_cond_t workerDataSet;
extern pthread_mutex_t worker_data_mutex;
extern pthread_mutex_t query_print_mutex;


extern pthread_mutex_t debug_mutex;
extern bool gotAllWorkers;

// auxiliarry functions for server main
void getArguments(int, char **);
void getArgument(string, int, char**, string**);
void initializeWorkerCommunication(CircularBuffer<BufferDuplet*>*);
void initWorker(int);
void initializeWorkerArray();
void updateWorkerData(int, string, string);
void updateCountriesList(int, string);
int getTotalWorkers(int, string*);
bool firstWorker();
bool allWorkersConnected(int);

// old communication functions updated for server
void getAvailableFD(int, int*, int*);
void getReadMaxFD(int*, int*);
void receiveStatistics(int);

void printGroupStats(int, int);
void printDiseaseStats(string*);
void printStatistics(string);

void getCountryFDs(int, int*, int*);
void searchPatientRecord(string, string*);
void addCases(string*, int*);
// void sendToEveryone(string, int **);
// void receiveFromEveryone(string*, int*);
void initializeListeningSockets();
void getMetaData(int, int*, int*, string*, string*);
void execWorkerTask(int);
bool execClientQuery(int);
void receiveRequests(CircularBuffer<BufferDuplet*>*);
void initLocalSockets(int**);
void terminateLocalSockets(int*);
void getAllWorkers();


#endif