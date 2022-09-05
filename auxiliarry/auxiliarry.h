#ifndef AUXILIARRY
#define AUXILIARRY

#define SOCKETERROR (-1)
#define SOCKET_BUFFER_SIZE 4096

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
#include <algorithm>
#include <signal.h>

// libraries for client-server communication
#include <errno.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

using namespace std;
//headers of structures library developed in the first project
#include "../buffer/circularBuffer.h"
#include "../structures/headers/LinkedList.h"
#include "../structures/headers/LinkedListNode.h"
#include "../structures/headers/Date.h"
#include "../structures/headers/Record.h"
#include "../structures/headers/AVL_Tree.h"
#include "../structures/headers/AVL_TreeNode.h"
#include "../structures/headers/BucketPair.h"
#include "../structures/headers/Bucket.h"
#include "../structures/headers/HashTable.h"
#include "../structures/headers/HeapEntry.h"
#include "../structures/headers/HeapNode.h"
#include "../structures/headers/MaxHeap.h"
#include "../structures/headers/LinkedList.h"


// second project auxiliarty functions
// mainly used for communication protocol functionality

int removeNonBlocking(int);
int addNonBlocking(int);
int numOfRows(string);
int getNumberOfEntitiesIn(string);
void changeBlockState(int);
void changeBlockStates(int, int*);
void printStreamFlags(int, int*);
void printFDFlags(int);
void sendMessage(int, int, string);
void receiveMessage(int, int, string*);
void sendText(int, int, string);
void receiveText(int, int, string*);
void sendEmptyText(int, int);
void gotAllPackets(bool*, int, char*);
void getPacketSize(int, char*, int*);
void getGroupArray(int, int **);
void addNewLine(string*);
void removeNewLine(string*);
void getQuery(string*, string*);
string getDate(string);
string getIthWord(string*, int);
int popMessage(string*, string*);
int popMessageAsString(string*, string*);
int getNumOfArguments(string*);
bool singletonEnd(int, char*);
bool lastMessage(string);
bool lastByte(string, string);
bool gotEmptyText(string);

// third project auxiliarty functions
int establishCommunicationSocket(string*, int, bool);
int establishListeningSocket(int);
int getRandomListeningSocket(int, int*);
int check(int, const char*);
void getPeerIP(int, string*);
void closeRequestSocket(int);

#endif