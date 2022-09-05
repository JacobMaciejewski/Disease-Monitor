#include "../headers/mainAux.h"


// auxiliarry functions for server main
//returns the given arguments through its parameters

void getArguments(int argc, char **argv)
{
    string  *queryPortNumStr, *statisticsPortNumStr, *numThreadsStr, *bufferSizeStr;

    getArgument("-q", argc, argv, &queryPortNumStr);
    getArgument("-s", argc, argv, &statisticsPortNumStr);
    getArgument("-w", argc, argv, &numThreadsStr);
    getArgument("-b", argc, argv, &bufferSizeStr);

    queryPortNum = stoi(*queryPortNumStr);
    statisticsPortNum = stoi(*statisticsPortNumStr);
    numThreads = stoi(*numThreadsStr);
    bufferSize = stoi(*bufferSizeStr);

    delete queryPortNumStr;
    delete statisticsPortNumStr;
    delete numThreadsStr;
    delete bufferSizeStr;

    return;
}

//returns the argument followed b the given identifier
void getArgument(string identifier, int numOfArguments, char **arguments, string **argumentString){

    for(int i = 0; i < numOfArguments - 1; i++){
        if(identifier == arguments[i]){
            (*argumentString) = new string(arguments[i + 1]);
            break;
        }
    }
    return;
}

//checks if we have connection with each worker
bool allWorkersConnected(int index)
{
    return (index == totalWorkers);
}

//checks if it is the first worker
bool firstWorker()
{
    return (totalWorkers == -1);
}

//checks each country of the current worker
//stores its name and worker's socket FD in the countries list
//TO DO: free list
void updateCountriesList(int workerID, string workerCountries)
{
    string currentCountry;
    string *dupletCountry;
    HeapEntry *currentDuplet;

    //extracting each country of current worker
    while(popMessageAsString(&workerCountries, &currentCountry))
    {
        dupletCountry = new string();
        (*dupletCountry) = currentCountry;
        currentDuplet = new HeapEntry(dupletCountry, workerID);
        countriesList->push(currentDuplet);
    }
    return;
}

//returns all the needed meta data
void getMetaData(int socketFD, int *numOfWorkers, int *workerID, string *workerPort, string *countriesGroup)
{
    sockaddr clientptr;
    socklen_t clientlen = sizeof(clientptr);

    string metaData, numOfWorkersStr, workerIDStr;
    //extracting the metadata message from worker
    receiveText(SOCKET_BUFFER_SIZE, socketFD, &metaData);

    //extracting total number of workers, worker id, worker port and countries
    popMessageAsString(&metaData, &numOfWorkersStr);
    popMessageAsString(&metaData, &workerIDStr);
    popMessageAsString(&metaData, workerPort);
    //remaining meta data are the countries of the current worker
    (*numOfWorkers) = stoi(numOfWorkersStr);
    (*workerID) = stoi(workerIDStr);
    (*countriesGroup) = metaData;

    return;
}

//allocating memory for worker data array
void initializeWorkerArray()
{
    int upperBound = totalWorkers * 2;
    workersData = (string**)malloc(sizeof(string*) * upperBound);

    for(int index = 0; index < upperBound; index++)
    {
        workersData[index] = NULL;
    }

    return;
}

//simply updates current worker data (IP and Port)
void updateWorkerData(int workerID, string workerIP, string workerPort)
{
    //a brand new worker connected
    if(workersData[workerID * 2] == NULL && workersData[workerID * 2 + 1] == NULL)
    {
        workersData[workerID * 2] = new string(workerIP);
        workersData[workerID * 2 + 1] = new string(workerPort);

    }else//replacing a dead worker
    {
        //first free the old worker data
        delete workersData[workerID * 2];
        delete workersData[workerID * 2 + 1];
        //then update the array
        workersData[workerID * 2] = new string(workerIP);
        workersData[workerID * 2 + 1] = new string(workerPort);
    }

    //it is the last worker, main thread, can continue its job
    if(workerID == totalWorkers - 1)
    {
        gotAllWorkers = true;
        pthread_cond_signal(&workerDataSet);
    }

    return;  
}



//receives the metadata from the 
void initWorker(int socketFD)
{
    //contemporary file descriptor for communication with specific worker
    socklen_t clientlen;
    //all the meta data returned by the worker
    int numOfWorkers, workerID;
    string workerPort, countriesGroup;
    string workerIP;

    //getting the ip of the worker
    getPeerIP(socketFD, &workerIP);

    //memory for countries should be allocated if it is the first worker
    if(firstWorker())
    {
        getMetaData(socketFD, &numOfWorkers, &workerID, &workerPort, &countriesGroup);
        totalWorkers = numOfWorkers;
        initializeWorkerArray();
    }else
    {
        getMetaData(socketFD, &numOfWorkers, &workerID, &workerPort, &countriesGroup);
        totalWorkers = numOfWorkers;
    }

    //adding new worker's ip and
    pthread_mutex_lock(&worker_data_mutex);

    updateWorkerData(workerID, workerIP, workerPort);
    //adds worker's countries to the dictionary
    updateCountriesList(workerID, countriesGroup);

    pthread_mutex_unlock(&worker_data_mutex);

    //incrementing the total number of active workers
    return;
}

//initializes connection with new worker and receives statistics
void execWorkerTask(int workerFD)
{
    //updating the worker data structures
    initWorker(workerFD);
    //receive statistics from the given worker
    receiveStatistics(workerFD);

    return;
}


void initializeWorkerCommunication(CircularBuffer<BufferDuplet*> *FD_Buffer)
{
    int firstWorkerFD;
    int workerIndex = 0;
    int newsock;

    struct sockaddr_in workerAddr;
    socklen_t workerlen;

    //initializing the countries list
    countriesList = new LinkedList<HeapEntry*>;
    //initializing global variables for the number of workers
    totalWorkers = -1;
    //getting the first connection from main thread and receiving all the necessary info including statistics
    workerlen = sizeof(workerAddr);
    check(firstWorkerFD = accept(*workersMainSocket, (struct sockaddr*) &workerAddr, &workerlen), "Unable to accept connection from first worker!");
    //execute the necessary communication establishment with first worker

    execWorkerTask(firstWorkerFD);

    // check(shutdown(firstWorkerFD, SHUT_RDWR), "Unable to terminate  RW connection with first worker socket!");
    // check(close(firstWorkerFD), "Unable to close first worker socket!");
    closeRequestSocket(firstWorkerFD);

    //index starts from 1 as we have already taken care of the first worker
    //receiving FDs from each worker, putting it into the buffer
    BufferDuplet *newConnectionDP;


    for(int index = 1; index < totalWorkers; index++)
    {
        workerlen = sizeof(workerAddr);
        check(newsock = accept(*workersMainSocket, (struct sockaddr*) &workerAddr, &workerlen), "Unable to establish a new client connection!");
        //put the FD of newly connected worker into the buffer
        newConnectionDP = new BufferDuplet(newsock, false);
        FD_Buffer->push(newConnectionDP);
    }
    return;
}

//returns a FD available for read
void getAvailableFD(int maxReadFD, int *availableFD, int *currWorkerSockets)
{
    fd_set fdSet;
    int index;

    FD_ZERO(&fdSet);

    for(index = 0; index < totalWorkers; index++)
    {
        if(currWorkerSockets[index] != -1)
        {
            FD_SET(currWorkerSockets[index], &fdSet);
        }
    }

    int r = select(maxReadFD + 1, &fdSet, NULL, NULL, NULL);
    
    for(index = 0; index < totalWorkers; index++)
    {
        if(FD_ISSET(currWorkerSockets[index], &fdSet) && currWorkerSockets[index] != -1)
        {
            (*availableFD) = currWorkerSockets[index];
            return;
        }
    }
    return;
}

//returns the biggest FD of worker's socket in the server
void getReadMaxFD(int *maxReadFD, int *currWorkerSockets)
{
    int currFD;
    int currMax = -1;

    for(int index = 0; index < totalWorkers; index++)
    {
        currFD = currWorkerSockets[index];

        if(currFD > currMax && currFD != -1)
        {
            currMax = currFD;
        }
    }
    (*maxReadFD) = currMax;
    return;
}

//takes the worker socket FD and receives statistics from it
void receiveStatistics(int workerFD)
{
    string receivedText;
    string TextToPrint;

    //keep receiving statistics till empty message appears
    do{
        receiveText(SOCKET_BUFFER_SIZE, workerFD, &receivedText);
        TextToPrint = receivedText;
        //received all statistics from index-th child
        if(gotEmptyText(receivedText))
        {
            break;
        }else
        {
            pthread_mutex_lock(&query_print_mutex);
            printStatistics(TextToPrint);
            pthread_mutex_unlock(&query_print_mutex);
        }
    }while(!gotEmptyText(receivedText));
    return;
}

//prints the stats for each age group
void printGroupStats(int cases, int index)
{
    switch(index){
        case 0:
            cout << "Age range 0-20 years: " << cases << " cases" << endl;
            break;

        case 1:
            cout << "Age range 21-40 years: " << cases << " cases" << endl;
            break;
        case 2:
            cout << "Age range 41-60 years: " << cases << " cases" << endl;
            break;
        default:
            cout << "Age range 60+ years: " << cases << " cases" << endl;
            break;
    }
    return;
}

//printing the five lines corresponding 
void printDiseaseStats(string *diseaseStats)
{
    string diseaseName, groupStats;
    int line, cases;

    popMessage(diseaseStats, &diseaseName);
    cout << diseaseName;

    for(line = 0; line < 4; line++)
    {
        popMessageAsString(diseaseStats, &groupStats);
        cases = stoi(groupStats);
        printGroupStats(cases, line);
    }
    return;
}

//received stats is a text, starting with date and country name
//it is followed by subtexts of size 5, containing info about the age groups of specific disease
void printStatistics(string receivedStats)
{
    string currentLine, country, date;
    popMessage(&receivedStats, &date);
    popMessage(&receivedStats, &country);

    cout << date;
    cout << country << endl;

    //printing till we get no new disease stats
    while(!receivedStats.empty())
    {
        printDiseaseStats(&receivedStats);
        cout << endl;
    }
    return;
}

//function takes country index as input
//gets corresponding IP and port number
//open a new connection to communicate with worker server
void getCountryFDs(int countryIndex, int *CNT_R, int *CNT_W)
{
    string *workerIPString;
    workerIPString = workersData[2 * countryIndex];

    int workerPort = stoi(*(workersData[2 * countryIndex + 1]));
    int workerSocketFD;

    workerSocketFD = establishCommunicationSocket(workerIPString, workerPort, true);

    (*CNT_R) = workerSocketFD;
    (*CNT_W) = workerSocketFD;
    return;
}

//send and receive from everyone
void sendReceiveEveryone(string request, string *result)
{
    int currWorkerPortInt, currSocket;
    string *currWorkerIP, *currWorkerPort;
    string workerResult, partialResult;

    for(int index = 0; index < totalWorkers; index++)
    {
        //getting the worker ip and port
        currWorkerIP = workersData[index * 2];
        currWorkerPort = workersData[index * 2 + 1];

        if(currWorkerIP != NULL && currWorkerPort != NULL)
        {
            currWorkerPortInt = stoi(*currWorkerPort);
            currSocket = establishCommunicationSocket(currWorkerIP, currWorkerPortInt, true);
            //sending and receiving query/answer from worker
            sendText(SOCKET_BUFFER_SIZE, currSocket, request);
            receiveText(SOCKET_BUFFER_SIZE, currSocket, &workerResult);
            close(currSocket);
            partialResult = partialResult + workerResult;
        }
    }
    (*result) = partialResult;
    return;
}

//ask every worker, gathers result
//if it is empty, no record found
void searchPatientRecord(string request, string *result)
{
    string records;
    string searchedID;
    string noRecordResult;

    searchedID = getIthWord(&request, 2);
    //ask every worker
    sendReceiveEveryone(request, &records);
    //if no record in database, empty text is sent, empty result implies no such record
    if(gotEmptyText(records))
    {
        noRecordResult = "No record with id " + searchedID + "\n";
        (*result) = noRecordResult;
    }else
    {
        (*result) = records;
    }
    return;
}

//turns each message into number and adds them up
void addCases(string *result, int *cases)
{
    string tmpCaseStr;
    int currentCase, tmpCases = 0;

    for(int index = 0; index < totalWorkers; index++)
    {   
        popMessageAsString(result, &tmpCaseStr);
        currentCase = stoi(tmpCaseStr);
        tmpCases = tmpCases + currentCase;
    }
    (*cases) = tmpCases;
    return;
}

//initializing the local worker socket array
void initLocalSockets(int **localSockets)
{
    (*localSockets) = (int*) malloc(sizeof(int) * totalWorkers);

    for(int index = 0; index < totalWorkers; index++)
    {
        (*localSockets)[index] = -1;
    }
    return;
}

//closing the sockets corresponding to local sockets
//used to get answers for a specific query
void terminateLocalSockets(int *localSockets)
{
    for(int index = 0; index < totalWorkers; index++)
    {
        if(localSockets[index] != -1)//there was an actual connection with current worker
        {
            // check(shutdown(localSockets[index], SHUT_RDWR), "Unable to terminate RW with local worker socket!");
            // check(close(localSockets[index]), "Unable to close local worker socket!");
            // shutdown(localSockets[index], SHUT_RDWR);
            close(localSockets[index]);
        }
    }
    return;
}

//takes a file descriptor corresponding to
//client communication
//sends given query to workers, receives the result
//the result is being printed on server level and sent to client
//returns true if received exit query
bool execClientQuery(int clientFD)
{
    string line;
    string query;
    string messageToClient;
    //classic message for wrong number of arguments
    string wrongArgNum = "Wrong number of arguments !";
    wrongArgNum = wrongArgNum + "\n";
    //the exact query sent to worker(s)
    string request;
    string result;
    int cases;
    //counts the number of given arguments
    int arguments = 0;
    //variables that may be used in some queries
    string country;
    int countryIndex;
    int cntReadFD = -1, cntWriteFD = -1;
    //receive a local array of socket FDs for workers

    //receiving the request from the client
    receiveText(SOCKET_BUFFER_SIZE, clientFD, &line);
    //removing the newline character for proper checks
    removeNewLine(&line);
    //get the name of the given query
    getQuery(&line, &query);
    //getting the total number of arguments 
    arguments = getNumOfArguments(&line);
    //making the exact query that will be sent to workers
    request = line;
    addNewLine(&request);
    cout << endl;

    if(query == "/diseaseFrequency")
    {
        //sending disease frequency request to every active worker
        if(arguments == 4)
        {
            sendReceiveEveryone(request, &result);
            addCases(&result, &cases);
            //printing the total cases on server level and sending result to client
            messageToClient = to_string(cases) + "\n";
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);

        }else if(arguments == 5)
        {
            country = getIthWord(&line, arguments);
            //checking for the existence of this specific country
            if(countriesList->countryWorkerExists(&country, &countryIndex))
            {
                getCountryFDs(countryIndex, &cntReadFD, &cntWriteFD);
                sendText(SOCKET_BUFFER_SIZE, cntWriteFD, request);
                receiveText(SOCKET_BUFFER_SIZE, cntReadFD, &result);
                //get the result in the form of integer
                removeNewLine(&result);
                messageToClient = result + "\n";
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            }else
            {
                messageToClient = "No " + country + " in database!" + "\n";
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            }   
        }else
        {
            messageToClient = wrongArgNum;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }
        
    }else if(query == "/topk-AgeRanges")
    {
        if(arguments == 6)
        {
            country = getIthWord(&line, 3);
            if(countriesList->countryWorkerExists(&country, &countryIndex))
            {
                getCountryFDs(countryIndex, &cntReadFD, &cntWriteFD);
                sendText(SOCKET_BUFFER_SIZE, cntWriteFD, request);
                receiveText(SOCKET_BUFFER_SIZE, cntReadFD, &result);
                //get the result in the form of integer
                messageToClient = result;
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            }else
            {
                messageToClient = "No " + country + " in database!" + "\n";
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            } 
        }else
        {
            messageToClient = wrongArgNum;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }
    }else if(query == "/searchPatientRecord")
    {
        if(arguments == 2)
        {
            searchPatientRecord(request, &result);
            messageToClient = result;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }else
        {
            messageToClient = wrongArgNum;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }

    }else if(query == "/numPatientAdmissions")
    {
        if(arguments == 5)//ask specific country
        {
            country = getIthWord(&line, arguments);
            if(countriesList->countryWorkerExists(&country, &countryIndex))
            {
                getCountryFDs(countryIndex, &cntReadFD, &cntWriteFD);
                sendText(SOCKET_BUFFER_SIZE, cntWriteFD, request);
                receiveText(SOCKET_BUFFER_SIZE, cntReadFD, &result);
                messageToClient = country + " " + result;
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            }else
            {
                messageToClient = "No " + country + " in database!" + "\n";
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            }   
        }else if(arguments == 4)//ask each worker for patient admissions
        {
            sendReceiveEveryone(request, &result);
            messageToClient = result;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }else
        {
            messageToClient = wrongArgNum;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }
    }else if(query == "/numPatientDischarges")
    {
        if(arguments == 5)//ask specific country
        {
            country = getIthWord(&line, arguments);
            if(countriesList->countryWorkerExists(&country, &countryIndex))
            {
                getCountryFDs(countryIndex, &cntReadFD, &cntWriteFD);
                sendText(SOCKET_BUFFER_SIZE, cntWriteFD, request);
                receiveText(SOCKET_BUFFER_SIZE, cntReadFD, &result);
                messageToClient = result;
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            }else
            {
                messageToClient = "No " + country + " in database!" + "\n";
                sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
            }   
        }else if(arguments == 4)//ask each worker for patient admissions
        {
            sendReceiveEveryone(request, &result);
            messageToClient = result;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }else
        {
            messageToClient = wrongArgNum;
            sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        }
    }else if(query == "/exit")
    {
        messageToClient = "EXITING!";
        messageToClient = messageToClient + "\n";
        sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
        //received an exit queue, free local sockets
        return false;
    }
    else//invalid query name from user, time for some suggestions
    {
        // showAvailableQueries();
        messageToClient = "Invalid query!";
        messageToClient = messageToClient + "\n";
        sendText(SOCKET_BUFFER_SIZE, clientFD, messageToClient);
    }

    pthread_mutex_lock(&query_print_mutex);
    cout << "Query: " << request;
    cout << "Answer: " << messageToClient;
    pthread_mutex_unlock(&query_print_mutex);


    //for pretty print purposes
    cout << endl;

    //sent a request to single worker, time to close him
    if(cntReadFD != -1)
    {
        // check(shutdown(cntReadFD, SHUT_RDWR), "Unable to terminate  RW connection with a worker!");
        // check(close(cntReadFD), "Unable to close a worker socket!");
        // shutdown(cntReadFD, SHUT_RDWR);
        close(cntReadFD);
    }
    return true;
}

//simply allocate memory and bind the two listening sockets
void initializeListeningSockets()
{
    clientMainSocket = (int*) malloc(sizeof(int) * 1);
    workersMainSocket = (int*) malloc(sizeof(int) * 1);

    (*workersMainSocket) = establishListeningSocket(statisticsPortNum);
    (*clientMainSocket) = establishListeningSocket(queryPortNum);

    return;
}

void receiveRequests(CircularBuffer<BufferDuplet*> *FD_Buffer)
{
    int WC_ListeningSocks[2];
    int maxQueryFD;
    int availableFD;
    int r;
    fd_set fdSet;
    //duplet containing each new communication request FD
    BufferDuplet *currDP;
    int currFD;
    bool currClient;
    //storing listening sockets in an array
    WC_ListeningSocks[0] = (*clientMainSocket);
    WC_ListeningSocks[1] = (*workersMainSocket);
    //for proper accept initialization
    sockaddr requestPtr;
    socklen_t requestLen;

    maxQueryFD = max(WC_ListeningSocks[0], WC_ListeningSocks[1]);

    while(1)
    {
        FD_ZERO(&fdSet);
        FD_SET(WC_ListeningSocks[0], &fdSet);
        FD_SET(WC_ListeningSocks[1], &fdSet);
        r = select(maxQueryFD + 1, &fdSet, NULL, NULL, NULL);

        //check if we have a new connection request from client or worker
        for(int index = 0; index < 2; index++)
        {
            if(FD_ISSET(WC_ListeningSocks[index], &fdSet))
            {
                availableFD = WC_ListeningSocks[index];

                if(index == 0)//got a client request
                {
                    currClient = true;
                }else//got a worker request
                {
                    currClient = false;
                }   
            }
        }

        requestLen = sizeof(requestPtr);

        //accept the new communication request
        if(currClient)
        {
            check(currFD = accept(*clientMainSocket, &requestPtr, &requestLen), "Unable to establish a new client request connection!");
        }else
        {
            check(currFD = accept(*workersMainSocket, &requestPtr, &requestLen), "Unable to establish a new worker request connection!");
        }
        
        //add the new communication to FD buffer
        currDP = new BufferDuplet(currFD, currClient);
        FD_Buffer->push(currDP);
    }
}


void getAllWorkers()
{
    pthread_mutex_lock(&worker_data_mutex);
    if(!gotAllWorkers)
    {
        pthread_cond_wait(&workerDataSet, &worker_data_mutex);
    }
    pthread_mutex_unlock(&worker_data_mutex);
}