#ifndef AVL_TREE
#define AVL_TREE
#include "AVL_Tree.h"
#include "LinkedList.h"
#include <iostream>
using namespace std;

class AVL_TreeNode;
class Record;
class Date;

class AVL_Tree
{
private:
    AVL_TreeNode *root;
    int elements;
public:
    AVL_Tree();
    ~AVL_Tree();
    void setRoot(AVL_TreeNode*);
    void setElements(int);
    void incrementElements();
    void insert(Record*);
    void print();
    void getCasesInRange(Date*, Date*, int*);
    void getCasesForString(Date*, Date*, string*, int*, int);
    void numCurrentPatients(int*);
    void getAgeGroupsCases(string *, int*, Date*, Date*);
    int getNumOfElements();
    int getHeight();
    AVL_TreeNode* getRoot();
    void getRecordsInDate(Date *, LinkedList<Record*>**);
    bool isEmpty();
};




#endif
