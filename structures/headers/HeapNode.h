#ifndef HEAP_NODE
#define HEAP_NODE
#include <iostream>
#include <cstdlib>
#include "HeapEntry.h"
using namespace std;


class HeapNode
{
private:
    HeapEntry *data;
    HeapNode *parent;
    HeapNode *leftChild;
    HeapNode *rightChild;
public:
    HeapNode(HeapEntry*);
    ~HeapNode();
    void setData(HeapEntry*);
    void setParent(HeapNode*);
    void setLeftChild(HeapNode*);
    void setRightChild(HeapNode*);
    void insertBelow(HeapNode*);
    void heapifyUp();
    void heapifyDown();
    void preOrderPrint();
    HeapEntry* getData();
    HeapNode* getParent();
    HeapNode* getLeftChild();
    HeapNode* getRightChild();
    bool isRoot();
    bool isLeftChild();
    bool isRightChild();
    bool hasLeftChild();
    bool hasRightChild();
    bool hasTwoChildren();
    bool hasNoChildren();
    int getHeight();
    friend bool operator==(HeapNode&, HeapNode&);
    friend bool operator<(HeapNode&, HeapNode&);
    friend bool operator>(HeapNode&, HeapNode&);
    friend ostream& operator<<(ostream&, HeapNode&);
};

void swap(HeapNode*, HeapNode*);
HeapNode* max(HeapNode*, HeapNode*);




#endif
