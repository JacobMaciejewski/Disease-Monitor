#ifndef MAIN_AUX
#define MAIN_AUX
#include <string>
#include <iostream>
#include <fstream>
#include "Record.h"
#include "HashTable.h"
#include "LinkedList.h"
#include "LinkedListNode.h"
#include "Date.h"
#include "Record.h"
#include "AVL_Tree.h"
#include "AVL_TreeNode.h"
#include "HashTable.h"
#include "MaxHeap.h"
using namespace std;

void getArgument(std::string, int, char**, std::string**);
void recordErrorPrint(std::string, Record*);
void insertToDatabase(HashTable*, HashTable*, HashTable*, HashTable*, HashTable*, LinkedList<Record*>*, string*);
void showAvailableQueries();
void getQuery(string*, string*);
void initializeHeap(HashTable*, MaxHeap*, LinkedList<string*>*, Date*, Date*, string*, int);
void getUserQueries(HashTable*, HashTable*, HashTable*, HashTable*, HashTable*, LinkedList<Record*>*);
void globalDiseaseStats(HashTable*, HashTable*, string*, int);
void diseaseFrequency(HashTable*, HashTable*, HashTable*, HashTable*, string*, int);
void topKQuery(HashTable*, HashTable*, HashTable*, string*, int, int);
void topKDiseases(HashTable*, HashTable*, HashTable*, string*, int);
void topKCountries(HashTable*, HashTable*, HashTable*, string*, int);
void topKPrint(MaxHeap*, int);
void insertPatientRecord(HashTable*, HashTable*, HashTable*, HashTable*, HashTable*, LinkedList<Record*>*, string*, int);
void recordPatientExit(HashTable*, LinkedList<Record*>*, string*);
void numCurrentPatients(HashTable*, HashTable*, string*, int);
void freeDataStructures(HashTable**, HashTable**, HashTable**, HashTable**, HashTable**, LinkedList<Record*>**);
int getNumOfArguments(string*);
bool invalidDates(string, string);
string getIthWord(string*, int);
#endif
