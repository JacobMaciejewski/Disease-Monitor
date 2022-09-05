#include "../headers/workerMainAux.h"
using namespace std;

//SOME GLOBAL VARIABLES TO AVOID LONG ARGUMENT LINES
int countriesNum;
int bufferSize;
//array containg the countries of current process
string **countriesArray;
//references to pipes FDs
string *writePipe;
string *readPipe;
//main data structures of our database
HashTable *id_HT;
HashTable *disID_HT;
HashTable *cntID_HT;
HashTable *dis_HT;
HashTable *cnt_HT;
//each country directory has a hashtable with file names
HashTable **fldr_HT;
//list containing all records of our database
LinkedList<Record*> *recordList;
//array with two FDs, for reading and writing to father process
//WR[0] = write_FD, WR[1] = read_FD
int *WR_FDs;
int succQueries;
//number of queries
int total;
int success;
int fail;
//for checking within handlers if signals are blocked
bool signalsBlocked;
sigset_t handlerMask;
sigset_t blocked_signals;
sigset_t changeBlockSignal;
string dataDirectory;
//used to handle SIGQUIT/SIGINT and SIGUSR1 respectively
struct sigaction exitAction;
struct sigaction newFileAction;
//used to handle SIGUSR2, checking if it should block/unblock signals
struct sigaction allowActions;
//third project global variables (for communication with server)
string *serverIP;
int serverPort;
int serverCommunicationSocket;
int serverListeningSocket;
int serverListeningPort;


int main(int argc, char *argv[])
{
    sigset_t whatIsBlocked;
    string workersDataStr;
    //signals that are blocked at a specific part of the process code
    sigemptyset(&blocked_signals);
    //made custom functions for given signals, they may be blocked in some parts of code
    // sigaddset(&blocked_signals, SIGINT);
    // sigaddset(&blocked_signals, SIGQUIT);
    sigaddset(&blocked_signals, SIGUSR1);
    sigaddset(&blocked_signals, SIGTSTP);
    sigemptyset(&changeBlockSignal);
    sigaddset(&changeBlockSignal, SIGUSR2);
    //constructing proper handlers for SIGUSR1, SIGINT, SIGQUIT
    //initializing sigaction with new handlers
    initializeSigaction();
    //initial message received from parent process
    string countriesMessage;
    //initialize query counters
    initCounters();
    //INITIALLY BLOCKING SIGINT, SIGQUIT, SIGUSR1
    //PARENT WILL USE SIGUSR2 TO INFORM CHILD THAT IT CAN TAKE THEM
    sigprocmask(SIG_SETMASK, &changeBlockSignal, NULL);
    sigprocmask(SIG_BLOCK, &blocked_signals, NULL);
    signalsBlocked = true;

    if(argc != 7)
    {
        cout << "Father gave wrong number of arguments!" << endl;
        exit(-1);
    }

    getArguments(argv);
    //opens given pipes for communication with parent
    //returns array with proper file descriptors
    establishCommunication();
    //receive countries from parent process
    receiveWorkersData(&workersDataStr);
    receiveCountries(&countriesMessage);

    //received no countries, just exit
    if(gotEmptyText(countriesMessage))
    {
        cout << "Process[" << getpid() << "] got no country, will be closed!" << endl;
        //closing read/write fifos for this child
        close(WR_FDs[0]);
        close(WR_FDs[1]);
        free(WR_FDs);
        //freeing the pipe names 
        delete readPipe;
        delete writePipe;
        delete serverIP;
        cout << "Process[" << getpid() << "] will leave!" << endl;  
        exit(0);
    }
    initializeServerCommunication();
    //worker will behave as a server from now on
    initializeListening();
    sendMetaData(workersDataStr, countriesMessage);
    //initializing hash tables and record list
    initializeDataStructures(20, 5);
    //initializing array containing countries files names
    getRowsInArray(countriesMessage); 
    //allocating data for files hash tables
    initFolderHash();
    //reading given files, putting valid entries into the database, sending statistics 
    establishDataBase(dataDirectory);
    cout << "Worker [" << getpid() << "] established database!" << endl;
    //from now on worker will behave like
    receiveRequests();
    // freeDatabase();
    cout << "Process[" << getpid() << "] exited successfully!" << endl;
    return 0;
}