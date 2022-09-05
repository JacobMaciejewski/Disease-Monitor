#include "../headers/AVL_Tree.h"
#include "../headers/AVL_TreeNode.h"
#include "../headers/Record.h"
#include "../headers/Date.h"
#include <iostream>
using namespace std;

//avl_tree initialized, no initial node
AVL_Tree::AVL_Tree(){
    this->setRoot(NULL);
    //initially no elements in
    this->setElements(0);
}

AVL_Tree::~AVL_Tree(){

    //deleting nodes, only if there are any
    if(!this->isEmpty())
    {
        delete this->getRoot();
    }
}

void AVL_Tree::setRoot(AVL_TreeNode* newRoot){
    this->root = newRoot;
}

void AVL_Tree::setElements(int givenElements)
{
    this->elements = givenElements;
}

void AVL_Tree::incrementElements()
{
    int currElements = this->getNumOfElements();
    this->setElements(currElements + 1);
    return;
}

void AVL_Tree::insert(Record* newRecord){
    AVL_TreeNode *currentRoot, *newRoot;

    if(this->isEmpty()){
        newRoot = new AVL_TreeNode(newRecord);
        this->setRoot(newRoot);
    }else{
        currentRoot = this->getRoot();
        newRoot = currentRoot->insert(newRecord);
        this->setRoot(newRoot);
    }
    //new element added into the tree
    this->incrementElements();
    return;
}

void AVL_Tree::print(){
    AVL_TreeNode *currentRoot;
    currentRoot = this->getRoot();

    if(currentRoot != NULL){
        currentRoot->print();
    }else{
        cout << "AVL TREE IS EMPTY!" << endl;
    }
    return;
}

//returns cases of records of given tree in dates range
void AVL_Tree::getCasesInRange(Date *startDate, Date *endDate, int *cases)
{
    AVL_TreeNode *currRoot;
    //no records in the tree
    if(this->isEmpty())
    {
        *cases = 0;
        return;
    }else
    {
        currRoot = this->getRoot();
        currRoot->getCasesInRange(startDate, endDate, cases);
        return;
    }
}

//returns the number of instances of given string (country/disease)
//using identifier variable (0 = disease, 1 = country)
void AVL_Tree::getCasesForString(Date *startDate, Date *endDate, string *givenString, int *cases, int identifier)
{
    AVL_TreeNode *currRoot;

    //no entries in current tree
    if(this->isEmpty())
    {
        *cases = 0;
        return;
    }else
    {
        currRoot = this->getRoot();
        currRoot->getCasesForString(startDate, endDate, givenString, cases, identifier);
        return;
    }
}

//returns the number of sick people (still in hospital) through the given parameter
void AVL_Tree::numCurrentPatients(int *sickPeople)
{
    AVL_TreeNode *currRoot;

    if(this->isEmpty())
    {
        *sickPeople = 0;
    }else
    {
        currRoot = this->getRoot();
        currRoot->numCurrentPatients(sickPeople);
    }
    return;
}


int AVL_Tree::getHeight(){
    AVL_TreeNode *currentRoot;

    if(this->isEmpty())
    {
        return 0;
    }else
    {
        currentRoot = this->getRoot();
        return currentRoot->getHeight();
    }
}

//returns an array containing cases for given disease for each age group
void AVL_Tree::getAgeGroupsCases(string *disease, int *groupAgesCases, Date *entryDate, Date *exitDate)
{
    AVL_TreeNode *treeHead;

    if(this->isEmpty())
    {
        return;
    }else
    {
        treeHead = this->getRoot();
        treeHead->getAgeGroupCases(disease, groupAgesCases, entryDate, exitDate);
    }
    return;
}

//used to count the number of elements within tree
//using traversal (debugging purposes)
int AVL_Tree::getNumOfElements()
{
    return this->elements;
}

AVL_TreeNode* AVL_Tree::getRoot(){
    return this->root;
}

void AVL_Tree::getRecordsInDate(Date *givenDate, LinkedList<Record*> **recordList)
{
    AVL_TreeNode *treeHead;
    if(this->isEmpty())
    {
        recordList = NULL;
    }else
    {
        treeHead = this->getRoot();
        treeHead->getRecordsInDate(givenDate, recordList);
    }
    return;
}


bool AVL_Tree::isEmpty(){
    return (this->getRoot() == NULL);
}
