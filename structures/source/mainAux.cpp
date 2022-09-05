#include "../headers/mainAux.h"
using namespace std;

void getArgument(string identifier, int numOfArguments, char **arguments, string **argumentString){

    for(int i = 0; i < numOfArguments - 1; i++){
        if(identifier == arguments[i]){
            *argumentString = new string(arguments[i + 1]);
            break;
        }
    }
    return;
}

void recordErrorPrint(string givenError, Record *givenRecord)
{
    if(givenError == "Record id repetition!")
    {
        string recordID = *givenRecord->getRecordID();
        cout << "Record with id " << recordID << " already in file!" << endl;
        return;
    }

    if(givenError == "Record with invalid dates!")
    {
        string recordID = *givenRecord->getRecordID();
        cout << "Record with id " << recordID << " has invalid dates!" << endl;
    }

    return;
}

//TO DO: free record if bad id
void insertToDatabase(HashTable *id_HT, HashTable *disID_HT , HashTable *cntID_HT, HashTable *dis_HT, HashTable *cnt_HT, LinkedList<Record*> *recordList, string *line)
{
    Record *newRecord;
    string givenID, givenDisease, givenCountry, givenEntryDate, givenExitDate;
    string *newID, *newDisease, *newCountry;

    //check given data
    givenID = getIthWord(line, 1);
    givenDisease = getIthWord(line, 4);
    givenCountry = getIthWord(line, 5);

    //entry date bigger than the exit date
    //entry and exit dates are the 6th and 7th argument
    if(invalidDates(getIthWord(line, 6), getIthWord(line, 7)))
    {
        return;
    }



    if(!id_HT->containsIdentifier(&givenID))
    {
        //get name, surname and
        newRecord = new Record(*line);

        //updating ids hash and new record
        newID = new string(givenID);
        id_HT->insertIdentifier(newID);
        newRecord->setRecordID(newID);

        //new disease?
        if(!disID_HT->containsIdentifier(&givenDisease))
        {
            newDisease = new string(givenDisease);
            disID_HT->insertIdentifier(newDisease);
            newRecord->setDiseaseID(newDisease);
            //new pair of disease and data tree
            dis_HT->insert(newRecord, new string(givenDisease));
        }else//disease already in database, update entry with pointer to its string
        {
            newRecord->setDiseaseID(disID_HT->getStringPointer(&givenDisease));
            //no new string allocation, just update disease tree
            dis_HT->insert(newRecord, &givenDisease);
        }

        //new country?
        if(!cntID_HT->containsIdentifier(&givenCountry))
        {
            newCountry = new string(givenCountry);
            cntID_HT->insertIdentifier(newCountry);
            newRecord->setCountry(newCountry);
            //new pair of country and data tree
            cnt_HT->insert(newRecord, new string(givenCountry));
        }else//country already in database, just get country string pointer
        {
            newRecord->setCountry(cntID_HT->getStringPointer(&givenCountry));
            //no new string allocation, just update country tree
            cnt_HT->insert(newRecord, &givenCountry);
        }

        //add new record to the record list
        recordList->push(newRecord);

    }else//already have record with given id
    {
        cout << endl << "Record with id " << givenID << " already in database!" << endl;
    }
    return;
}

void showAvailableQueries()
{
    cout << endl << "Invalid query name, available ones: " << endl;
    cout << endl << "/globalDiseaseStats [date1 date2]" << endl;
    cout << endl << "/diseaseFrequency virusName [country] date1 date2" << endl;
    cout << endl << "/topk-Diseases k country [date1 date2]" << endl;
    cout << endl << "/topk-Countries k disease [date1 date2]" << endl;
    cout << endl << "/insertPatientRecord recordID patientFirstName patientLastName diseaseID " <<
                    "country entryDate [exitDate]" << endl;
    cout <<  endl << "/recordPatientExit recordID exitDate" << endl;
    cout << endl << "/numCurrentPatients [disease]" << endl;
    cout << endl << "/printAllEntries" << endl;
    cout << endl << "/exit" << endl << endl;
    return;
}

void getQuery(string* line, string *query)
{
    *query = line->substr(0, line->find(" "));
    return;
}

//executing user queries till exit call
//id_HT = hashtable containing ids of all records
//disID_HT = hashtable containing names of diseases
//cntID_HT = hashtable containing names of countries
//dis_HT = hashtable containing data trees for diseases
//cnt_HT = hashtable containing data trees for countries
void getUserQueries(HashTable *id_HT, HashTable *disID_HT , HashTable *cntID_HT, HashTable *dis_HT, HashTable *cnt_HT, LinkedList<Record*> *recordList)
{
    string *line = new string();
    string *query = new string();
    //counts the number of given arguments
    int arguments = 0;

    //getting input from user
    while(getline(cin, *line))
    {
        getQuery(line, query);
        arguments = getNumOfArguments(line);

        if(*query == "/globalDiseaseStats")
        {
            if(arguments == 1 || arguments == 3)
            {
                globalDiseaseStats(disID_HT, dis_HT, line, arguments);
            }else
            {
                cout << "error" << endl;
            }

        }else if(*query == "/diseaseFrequency")
        {
            if(arguments == 4 || arguments == 5)
            {
                diseaseFrequency(disID_HT, cntID_HT, dis_HT, cnt_HT, line, arguments);
            }else
            {
                cout << "error" << endl;
            }

        }else if(*query == "/topk-Diseases")
        {
            if(arguments == 3 || arguments == 5)
            {
                topKDiseases(cntID_HT, disID_HT, cnt_HT, line, arguments);
            }else
            {
                cout << "error" << endl;
            }

        }else if(*query == "/topk-Countries")
        {
            if(arguments == 3 || arguments == 5)
            {
                topKCountries(disID_HT, cntID_HT, dis_HT, line, arguments);
            }else
            {
                cout << "error" << endl;
            }

        }else if(*query == "/insertPatientRecord")
        {
            if(arguments == 7 || arguments == 8)
            {
                insertPatientRecord(id_HT, disID_HT , cntID_HT, dis_HT, cnt_HT, recordList, line, arguments);
            }else
            {
                cout << "error" << endl;
            }

        }else if(*query == "/recordPatientExit")
        {
            //giving patient with given id new exit date
            if(arguments == 3)
            {
                recordPatientExit(id_HT, recordList, line);
            }else
            {
                cout << "error" << endl;
            }

        }else if(*query == "/numCurrentPatients")
        {
            if(arguments == 1 || arguments == 2)
            {
                numCurrentPatients(disID_HT, dis_HT, line, arguments);
            }else
            {
                cout << "error" << endl;
            }

        }else if(*query == "/printAllEntries")
        {
            recordList->print();

        }else if(*query == "/exit")
        {
            freeDataStructures(&id_HT, &disID_HT, &cntID_HT, &dis_HT, &cnt_HT, &recordList);
            //stop taking input, delete query, line string classes
            delete query;
            delete line;
            cout << "exiting" << endl;
            return;
        }
        else//invalid query name from user, time for some suggestions
        {
            showAvailableQueries();
        }
        //for pretty print purposes
    }
}

//TO DO: free string, currHead, dates
void globalDiseaseStats(HashTable *disID_HT, HashTable *dis_HT, string *line, int arguments)
{
    //list containing all diseases names
    LinkedList<string*> *diseaseList;
    LinkedListNode<string*> *currHead;
    string *diseaseName;
    //tree contains all records for given disease
    AVL_Tree *diseaseTree;
    //strings representing the dates
    string stringDate1, stringDate2;
    Date *Date1, *Date2;
    //total cases in specific time period
    int *periodCases = (int*)malloc(sizeof(int));


    diseaseList = new LinkedList<string*>;
    //getting all possible disease names
    disID_HT->getAllStrings(diseaseList);

    //simply print the number of cases for each disease
    if(arguments == 1)
    {
        while(!diseaseList->isEmpty())
        {
            currHead = diseaseList->pop();
            diseaseName = currHead->getItem();

            //getting the tree with cases of given disease
            diseaseTree = dis_HT->getStringFile(diseaseName);
            cout << *diseaseName << " " << diseaseTree->getNumOfElements() << endl;
            //each popped node won't be used anywhere else, delete
            delete currHead;
        }

    }else
    {
        //produce date class instance, for comparison within the disease tress
        stringDate1 = getIthWord(line, 2);
        stringDate2 = getIthWord(line, 3);
        Date1 = new Date(stringDate1);
        Date2 = new Date(stringDate2);

        while(!diseaseList->isEmpty())
        {
            *periodCases = 0;
            currHead = diseaseList->pop();
            diseaseName = currHead->getItem();
            //getting the tree with disease records
            diseaseTree = dis_HT->getStringFile(diseaseName);
            //traverse the tree to get # records in range
            diseaseTree->getCasesInRange(Date1, Date2, periodCases);
            cout << *diseaseName << " " << *periodCases << endl;
            //popped nodes deleted, no use for them
            delete currHead;
        }
        //delete contemporary search dates
        delete Date1;
        delete Date2;
    }
    //free all the contemporary items' space
    free(periodCases);
    delete diseaseList;
    return;
}

//TO DO: free dates, cases
//if country is given, prints the number of cases of given disease in it in a date range
void diseaseFrequency(HashTable *disID_HT, HashTable *cntID_HT, HashTable *dis_HT, HashTable *cnt_HT, string *line, int arguments)
{
    Date *Date1, *Date2;
    string stringDate1, stringDate2, givenCountry, givenDisease;
    AVL_Tree *dataTree;
    int *cases = (int *)malloc(sizeof(int));
    //initialized to 0
    *cases = 0;

    givenDisease = getIthWord(line, 2);

    //checking if given disease actually exists in our database
    if(!disID_HT->containsIdentifier(&givenDisease))
    {
        cout << endl << "error" << endl;
        free(cases);
        return;
    }

    //only printing cases for given disease in date range
    if(arguments == 4)
    {
        stringDate1 = getIthWord(line, 3);
        stringDate2 = getIthWord(line, 4);
        Date1 = new Date(stringDate1);
        Date2 = new Date(stringDate2);

        dataTree = dis_HT->getStringFile(&givenDisease);
        dataTree->getCasesInRange(Date1, Date2, cases);
        cout << givenDisease << " " << *cases << endl;
    }else//printing number of cases of given virus in given country
    {
        stringDate1 = getIthWord(line, 3);
        stringDate2 = getIthWord(line, 4);
        givenCountry = getIthWord(line, 5);
        Date1 = new Date(stringDate1);
        Date2 = new Date(stringDate2);

        //checking if given country exists in our database
        if(!cntID_HT->containsIdentifier(&givenCountry))
        {
            cout << "error" << endl;
            delete Date1;
            delete Date2;
            free(cases);
            return;
        }

        //get country tree, check how many cases of given disease in given dates
        dataTree = cnt_HT->getStringFile(&givenCountry);
        dataTree->getCasesForString(Date1, Date2, &givenDisease, cases, 0);
        //pretty print
        cout << givenDisease << " " << *cases << endl;
    }
    //free Dates and cases pointer
    delete Date1;
    delete Date2;
    free(cases);
    return;
}

//TO DO: DONE?
//general function, executes topKDiseases, topKCountries dependent on last parameter
//first parameter is the hashtable of the item we want to find top k elements for
void topKQuery(HashTable *itemID_HT, HashTable *elemID_HT, HashTable *given_HT, string *line, int arguments, int countryQuery)
{
    string *givenItem = new string();
    string stringDate1, stringDate2, numOfItemsString;
    int searchIdentifier;
    //user may give date limits
    Date *Date1, *Date2;

    //represents the k given by the user
    int numOfItems;
    //used to get the top k diseases
    MaxHeap *myHeap = new MaxHeap();
    //used to store all diseases
    LinkedList<string*> *elementsList = new LinkedList<string*>();

    //top K countries query
    if(countryQuery)
    {
        searchIdentifier = 0;
    }else
    {
        searchIdentifier = 1;
    }

    //getting the k and country arguments
    numOfItemsString = getIthWord(line, 2);
    numOfItems = stoi(numOfItemsString);
    *givenItem = getIthWord(line, 3);

    //checking if given item exists in our database
    if(!itemID_HT->containsIdentifier(givenItem))
    {
        cout << "error" << endl;
        delete givenItem;
        delete myHeap;
        delete elementsList;
        return;
    }

    elemID_HT->getAllStrings(elementsList);

    //just print top k diseases for given country
    if(arguments == 3)
    {
        //for each disease in list, check how many instances for given country
        //put result into the given heap
        initializeHeap(given_HT, myHeap, elementsList, NULL, NULL, givenItem, searchIdentifier);
        //get the top numOfDiseases diseases for given country
        topKPrint(myHeap, numOfItems);

    }else//search for top k diseases but in given dates
    {

        stringDate1 = getIthWord(line, 4);
        stringDate2 = getIthWord(line, 5);
        Date1 = new Date(stringDate1);
        Date2 = new Date(stringDate2);

        //for each disease check the number of instances in given dates for the country
        //put result into a heap
        initializeHeap(given_HT, myHeap, elementsList, Date1, Date2, givenItem, searchIdentifier);
        topKPrint(myHeap, numOfItems);
        //temporary comparison dates can be freed as well
        delete Date1;
        delete Date2;
    }
    //popped all the needed items from heap, we can free its memory now
    delete myHeap;
    //query string can be freed now
    delete givenItem;
    //temporary elements list can be deleted
    delete elementsList;
    return;
}

//returns top k diseases in regard to cases for given country
void topKDiseases(HashTable *cntID_HT, HashTable *disID_HT, HashTable *cnt_HT, string *givenCountry, int arguments)
{
    //last argument implies we are looking for country's top diseases
    topKQuery(cntID_HT, disID_HT, cnt_HT, givenCountry, arguments, 1);
    return;
}

//returns top k countries in regard to cases for given disease
void topKCountries(HashTable *disID_HT, HashTable *cntID_HT, HashTable *dis_HT, string *givenDisease, int arguments)
{
    //last argument implies we are looking for disease's top countries
    topKQuery(disID_HT, cntID_HT, dis_HT, givenDisease, arguments, 0);
    return;
}

//TO DO: free heap nodes
//prints the top k elements of given heap
void topKPrint(MaxHeap *givenHeap, int k)
{
    int foundItems = 0;
    int wantedItems = k;
    HeapNode *givenHeapNode;
    HeapEntry *givenHeapEntry;

    //printing till all wanted diseases displayed or heap is empty
    while(wantedItems != 0 && !givenHeap->isEmpty())
    {
        foundItems++;
        wantedItems--;

        givenHeapNode = givenHeap->pop();
        givenHeapEntry = givenHeapNode->getData();

        //printing the ith element
        cout << *(givenHeapEntry->getString()) << " " << givenHeapEntry->getInstances() << endl;
        //deleting each node extracted from heap, we won't need them anymore
        //node contains entry (it gets deleted as well)
        delete givenHeapNode;
    }
    return;
}

//for each element of given list (disease/country)
//checks how many instances of them exist in given string tree
//tree is taken from given hash table
//given dates pose limits on the records dates
//identifier specifies if elements are (diseases = 0, countries = 1)
//frees space of the temporary list
void initializeHeap(HashTable *givenHashTable, MaxHeap *givenHeap, LinkedList<string*> *givenList, Date *startDate, Date *endDate, string *givenString, int identifier)
{
    int instances = 0;
    LinkedListNode<string*> *givenListNode;
    string *currElement;
    //we copy the given string, as we want it deallocated later
    string *currElementInstance;
    AVL_Tree *givenStringTree;
    HeapEntry *myHeapEntry;

    while(!givenList->isEmpty())
    {
        //initializing instances for each element of the given list
        instances = 0;
        givenListNode = givenList->pop();
        currElement = givenListNode->getItem();
        //allocating new string for heap
        currElementInstance = new string(*currElement);

        //getting the tree of the given string
        //easily accessed by the given hashtable
        givenStringTree = givenHashTable->getStringFile(givenString);
        //checking how many instances of current element does given string tree have
        givenStringTree->getCasesForString(startDate, endDate, currElement, &instances, identifier);

        //initializing a heap entry [element, instances]
        //pushing it into the given heap
        myHeapEntry = new HeapEntry(currElementInstance, instances);
        givenHeap->push(myHeapEntry);

        //free list elements that have been copied into the heap
        //node contains currElement
        delete givenListNode;
    }
    return;
}

//TO DO: free string
void insertPatientRecord(HashTable *id_HT, HashTable *disID_HT, HashTable *cntID_HT, HashTable *dis_HT, HashTable *cnt_HT, LinkedList<Record*> *recordList, string *line, int arguments)
{
    string *dataString;

    dataString = new string(line->substr(line->find(' ') + 1));
    //missing exit date, set it as '-', necessary for record initialization
    if(arguments == 7)
    {
        dataString->append(" -");
    }

    //simply insert the record into the database (proper checks within the function)
    insertToDatabase(id_HT, disID_HT, cntID_HT, dis_HT, cnt_HT, recordList, dataString);
    cout << "Record added" << endl;
    //data string initialized new entry, can be deleted
    delete dataString;
    return;
}

//extracting new exit date from given line
//updating patient with given id
void recordPatientExit(HashTable *id_HT, LinkedList<Record*> *recordList, string *line)
{
    string givenID, newExitDate;
    Record *givenRecord;
    Date *updatedExitDate;

    givenID = getIthWord(line, 2);
    newExitDate = getIthWord(line, 3);

    //if exists a patient with given id
    if(!id_HT->containsIdentifier(&givenID))
    {
        cout << "Not found" << endl;
        return;
    }else
    {
        givenRecord = recordList->getRecordWithID(&givenID);
        //exit date updated to "still in hospital"
        if(newExitDate == "-")
        {
            givenRecord->updateExitDate(NULL);
        }else
        {
            givenRecord->updateExitDate(&newExitDate);
        }
    }
    cout << "Record updated" << endl;
    return;
}

//TO DO: DONE?
void numCurrentPatients(HashTable *disID_HT, HashTable *dis_HT, string *line, int arguments)
{
    LinkedList<string*> *diseaseList;
    LinkedListNode<string*> *diseaseListNode;
    AVL_Tree *diseaseTree;
    string *diseaseName;
    string givenDisease;
    int sickPeople;

    //no specific disease given, print for every disease in our database
    if(arguments == 1)
    {
        diseaseList = new LinkedList<string*>();
        //get all disease names
        disID_HT->getAllStrings(diseaseList);

        while(!diseaseList->isEmpty())
        {
            sickPeople = 0;
            diseaseListNode = diseaseList->pop();
            diseaseName = diseaseListNode->getItem();
            diseaseTree = dis_HT->getStringFile(diseaseName);
            diseaseTree->numCurrentPatients(&sickPeople);

            cout << *diseaseName << " " << sickPeople << endl;

            //free the content of the contemporary disease list
            delete diseaseListNode;
        }
        //got all disease names, list can be destroyed
        delete diseaseList;

    }else//specific disease given, print the number of patients still suffering
    {
        sickPeople = 0;
        givenDisease = getIthWord(line, 2);
        if(!disID_HT->containsIdentifier(&givenDisease))
        {
            cout << givenDisease << " " << 0 << endl;
            return;
        }

        diseaseTree = dis_HT->getStringFile(&givenDisease);
        diseaseTree->numCurrentPatients(&sickPeople);
        cout << givenDisease << " " << sickPeople << endl;
    }
    return;
}

//returns the number of arguments in the given line
//starts with 1 (we made sure we were given the query name)
int getNumOfArguments(string *givenString)
{
    int arguments = 1;

    for(int index = 0; index < givenString->length(); index++)
    {
        if(givenString->at(index) == ' ')
        {
            arguments++;
        }
    }
    return arguments;
}

//checks if given dates are invalid
bool invalidDates(string stringDate1, string stringDate2)
{
    Date *Date1, *Date2;

    //no exit date, entry date always smaller
    if(stringDate2 == "-")
    {
        return false;
    }

    Date1 = new Date(stringDate1);
    Date2 = new Date(stringDate2);

    if(*Date1 > *Date2)
    {
        cout << endl << "Entry date bigger than exit date, invalid input!" << endl;
        delete Date1;
        delete Date2;
        return true;
    }else
    {
        delete Date1;
        delete Date2;
        return false;
    }
}



//COUNTING STARTS FROM 1!
string getIthWord(string *givenString, int index)
{
    string tempString;
    istringstream iss (*givenString);
    while(index-- > 0 && (iss >> tempString));
    return tempString;
}

//deleting all the data structures we have constucted for our database
void freeDataStructures(HashTable **id_HT, HashTable **disID_HT, HashTable **cntID_HT, HashTable **dis_HT, HashTable **cnt_HT, LinkedList<Record*> **recordList)
{
    //deleting all the records
    delete *recordList;
    //delete all the used hashtables
    delete *id_HT;
    delete *disID_HT;
    delete *cntID_HT;
    delete *dis_HT;
    delete *cnt_HT;
    return;
}
