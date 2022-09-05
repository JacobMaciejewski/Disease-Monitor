#ifndef AVL_TREE_NODE
#define AVL_TREE_NODE
#include <iostream>
#include "Record.h"
using namespace std;

template<typename DataType>
class LinkedList;

template<typename DataType>
class LinkedListNode;

class AVL_TreeNode
{
private:
    int height;
    AVL_TreeNode *leftChild;
    AVL_TreeNode *rightChild;
    LinkedList<Record*> *recordList;
public:
    AVL_TreeNode(Record*);
    ~AVL_TreeNode();
    Record* getData();
    int getHeight();
    AVL_TreeNode* getLeftChild();
    AVL_TreeNode* getRightChild();
    void setData(Record*);
    void setHeight(int);
    void setLeftChild(AVL_TreeNode *);
    void setRightChild(AVL_TreeNode *);
    void getBalanceFactor(int*);
    void print();
    void getCasesInRange(Date*, Date*, int*);
    void getCasesForString(Date*, Date*, string*, int*, int);
    void numCurrentPatients(int*);
    void setRecordList(LinkedList<Record*>*);
    void getRecordsInDate(Date*, LinkedList<Record*>**);
    void push(Record*);
    void getAgeGroupCases(string*, int*, Date*, Date*);
    bool hasChildren();
    bool hasLeftChild();
    bool hasRightChild();
    bool under(Date*);
    bool over(Date*);
    bool equal(Date*);
    bool inRange(Date*, Date*);
    LinkedList<Record*>* getRecordList();
    Record* getFirstElement();
    Date* getNodeDate();
    AVL_TreeNode* insert(Record*);
    AVL_TreeNode* rotateLeft();
    AVL_TreeNode* rotateRight();
    AVL_TreeNode* updateParent(int, Record*);
};

//auxiliary function for node height calculation
//avoiding null node calls
int calculateHeight(AVL_TreeNode*);

#endif
