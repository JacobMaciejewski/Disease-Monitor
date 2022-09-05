#include "../headers/Bucket.h"
#include "../headers/LinkedList.h"

using namespace std;

Bucket::Bucket(int sizeOfBucket)
{
    int maxPairs;
    BucketPair **bucketPairs;

    this->setSize(sizeOfBucket);
    //initially empty
    this->setNumOfPairs(0);

    maxPairs = this->calculateMaxPairs();
    bucketPairs = (BucketPair **)malloc(sizeof(BucketPair*) * maxPairs);
    this->setPairs(bucketPairs);
}

//deleting all the pairs contained within given bucket
Bucket::~Bucket()
{
    int currPairsIn;
    BucketPair *currPair;
    currPairsIn = this->getNumOfPairs();

    for(int index = 0; index < currPairsIn; index++)
    {
        currPair = this->getIthPair(index);
        delete currPair;
    }
    //deleting the reference to pairs array
    free(this->getPairs());
}

void Bucket::setSize(int givenSize)
{
    this->size = givenSize;
}


void Bucket::setNumOfPairs(int numberOfPairs)
{
    this->pairsIn = numberOfPairs;
}void incrementPairs();

void Bucket::setPairs(BucketPair **pairArray)
{
    this->pairs = pairArray;
}

void Bucket::setIthPair(int index, BucketPair *givenPair)
{
    BucketPair **currentPairs;
    currentPairs = this->getPairs();
    currentPairs[index] = givenPair;
    return;
}

//number of pairs inside the bucket ++
void Bucket::incrementPairs()
{
    int currentPairs = this->getNumOfPairs();
    this->setNumOfPairs(currentPairs + 1);
    return;
}

//checks if bucket contains a pair with the string of the given record
//if so, record is added to the avl tree of this pair
//if not, new pair is being constructed
void Bucket::insert(string *givenString)
{
    BucketPair *newPair;
    int insertIndex = this->getInsertIndex();

    newPair = new BucketPair(givenString);
    this->setIthPair(insertIndex, newPair);
    //new pair added
    this->incrementPairs();
    return;
}

//stdout stream doesn't support recursion
//function for proper printing of buckets
void Bucket::print()
{
    int pairsContained = this->getNumOfPairs();
    BucketPair *currPair;
    string *pairString;
    AVL_Tree *pairTree;

    //no pairs to print pair = [string, tree]
    if(this->isEmpty())
    {
        cout << endl << "Bucket is empty" << endl;
    }else{

        cout << endl << "Bucket pairs: " << endl;
        for(int index = 0; index < pairsContained; index++)
        {
            currPair = this->getIthPair(index);
            pairString = currPair->getHashString();
            cout << endl << "Pair " << index + 1 << " [" << *pairString << "]:" << endl;
            pairTree = currPair->getDataTree();
            //no data in current pair tree
            if(pairTree == NULL)
            {
                cout << endl << "No data tree!" << endl;
            }
            pairTree->print();
        }

    }
    return;
}

//inserts all bucket strings into the given list
void Bucket::getAllStrings(LinkedList<string*> *stringList)
{
    int pairsContained;
    BucketPair *currPair;
    string *currString;
    string *currStringInstance;
    pairsContained = this->getNumOfPairs();

    for(int index = 0; index < pairsContained; index++)
    {
        currPair = this->getIthPair(index);
        currString = currPair->getHashString();
        //producing new string, we don't want a reference to bucket string
        currStringInstance = new string(*currString);
        stringList->push(currStringInstance);
    }
    return;
}

//returns the total size in bytes of the given bucket
int Bucket::getSize()
{
    return this->size;
}

int Bucket::getNumOfPairs()
{
    return this->pairsIn;
}

//returns the max number of pair references
//we can store in a bucket
int Bucket::calculateMaxPairs()
{
    int sizeOfBucket = this->getSize();
    //adding the size of two integers
    int headerSize = (2 * sizeof(int));
    int spaceForPairs = sizeOfBucket - headerSize;
    int maxNumOfPairs = spaceForPairs / sizeof(BucketPair*);
    return maxNumOfPairs;
}

//returns the next available index
//in the array of references to pairs
int Bucket::getInsertIndex()
{
    return this->getNumOfPairs();
}

//returns the pointer to the given string (stored in bucket pair)
string* Bucket::getStringPointer(string *givenString)
{
    BucketPair *stringPair;
    stringPair = this->getStringPair(givenString);
    return stringPair->getHashString();
}


BucketPair** Bucket::getPairs()
{
    return this->pairs;
}

BucketPair* Bucket::getIthPair(int index)
{
    BucketPair **currentPairs;
    currentPairs = this->getPairs();

    return currentPairs[index];
}

//returns the pair that contains the given string
BucketPair* Bucket::getStringPair(string *givenString)
{
    int pairIndex = this->getPairIndex(givenString);
    return this->getIthPair(pairIndex);
}

//returns the avl tree that corresponds to given string
//ex. returns the tree containing chinese cases for "China"
AVL_Tree* Bucket::getStringTree(string *givenString)
{
    BucketPair *pairWithGivenString;
    pairWithGivenString = this->getStringPair(givenString);
    return pairWithGivenString->getDataTree();
}

//returns the index of the pair reference
//that contains the given string
int Bucket::getPairIndex(string* givenString)
{
    int currentPairsNum = this->getNumOfPairs();
    int index;
    string *pairString;
    BucketPair *currentPair;

    //iterating through pairs of current bucket
    for(index = 0; index < currentPairsNum; index++)
    {
        currentPair = this->getIthPair(index);
        pairString = currentPair->getHashString();
        //found pair containing given string
        if(*pairString == *givenString)
        {
            return index;
        }
    }
    //didn't find pair with given string, should not happen
    return -1;
}

//needed only for code purity regarding templating linked list
//won't be used through execution
bool operator==(Bucket &bucket1, Bucket &bucket2)
{
    return false;
}

//checks if bucket has no pairs
bool Bucket::isEmpty()
{
    int currPairs;
    currPairs = this->getNumOfPairs();
    return (currPairs == 0);
}

//checks if bucket contains reference to a pair
//with given string
bool Bucket::contains(string *givenString)
{
    int currentPairsNum = this->getNumOfPairs();
    int index;
    string *pairString;
    BucketPair *currentPair;

    //iterating through pairs of current bucket
    for(index = 0; index < currentPairsNum; index++)
    {
        currentPair = this->getIthPair(index);
        pairString = currentPair->getHashString();
        //found pair containing given string
        if(*pairString == *givenString)
        {
            return true;
        }
    }
    //no instance of pair containing given string
    return false;
}

//no more pair references can be stored in current bucket
bool Bucket::isFull()
{
    int currentPairsNum = this->getNumOfPairs();
    int maxPairsNum = this->calculateMaxPairs();
    return (currentPairsNum >= maxPairsNum);
}
