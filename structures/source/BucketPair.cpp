#include "../headers/BucketPair.h"
using namespace std;

BucketPair::BucketPair(string *givenString)
{
    AVL_Tree *pairTree;
    pairTree = this->getDataTree();

    pairTree = new AVL_Tree();
    this->setHashString(givenString);
    this->setDataTree(pairTree);
}

BucketPair::~BucketPair()
{
    //identifier may be deleted through record list
    if(this->getHashString() != NULL)
    {
        delete this->getHashString();
    }
    //deleting pair's tree, only if it exists (bucket not within id hash table)
    if(this->getDataTree() != NULL)
    {
        delete this->getDataTree();
    }
}

void BucketPair::setHashString(string* givenString)
{
    this->hashString = givenString;
}

void BucketPair::setDataTree(AVL_Tree *givenTree)
{
    this->dataTree = givenTree;
}

string* BucketPair::getHashString()
{
    return this->hashString;
}

AVL_Tree* BucketPair::getDataTree()
{
    return this->dataTree;
}
