#ifndef LINKED_LIST_NODE
#define LINKED_LIST_NODE
#include <iostream>
#include <cstdlib>
#include <string>
#include "Date.h"
#include "Record.h"

template<typename Type>
class LinkedList;

class AVL_Tree;

using namespace std;

template <typename DataType>
class LinkedListNode
{
private:
    DataType item;
    LinkedListNode<DataType> *nextNode;
public:
    LinkedListNode(DataType, LinkedListNode*);
    ~LinkedListNode();
    void setNextNode(LinkedListNode*);
    void printBuckets();
    void pushToFile(Record*, string*, int);
    void insertIdentifier(string*, int);
    void getAllStrings(LinkedList<string*>*);
    void getStringList(LinkedList<string*>*, int);
    void fillGroupStats(string*, int*);
    void numPatientDischarges(int*, string*, string*, Date*, Date*, bool);
    void print();
    void freeRecords();
    void freeContents();
    string* getStringPointer(string*);
    Record* getRecordWithID(string*);
    AVL_Tree* getStringFile(string*);
    DataType getItem();
    bool isLastNode();
    bool contains(DataType);
    bool containsIdentifier(string*);
    bool exitInRange(Date*, Date*);
    bool hasDisease(string*);
    bool inCountry(string*);
    int numCurrentPatients();
    int getLength();
    int getCasesForString(string*, int);
    LinkedListNode* getNextNode();
    bool countryWorkerExists(string*, int*);
};


#endif