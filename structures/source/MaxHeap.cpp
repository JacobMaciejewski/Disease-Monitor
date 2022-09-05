 #include "../headers/MaxHeap.h"

 MaxHeap::MaxHeap()
 {
     //initially no entries
     this->setRoot(NULL);
     this->setLastNode(NULL);
 }

//calls delete upon root
//last node is not freed implicitily, this will be done through root recursion
 MaxHeap::~MaxHeap()
 {
     //there are nodes to free
     if(!this->isEmpty())
     {
         delete this->getRoot();
     }
 }

void MaxHeap::setRoot(HeapNode *newRoot)
{
    this->root = newRoot;
}

void MaxHeap::setLastNode(HeapNode *newLastNode)
{
    this->lastNode = newLastNode;
}

void MaxHeap::push(HeapEntry *newEntry)
{
    HeapNode *newNode;
    HeapNode *newNodeFather;
    newNode = new HeapNode(newEntry);

    //heap empty, simply setting new node as root
    if(this->isEmpty())
    {
        this->setRoot(newNode);
        this->setLastNode(newNode);
        return;
    }

    //calculate under which node our new entry will be inserted
    newNodeFather = this->calculateInsertPosition();
    newNodeFather->insertBelow(newNode);
    //currently inserted node, becomes the last inserted node
    this->setLastNode(newNode);
    //establishing balanced tree property, starting from new node
    newNode->heapifyUp();

    return;
}

//printing heap nodes in preorder traversal
void MaxHeap::preOrderPrint()
{
    HeapNode *currRoot;

    if(this->isEmpty())
    {
        cout << "Heap empty!" << endl;
    }else
    {
        currRoot = this->getRoot();
        currRoot->preOrderPrint();
    }
    return;
}

//used to evaluate the new last node, after poping
void MaxHeap::updateLastNode()
{
    //used for traversal, will end up being the last node
    HeapNode *traversalNode;
    traversalNode = this->getLastNode();

    //as long as we are the left child or we got to the root
    while(!traversalNode->isRoot() && traversalNode->isLeftChild())
    {
        traversalNode = traversalNode->getParent();
    }


    //didn't get to a root node, have to traverse the left subtree of father
    if(!traversalNode->isRoot())
    {
        traversalNode = traversalNode->getParent()->getLeftChild();
    }

    //get to the rightmost node in current subtree
    while(traversalNode->hasRightChild())
    {
        traversalNode = traversalNode->getRightChild();
    }

    //got to the new last node
    //update its reference in max heap
    this->setLastNode(traversalNode);
    return;
}

HeapNode* MaxHeap::getRoot()
{
    return this->root;
}

HeapNode* MaxHeap::getLastNode()
{
    return this->lastNode;
}


HeapNode* MaxHeap::pop()
{
    HeapNode  *oldRoot, *currLastNode, *currLastNodeFather,
              *oldRootLeftChild, *oldRootRightChild,
              *newRoot;
    if(this->isEmpty())
    {
        cout << endl << "Cannot pop, heap is empty!" << endl;
        return NULL;
    }else
    {
        oldRoot = this->getRoot();
        currLastNode = this->getLastNode();
        currLastNodeFather = currLastNode->getParent();

        //heap includes only one node
        //setting heap as empty and returning root
        if(currLastNode->isRoot())
        {
            this->setLastNode(NULL);
            this->setRoot(NULL);
            return oldRoot;
        }

        //last node is set as root, look for new last node
        this->updateLastNode();

        //disconnecting old last node from its father
        if(currLastNode->isLeftChild())
        {
            currLastNodeFather->setLeftChild(NULL);
            //last node was a left child, its father becomes new last node
            //this->setLastNode(currLastNodeFather);
        }else
        {
            currLastNodeFather->setRightChild(NULL);
            //last node was a right child, father still has one
            //this->setLastNode(currLastNodeFather->getLeftChild());
        }

        //set last node as root
        //NULL father implies last node becomes a root
        currLastNode->setParent(NULL);

        //connecting new root with previous root children
        oldRootLeftChild = oldRoot->getLeftChild();
        oldRootRightChild = oldRoot->getRightChild();
        //old root children have new father
        if(oldRootLeftChild != NULL)
        {
            oldRootLeftChild->setParent(currLastNode);
        }
        if(oldRootRightChild != NULL)
        {
            oldRootRightChild->setParent(currLastNode);
        }

        currLastNode->setLeftChild(oldRootLeftChild);
        currLastNode->setRightChild(oldRootRightChild);

        //old root removed, disconnect it from its children
        oldRoot->setLeftChild(NULL);
        oldRoot->setRightChild(NULL);

        this->setRoot(currLastNode);
        //last node is the new root
        newRoot = this->getRoot();
        //time to heapify down to restore tree property
        newRoot->heapifyDown();
    }
    //returning the old root
    return oldRoot;
}

//returns reference to the father of the new node
HeapNode* MaxHeap::calculateInsertPosition()
{
    //used for traversal, will become new node's father
    HeapNode *traversalNode;
    //started from the position we last inserted to
    traversalNode = this->getLastNode();

    //traversing up as long as we are the right child or we reach root
    while(!traversalNode->isRoot() && traversalNode->isRightChild())
    {
        traversalNode = traversalNode->getParent();
    }

    //didn't get to the root, remaining in the same subtree
    if(!traversalNode->isRoot())
    {
        //parent has right child
        //searched position is the leftmost one on the right subtree of parent
        if(traversalNode->getParent()->hasRightChild())
        {
            traversalNode = traversalNode->getParent()->getRightChild();
            while(traversalNode->hasLeftChild())
            {
                traversalNode = traversalNode->getLeftChild();
            }
        }else //parent has no right child, current node will become its right child
        {
            traversalNode = traversalNode->getParent();
        }

    }else // root has been reached, tree expands to new level (new node = left most child in the new level)
    {
        while(traversalNode->hasLeftChild())
        {
            traversalNode = traversalNode->getLeftChild();
        }
    }

    //the father of the new node
    return traversalNode;
}

//returns the height of the heap (left most path lenght)
int MaxHeap::getHeight()
{
    HeapNode *currRoot;
    if(this->isEmpty())
    {
        return 0;
    }else
    {
        currRoot = this->getRoot();
        currRoot->getHeight();
    }

}

//checks if heap is empty
bool MaxHeap::isEmpty()
{
    HeapNode *currRoot;
    currRoot = this->getRoot();

    return (currRoot == NULL);
}

bool MaxHeap::isLastNode()
{
    HeapNode *currRoot;
    HeapNode *currLastNode;

    return (currRoot == currLastNode);
}
