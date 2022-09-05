#ifndef MAIN_AUX
#define MAIN_AUX

#include <iostream>
#include <stdio.h> 
#include <cstdlib>
#include <string>
#include <string.h>
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
#include <wait.h>
#include <dirent.h>
#include <fcntl.h>
#include "../../auxiliarry/auxiliarry.h"
using namespace std;

//communication related global variables
extern int numWorkers;
extern int bufferSize;
extern int foldersNum;
extern int activeWorkers;
extern int *readFD;
extern int *writeFD;
extern int *childrenIDs;
extern string **foldersArray;
extern string **pipesArray;
extern string *input_dir;

//signal related global variables
extern sigset_t handlerMask;
extern sigset_t blocked_signals;
extern struct sigaction progExitAction;
extern struct sigaction newStatsAction;
extern struct sigaction deadChildAction;

//counters for queries
extern int total;
extern int success;
extern int fail;

//third project global variables
extern int serverPort;
extern string *serverIP;

void getArgument(string, int, char**, string**);
void getArguments(int, char**);
void getFolderArray();
void getPipesArray();
void getPipeNames(int, string*, string*);
void closeInactivePipes();
void closeActivePipes();
void initializeCommunication(string, string*, string*);
void initializeFD();
void setFileDescriptor(int, int, int*);
void initializeSending(int);
void sendCountries();
void receiveStatistics();
void printStatistics(string);
void printDiseaseStats(string*);
void printGroupStats(int, int);
void listCountries();
void waitChildren();
void waitInactiveChildren();
void getCountryFDs(int, int*, int*);
void sendToEveryone(string);
void receiveFromEveryone(string*);
void addCases(string*, int*);
void searchPatientRecord(string , string*);
void showAvailableQueries();
void getUserQueries();
void changeSignalAllowance();
void changeChildAllowance(int);
void initializeSigaction();
void progExitHandler(int);
void newStatsHandler(int, siginfo_t*, void*);
void deadChildHandler(int, siginfo_t*, void*);
void freeMemory();
void getID_FD(int, int*, int*);
void getIDIndex(int, int*);
void getChildCountries(int, string*);
void initializeQueryCounters();
void incrementSuccess();
void incrementFail();
string getIndexPipe(int, string**);
bool parentProcess(int);
bool childProcess(int);
bool unidentifiedProcess(int);
bool folderExists(string, int*);

//third project functions
void sendWorkersData();


#endif