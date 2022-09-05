#include "../headers/mainAux.h"

//global variables for easier function traversals
int numWorkers;
int bufferSize;
int foldersNum;
int activeWorkers;
int *readFD;
int *writeFD;
int *childrenIDs;
string **foldersArray;
string **pipesArray;
string *input_dir;

//third project global variables
int serverPort;
string *serverIP;


//global variables for signal handling
sigset_t handlerMask;
sigset_t blocked_signals;
struct sigaction progExitAction;
struct sigaction newStatsAction;
struct sigaction deadChildAction;

//counters for queries
int total;
int success;
int fail;

//TO DO: FREE ALL POINTERS
int main(int argc, char *argv[])
{
    int folderIndex, writePipeIndex;
    string readPipe, writePipe, bufferSizeStr;
    //process id of current process
    int pid, status, tmp_FD;
    //arrays containing file descriptors for parent
    char buf[bufferSize];
    //directory containing countries' subdirectories
    initializeQueryCounters();
    //makes a set of blocking signals
    //signals that are blocked at a specific part of the process code
    sigemptyset(&blocked_signals);
    initializeSigaction();
    sigprocmask(SIG_BLOCK, &blocked_signals, NULL);
    //made custom functions for given signals, they may be blocked in some parts of code
    if(argc != 11)
    {
        cout << "Wrong number of arguments!" << endl;
        return -1;
    }

    //gets user input
    getArguments(argc, argv);
    //getting an array with the names of the country folders
    getFolderArray();
    //getting an array containing the names of the constructed pipes
    getPipesArray();
    //getting the buffer size as string
    bufferSizeStr = to_string(bufferSize);
    //initialize file descriptor arrays
    initializeFD();
    //initializes children processes and pipes
    initializeCommunication(bufferSizeStr, &writePipe, &readPipe);
    //send the total number of workers to each of them so server can be informed
    sendWorkersData();
    sendCountries();
    //sends countries to children
    //disable inactive pipes
    closeInactivePipes();
    waitInactiveChildren();
    //each active child returns statistics for the files of its countries folders
    // changeBlockStates(activeWorkers, readFD);
    //receive statistics for each file of workers, print them out
    // receiveStatistics();
    // sigaction(SIGCHLD, &deadChildAction, NULL);
    // sigaddset(&blocked_signals, SIGCHLD);
    // getUserQueries();
    //wait for active children
    waitChildren();
    //closing the pipes corresponding to still active children
    closeActivePipes();
    freeMemory();
    cout << "Master exited successfully" << endl;
    return 0;
}