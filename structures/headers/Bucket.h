#ifndef BUCKET
#define BUCKET
#include "BucketPair.h"
#include "AVL_Tree.h"

template<typename DataType>
class LinkedList;

using namespace std;

class Bucket
{
private:
    int size;
    int pairsIn;
    BucketPair **pairs;
public:
    Bucket(int);
    ~Bucket();
    void setSize(int);
    void setNumOfPairs(int);
    void setPairs(BucketPair**);
    void setIthPair(int, BucketPair*);
    void incrementPairs();
    void insert(string*);
    void print();
    void getAllStrings(LinkedList<string*>*);
    int getSize();
    int getNumOfPairs();
    int calculateMaxPairs();
    int getInsertIndex();
    int getPairIndex(string*);
    string *getStringPointer(string*);
    BucketPair** getPairs();
    BucketPair* getIthPair(int);
    BucketPair* getStringPair(string*);
    AVL_Tree* getStringTree(string*);
    friend bool operator==(Bucket&, Bucket&);
    bool contains(string*);
    bool isFull();
    bool isEmpty();
};


#endif
