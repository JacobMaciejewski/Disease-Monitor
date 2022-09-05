#ifndef MAX_HEAP
#define MAX_HEAP
#include "HeapNode.h"
using namespace std;

class MaxHeap
{
private:
    HeapNode *root;
    //used to calculate new insert position
    HeapNode *lastNode;
public:
    MaxHeap();
    ~MaxHeap();
    void setRoot(HeapNode*);
    void setLastNode(HeapNode*);
    void push(HeapEntry*);
    void preOrderPrint();
    void updateLastNode();
    HeapNode* getRoot();
    HeapNode* getLastNode();
    HeapNode* pop();
    HeapNode* calculateInsertPosition();
    int getHeight();
    bool isEmpty();
    bool isLastNode();
};

#endif
