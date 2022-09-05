#include <iostream>
#include <cstdlib>
#include "../headers/mainAux.h"
using namespace std;

int main(int argc, char **argv){
    string *patientRecordFile, *dHNOEString,
           *cHNOEString, *bSString;
    int diseaseHashtableNumOfEntries, countryHashtableNumOfEntries, bucketSize;

    if(argc != 9)
    {
        cout << "Wrong number of arguments!" << endl;
        return -1;
    }

    getArgument("-p", argc, argv, &patientRecordFile);
    getArgument("-h1", argc, argv, &dHNOEString);
    getArgument("-h2", argc, argv, &cHNOEString);
    getArgument("-b", argc, argv, &bSString);

    diseaseHashtableNumOfEntries = stoi(*dHNOEString);
    countryHashtableNumOfEntries = stoi(*cHNOEString);
    bucketSize = stoi(*bSString);

    ifstream file(*patientRecordFile);
    string line;

    //DATABASE
    HashTable *idHashTable = new HashTable(bucketSize, 5);
    HashTable *diseaseIDHashTable = new HashTable(bucketSize, 5);
    HashTable *countryIDHashTable = new HashTable(bucketSize, 5);
    LinkedList<Record*> *recordList = new LinkedList<Record*>;

    HashTable *diseaseHashTable = new HashTable(bucketSize, diseaseHashtableNumOfEntries);
    HashTable *countryHashTable = new HashTable(bucketSize, countryHashtableNumOfEntries);

    if(file.is_open())
    {
        while(getline(file, line))
        {
            //producing record with given data, checking for correctness, inserting to database
            insertToDatabase(idHashTable, diseaseIDHashTable, countryIDHashTable, diseaseHashTable, countryHashTable, recordList, &line);
        }
    }

    //executing given queries, till exit call
    getUserQueries(idHashTable, diseaseIDHashTable, countryIDHashTable, diseaseHashTable, countryHashTable, recordList);

    delete patientRecordFile;
    delete dHNOEString;
    delete cHNOEString;
    delete bSString;

    return 0;
}
