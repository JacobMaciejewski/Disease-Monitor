#include "../headers/HashTable.h"
using namespace std;

HashTable::HashTable(int sizeOfBucket, int sizeOfTable)
{
    LinkedList<Bucket*> **arrayOfLists;


    this->setBucketSize(sizeOfBucket);
    this->setTableSize(sizeOfTable);
    arrayOfLists = (LinkedList<Bucket*>**)malloc(sizeof(LinkedList<Bucket*>*) * sizeOfTable);
    this->setBucketArray(arrayOfLists);

    //no bucket is initialized (no data)
    for(int index = 0; index < sizeOfTable; index++)
    {
        arrayOfLists[index] = NULL;
    }
}

HashTable::~HashTable()
{
    int numOfLists;
    LinkedList<Bucket*> *listOfBuckets;
    numOfLists = this->getTableSize();

    //deleting each bucket list
    for(int index = 0; index < numOfLists; index++)
    {
        listOfBuckets = this->getIthBucket(index);
        if(listOfBuckets != NULL)
        {
            listOfBuckets->freeContents();
            delete listOfBuckets;
        }
    }
    //free the reference to the bucket list array
    free(this->getBucketArray());
};

int HashTable::getBucketSize()
{
    return this->bucketSize;
}

int HashTable::getTableSize()
{
    return this->tableSize;
}

//returns index to hash table array of lists of buckets
int HashTable::hashFunction(string* givenString)
{
    int myTableSize = this->getTableSize();
    string myString = *givenString;
    unsigned int hashVal = 0;

    for (int letter = 0; letter < myString.length(); ++letter)
    {
        hashVal ^= (hashVal << 5) +
                   (hashVal >> 2) +
                   myString[letter];
    }
    return hashVal % tableSize;
}

LinkedList<Bucket*>** HashTable::getBucketArray()
{
    return this->bucketArray;
}

//return the ith list of buckets
LinkedList<Bucket*>* HashTable::getIthBucket(int index)
{
    LinkedList<Bucket*>** myBucketList;
    myBucketList = this->getBucketArray();

    return myBucketList[index];
}


//returns a pointer to the list of buckets, we should search in
LinkedList<Bucket*>* HashTable::getBucketList(string *givenString)
{
    int index = this->hashFunction(givenString);
    LinkedList<Bucket*>* myBucketList;

    //getting the reference to the bucket, hash function pointed to
    myBucketList = this->getIthBucket(index);

    if(myBucketList == NULL)
    {
        myBucketList = new LinkedList<Bucket*>();
        this->setIthList(index, myBucketList);
        return myBucketList;
    }

    return myBucketList;
}

//returns the tree containing the data corresponding to the given string
//calls the procedure to the bucket list level
AVL_Tree* HashTable::getStringFile(string *givenString)
{
    AVL_Tree *searchedTree;
    LinkedList<Bucket*>* targetBucketList;

    targetBucketList = this->getBucketList(givenString);
    searchedTree = targetBucketList->getStringFile(givenString);
    return searchedTree;
}

//returns the pointer to the given string
string* HashTable::getStringPointer(string *givenString)
{
    LinkedList<Bucket*> *stringList;
    stringList = this->getBucketList(givenString);

    return stringList->getStringPointer(givenString);
}




//checks if given identifier (country/disease)
//is contained in our database
bool HashTable::containsIdentifier(string *givenString)
{
    LinkedList<Bucket*> *stringList;
    //getting the bucket in which the string should be in
    stringList = this->getBucketList(givenString);

    //checking if list contains bucket with given string
    if(stringList->containsIdentifier(givenString))
    {
        return true;
    }else
    {
        return false;
    }
}

void HashTable::setBucketSize(int givenBucketSize)
{
    this->bucketSize = givenBucketSize;
}

void HashTable::setTableSize(int givenTableSize)
{
    this->tableSize = givenTableSize;
}

void HashTable::setBucketArray(LinkedList<Bucket*>** givenArray)
{
    this->bucketArray = givenArray;
}

void HashTable::setIthList(int index, LinkedList<Bucket*>* givenList)
{
    LinkedList<Bucket*>** currArray;
    currArray = this->getBucketArray();
    currArray[index] = givenList;
    return;
}

//moves Record and hashable string to the list of buckets
void HashTable::insert(Record* givenRecord, string* givenString)
{
    LinkedList<Bucket*>* targetBucketList;
    int sizeOfBucket = this->getBucketSize();

    targetBucketList = this->getBucketList(givenString);
    targetBucketList->pushToFile(givenRecord, givenString, sizeOfBucket);
    return;
}

//inserts new string into a bucket
void HashTable::insertIdentifier(string *givenString)
{
    LinkedList<Bucket*>* targetBucketList;
    int sizeOfBucket = this->getBucketSize();

    targetBucketList = this->getBucketList(givenString);
    targetBucketList->insertIdentifier(givenString, sizeOfBucket);
    return;
}



//returns all the strings contained in the buckets
void HashTable::getAllStrings(LinkedList<string*>* stringList)
{
    int myTableSize;
    LinkedList<Bucket*> *currList;
    myTableSize = this->getTableSize();

    for(int index = 0; index < myTableSize; index++)
    {
        currList = this->getIthBucket(index);
        //if current bucket initialized, get its strings
        if(currList != NULL)
        {
            currList->getAllStrings(stringList);
        }
    }
    return;
}

//prints the pairs of all bucket lists, hash table points to
void HashTable::print()
{
    int listsNum;
    LinkedList<Bucket*>* currList;

    listsNum = this->getTableSize();

    cout << endl << "Hash table lists:" << endl;

    for(int index = 0; index < listsNum; index++)
    {
        currList = this->getIthBucket(index);
        //uninitialized list of buckets
        if(currList != NULL)
        {
            currList->printBuckets();
        }else
        {
            cout << endl << "Empty list!" << endl;
        }

    }
    return;
}
