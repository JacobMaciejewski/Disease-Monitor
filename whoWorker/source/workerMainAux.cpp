#include "../headers/workerMainAux.h"

void getArguments(char **argv)
{
    string serverPortStr;
    readPipe = new string(argv[2]);
    writePipe = new string(argv[3]);
    dataDirectory = string(argv[4]);
    bufferSize = atoi(argv[1]);
    serverIP = new string(argv[5]);
    serverPortStr = string(argv[6]);
    serverPort = atoi(serverPortStr.c_str());

    return;
}

//initializing the main hash tables and record list
void initializeDataStructures(int bucketSize, int tableSize)
{
    id_HT = new HashTable(bucketSize, tableSize);
    disID_HT = new HashTable(bucketSize, tableSize);
    cntID_HT = new HashTable(bucketSize, tableSize);
    dis_HT = new HashTable(bucketSize, tableSize);
    cnt_HT = new HashTable(bucketSize, tableSize); 
    recordList = new LinkedList<Record*>;
    return;
}


//initializes filedescriptor array
//opens pipes for communication
void establishCommunication()
{
    int writeFD, readFD;
    writeFD = open(writePipe->c_str(), O_WRONLY);
    readFD = open(readPipe->c_str(), O_RDONLY);

    int id = getpid();
    WR_FDs = (int*) malloc(2 * sizeof(int));

    WR_FDs[0] = writeFD;
    WR_FDs[1] = readFD;
    return;
}

void receiveCountries(string *countries)
{
    string countryText;
    int fileDesc = WR_FDs[1];
    receiveText(bufferSize, fileDesc, &countryText);
    //no country received, just terminate the worker
    (*countries) = countryText;
    return;
}

void setSendingInfo(int *sendingInfo)
{
    sendingInfo[0] = bufferSize;
    sendingInfo[1] = WR_FDs[0];
    return;
}

//initializes hashtables corresponding to files of each country
//initially empty
void initFolderHash()
{
    fldr_HT = (HashTable **) malloc(countriesNum * sizeof(HashTable *));

    for(int index = 0; index < countriesNum; index++)
    {
        fldr_HT[index] = new HashTable(20, 5);
    }
    return;
}

//checks if given string corresponds to valid age [1-120]
bool validAge(string givenAge)
{
    int age = stoi(givenAge);

    return (age > 0 && age <= 120);
}

//checks if given string corresponds to valid state [ENTER, EXIT]
bool validState(string givenState)
{
    return (givenState == "ENTER" || givenState == "EXIT");
}

//checking if we have already traversed the file in current folder
bool checkedFile(int folderIndex, string filePath)
{
    HashTable *folderHT;
    folderHT = fldr_HT[folderIndex];

    return (folderHT->containsIdentifier(&filePath));
}


void getRecordParts(string *id, string *state, string *name, string *surname, string *disease, string *age, string *recordLine)
{
    *(id) = getIthWord(recordLine, 1);
    *(state) = getIthWord(recordLine, 2);
    *(name) = getIthWord(recordLine, 3);
    *(surname) = getIthWord(recordLine, 4);
    *(disease) = getIthWord(recordLine, 5);
    *(age) = getIthWord(recordLine, 6);
    return;
}

//takes the line of given record, checks its validity, constructs new record, inserts its data to proper data structures
void insertToDatabase(string recordCountry, string recordDate, string *recordLine)
{
    string recordID, recordState, recordName, recordSurname, recordDisease, recordAge;
    //getting parts of given record line
    getRecordParts(&recordID, &recordState, &recordName, &recordSurname, &recordDisease, &recordAge, recordLine);
    Record *newRecord, *existingRecord;
    //new record may be the exit state of an already existing one
    Date *exitDate;
    //record may add new country, disease and id to database
    string *newID, *newCountry, *newDisease;
    string *finalDisease, *finalCountry;
    string *HT_Country, *HT_Disease;

    //checking for age and state validity
    if(!validAge(recordAge) || !validState(recordState))
    {
        // cerr << "Error" << endl;
        return;
    }

    //no entry with given id in our database
    if(!id_HT->containsIdentifier(&recordID))
    {
        //got exit entry before entry, invalid
        if(recordState == "EXIT")
        {
            // cerr << "Error" << endl;
            return;
        }else
        {
            //new id added to ids hashtable
            newID = new string(recordID);
            id_HT->insertIdentifier(newID);

            //new disease added to our database
            if(!disID_HT->containsIdentifier(&recordDisease))
            {
                newDisease = new string(recordDisease);
                disID_HT->insertIdentifier(newDisease);
                finalDisease = newDisease;
                HT_Disease = new string(recordDisease);
            }else//just extract disease string from our hash
            {
                finalDisease = disID_HT->getStringPointer(&recordDisease);
                HT_Disease = &recordDisease;
            }

            //new country added to our database
            if(!cntID_HT->containsIdentifier(&recordCountry))
            {
                newCountry = new string(recordCountry);
                cntID_HT->insertIdentifier(newCountry);
                finalCountry = newCountry;
                HT_Country = new string(recordCountry);
            }else//country already in database, just get country string pointer
            {
                finalCountry = cntID_HT->getStringPointer(&recordCountry);
                HT_Country = &recordCountry;
            }

            newRecord = new Record(finalDisease, finalCountry, newID, recordName, recordSurname, recordAge, recordDate);
            //record stored in country and diseases avl trees
            dis_HT->insert(newRecord, HT_Disease);
            cnt_HT->insert(newRecord, HT_Country);
            //add record to the record list
            recordList->push(newRecord);
        }

    }else//entry instance already in database
    {
        if(recordState == "ENTER")//second instance of entrance, invalid
        {
            //cerr << "Error" << endl;
            return;
        }
        //check if exit state is valid and if so, update record exit date
        existingRecord = recordList->getRecordWithID(&recordID);
        exitDate = new Date(recordDate);

        if(*(existingRecord->getEntryDate()) > *(exitDate))
        {
            //ceroo << "Error" << endl;
            delete exitDate;
            return;
        }
        else
        {
            existingRecord->updateExitDate(&recordDate);
        }
        delete exitDate;    
    }
    return;
}

//returns a text containing number of cases for each group as seperate messages
string makeDiseaseStats(string *disease)
{
    int index, groupCases;
    int groupCounters[4];
    string stats, groupStats, cases;
    //stats start with the disease name
    stats = *disease + "\n";

    //initially no cases for each group
    for(index = 0; index < 4; index++)
    {
        groupCounters[index] = 0;
    }

    //traverse the records and get the number of cases for each group
    recordList->fillGroupStats(disease, groupCounters);

    for(index = 0; index < 4; index++)
    {
        groupCases = groupCounters[index];
        cases = to_string(groupCases);
        groupStats = cases + "\n";
        //add group cases to total message
        stats = stats + groupStats;
    }
    return stats;
}


//TO DO: delete string list
void sendStatistics(string countryName, string filePath)
{
    Date *currentFile;
    string fileName, totalStats;
    //name of disease recorded in this date
    string *diseaseName;
    string diseaseStats;
    //all records that were valid in this file (date)
    AVL_Tree *countryRecords;
    LinkedList<Record*> *recordList = NULL;
    LinkedList<string*> *diseasesInDate = new LinkedList<string*>;
    LinkedListNode<string*> *diseaseNode;
    //getting the write FD
    int fileDesc = WR_FDs[0];

    //constructing a date corresponding to file name
    fileName = getDate(filePath);
    currentFile = new Date(fileName);
    //statistics always start with date and country name
    totalStats = fileName + "\n" + countryName + "\n";

    countryRecords = cnt_HT->getStringFile(&countryName);
    //getting the list containing all records for given date (file)
    countryRecords->getRecordsInDate(currentFile, &recordList);

    //no entries in the given date, just return empty stats
    if(recordList == NULL)
    {
        sendText(SOCKET_BUFFER_SIZE, fileDesc, totalStats);
        delete currentFile;
        delete diseasesInDate;
        return;
    }

    //getting the list containing all diseases recorded in given date
    recordList->getStringList(diseasesInDate, 1);

    //for each disease recorded in given date
    while((diseaseNode = diseasesInDate->pop()) != NULL)
    {
        diseaseName = diseaseNode->getItem();
        diseaseStats = makeDiseaseStats(diseaseName);
        totalStats = totalStats + diseaseStats;
        delete diseaseNode;
        delete diseaseName;
    }
    sendText(SOCKET_BUFFER_SIZE, fileDesc, totalStats);

    delete diseasesInDate;
    delete currentFile;
}


//for each entry of the given file, check its validity and insert to proper data structures
//when whole file traversed, send statistics to parent process
void fileToDatabase(int countryIndex, string countryName, string fileName)
{
    ifstream file(fileName);
    string line;
    //date is equal to the name of the file without the path
    string date;

    date = getDate(fileName);

    if(file.is_open())
    {
        while(getline(file, line))
        { 
            //producing record with given data, checking for correctness, inserting to database
            insertToDatabase(countryName, date, &line);
        }
    }
    //send statistics of current file to the parent process
    sendStatistics(countryName, fileName);
    return;
}


//returns an array of countries strings which workers takes care of
void getRowsInArray(string countryMessage)
{
    int rowsNum = numOfRows(countryMessage);
    int index = 0;
    string currentCountry;
    countriesArray = (string**) malloc(sizeof(string*) * rowsNum);

    for(int index = 0; index < rowsNum; index++)
    {
        popMessageAsString(&countryMessage, &currentCountry);
        countriesArray[index] = new string(currentCountry);
    }
    countriesNum = rowsNum;
    return;
}

//  comparison function which returns ​a negative integer value if the first argument is less than the second,
//   a positive integer value if the first argument is greater than the second and zero if the arguments are equal.


int cmpfunc (const void *Date1, const void *Date2)
{
   if( *(Date*)Date1 < *(Date*)Date2 )
   {
       return -1;
   }else if( *(Date*)Date1 > *(Date*)Date2 )
   {
       return 1;
   }else
   {
       return 0;
   } 
}

//traverses the files of the given directory, inserting their valid content to given data structures
//calls file to database, where statistics message is sent to parent process
void dirToDatabase(int countryIndex, string countryName, string dirPath)
{
    DIR *currentDirectory;
    //getting directory in char format
    struct dirent *dent;
    const char* directoryName = dirPath.c_str();
    //opening given country directory
    currentDirectory = opendir(directoryName);
    //contains the path to the file that we will explore
    string fileName, filePath;
    //array storing dates corresponding to file names
    Date *dateArray;
    //total number of files in given directory
    int filesNum, fileArraySize;
    int i, index;

    filesNum = getNumberOfEntitiesIn(dirPath);
    fileArraySize = filesNum + 1;
    //allocating array for dates corresponding to files names
    dateArray = (Date *) malloc(sizeof(Date) * filesNum);

    index = 0;
    if(currentDirectory!=NULL)
    {
        while((dent=readdir(currentDirectory))!=NULL)
        {
            if(dent->d_name[0] != '.')
            {
                //getting the full path to file that we will extracting data from
                fileName = dent->d_name;
                //getting the date corresponding to given file
                dateArray[index] = Date(fileName);
                index++;
            }
        }
    }

    qsort(dateArray, filesNum, sizeof(Date), cmpfunc);

    for(int fileIndex = 0; fileIndex < filesNum; fileIndex++)
    {
        filePath = dateToString(dateArray[fileIndex]); //array contains dates, extracting them as strings
        filePath = dirPath + "/" + filePath;
        //checking if we have already traversed given file
        //updating proper hashtable coresponding to our country directory
        if(!checkedFile(countryIndex, filePath))
        {
            fldr_HT[countryIndex]->insertIdentifier(new string(filePath));
        }
        fileToDatabase(countryIndex, countryName, filePath);
    }
    free(dateArray);
    closedir(currentDirectory);
}

//for each of the given country directories, traverses its data files, inserts valid entries into database
//for each file, it sends a statistics info to parent
void establishDataBase(string dataDirectory)
{
    string countryName;
    string countryPath;
    string emptyText = "";

    for(int countryIndex = 0; countryIndex < countriesNum; countryIndex ++)
    {
        //getting the directory of the given country
        countryName = *(countriesArray[countryIndex]);
        countryPath = dataDirectory + "/" + countryName;
        //for each file in the given country folder
        dirToDatabase(countryIndex, countryName, countryPath);
        //took care of current country directory, close it
    }
    sendText(SOCKET_BUFFER_SIZE, WR_FDs[0], emptyText);
    // shutdown(WR_FDs[0], SHUT_RDWR);
    close(WR_FDs[0]);
    //waiting for signal block change, making sure we will intercept it
    return;
}

void diseaseFrequency(string *line, int arguments, bool *foundData, string *output)
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
        (*foundData) = true;
        (*output) = to_string(0) + "\n";
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

        (*output) = to_string(*cases) + "\n";
        (*foundData) = true;
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
            cout << "NO WAY WE GOT HERE" << endl;
            (*foundData) = true;
            (*output) = to_string(0) + "\n";
            delete Date1;
            delete Date2;
            free(cases);
            return;
        }

        //get country tree, check how many cases of given disease in given dates
        dataTree = cnt_HT->getStringFile(&givenCountry);
        dataTree->getCasesForString(Date1, Date2, &givenDisease, cases, 0);
        //change cases to string
        (*output) = to_string(*cases) + "\n";
        (*foundData) = true;
        
    }
    //free Dates and cases pointer
    delete Date1;
    delete Date2;
    free(cases);
    return;
}


// /numPatientDischarges disease date1 date2 [country]
// /numPatientDischarges disease date1 date2 [country]
void numPatientDischarges(int arguments, string *query, bool *foundData, string *output)
{
    //will increment the cases
    int *cases = (int *) malloc(sizeof(int));
    (*cases) = 0;
    //all the necessary strings
    string country, countryInfo;
    string disease = getIthWord(query, 2);
    string entryDateStr = getIthWord(query, 3);
    string exitDateStr = getIthWord(query, 4);
    Date *entryDate = new Date(entryDateStr);
    Date *exitDate = new Date(exitDateStr);

    //checking for all countries
    if(arguments == 4)
    {
        //for each country corresponding to current process
        //get the number of exiters in given date range
        for(int coutryIndex = 0; coutryIndex < countriesNum; coutryIndex++)
        {
            country = *(countriesArray[coutryIndex]);
            //getting number of exiters for given country
            recordList->numPatientDischarges(cases, &disease, &country, entryDate, exitDate, false);
            countryInfo = country + " " + to_string(*cases) + "\n";
            (*output) = (*output) + countryInfo;    
        }
        (*foundData) = true;
    }else//checking for specific country
    {
        country = getIthWord(query, 5);
        if(!cntID_HT->containsIdentifier(&country))//no such country in database
        {
            (*foundData) = false;
        }else
        {
            recordList->numPatientDischarges(cases, &disease, &country, entryDate, exitDate, false);
            (*foundData) = true;
            (*output) = country + " " + to_string(*cases) + "\n";
        }
    }

    free(cases);
    delete entryDate;
    delete exitDate;
    return;
}

//returns the number of patients that entered hospital in given date range or if no country specified
//gets this statistic for each country
void numPatientAdmissions(int arguments, string *query, bool *foundData, string *output)
{
    string country;
    string updatedQuery;
    string partialOutput;
    int queryLength;

    if(arguments == 4)
    {
        //for each country of the process, get disease frequency in given date range
        for(int countryIndex = 0; countryIndex < countriesNum; countryIndex++)
        {
            country = *(countriesArray[countryIndex]);
            //getting rid of newline and adding country name
            updatedQuery = (*query) + " " + country;
            diseaseFrequency(&updatedQuery, 5, foundData, &partialOutput);
            (*output) = (*output) + country + " " + partialOutput;
        }
    }else//specific country given, get its instances of entrances
    {
        diseaseFrequency(query, 5, foundData, output);
    }
    return;
}

//sending record info to parent
string getRecord(Record *searchedRecord)
{

    Date *entryDate = searchedRecord->getEntryDate();
    Date *exitDate = searchedRecord->getExitDate();
    string entryDateStr = dateToString(*entryDate);
    string exitDateStr;
    //getting info
    string id = *(searchedRecord->getRecordID());
    string name = *(searchedRecord->getPatientFirstName());
    string surname = *(searchedRecord->getPatientLastName());
    string disease = *(searchedRecord->getDiseaseID());
    string age = to_string(searchedRecord->getAge());
    string finalMessage;

    // 776 Larry Jones SARS-1 87 23-2-2020 28-2-2020
    if(searchedRecord->inHospital())
    {
        exitDateStr = "--";
    }else
    {
        exitDateStr = dateToString(*exitDate);
    }

    finalMessage = id + " " + name + " " + surname + " " + disease + " " + age + " " + entryDateStr + " " + exitDateStr + "\n";
    return finalMessage;
}


//checks if given record is stored in process database
//if so, send his data to parent
void searchPatientRecord(string *query, bool *foundData, string *output)
{
    string searchedID;
    string emptyString = "";
    searchedID = getIthWord(query, 2);
    Record *searchedRecord;

    //this process has info about given id
    if(id_HT->containsIdentifier(&searchedID))
    {
        (*foundData) = true;
        searchedRecord = recordList->getRecordWithID(&searchedID);
        (*output) = getRecord(searchedRecord);
    }else
    {
        (*foundData) = false;
    }
    return;
}

//returns the string corresponding to given age group index
string getAgeGroup(int groupIndex)
{
    string groupString;

    switch(groupIndex){
        case 0:
            groupString = "0-20: ";
            break;
        case 1:
            groupString = "21-40: ";
            break;
        case 2:
            groupString = "41-60: ";
            break;
        default:
            groupString = "60+: ";
            break;
    }
    return groupString;
}


//incremenets the final group message with the info for given group
void updateTopKMessage(int groupIndex, int totalCases, int *groupCases, string *output)
{
    string currOutput = *output;
    string newGroupStats;
    int currCases = groupCases[groupIndex];
    float percentage;
    int finalPercentage;

    //new group stat is being added to the output
    newGroupStats = getAgeGroup(groupIndex);

    if(totalCases)
    {
        percentage = ((float(currCases)) / (float(totalCases))) * 100.0;
        finalPercentage = int(percentage);
        newGroupStats = newGroupStats + to_string(finalPercentage) + "%\n";
        *(output) = currOutput + newGroupStats;
    }else
    {
        newGroupStats = newGroupStats + "0%\n";
        *(output) = currOutput + newGroupStats;
    }  
    return;
}


//constructs the output message for top k age groups
//heap is used to print the groups in cases number order
void topkAgeRangesMessage(int groups, int k, int *groupCases, string *output)
{
    int i, groupIndex;
    int groupCap = min(k, groups);
    int totalCases = 0;
    string *index;
    MaxHeap *groupHeap = new MaxHeap();
    HeapEntry *ageGroupEntry;
    HeapNode *heapNode;

    //push each group into the heap
    for(i = 0; i < groups; i++)
    {
        //getting the total cases in order to print the percentage part
        totalCases = totalCases + groupCases[i];
        //getting the index of the current group
        index = new string(to_string(i));
        //pushing age group into heap
        ageGroupEntry = new HeapEntry(index, groupCases[i]);
        groupHeap->push(ageGroupEntry);
    }

    //extract only the k ones
    for(i = 0; i < groupCap - 1; i++)
    {        
        heapNode = groupHeap->pop();
        ageGroupEntry = heapNode->getData();
        //we can imply which age group we have popped
        groupIndex = atoi(ageGroupEntry->getString()->c_str());
        updateTopKMessage(groupIndex, totalCases, groupCases, output);
        //heap node no more useful, can be deleted
        delete heapNode;
    }

    //used to properly add the last group to the final result
    if(groupCap == groups)
    {
        heapNode = groupHeap->getRoot();
        ageGroupEntry = heapNode->getData();
        groupIndex = atoi(ageGroupEntry->getString()->c_str());
        updateTopKMessage(groupIndex, totalCases, groupCases, output);
    }else if(groupCap > 0)
    {
        heapNode = groupHeap->pop();
        ageGroupEntry = heapNode->getData();
        groupIndex = atoi(ageGroupEntry->getString()->c_str());
        updateTopKMessage(groupIndex, totalCases, groupCases, output);
        delete heapNode;
    }
    
    //delete the remaining entries in the heap
    delete groupHeap;
    return;
}

//returns a message containing info about top-k groups
void topkAgeRanges(string *query, bool *foundData, string *output)
{
    string k = getIthWord(query, 2);
    string country = getIthWord(query, 3);
    string disease = getIthWord(query, 4);
    string entryDateStr = getIthWord(query, 5);
    string exitDateStr = getIthWord(query, 6);
    //file of the given country
    AVL_Tree *countryFile;
    int *groupCases;
    Date *entryDate = new Date(entryDateStr);
    Date *exitDate = new Date(exitDateStr);

    getGroupArray(4, &groupCases);

    //given country/disease duet is not available in process' database
    if(!cntID_HT->containsIdentifier(&country))// || !disID_HT->containsIdentifier(&disease) TODO: MAYBE ADD IT
    {
        cout << "TOP-k, we should have never got here!" << endl;
        *(foundData) = false;
        return;
    }else
    {
        countryFile = cnt_HT->getStringFile(&country);
        countryFile->getAgeGroupsCases(&disease, groupCases, entryDate, exitDate);
        topkAgeRangesMessage(4, stoi(k), groupCases, output);
        *(foundData) = true;
    }
    
    free(groupCases);
    delete entryDate;
    delete exitDate;
    return;
}


//functions waits till SIGUSR2 signal is received, implying a change is signal blockade state
//in this way, we are safe to read queries from user, knowing that read won't be interrrupted
void waitSigBlockChange()
{
    int blockStateChange;
    //waiting for SIGUSR2, so we can safely read query from user
    sigwait(&changeBlockSignal, &blockStateChange);

    if(blockStateChange == SIGUSR2)
    {
        if(signalsBlocked)
        {
            sigprocmask(SIG_UNBLOCK, &blocked_signals, NULL);
            signalsBlocked = false;
        }else
        {
            sigprocmask(SIG_BLOCK, &blocked_signals, NULL);
            signalsBlocked = true;
        }
    }
    return;
}

//keeps receiving requests till exit request is given
void receiveRequests()
{
    bool noExit = true;
    bool foundData;
    //string corresponding to request, result
    string request = "";
    string result;
    //name of the given command
    string query;
    int arguments;
    //variables for proper server communication initialization
    int localServerFD;
    struct sockaddr_in peer_addr;                                                                                                                
    socklen_t peer_addr_size;

    do
    {
        //accepting requests from server and establishing a new local communication
        peer_addr_size = sizeof(peer_addr);
        check(localServerFD = accept(serverListeningSocket, (struct sockaddr *)&peer_addr, &peer_addr_size), "Unable to initialize a new communication with server");

        //receiving request till exit one is given
        receiveText(SOCKET_BUFFER_SIZE, localServerFD, &request);
        removeNewLine(&request);
        getQuery(&request, &query);
        arguments = getNumOfArguments(&request);

        if(query == "/diseaseFrequency")
        {
            diseaseFrequency(&request, arguments, &foundData, &result);
        }else if(query == "/topk-AgeRanges")
        {
            topkAgeRanges(&request, &foundData, &result);
        }else if(query == "/searchPatientRecord")
        {
            searchPatientRecord(&request, &foundData, &result);
        }else if(query == "/numPatientAdmissions")
        {   
            numPatientAdmissions(arguments, &request, &foundData, &result);
        }else if(query == "/numPatientDischarges")
        {
            numPatientDischarges(arguments, &request, &foundData, &result);
        }else
        {
            noExit = false;
            return;
        }

        if(!foundData)//when we were unable to extract requested data, we return empty message
        {
            incrementFail();
            sendEmptyText(SOCKET_BUFFER_SIZE, localServerFD);
        }else
        {
            incrementSuccess();
            sendText(SOCKET_BUFFER_SIZE, localServerFD, result);
        }
        // check(shutdown(localServerFD, SHUT_RDWR), "Unable to terminate local RW communication with server!");
        // check(close(localServerFD), "Unable to close server socket!");
        closeRequestSocket(localServerFD);
        result = "";
    }while(noExit);
    return;
}

void initCounters()
{
    total = 0;
    success = 0;
    fail = 0;
    return;
}

//increments success (and total)
void incrementSuccess()
{
    total++;
    success++;
    return;
}

//increments success (and total)
void incrementFail()
{
    total++;
    fail++;
    return;
}

//this handler takes care of SIGSR1 signal, implying addition of new files
//process traverses all available country folders, finds new files, updates database and sends statistics
void fileHandler(int signum)
{
    string currCountry;
    string currCountryPath;
    string currDatePath;
    string currFile;

    DIR *currentDirectory;
    struct dirent *dent;
    //directory containing the countries subdirectories
    const char* countryFolder;

    if(signum == SIGUSR1)//received a file update signal
    {
        //informing father that he will receive the stats of newly added files
        kill(getppid(), SIGUSR1);
        //for each country folder process is taking care of, check for new files
        for(int countryIndex = 0; countryIndex < countriesNum; countryIndex++)
        {
            currCountry = *(countriesArray[countryIndex]);
            currCountryPath = dataDirectory + "/" + currCountry;
            countryFolder = (currCountryPath).c_str();
            //opening the folder of a specific country
            currentDirectory = opendir(countryFolder);

            //traversing all the files in the given country folder
            //reading from new ones
            if(currentDirectory!=NULL)
            {
                while((dent=readdir(currentDirectory))!=NULL)
                {
                    if(dent->d_name[0] != '.'){

                        currFile = dent->d_name;
                        currDatePath = currCountryPath + "/" + currFile;
                        //found the new added file
                        if(!fldr_HT[countryIndex]->containsIdentifier(&currDatePath))
                        {
                            //insert new file to database, send statistics to father
                            fileToDatabase(countryIndex, currCountry, currDatePath);
                        }
                    }
                }
            }
            closedir(currentDirectory);
        }
        //statistics end with an empty message
        sendEmptyText(bufferSize, WR_FDs[0]);
    }
    return;
}

//frees the main structures of our database
void freeDatabase()
{
    //freeing some sending info data
    // close(WR_FDs[0]);
    // close(WR_FDs[1]);
    free(WR_FDs);
    delete writePipe;
    delete readPipe;
    delete serverIP;

    for(int index = 0; index < countriesNum; index++)
    {
        delete countriesArray[index];
    }
    free(countriesArray);

    for(int index = 0; index < countriesNum; index++)
    {
        //deleting the ith hash table
        delete fldr_HT[index];
    }
    free(fldr_HT);
    //traversing record list and deleting its entries (list destructor only destroys nodes)
    recordList->freeRecords();
    delete recordList;
    delete id_HT;
    delete disID_HT;
    delete cntID_HT;
    delete dis_HT;
    delete cnt_HT;
    return;
}

//simply makes a log file with query statistics
void makeLogFile()
{
    string logFileName;
    int myID = getpid();

    ofstream outfile;
    logFileName = "log_file.";
    logFileName = logFileName + to_string(myID) + ".txt";

    outfile.open (logFileName);
    //add country names to the log file
    for(int index = 0; index < countriesNum; index++)
    {
        outfile << *(countriesArray[index]) << endl;
    }
    outfile << "TOTAL " << total << endl;
    outfile << "SUCCESS " << success << endl;
    outfile << "FAIL " << fail << endl;

    outfile.close();
    return;
}

//handler simply frees database and terminates the program
void exitHandler(int signum)
{   

    if(signum == SIGINT || signum == SIGQUIT)
    {
        //simply frees all contents of database
        makeLogFile();
        freeDatabase();
        cout << "Worker[" << getpid() << "] exited successfully!" << endl;
        exit(signum);
    }
    return;
}

//initializes the three signal handlers
//fileHandler treats SIGUSR1 signal to check for new files
//exitHandler treats SIGINT/SIGQUIT signals to free database and inform father about the collapse
//allowActions treats SIGUSR2 signal to check if it can receive the other signals
void initializeSigaction()
{
    sigfillset(&handlerMask);

    newFileAction.sa_handler = fileHandler; 
    exitAction.sa_handler = exitHandler;

    newFileAction.sa_mask = handlerMask;
    exitAction.sa_mask = handlerMask;

    sigaction(SIGUSR1, &newFileAction, NULL);
    //sigaction(SIGUSR2, &allowActions, NULL);
    sigaction(SIGINT, &exitAction, NULL);
    sigaction(SIGQUIT, &exitAction, NULL);
    return;
}

//takes date as an input, returns its string representation
string dateToString(Date givenDate)
{
    int dayNum, monthNum, yearNum;
    string day, month, year, dateAsString;
    dayNum = givenDate.getDay();
    monthNum = givenDate.getMonth();
    yearNum = givenDate.getYear();

    if(dayNum <= 9)
    {
        day = "0" + to_string(dayNum);
    }else
    {
        day = to_string(dayNum);
    }

    if(monthNum <= 9)
    {
        month = "0" + to_string(monthNum);
    }else
    {
        month = to_string(monthNum);
    }

    year = to_string(yearNum);

    dateAsString = day + "-" + month + "-" + year;
    return dateAsString;
}


// initializes a socket for communication with server with ip given by the master
//drops the pipes file descriptors, as there is no reason to keep communicating with master
void initializeServerCommunication()
{
    //no more communication with the master process
    close(WR_FDs[0]);
    close(WR_FDs[1]);

    //sending a communication request to the server
    serverCommunicationSocket = establishCommunicationSocket(serverIP, serverPort, true);
    //reading and writing to server communication socket from now on
    WR_FDs[0] = serverCommunicationSocket;
    WR_FDs[1] = serverCommunicationSocket;
    return;
}

//receiving a message containing the total number of workers
void receiveWorkersData(string *totalWorkers)
{
    receiveText(bufferSize, WR_FDs[1], totalWorkers);
    return;
}

//inform the server about the countries current worker takes care of
//also give information about the total number of workers
//and the port of communication of worker server
void sendMetaData(string totalWorkersStr, string countriesList)
{
    string metaData;
    string workersString;
    string portString;

    portString = to_string(serverListeningPort) + "\n";

    metaData = totalWorkersStr + portString + countriesList;
    sendText(SOCKET_BUFFER_SIZE, WR_FDs[0], metaData);
    return;
}

//simply initializes a new listening socket for requests from server
//on a new random port
void initializeListening()
{
    serverListeningSocket = getRandomListeningSocket(0, &serverListeningPort);
    return;
}