#ifndef BUCKET_PAIR
#define BUCKET_PAIR
#include "AVL_Tree.h"
#include <string>
#include <iostream>
using namespace std;

class BucketPair
{
private:
    string *hashString;
    AVL_Tree *dataTree;

public:
    BucketPair(string*);
    ~BucketPair();
    void setHashString(string*);
    void setDataTree(AVL_Tree*);
    std::string *getHashString();
    AVL_Tree *getDataTree();
};

#endif
