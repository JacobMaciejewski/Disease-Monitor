#include "../headers/AVL_TreeNode.h"
#include "../headers/LinkedList.h"
#include "../headers/LinkedListNode.h"
using namespace std;

AVL_TreeNode::AVL_TreeNode(Record *nodeData)
{
    LinkedList<Record*> *nodeRecordList;
    nodeRecordList = new LinkedList<Record*>;

    this->setRecordList(nodeRecordList);
    this->push(nodeData);
    //got to the leaf level, no children
    this->height = 1;
    this->leftChild = NULL;
    this->rightChild = NULL;
}

//function doesn't free the record contained within the node
//records, stored in a main list, are destroyed only from there
AVL_TreeNode::~AVL_TreeNode()
{
    if(this->hasLeftChild())
    {
        delete this->getLeftChild();
    }

    delete this->getRecordList();
    
    if(this->hasRightChild())
    {
        delete this->getRightChild();
    }
}

int AVL_TreeNode::getHeight(){
    return (this == NULL ? 0 : this->height);
}

//checks if current Node has sick patient, if so, new record still in hospital
void AVL_TreeNode::numCurrentPatients(int *sickPeople)
{
    int currSick = *sickPeople;
    int newSick;
    AVL_TreeNode *currLeftChild, *currRightChild;
    LinkedList<Record*> *nodeRecordList;

    //getting all still sick people from this date
    nodeRecordList = this->getRecordList();
    newSick = nodeRecordList->numCurrentPatients();
    *sickPeople = currSick + newSick;

    if(this->hasLeftChild())
    {
        currLeftChild = this->getLeftChild();
        currLeftChild->numCurrentPatients(sickPeople);
    }

    if(this->hasRightChild())
    {
        currRightChild = this->getRightChild();
        currRightChild->numCurrentPatients(sickPeople);
    }
    return;
}

AVL_TreeNode* AVL_TreeNode::getLeftChild(){
    return this->leftChild;
}

AVL_TreeNode* AVL_TreeNode::getRightChild(){
    return this->rightChild;
}

void AVL_TreeNode::setHeight(int givenHeight){
    this->height = givenHeight;
}

void AVL_TreeNode::setLeftChild(AVL_TreeNode *givenChild){
    this->leftChild = givenChild;
}

void AVL_TreeNode::setRightChild(AVL_TreeNode *givenChild){
    this->rightChild = givenChild;
}

//balance factor is equal to the difference between
//heights of two children
void AVL_TreeNode::getBalanceFactor(int* balanceFactor){
    AVL_TreeNode *myLeftChild, *myRightChild;
    int leftHeight, rightHeight;

    myLeftChild = this->getLeftChild();
    myRightChild = this->getRightChild();

    leftHeight = calculateHeight(myLeftChild);
    rightHeight = calculateHeight(myRightChild);

    *(balanceFactor) = (leftHeight - rightHeight);
    return;
}


//printing the record that the node points to
void AVL_TreeNode::print(){
    AVL_TreeNode *myLeftChild, *myRightChild;
    LinkedList<Record*> *nodeRecordList;
    myLeftChild = this->getLeftChild();
    myRightChild = this->getRightChild();

    if(myLeftChild != NULL){
        myLeftChild->print();
    }

    nodeRecordList = this->getRecordList();
    nodeRecordList->print();

    if(myRightChild != NULL){
        myRightChild->print();
    }
    return;
}

//returns as parameter the amount of cases in given date range
void AVL_TreeNode::getCasesInRange(Date *startDate, Date *endDate, int *cases)
{
    int currCases = *cases;
    int newCases = 0;
    AVL_TreeNode *currLeftChild, *currRightChild;
    LinkedList<Record*> *nodeRecordList;

    //current node's date within given dates, both children may also be in
    if(this->inRange(startDate, endDate))
    {
        nodeRecordList = this->getRecordList();
        //getting the number of records in current date
        newCases = nodeRecordList->getLength();
        *cases = currCases + newCases;
        if(this->hasLeftChild())
        {
            currLeftChild = this->getLeftChild();
            currLeftChild->getCasesInRange(startDate, endDate, cases);
        }
        if(this->hasRightChild())
        {
            currRightChild = this->getRightChild();
            currRightChild->getCasesInRange(startDate, endDate, cases);
        }
        return;
    //current node under given dates, right child may be still in
    }else if(this->under(startDate))
    {
        if(this->hasRightChild())
        {
            currRightChild = this->getRightChild();
            currRightChild->getCasesInRange(startDate, endDate, cases);
        }
        return;
    }else//current node over given dates, left child may still be in
    {
        if(this->hasLeftChild())
        {
            currLeftChild = this->getLeftChild();
            currLeftChild->getCasesInRange(startDate, endDate, cases);
        }
        return;
    }
    return;
}

//returns the number of instances of given string (country/disease)
//using identifier variable (0 = disease, 1 = country)
//if no dates are given (NULL, NULL), checking all nodes
void AVL_TreeNode::getCasesForString(Date *startDate, Date *endDate, string *givenString, int *cases, int identifier)
{
    int currCases = *cases;
    int newCases = 0;
    AVL_TreeNode *currLeftChild, *currRightChild;
    Record *currRecord;
    string *compareString;
    LinkedList<Record*> *nodeRecordList;

    //no date limits have been given

    //current node's date within given dates, both children may also be in
    if(this->inRange(startDate, endDate))
    {
        nodeRecordList = this->getRecordList();
        //traversing node's list for instances of given string
        newCases = nodeRecordList->getCasesForString(givenString, identifier);

        *cases = currCases + newCases;

        if(this->hasLeftChild())
        {
            currLeftChild = this->getLeftChild();
            currLeftChild->getCasesForString(startDate, endDate, givenString, cases, identifier);
        }
        if(this->hasRightChild())
        {
            currRightChild = this->getRightChild();
            currRightChild->getCasesForString(startDate, endDate, givenString, cases, identifier);
        }
        return;
    //current node under given dates, right child may be still in
    }else if(this->under(startDate))
    {
        if(this->hasRightChild())
        {
            currRightChild = this->getRightChild();
            currRightChild->getCasesForString(startDate, endDate, givenString, cases, identifier);
        }
        return;
    }else//current node over given dates, left child may still be in
    {
        if(this->hasLeftChild())
        {
            currLeftChild = this->getLeftChild();
            currLeftChild->getCasesForString(startDate, endDate, givenString, cases, identifier);
        }
        return;
    }
    return;
}

void AVL_TreeNode::setRecordList(LinkedList<Record*> *givenList)
{
    this->recordList = givenList;
}

void AVL_TreeNode::push(Record *givenRecord)
{
    LinkedList<Record*> *nodeRecordList;
    nodeRecordList = this->getRecordList();

    nodeRecordList->push(givenRecord);
    return;
}

//returns the list of records corresponding to the given date
void AVL_TreeNode::getRecordsInDate(Date *givenDate, LinkedList<Record*> **recordList)
{
    AVL_TreeNode *nextChild;
    //found the node corresponding to given date
    if(*(this->getNodeDate()) == *givenDate)
    {
        *(recordList) = this->getRecordList();
        return;
    }else if(this->over(givenDate))
    {
        //should check the left subtree
        if(this->hasLeftChild())
        {
            nextChild = this->getLeftChild();
            nextChild->getRecordsInDate(givenDate, recordList);
        }else
        {
            recordList = NULL;
            return;
        }     
    }else
    {
        if(this->hasRightChild())
        {
            nextChild = this->getRightChild();
            nextChild->getRecordsInDate(givenDate, recordList);
        }else
        {
            recordList = NULL;
            return;
        }   
    }
    return;
}

//fills the group age array with number of cases for given disease
void AVL_TreeNode::getAgeGroupCases(string *disease, int *groupAgesCases, Date *entryDate, Date *exitDate)
{
    LinkedList<Record*> *nodeRecordList;
    AVL_TreeNode *currLeftChild = this->getLeftChild();
    AVL_TreeNode *currRightChild = this->getRightChild();

    //current node's date within given dates, both children may also be in
    if(this->inRange(entryDate, exitDate))
    {
        nodeRecordList = this->getRecordList();
        //getting the number of records in current date
        nodeRecordList->fillGroupStats(disease, groupAgesCases);

        if(this->hasLeftChild())
        {
            currLeftChild = this->getLeftChild();
            currLeftChild->getAgeGroupCases(disease, groupAgesCases, entryDate, exitDate);
        }
        if(this->hasRightChild())
        {
            currRightChild = this->getRightChild();
            currRightChild->getAgeGroupCases(disease, groupAgesCases, entryDate, exitDate);
        }
        return;
    //current node under given dates, right child may be still in
    }else if(this->under(entryDate))
    {
        if(this->hasRightChild())
        {
            currRightChild = this->getRightChild();
            currRightChild->getAgeGroupCases(disease, groupAgesCases, entryDate, exitDate);
        }
        return;
    }else//current node over given dates, left child may still be in
    {
        if(this->hasLeftChild())
        {
            currLeftChild = this->getLeftChild();
            currLeftChild->getAgeGroupCases(disease, groupAgesCases, entryDate, exitDate);
        }
        return;
    }
    return;
}

int calculateHeight(AVL_TreeNode* givenNode){
    //no node, got to the leaf level
    if(givenNode == NULL){
        return 0;
    }else{
        return givenNode->getHeight();
    }
}

bool AVL_TreeNode::hasChildren(){
    return (this->getLeftChild() != NULL || this->getRightChild() != NULL);
}

bool AVL_TreeNode::hasLeftChild()
{
    return (this->getLeftChild() != NULL);
}

bool AVL_TreeNode::hasRightChild()
{
    return (this->getRightChild() != NULL);
}

//checks if contained record has smaller insertion date than the given one
bool AVL_TreeNode::under(Date *givenDate)
{
    Date *currDate;
    currDate = this->getNodeDate();

    return (*currDate < *givenDate);
}

//checks if contained record has smaller insertion date than the given one
bool AVL_TreeNode::over(Date *givenDate)
{
    Date *currDate;
    currDate = this->getNodeDate();

    return (*currDate > *givenDate);
}

bool AVL_TreeNode::equal(Date *givenDate)
{
    Date *currDate;
    currDate = this->getNodeDate();

    return (*currDate == *givenDate);
}

//checks if current node record has entry date within given dates
//if no dates were given, we are in range
bool AVL_TreeNode::inRange(Date *startDate, Date *endDate)
{
    if(startDate == NULL || endDate == NULL)
    {
        return true;
    }
    return ((this->over(startDate) || this->equal(startDate)) && (this->under(endDate) || this->equal(endDate)));
}

LinkedList<Record*>* AVL_TreeNode::getRecordList()
{
    return this->recordList;
}

//returns the first record in node's list
Record* AVL_TreeNode::getFirstElement()
{
    LinkedList<Record*> *nodeRecordList;
    LinkedListNode<Record*> *recordListHead;

    nodeRecordList = this->getRecordList();
    recordListHead = nodeRecordList->getListHead();

    return (recordListHead->getItem());
}

//returns the date coresponding to the given avl tree node
Date* AVL_TreeNode::getNodeDate()
{
    Record *nodeFirstElement;

    nodeFirstElement = this->getFirstElement();
    return (nodeFirstElement->getEntryDate());
}

//current node moved left
//right node becoming its parents
AVL_TreeNode* AVL_TreeNode::rotateLeft(){
    int h1, h2, h3, h4;
    AVL_TreeNode* newParent = this->getRightChild();
    AVL_TreeNode* brokenNode = newParent->getLeftChild();


    //current node, becomes the left child of its previously right child
    newParent->setLeftChild(this);
    //broken node, becomes the left child of current node
    this->setRightChild(brokenNode);

    //group rebalanced, updating heights
    h1 = calculateHeight(this->getLeftChild());
    h2 = calculateHeight(this->getRightChild());
    //calculating the height of current node
    this->setHeight(max(h1, h2) + 1);

    h3 = calculateHeight(newParent->getRightChild());
    h4 = calculateHeight(newParent->getLeftChild());
    //calculating the height of the new parent
    newParent->setHeight(max(h3, h4) + 1);

    //returning the new parent node to the upper level
    return newParent;
}

//left child of current node becomes parent
//current child becomes right child
//previous left child remains left child
AVL_TreeNode* AVL_TreeNode::rotateRight(){
    int h1, h2, h3, h4;
    AVL_TreeNode* newParent = this->getLeftChild();
    AVL_TreeNode* brokenNode = newParent->getRightChild();

    //current node, becomes the right child of its previously left child
    newParent->setRightChild(this);
    //broken node, becomes the left child of current node
    this->setLeftChild(brokenNode);

    //group rebalanced, updating heights
    h1 = calculateHeight(this->getLeftChild());
    h2 = calculateHeight(this->getRightChild());
    //calculating the height of current node
    this->setHeight(max(h1, h2) + 1);

    h3 = calculateHeight(newParent->getLeftChild());
    h4 = calculateHeight(newParent->getRightChild());
    //calculating the height of the new parent
    newParent->setHeight(max(h3, h4) + 1);

    //returning the new parent node to the upper level
    return newParent;
}

//executes all the needed rotations, dependent on balance factor
//returns the new node that will be returned to the upper level
AVL_TreeNode* AVL_TreeNode::updateParent(int balanceFactor, Record* newRecord){
    AVL_TreeNode *currLeftChild, *currRightChild, *tmpNode;
    Date *leftChildDate, *rightChildDate, *newRecordDate;

    newRecordDate = newRecord->getEntryDate();

    if(balanceFactor > 1)
    {
        currLeftChild = this->getLeftChild();
        leftChildDate = currLeftChild->getNodeDate();

        //LL CASE (More nodes on left, new node smaller than left child's child)
        if(*newRecordDate < *leftChildDate)
        {
            return this->rotateRight();
        }else//LR CASE (More nodes on left, new node bigger than left child's child)
        {
            tmpNode = currLeftChild->rotateLeft();
            this->setLeftChild(tmpNode);
            this->setHeight(1 + max(this->getLeftChild()->getHeight(), this->getRightChild()->getHeight()));
            return this->rotateRight();
        }
    }

    if(balanceFactor < -1)
    {
        currRightChild = this->getRightChild();
        rightChildDate = currRightChild->getNodeDate();

        //RL CASE (More nodes on right, new node smaller than right child's child)
        if(*newRecordDate < *rightChildDate)
        {
            tmpNode = currRightChild->rotateRight();
            this->setRightChild(tmpNode);
            this->setHeight(1 + max(this->getLeftChild()->getHeight(), this->getRightChild()->getHeight()));
            return this->rotateLeft();
        }else//RR CASE (More nodes on right, new node bigger than right child's child)
        {
            return this->rotateLeft();
        }

    }
    //both subtrees are balanced, just return yourself
    return this;
}


AVL_TreeNode* AVL_TreeNode::insert(Record* newRecord){
    int balanceFactor; //used to check whether current node is balanced or not
    AVL_TreeNode *newChild, *myLeftChild, *myRightChild, *newParent,
                 *parentLeftChild, *parentRightChild;
    myLeftChild = this->getLeftChild();
    myRightChild = this->getRightChild();


    //found a node with date equal to new record's one, store it here
    if(*(newRecord->getEntryDate()) == *(this->getNodeDate()))
    {
        this->push(newRecord);
        return this;
    }

    //given record entry date smaller/equal, should be added to
    //the left subtree
    if(*(newRecord->getEntryDate()) < *(this->getNodeDate())){
        //no left child
        if(myLeftChild == NULL){
            newChild = new AVL_TreeNode(newRecord);
            //no previous children, height should be increamented
            if(!this->hasChildren()){
                this->setHeight(this->getHeight() + 1);
            }else{
                this->setHeight(myRightChild->getHeight() + 1);
            }
            this->setLeftChild(newChild);
            //no rebalancing here, just return pre-last node
            return this;
        }else{
            //left recursive call, update left child
            newChild = myLeftChild->insert(newRecord);
            this->setLeftChild(newChild);
            this->setHeight(1 + max(calculateHeight(this->getLeftChild()), calculateHeight(this->getRightChild())));
        }
    }else{
        //no right child
        if(myRightChild == NULL){
            newChild = new AVL_TreeNode(newRecord);
            //no previous children, height should be increamented
            if(!this->hasChildren()){
                this->setHeight(this->getHeight() + 1);
            }else{
                this->setHeight(myLeftChild->getHeight() + 1);
            }
            this->setRightChild(newChild);
            //no rebalancing here, just return pre-last node
            return this;
        }else{
            //right recursive call, update right child
            newChild = myRightChild->insert(newRecord);
            this->setRightChild(newChild);
            this->setHeight(1 + max(calculateHeight(this->getLeftChild()), calculateHeight(this->getRightChild())));
        }
    }

    this->getBalanceFactor(&balanceFactor);
    //calculating the node that will be connected with the upper level
    newParent = this->updateParent(balanceFactor, newRecord);
    //new node inserted into subtree, increamenting height
    parentLeftChild = newParent->getLeftChild();
    parentRightChild = newParent->getRightChild();
    newParent->setHeight(1 + max(calculateHeight(parentLeftChild), calculateHeight(parentRightChild)));
    return newParent;
}
