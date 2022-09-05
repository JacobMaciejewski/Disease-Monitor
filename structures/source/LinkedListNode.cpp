#include "../headers/LinkedListNode.h"
#include "../headers/LinkedList.h"
#include "../headers/AVL_Tree.h"
#include "../headers/Bucket.h"
#include "../headers/HeapEntry.h"
using namespace std;


template <typename DataType>
LinkedListNode<DataType>::LinkedListNode(DataType item, LinkedListNode *nextNode){
    this->item = item;
    this->setNextNode(nextNode);
}

template <typename DataType>
LinkedListNode<DataType>::~LinkedListNode()
{
    //still have nodes on the right to delete
    if(!this->isLastNode())
    {
        delete this->getNextNode();
    }
}

template <typename DataType>
void LinkedListNode<DataType>::setNextNode(LinkedListNode* nextNode){
    this->nextNode = nextNode;
}

//simply calls bucket print for current list node
template <>
void LinkedListNode<Bucket*>::printBuckets()
{
    Bucket *nodeBucket;
    LinkedListNode<Bucket*>* nextNode;
    nodeBucket = this->getItem();
    //printing the bucket of the current node
    nodeBucket->print();

    //got to the end of the list
    if(!this->isLastNode())
    {
        nextNode = this->getNextNode();
        nextNode->printBuckets();
    }
    return;
}

template <>
void LinkedListNode<Bucket*>::getAllStrings(LinkedList<string*> *stringList)
{
    Bucket *currBucket;
    LinkedListNode<Bucket*> *nextNode;
    //get the strings from the bucket contained within current node
    currBucket = this->getItem();
    currBucket->getAllStrings(stringList);

    //repeat for the next bucket
    if(!this->isLastNode())
    {
        nextNode = this->getNextNode();
        nextNode->getAllStrings(stringList);
    }
    return;
}

template <>
void LinkedListNode<string*>::print()
{
    LinkedListNode<string*> *nextNode;
    string* currString;

    currString = this->getItem();

    cout << endl << *currString << endl;

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->print();
    }
    
}

template <>
void LinkedListNode<Record*>::print()
{
    LinkedListNode<Record*> *nextNode;
    Record* currentRecord;

    currentRecord = this->getItem();

    cout << endl << *currentRecord << endl;

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->print();
    }
    
}

template<>
void LinkedListNode<HeapEntry*>::print()
{
    HeapEntry *currEntry = this->getItem();
    LinkedListNode<HeapEntry*> *nextNode;

    cout << *currEntry << endl;

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->print();
    }
}



template <>
void LinkedListNode<Record*>::getStringList(LinkedList<string*> *stringList, int disease)
{
    Record *currRecord;
    string *currString;
    //found a new string, copy it and add it to the list
    string *newString;
    string currStringContent;

    currRecord = this->getItem();

    //checking which string we will choose
    if(disease)
    {
        currString = currRecord->getDiseaseID();
    }else
    {
        currString = currRecord->getCountry();
    }

    //found a new string, add it to the list
    if(!stringList->contains(currString))
    {
        newString = new string();
        currStringContent = *currString;
        *(newString) = currStringContent;
        stringList->push(newString);
    }

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->getStringList(stringList, disease);
    }
    return;
}

//checks each record for given disease, if so increment its group counter
template<>
void LinkedListNode<Record*>::fillGroupStats(string *disease, int *groupCounters)
{
    LinkedListNode<Record*> *nextNode;
    Record *currRecord = this->getItem();
    string *recordDisease;
    int groupIndex;

    recordDisease = currRecord->getDiseaseID();
    //found a record with given disease, check his group, increment
    if(*recordDisease == *disease)
    {
        groupIndex = currRecord->getAgeGroup();
        groupCounters[groupIndex]++;
    }

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->fillGroupStats(disease, groupCounters);
    }
    
}


//checks if the record contained has exited hospital in given dates
template<>
bool LinkedListNode<Record*>::exitInRange(Date *entryDate, Date *exitDate)
{
    Record *nodeRecord;
    nodeRecord = this->getItem();

    if(nodeRecord->exitInRange(entryDate, exitDate))
    {
        return true;
    }else
    {
        return false;
    }
}

//checks if contained record has given disease
template<>
bool LinkedListNode<Record*>::hasDisease(string *disease)
{
    Record *nodeRecord;
    string *recordDisease;
    nodeRecord = this->getItem();
    recordDisease = nodeRecord->getDiseaseID();

    return (*recordDisease == *disease);
}

//checks if contained record is from given country
template<>
bool LinkedListNode<Record*>::inCountry(string *country)
{
    Record *nodeRecord;
    string *recordCountry;
    nodeRecord = this->getItem();
    recordCountry = nodeRecord->getCountry();

    return (*recordCountry == *country);
}

//returns the number of discharges in given date range
//boolean refers to all countries list or specific one
template<>
void LinkedListNode<Record*>::numPatientDischarges(int *cases, string *disease, string *country, Date *entryDate, Date *exitDate, bool allCountries)
{
    int currCases = *cases;
    LinkedListNode<Record*> *nextNode;

    //should I just check dates range and disease or country as well?
    if(allCountries)
    {
        if(this->hasDisease(disease) && this->exitInRange(entryDate, exitDate))
        {
            (*cases) = currCases + 1;
        }
    }else
    {
        if(this->hasDisease(disease) && this->exitInRange(entryDate, exitDate) && this->inCountry(country))
        {
            (*cases) = currCases + 1;
        }
    }

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->numPatientDischarges(cases, disease, country, entryDate, exitDate, allCountries);
    }
    return;
}


template <typename DataType>
LinkedListNode<DataType>* LinkedListNode<DataType>::getNextNode(){
    return this->nextNode;
}


template <typename DataType>
DataType LinkedListNode<DataType>::getItem(){
    return this->item;
}

template <typename DataType>
bool LinkedListNode<DataType>::isLastNode(){
    return (this->getNextNode() == NULL);
}

template <typename DataType>
bool LinkedListNode<DataType>::contains(DataType givenItem){
    LinkedListNode *nextNode;

    if(*givenItem == *(this->getItem())){
        return true;
    }else if(!this->isLastNode()){
        nextNode = this->getNextNode();
        return nextNode->contains(givenItem);
    }else{
        return false;
    }
}

//parsing function to find out if list contains a node with a bucket
//containing given string in a pair
template <>
bool LinkedListNode<Bucket*>::containsIdentifier(string *givenString)
{
    LinkedListNode *nextNode;
    Bucket *currBucket;
    currBucket = this->getItem();

    if(currBucket->contains(givenString))
    {
        return true;
    }else if(!this->isLastNode())
    {
        nextNode = this->getNextNode();
        return nextNode->containsIdentifier(givenString);
    }else
    {
        return false;
    }
}

//returns the pointer to the given string
template<>
string* LinkedListNode<Bucket*>::getStringPointer(string *givenString)
{
    Bucket *currBucket;
    string *stringPointer;
    LinkedListNode<Bucket*> *nextNode;

    currBucket = this->getItem();

    if(currBucket->contains(givenString))
    {
        stringPointer = currBucket->getStringPointer(givenString);
        return stringPointer;
    }else if(!this->isLastNode())
    {
        nextNode = this->getNextNode();
        return nextNode->getStringPointer(givenString);
    }else//didn't find pointer (shouldn't happen)
    {
        return NULL;
    }
}


//returns the reference to the record with given id
template<>
Record* LinkedListNode<Record*>::getRecordWithID(string *givenID)
{
    Record *currRecord;
    string *currRecordID;
    LinkedListNode<Record*> *nextNode;

    currRecord = this->getItem();
    currRecordID = currRecord->getRecordID();

    //found the record we were looking for
    if(*currRecordID == *givenID)
    {
        return currRecord;
    }else if(!this->isLastNode())
    {
        nextNode = this->getNextNode();
        return nextNode->getRecordWithID(givenID);
    }else
    {
        return NULL;
    }
}

template<>
void LinkedListNode<Bucket*>::pushToFile(Record *givenRecord, string *givenString, int sizeOfBucket)
{
    Bucket *currentBucket;
    Bucket *newBucket;
    currentBucket = this->getItem();
    AVL_Tree *stringTree;
    LinkedListNode *newNode, *nextNode;

    //bucket of current node contains a pair corresponding to given string
    //push the record into its avl tree
    if(currentBucket->contains(givenString))
    {
        stringTree = currentBucket->getStringTree(givenString);
        stringTree->insert(givenRecord);
        return;
    }else if(this->isLastNode())
    {
        //got to the end and we have a full last bucket
        if(currentBucket->isFull())
        {//traversed the whole list, found no pair, construct new bucket with given string
            nextNode = NULL;
            //initializing bucket for the new end node
            //inserting record into its tree
            newBucket = new Bucket(sizeOfBucket);
            newBucket->insert(givenString);
            stringTree = newBucket->getStringTree(givenString);
            stringTree->insert(givenRecord);

            //constructing the new node and adding the new bucket to it
            newNode = new LinkedListNode(newBucket, nextNode);
            newNode->item = newBucket;
            this->setNextNode(newNode);
            return;
        }else//got to the end of the list, but enough space to insert
        {
            currentBucket->insert(givenString);
            stringTree = currentBucket->getStringTree(givenString);
            stringTree->insert(givenRecord);
            return;
        }

    }else
    {//checking if we can insert given string into the next bucket
        nextNode = this->getNextNode();
        nextNode->pushToFile(givenRecord, givenString, sizeOfBucket);
        return;
    }
}

//inserts given string into the first bucket with enough space
//we made sure it is not contained already in the bucket list
template<>
void LinkedListNode<Bucket*>::insertIdentifier(string *givenString, int sizeOfBucket)
{
    Bucket *currentBucket;
    Bucket *newBucket;
    currentBucket = this->getItem();
    LinkedListNode *newNode, *nextNode;

    //cannot insert into current bucket
    if(currentBucket->isFull())
    {
        //this is the last bucket
        //we have to construct a new node/bucket
        //and insert our string into it
        if(this->isLastNode())
        {
            nextNode = NULL;
            newBucket = new Bucket(sizeOfBucket);
            newBucket->insert(givenString);

            newNode = new LinkedListNode(newBucket, nextNode);
            newNode->item = newBucket;
            this->setNextNode(newNode);
            return;
        }else//still some buckets left, check if we can insert into them
        {
            nextNode = this->getNextNode();
            nextNode->insertIdentifier(givenString, sizeOfBucket);
        }

    }else//enough space, simply insert the string into the bucket
    {
        currentBucket->insert(givenString);
    }
    return;
}


//custom record list node function to delete contained records
//main destructor doesn't do that (now we only free records)
template<>
void LinkedListNode<Record*>::freeRecords()
{
    Record *nodeRecord;
    LinkedListNode<Record*> *nextNode;
    //deleting the contained record
    nodeRecord = this->getItem();
    delete nodeRecord;

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->freeRecords();
    }
    return;
}

template<typename Datatype>
void LinkedListNode<Datatype>::freeContents()
{
    Datatype nodeContent;
    LinkedListNode<Datatype> *nextNode;
    //deleting the contained record
    nodeContent = this->getItem();
    
    delete nodeContent;

    if(this->isLastNode())
    {
        return;
    }else
    {
        nextNode = this->getNextNode();
        nextNode->freeContents();
    }
    return;
}


//returns the number of patients stored in list that are still sick
template<>
int LinkedListNode<Record*>::numCurrentPatients()
{
    LinkedListNode<Record*> *nextNode;
    Record *currRecord;
    int currCase = 0;

    currRecord = this->getItem();
    //checking if patient is still sick
    if(currRecord->inHospital())
    {
        currCase = 1;
    }else
    {
        currCase = 0;
    }

    if(this->isLastNode())
    {
        return currCase;
    }else
    {
        nextNode = this->getNextNode();
        return (currCase + nextNode->numCurrentPatients());
    }
}

template<typename DataType>
int LinkedListNode<DataType>::getLength()
{
    LinkedListNode *nextNode;

    if(this->isLastNode())
    {
        return 1;
    }else
    {
        nextNode = this->getNextNode();
        return (1 + nextNode->getLength());
    }  
}

//checks how many instances of records belonging to specific string (country or disease)
template <>
int LinkedListNode<Record*>::getCasesForString(string *givenString, int identifier)
{
    LinkedListNode<Record*> *nextNode;
    Record *currentRecord;
    string *compareString;
    int currCase = 0;
    currentRecord = this->getItem();

    //checking if current record's country is equal to given string
    if(identifier)
    {
        compareString = currentRecord->getCountry();
        //found a new disease case in given country
        if(*givenString == *compareString)
        {
            currCase = 1;
        }
    }else
    {
        compareString = currentRecord->getDiseaseID();
        //found a new disease case in given country
        if(*givenString == *compareString)
        {
            currCase = 1;
        }
    }

    if(this->isLastNode())
    {
        return currCase;
    }else
    {
        nextNode = this->getNextNode();
        return (currCase + nextNode->getCasesForString(givenString, identifier));
    }
    
}

//returns the avl tree of the given string
//contained within a bucket
template<>
AVL_Tree*  LinkedListNode<Bucket*>::getStringFile(string *givenString)
{
    Bucket *currentBucket;
    AVL_Tree *stringTree;
    LinkedListNode<Bucket*>* listNextNode;

    currentBucket = this->getItem();

    //there is a data tree corresponding to the given string
    //return the tree
    if(currentBucket->contains(givenString))
    {
        stringTree = currentBucket->getStringTree(givenString);
        return stringTree;
    }else if(!this->isLastNode())
    {   //search in the next bucket
        listNextNode = this->getNextNode();
        return listNextNode->getStringFile(givenString);
    }else //got to the end of the list, no bucket contained string's avl tree
    {
        return NULL;
    }
}

//checks if we have a worker that takes care
//of the given country string
//if so, returns the FD corresponding to worker socket
template<>
bool LinkedListNode<HeapEntry*>::countryWorkerExists(string *givenString, int *workerFD)
{
    HeapEntry *currEntry;
    string *currString;
    LinkedListNode<HeapEntry*> *nextNode;

    currEntry = this->getItem();
    currString = currEntry->getString();

    //current duplet corresponds to asked country, return true/FD of worker
    if(*currString == *givenString)
    {
        (*workerFD) = currEntry->getInstances();
        return true;
    }

    if(this->isLastNode())
    {
        return false;
    }else
    {
        nextNode = this->getNextNode();
        return nextNode->countryWorkerExists(givenString, workerFD);
    }
}

template class LinkedListNode<int*>;
template class LinkedListNode<string*>;
template class LinkedListNode<Date*>;
template class LinkedListNode<Record*>;
template class LinkedListNode<Bucket*>;
template class LinkedListNode<HeapEntry*>;
