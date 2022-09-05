#ifndef LINKED_LIST
#define LINKED_LIST
#include <iostream>
#include <cstdlib>
#include <string>
#include "Date.h"
#include "Record.h"
using namespace std;

template<typename DataType>
class LinkedListNode;

class AVL_Tree;

template <typename DataType>
class LinkedList
{
private:
    LinkedListNode<DataType> *listHead;
public:
    LinkedList();
    ~LinkedList();
    void setListHead(LinkedListNode<DataType>*);
    void push(DataType);
    void printBuckets();
    void pushToFile(Record*, string*, int);
    void insertIdentifier(string*, int);
    void getAllStrings(LinkedList<string*>*);
    void print();
    void getStringList(LinkedList<string*>*, int);
    void fillGroupStats(string*, int*);
    void numPatientDischarges(int*, string*, string*, Date*, Date*, bool);
    void freeRecords();
    void freeContents();
    string* getStringPointer(string*);
    Record* getRecordWithID(string*);
    AVL_Tree* getStringFile(string*);
    LinkedListNode<DataType>* getListHead();
    LinkedListNode<DataType>* pop();
    int numCurrentPatients();
    int getLength();
    int getCasesForString(string*, int);
    bool isEmpty();
    bool contains(DataType);
    bool containsIdentifier(string*);
    bool countryWorkerExists(string*, int*);
};

#endif