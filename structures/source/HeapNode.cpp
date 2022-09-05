#include "../headers/HeapNode.h"
using namespace std;


HeapNode::HeapNode(HeapEntry *givenData)
{
    this->setData(givenData);
    this->setLeftChild(NULL);
    this->setRightChild(NULL);
    this->setParent(NULL);
}

//doesn't free parent node, it will be done through
//back propagation of recursion
HeapNode::~HeapNode()
{
    //deleting the entry contained within current node
    delete this->getData();

    if(this->hasLeftChild())
    {
        delete this->getLeftChild();
    }

    if(this->hasRightChild())
    {
        delete this->getRightChild();
    }
}

void HeapNode::setData(HeapEntry *givenEntry)
{
    this->data = givenEntry;
}

void HeapNode::setParent(HeapNode *givenParent)
{
    this->parent = givenParent;
}

void HeapNode::setLeftChild(HeapNode *givenLeftChild)
{
    this->leftChild = givenLeftChild;
}

void HeapNode::setRightChild(HeapNode *givenRightChild)
{
    this->rightChild = givenRightChild;
}

//finds which child is empty, inserts new node in its place
//called only for nodes with one child
void HeapNode::insertBelow(HeapNode *newNode)
{
    //current node becomes the father of the given node
    newNode->setParent(this);

    if(this->hasLeftChild())
    {
        this->setRightChild(newNode);
    }else
    {
        this->setLeftChild(newNode);
    }
    return;
}

//swaps nodes starting from the newly inserted one
//till balanced binary tree property established
void HeapNode::heapifyUp()
{
    HeapNode *currParent;
    currParent = this->getParent();

    //got to the root, property established
    if(this->isRoot())
    {
        return;
    }else if(*this > *currParent)
    {
        swap(this, currParent);
        //keep swapping for father and up
        currParent->heapifyUp();
        return;
    }else{//current node smaller/equal to father, property established
        return;
    }
}

//swaps subsequent nodes, till tree property restored
void HeapNode::heapifyDown()
{
    HeapNode *biggerChild;
    //got to a leaf node, terminate
    if(this->hasNoChildren())
    {
        return;
    }

    biggerChild = max(this->getLeftChild(), this->getRightChild());

    //found bigger child, have to swap
    if(*biggerChild > *this)
    {
        swap(this, biggerChild);
        biggerChild->heapifyDown();
    }else//no bigger children, property restored
    {
        return;
    }
}

//preorder print of heap nodes
void HeapNode::preOrderPrint()
{
    HeapNode *currLeftChild, *currRightChild;
    currLeftChild = this->getLeftChild();
    currRightChild = this->getRightChild();
    //simply print node's data
    cout << *this;

    if(this->hasLeftChild())
    {
        currLeftChild->preOrderPrint();
    }

    if(this->hasRightChild())
    {
        currRightChild->preOrderPrint();
    }

    return;
}

HeapEntry* HeapNode::getData()
{
    return this->data;
}

HeapNode* HeapNode::getParent()
{
    return this->parent;
}

HeapNode* HeapNode::getLeftChild()
{
    return this->leftChild;
}

HeapNode* HeapNode::getRightChild()
{
    return this->rightChild;
}

//checks if current node has no parent
bool HeapNode::isRoot()
{
    return (this->getParent() == NULL);
}

//checks if current node is the left child of its parent
bool HeapNode::isLeftChild()
{
    HeapNode *currParent;
    currParent = this->getParent();

    return (currParent->getLeftChild() == this);
}

//checks if current node is the right child of its parent
bool HeapNode::isRightChild()
{
    HeapNode *currParent;
    currParent = this->getParent();

    return (currParent->getRightChild() == this);
}

bool HeapNode::hasLeftChild()
{
    HeapNode *currLeftChild;
    currLeftChild = this->getLeftChild();

    return (currLeftChild != NULL);
}

bool HeapNode::hasRightChild()
{
    HeapNode *currRightChild;
    currRightChild = this->getRightChild();

    return (currRightChild != NULL);
}

//checks if current node has two children
bool HeapNode::hasTwoChildren()
{
    return (this->hasLeftChild() && this->hasRightChild());
}

//checks if current node has no children
bool HeapNode::hasNoChildren()
{
    return (!this->hasLeftChild() && !this->hasRightChild());
}

int HeapNode::getHeight()
{
    HeapNode *currLeftChild;
    currLeftChild = this->getLeftChild();

    if(this->hasLeftChild())
    {
        if(this->isRoot())
        {
            return currLeftChild->getHeight();
        }else
        {
            return 1 + currLeftChild->getHeight();
        }
    }else//got to the left most node of the last level
    {
        return 1;
    }
}

//checks if nodes contain data with same number of instances
bool operator==(HeapNode &HeapNode1, HeapNode &HeapNode2)
{
    HeapEntry *heapNodeEntry1, *heapNodeEntry2;
    heapNodeEntry1 = HeapNode1.getData();
    heapNodeEntry2 = HeapNode2.getData();

    return (*heapNodeEntry1 == *heapNodeEntry2);
}

//checks if first node containes data with more instances
bool operator>(HeapNode &HeapNode1, HeapNode &HeapNode2)
{
    HeapEntry *heapNodeEntry1, *heapNodeEntry2;
    heapNodeEntry1 = HeapNode1.getData();
    heapNodeEntry2 = HeapNode2.getData();

    return (*heapNodeEntry1 > *heapNodeEntry2);
}

//checks if first node containes data with less instances
bool operator<(HeapNode &HeapNode1, HeapNode &HeapNode2)
{
    HeapEntry *heapNodeEntry1, *heapNodeEntry2;
    heapNodeEntry1 = HeapNode1.getData();
    heapNodeEntry2 = HeapNode2.getData();

    return (*heapNodeEntry1 < *heapNodeEntry2);
}

//simply prints the entry contained in the node
ostream& operator<<(ostream &out, HeapNode &myHeapNode)
{
    HeapEntry *currHeapEntry;
    currHeapEntry = myHeapNode.getData();
    out << *currHeapEntry;
    return out;
}

//swaps the data of the given heap nodes
void swap(HeapNode *firstNode, HeapNode *secondNode)
{
    HeapEntry *firstNodeData, *secondNodeData, *tempData;

    firstNodeData = firstNode->getData();
    secondNodeData = secondNode->getData();

    tempData = firstNodeData;
    firstNode->setData(secondNodeData);
    secondNode->setData(tempData);
}

//returns a reference to the node with bigger data
//first node returned if bigger, else the second one
//if one of the given node is NULL, other is returned
HeapNode* max(HeapNode *firstNode, HeapNode *secondNode)
{
    //checking if one the nodes is null
    if(firstNode == NULL)
    {
        return secondNode;
    }

    if(secondNode == NULL)
    {
        return firstNode;
    }

    //none of the nodes is null
    //return the bigger one
    if(*firstNode > *secondNode)
    {
        return firstNode;
    }else
    {
        return secondNode;
    }
}
