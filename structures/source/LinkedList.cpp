#include "../headers/LinkedList.h"
#include "../headers/LinkedListNode.h"
#include "../headers/Bucket.h"
#include "../headers/AVL_Tree.h"
#include "../headers/HeapEntry.h"
using namespace std;

template <typename DataType>
LinkedList<DataType>::LinkedList()
{
    this->listHead = NULL;
}

template <typename DataType>
LinkedList<DataType>::~LinkedList()
{
    //if list is empty, we have nothing to do here
    if(!this->isEmpty())
    {
        delete this->getListHead();
    }
}

template <typename DataType>
void LinkedList<DataType>::setListHead(LinkedListNode<DataType> *newHead)
{
    this->listHead = newHead;
    return;
}

template <typename DataType>
void LinkedList<DataType>::push(DataType item)
{
    LinkedListNode<DataType> *currentHead = this->getListHead();
    this->listHead = new LinkedListNode<DataType>(item, currentHead);
    return;
}

template<>
void LinkedList<Bucket*>::printBuckets()
{
    LinkedListNode<Bucket*> *headOfList;

    if(this->isEmpty())
    {
        cout << "List is empty!" << endl;
    }else
    {
        cout << endl << "List of buckets: " << endl;
        headOfList = this->getListHead();
        headOfList->printBuckets();
    }

    return;
}

template <>
void LinkedList<string*>::print()
{
    LinkedListNode<string*> *listHead;
    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->print();
    }
    
}

template <>
void LinkedList<Record*>::print()
{
    LinkedListNode<Record*> *listHead;
    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->print();
    }
    
}

template<>
void LinkedList<HeapEntry*>::print()
{
    LinkedListNode<HeapEntry*> *listHead;

    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->print();
    }
}

//returns a list containing all strings (disease or country)
//contained in record list
//TO DO: FUNCTION ALLOCATES MEMORY FOR SEARCHED STRINGS
template <>
void LinkedList<Record*>::getStringList(LinkedList<string*> *stringList, int disease)
{
    LinkedListNode<Record*> *listHead;

    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->getStringList(stringList, disease);
    }
    return;  
}

//fills the array of counters for each group
template<>
void LinkedList<Record*>::fillGroupStats(string *disease, int *groupCounters)
{
    LinkedListNode<Record*> *listHead;
    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->fillGroupStats(disease, groupCounters);
    }   
}

//checks how many patients departed in the given date range (last argument implies if we should check for specific country or not)
template<>
void LinkedList<Record*>::numPatientDischarges(int *cases, string *disease, string *country, Date *entryDate, Date *exitDate, bool allCountries)
{
    LinkedListNode<Record*> *listHead;
    //initializing cases
    (*cases) = 0;

    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->numPatientDischarges(cases, disease, country, entryDate, exitDate, allCountries);
    }  
}

//custom record list function to delete contained records
//used as main list destructor, only takes care of the nodes
template<>
void LinkedList<Record*>::freeRecords()
{
    LinkedListNode<Record*> *listHead;
    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->freeRecords();
    }
    return;
}

template<typename Datatype>
void LinkedList<Datatype>::freeContents()
{
    LinkedListNode<Datatype> *listHead;
    if(this->isEmpty())
    {
        return;
    }else
    {
        listHead = this->getListHead();
        listHead->freeContents();
    }
    return;
}

//returns the pointer to given string (stored in hashtable)
template <>
string* LinkedList<Bucket*>::getStringPointer(string *givenString)
{
    LinkedListNode<Bucket*> *currHead;
    if(this->isEmpty())
    {
        return NULL;
    }else
    {
        currHead = this->getListHead();
        return currHead->getStringPointer(givenString);
    }
}

template <>
Record* LinkedList<Record*>::getRecordWithID(string *givenID)
{
    LinkedListNode<Record*> *currHead;

    if(this->isEmpty())
    {
        return NULL;
    }else
    {
        currHead = this->getListHead();
        return currHead->getRecordWithID(givenID);
    }
}

template <typename DataType>
LinkedListNode<DataType>* LinkedList<DataType>::getListHead()
{
    return this->listHead;
}

template <typename DataType>
bool LinkedList<DataType>::isEmpty()
{
    return (this->listHead == NULL);
}

template <typename DataType>
bool LinkedList<DataType>::contains(DataType givenItem)
{
    LinkedListNode<DataType> *head = this->getListHead();
    if(this->isEmpty()){
        return false;
    }else{
       return head->contains(givenItem);
    }
}

template <>
bool LinkedList<Bucket*>::containsIdentifier(string *givenString)
{
    LinkedListNode<Bucket*> *currHead;
    if(this->isEmpty())
    {
        return false;
    }else
    {
        currHead = this->getListHead();
        return currHead->containsIdentifier(givenString);
    }
}

//returns the number of patients that are still sick
template <>
int LinkedList<Record*>::numCurrentPatients()
{
    LinkedListNode<Record*> *listHead;
    if(this->isEmpty())
    {
        return 0;
    }else
    {
        listHead = this->getListHead();
        return listHead->numCurrentPatients();
    }
    
}

//returns the length of the list
template <typename DataType>
int LinkedList<DataType>::getLength()
{
    LinkedListNode<DataType> *listHead;
    if(this->isEmpty())
    {
        return 0;
    }else
    {
        listHead = this->getListHead();
        return listHead->getLength();
    } 
}

template <>
int LinkedList<Record*>::getCasesForString(string *givenString, int identifier)
{
    LinkedListNode<Record*> *listHead;
    if(this->isEmpty())
    {
        return 0;
    }else
    {
        listHead = this->getListHead();
        return listHead->getCasesForString(givenString, identifier);
    }
    
}


template <typename DataType>
LinkedListNode<DataType>* LinkedList<DataType>::pop()
{
    LinkedListNode<DataType> *currHead;
    currHead = this->getListHead();

    if(this->isEmpty())
    {
        return NULL;
    }else if(currHead->isLastNode())
    {
        this->setListHead(NULL);
        return currHead;
    }else
    {
        //next node becomes the new head
        this->setListHead(currHead->getNextNode());
        //disconnecting current head from its child (avoiding segmentation)
        currHead->setNextNode(NULL);
        return currHead;
    }
}


//function finds the bucket with the pair containing record's string AVL Tree
//and inserts the record into it
template<>
void LinkedList<Bucket*>::pushToFile(Record *givenRecord, string *givenString, int sizeOfBucket)
{
    LinkedListNode<Bucket*>* listHeadNode;
    LinkedListNode<Bucket*>* listNextNode = NULL;
    Bucket *headBucket;
    AVL_Tree *stringTree;

    //list is empty
    //constructing new node with new bucket
    //subsequently initializing a pair instance for the given string/record
    if(this->isEmpty())
    {
        headBucket = new Bucket(sizeOfBucket);
        listHeadNode = new LinkedListNode<Bucket*>(headBucket, listNextNode);
        this->setListHead(listHeadNode);
        //initializing a pair for given string
        headBucket->insert(givenString);
        stringTree = headBucket->getStringTree(givenString);
        //inserting given record to its corresponding pair tree
        stringTree->insert(givenRecord);
    }else
    {
        listHeadNode = this->getListHead();
        listHeadNode->pushToFile(givenRecord, givenString, sizeOfBucket);
    }
    return;
}


template<>
void LinkedList<Bucket*>::insertIdentifier(string *givenString, int sizeOfBucket)
{
    LinkedListNode<Bucket*>* listHeadNode;
    LinkedListNode<Bucket*>* listNextNode = NULL;
    Bucket *headBucket;

    //list is empty
    //constructing new node with new bucket
    //pushing string to the new bucket
    if(this->isEmpty())
    {
        headBucket = new Bucket(sizeOfBucket);
        listHeadNode = new LinkedListNode<Bucket*>(headBucket, listNextNode);
        this->setListHead(listHeadNode);
        //initializing a pair for given string
        headBucket->insert(givenString);
    }else
    {
        listHeadNode = this->getListHead();
        listHeadNode->insertIdentifier(givenString, sizeOfBucket);
    }
    return;
}


//returns all the strings contained in current list buckets
template<>
void LinkedList<Bucket*>::getAllStrings(LinkedList<string*> *stringList)
{
    LinkedListNode<Bucket*> *currHead;
    currHead = this->getListHead();

    if(this->isEmpty())
    {
        return;
    }else
    {
        currHead->getAllStrings(stringList);
    }
    return;
}

//returns the AVL Tree that corresponds to the given string
template<>
AVL_Tree* LinkedList<Bucket*>::getStringFile(string *givenString)
{
    AVL_Tree *stringTree;
    LinkedListNode<Bucket*> *myListHead;
    Bucket *headBucket;

    //current list is empty, no bucket containing a pair for given string
    //no AVL Tree corresponding to the given string
    if(this->isEmpty())
    {
        return NULL;
    }else//traverse the buckets of the list
    {
        myListHead = this->getListHead();
        stringTree = myListHead->getStringFile(givenString);
    }


    return stringTree;
}

//checks if we have a worker that takes care
//of the given country string
//if so, returns the FD corresponding to worker socket
template<>
bool LinkedList<HeapEntry*>::countryWorkerExists(string *givenString, int *workerFD)
{
    LinkedListNode<HeapEntry*> *listHead;

    if(this->isEmpty())
    {
        return false;
    }else
    {
        listHead = this->getListHead();
        return listHead->countryWorkerExists(givenString, workerFD);
    }
}

template class LinkedList<int*>;
template class LinkedList<string*>;
template class LinkedList<Date*>;
template class LinkedList<Record*>;
template class LinkedList<Bucket*>;
template class LinkedList<HeapEntry*>;
