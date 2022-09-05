#ifndef WORKER_MAIN_AUX
#define WORKER_MAIN_AUX

#include <iostream>
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

//useful worker data, used to properly exit worker
extern int countriesNum;
extern int bufferSize;
extern int *WR_FDs;
extern string **countriesArray;
extern string *writePipe;
extern string *readPipe;
//main data structures used in database
extern HashTable *id_HT;
extern HashTable *disID_HT;
extern HashTable *cntID_HT;
extern HashTable *dis_HT;
extern HashTable *cnt_HT;
extern HashTable **fldr_HT;
extern LinkedList<Record*> *recordList;


//variables for signal handling
extern int total;
extern int success;
extern int fail;
extern bool signalsBlocked;
extern string dataDirectory;
extern sigset_t handlerMask;
extern sigset_t blocked_signals;
extern sigset_t changeBlockSignal;
extern struct sigaction exitAction;
extern struct sigaction newFileAction;
extern struct sigaction allowActions;

//third project global variables (for communication with server)
extern string *serverIP;
extern int serverPort;
extern int serverCommunicationSocket;
extern int serverListeningSocket;
extern int serverListeningPort;


//worker main functions
void getArguments(char**);
void establishCommunication();
void receiveCountries(string*);
void setSendingInfo(int*);
void incrementSuccess();
void incrementFail();
void initCounters();
void initializeSigaction();
void fileHandler(int);
void exitHandler(int);
void allowHandler(int);

//database related functions
void initializeDataStructures(int , int);
void establishDataBase(string);
void dirToDatabase(int, string, string);
void fileToDatabase(int, string, string);
void insertToDatabase(string, string, string*);
void initFolderHash();
void getRowsInArray(string);
void getRecordParts(string*, string*, string*, string*, string*, string*, string*);
void diseaseFrequency(string*, int, bool*, string*);
void searchPatientRecord(string*, bool*, string*);
void topkAgeRanges(string*, bool*, string*);
void updateTopKMessage(int, int, int*, string*);
void topkAgeRangesMessage(int, int, int*, string*);
void numPatientDischarges(int, string*, bool*, string*);
void numPatientAdmissions(int, string*, bool*, string*);
void sendStatistics(string, string);
void receiveRequests();
void freeDatabase();
void makeLogFile();
void waitSigBlockChange();
string dateToString(Date);
string makeDiseaseStats(string*);
string getAgeGroup(int);
string getRecord(Record*);
bool validAge(string);
bool validState(string);
bool checkedFile(int, string);

//server communication functions
void initializeServerCommunication();
void receiveWorkersData(string*);
void sendMetaData(string, string);
void initializeListening();

#endif