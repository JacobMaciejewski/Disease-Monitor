#ifndef HASH_TABLE
#define HASH_TABLE
#include "LinkedList.h"
#include "Bucket.h"
#include "AVL_Tree.h"
#include <cstdlib>
using namespace std;

class HashTable
{
private:
    int bucketSize;
    int tableSize;
    LinkedList<Bucket*> **bucketArray;
public:
    HashTable(int, int);
    ~HashTable();
    int getBucketSize();
    int getTableSize();
    int hashFunction(string*);
    LinkedList<Bucket*>** getBucketArray();
    LinkedList<Bucket*>* getIthBucket(int);
    LinkedList<Bucket*>* getBucketList(string*);
    AVL_Tree* getStringFile(string*);
    string* getStringPointer(string*);
    bool containsIdentifier(string*);
    void setBucketSize(int);
    void setTableSize(int);
    void setBucketArray(LinkedList<Bucket*>**);
    void setIthList(int, LinkedList<Bucket*>*);
    void insert(Record*, string*);
    void insertIdentifier(string*);
    void getAllStrings(LinkedList<string*>*);
    void print();
};



#endif
