#ifndef MAIN_AUX
#define MAIN_AUX

// including the auxiliarry functions from the previous project
#include "../../auxiliarry/auxiliarry.h"

using namespace std;

// arguments given by the user are globally defined

extern int numThreads;
extern int servPort;
extern string *queryFile;
extern string *servIP;

extern pthread_mutex_t print_mutex;
extern pthread_mutex_t stream_mutex;

extern int currentQuery;


// auxiliarry functions for client main

void initializeServerAddr(sockaddr_in**);
void getArguments(int, char **);
void getArgument(string, int, char**, string**);
void connectToListeningSocket(int*, sockaddr_in*);
void getProperQuery(string, string*);
// void sendQueries(pthread_t*);
void *makeRequest(void *);


#endif