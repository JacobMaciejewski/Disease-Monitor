#include "../headers/mainAux.h"

//returns the given arguments through its parameters
void getArguments(int argc, char **argv)
{
    string *numWorkerStr, *bufferSizeStr, *serverPortStr;

    getArgument("-w", argc, argv, &numWorkerStr);
    getArgument("-b", argc, argv, &bufferSizeStr);
    getArgument("-i", argc, argv, &input_dir);
    getArgument("-s", argc, argv, &serverIP);
    getArgument("-p", argc, argv, &serverPortStr);

    numWorkers = stoi(*numWorkerStr);
    bufferSize = stoi(*bufferSizeStr);
    serverPort = stoi(*serverPortStr);

    delete numWorkerStr;
    delete bufferSizeStr;
    delete serverPortStr;
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


//returns an array of references to country folder names
//and the total number of folders
void getFolderArray()
{
    //directory struct
    DIR *dir;
    struct dirent *dent;
    const char* directory = (*input_dir).c_str();
    dir = opendir((const char*)directory);

    //counter for main loop
    int index = 0;

    foldersNum = getNumberOfEntitiesIn(*input_dir);
    foldersArray = (string **) malloc(foldersNum * sizeof(string *));


    if(dir!=NULL){
        while((dent=readdir(dir))!=NULL){
            if(dent->d_name[0] != '.'){
                foldersArray[index] = new string(dent->d_name);
                index++;
            }
            
        }
    }
    free(dir);
    return;    
}

//returns an array containing the pipe names that will be used
//for communication between parent process (this main) and its children
//Pipes names are of form r{int}/w{int}, where character refers to read/write function for parent
void getPipesArray()
{
    int upperBound = numWorkers * 2, counter = 0;
    string writeStr, readStr;
    pipesArray = (string **) malloc(sizeof(string *) * (numWorkers * 2));

    for(int index = 0; index < upperBound; index = index + 2)
    {
        writeStr = "./pipes/w";
        readStr = "./pipes/r";
        pipesArray[index] = new string(writeStr.append(to_string(counter)));
        pipesArray[index + 1] = new string(readStr.append(to_string(counter)));
        counter++;
    }
    return;
}

//initializes the communication of parent with children, gathers theirs ids, opens pipes
void initializeCommunication(string bufferSizeString, string *writePipe, string *readPipe)
{
    int temporary_FD, pid;
    childrenIDs = (int *) malloc(sizeof(int) * numWorkers);
    //server communication arguments for workers
    string serverPortStr;
    serverPortStr = to_string(serverPort);

    //producing numWorkers child process, executing their routines
    for (int currWorker = 0; currWorker < numWorkers; currWorker++)
    {
        //getting the names of the pipes corresponding to current connection
        getPipeNames(currWorker, writePipe, readPipe);
        mkfifo(writePipe->c_str(), 0777);
        mkfifo(readPipe->c_str(), 0777);
        //initializing non blocking read for parent
        temporary_FD = open(readPipe->c_str(), O_RDONLY|O_NONBLOCK);
        setFileDescriptor(temporary_FD, currWorker, readFD);

        if((pid = fork()) < 0)
        {
            perror("Could not fork, please create clients manually");
        }
        else if(childProcess(pid))
        {
            execl("./workerProc", "./workerProc", bufferSizeString.c_str(), writePipe->c_str(), readPipe->c_str(), input_dir->c_str(), serverIP->c_str(), serverPortStr.c_str(), (char*) NULL);
            exit(69);
        }
        childrenIDs[currWorker] = pid;
        initializeSending(currWorker);
    }
    return;
}

//returns the pipes corresponding to the index
void getPipeNames(int index, string *writePipe, string *readPipe)
{
    *writePipe = *(pipesArray[index * 2]);
    *readPipe = *(pipesArray[index * 2 + 1]);
    return;
}

//initializing file descriptor arrays
void initializeFD()
{
    writeFD = (int*) malloc(sizeof(int) * numWorkers);
    readFD = (int*) malloc(sizeof(int) * numWorkers);
    return;
}

//sets the first parameter as FD for the given index in FD Array
void setFileDescriptor(int fileDescriptor, int index, int *FD_Array)
{
    FD_Array[index] = fileDescriptor;
    return;
}

//opens pipes for sending data to workers
//sets proper file descriptors in the given FD array
void initializeSending(int index)
{
    string pipeName;
    int writingFileDesc;

    pipeName = *(pipesArray[2 * index]);
    writingFileDesc = open(pipeName.c_str(), O_WRONLY);
    writeFD[index] = writingFileDesc;
    return;
}

//sends a text to each child, contaning the 
void sendCountries()
{
    string workerCountries[numWorkers], currentCountry;
    string emptyString = "";
    int countryIndex;
    int totalTexts = min(numWorkers, foldersNum);
    //evaluating the number of active workers (which received country folders)
    activeWorkers = totalTexts;

    for(int index = 0; index < foldersNum; index++)
    {
        countryIndex = index % numWorkers;
        currentCountry = *(foldersArray[index]);
        currentCountry = currentCountry.append("\n");
        //adding country to workers' country text
        workerCountries[countryIndex].append(currentCountry);
    }

    //sending text of countries to each worker (some may receive none)
    for(int index = 0; index < totalTexts; index++)
    {
        sendText(bufferSize, writeFD[index], workerCountries[index]);
    }

    //sending empty message to remaining workers (they will terminate)
    for(int index = totalTexts; index < numWorkers; index++)
    {
        sendText(bufferSize, writeFD[index], emptyString);
    }

    return;
}

//returns the biggest reading FD available to parent process
void getReadMaxFD(int *maxReadFD)
{
    int currFD;
    int currMax = -1;

    for(int index = 0; index < activeWorkers; index++)
    {
        currFD = readFD[index];
        if(currFD > currMax)
        {
            currMax = currFD;
        }
    }
    (*maxReadFD) = currMax;
    return;
}

//returns a FD available for read
void getAvailableFD(int maxReadFD, int *availableFD)
{
    fd_set fdSet;
    int index;

    FD_ZERO(&fdSet);

    for(index = 0; index < activeWorkers; index++)
    {
        FD_SET(readFD[index], &fdSet);
    }

    int r = select(maxReadFD + 1, &fdSet, NULL, NULL, NULL);
    
    for(index = 0; index < activeWorkers; index++)
    {
        if(FD_ISSET(readFD[index], &fdSet))
        {
            (*availableFD) = readFD[index];
            return;
        }
    }
    return;
}

//used to traverse all active pipes and received messages for each of the files of corresponding child
void receiveStatistics()
{
    int index, fileDesc, maxReadFD;
    string receivedText;
    string TextToPrint;

    getReadMaxFD(&maxReadFD);

    for(index = 0; index < activeWorkers; index++)
    {
        // fileDesc = readFD[index];
        getAvailableFD(maxReadFD, &fileDesc);
        //keep receiving statistics till empty message appears
        do{
            receiveText(bufferSize, fileDesc, &receivedText);
            TextToPrint = receivedText;
            //received all statistics from index-th child
            if(gotEmptyText(receivedText))
            {
                break;
            }else
            {
                printStatistics(TextToPrint);
            }
        }while(!gotEmptyText(receivedText));
    }
    return;
}

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

//prints each country and the process ids
void listCountries()
{
    for(int index = 0; index < foldersNum; index++)
    {
        cout << *(foldersArray[index]) << " " << childrenIDs[index % numWorkers] << endl;
    }
    return;
}

//wait for the children that will receive no countries
void waitInactiveChildren()
{
    int childID;
    int status;
    for(int index = activeWorkers; index < numWorkers; index++)
    {
        childID = wait(&status);
        if(WCOREDUMP(status))
        {
            cout << "Child with id " << childID << " core dumped!" << endl;
        }
    }
    return;
}

void waitChildren()
{
    int childID;
    int status;
    for(int index = 0; index < activeWorkers; index++)
    {
        childID = wait(&status);
        if(WCOREDUMP(status))
        {
            cout << "Child with id " << childID << " core dumped!" << endl;
        }
    }
    return;
}

//returns read and write fd for country index
void getCountryFDs(int countryIndex, int *cntRFD, int *cntWFD)
{
    (*cntRFD) = readFD[countryIndex % activeWorkers];
    (*cntWFD) = writeFD[countryIndex % activeWorkers];
    return;
}

//simply sends given request to every active worker
void sendToEveryone(string request)
{
    for(int index = 0; index < activeWorkers; index++)
    {
        sendText(bufferSize, writeFD[index], request);
    }
    return;
}

//simply receives messages from every active worker
//connects output into one big text
void receiveFromEveryone(string *result)
{
    string workerResult;
    string partialResult;
    int maxReadFD, availableFD;
    getReadMaxFD(&maxReadFD);

    for(int index = 0; index < activeWorkers; index++)
    {
        getAvailableFD(maxReadFD, &availableFD);
        receiveText(bufferSize, availableFD, &workerResult);
        partialResult = partialResult + workerResult;
    }
    (*result) = partialResult;
    return;
}

//turns each message into number and adds them up
void addCases(string *result, int *cases)
{
    string tmpCaseStr;
    int currentCase, tmpCases = 0;

    for(int index = 0; index < activeWorkers; index++)
    {   
        popMessageAsString(result, &tmpCaseStr);
        currentCase = stoi(tmpCaseStr);
        tmpCases = tmpCases + currentCase;
    }
    (*cases) = tmpCases;
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
    sendToEveryone(request);
    receiveFromEveryone(&records);
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


void showAvailableQueries()
{
    cout << endl << "Invalid query name, available ones: " << endl;
    cout << endl << "/listCountries" << endl;
    cout << endl << "/diseaseFrequency virusName date1 date2 [country]" << endl;
    cout << endl << "/topk-AgeRanges k country disease date1 date2" << endl;
    cout << endl << "/searchPatientRecord recordID" << endl;
    cout << endl << "/numPatientAdmissions disease date1 date2 [country]" << endl;
    cout << endl << "/numPatientDischarges disease date1 date2 [country]" << endl;
    cout << endl << "/exit" << endl << endl;
    return;
}


//sends SIGUSR2 signal to each active child
//in this way children will know if they can accept SIGINT/SIGQUIT/SIGSR1 signals or not
//these three are allowed only when parent reads from user (no query is being executed)
void changeSignalAllowance()
{
    int childPID;
    for(int index = 0; index < activeWorkers; index++)
    {

        childPID = childrenIDs[index];
        //this signal is used to reverse blocking state for signal reception in children
        kill(childPID, SIGUSR2);
    }
    return;
}

//frees the memory allocated by this program
void freeMemory()
{
    int index;
    int totalPipes = 2 * numWorkers;
    string pipePath;

    free(childrenIDs);
    free(readFD);
    free(writeFD);
    delete serverIP;
    delete input_dir;

    //free the pipe names
    for(index = 0; index < totalPipes; index++)
    {
        unlink(pipesArray[index]->c_str());
        delete pipesArray[index];
    }   
    free(pipesArray);

    //free the countries array
    for(index = 0; index < foldersNum; index++)
    {
        delete foldersArray[index];
    }
    free(foldersArray);
    return;
}

void initializeQueryCounters()
{
    success = 0;
    fail = 0;
    total = 0;
    return;
}

void incrementSuccess()
{
    total++;
    success++;
    return;
}

void incrementFail()
{
    total++;
    fail++;
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
    for(int index = 0; index < foldersNum; index++)
    {
        outfile << *foldersArray[index] << endl;
    }
    outfile << "TOTAL " << total << endl;
    outfile << "SUCCESS " << success << endl;
    outfile << "FAIL " << fail << endl;

    outfile.close();
    return;
}

//sends kill to children, frees memory
void progExitHandler(int signum)
{
    if(signum == SIGINT || signum == SIGQUIT)
    {
        //making a log file for the main parent process
        makeLogFile();
        //kill all children (may change it to SIGINT)
        for(int index = 0; index < activeWorkers; index++)
        {
            kill(childrenIDs[index], SIGINT);
        }
        waitChildren();
        //closing the pipes corresponding to still active children
        closeActivePipes();
        //freeing the memory
        freeMemory();
        cout << "Master process exited successfully!" << endl;
        exit(0);
    }
}

//returns the file descriptors corresponding to the given child id
void getID_FD(int childID, int *readingFD, int *writingFD)
{
    int childIndex;
    for(int index = 0; index < activeWorkers; index++)
    {
        if(childrenIDs[index] == childID)//found the index of the given child id
        {
            childIndex = index;
            break;
        }
    }
    (*readingFD) = readFD[childIndex];
    (*writingFD) = writeFD[childIndex];
    return;
}

//receives an sigurs1 signal, implying child has gathered data from new files
//will keep receiving them till no new message received
void newStatsHandler(int signum, siginfo_t *sip, void *notused)
{
    int childID;
    int readingFD, writingFD;
    string receivedText;
    string TextToPrint;

    if(signum == SIGUSR1)
    {
        childID = int(sip->si_pid);
        getID_FD(childID, &readingFD, &writingFD);
        do{//receive statistics from child process
            receiveText(bufferSize, readingFD, &receivedText);
            TextToPrint = receivedText;
            //received all statistics from index-th child
            if(gotEmptyText(receivedText))
            {
                break;
            }else
            {
                printStatistics(TextToPrint);
            }
        }while(!gotEmptyText(receivedText));
    }
    return;
}

//returns the index of the given id in the ids array
void getIDIndex(int childID, int *askedIndex)
{
    int childIndex;
    for(int index = 0; index < activeWorkers; index++)
    {
        if(childrenIDs[index] == childID)
        {
            childIndex = index;
            break;
        }
    }
    (*askedIndex) = childIndex;
}

//returns as a parameter the text containing countries corresponding to given child index
void getChildCountries(int childIndex, string *childCountries)
{
    string partialCountries;
    for(int index = 0; index < foldersNum; index++)
    {
        //found a country corresponding to child with given index
        if(index % numWorkers == childIndex)
        {
            partialCountries = partialCountries + *(foldersArray[index]) + "\n";
        }
    }
    (*childCountries) = partialCountries;
    return;
}

//receives a sigchild signal, implying the child has been terminated unexpectedly
//produces a new child, establishes communication 
void deadChildHandler(int signum, siginfo_t *sip, void *notused)
{
    int childID;
    int previousChildIndex;
    int newPid;
    int deadChildStat;
    int oldReadingFD, oldWritingFD;
    int newReadingFD, newWritingFD;
    string pipeToWrite, pipeToRead;
    string bufferSizeString = to_string(bufferSize);
    string receivedText, TextToPrint;
    string childCountries;

    //child terminated unexpectedly
    if(signum == SIGCHLD)
    {
        childID = int(sip->si_pid);
        int skurwi = wait(&deadChildStat);
        getIDIndex(childID, &previousChildIndex);
        getID_FD(childID, &oldReadingFD, &oldWritingFD);
        //closing the pipes of the parent, reastablishing them
        close(oldReadingFD);
        close(oldWritingFD);
        //reopening the reading pipe for the father in non blocking mode
        getPipeNames(previousChildIndex, &pipeToWrite, &pipeToRead);
        newReadingFD = open(pipeToRead.c_str(),O_RDONLY|O_NONBLOCK);
        readFD[previousChildIndex] = newReadingFD;

        //producing a new child
        if((newPid = fork()) < 0)
        {
            perror("Could not fork, please create clients manually");
        }
        else if(childProcess(newPid))
        {
            execl("./workerProc", "./workerProc", bufferSizeString.c_str(), pipeToWrite.c_str(), pipeToRead.c_str(), input_dir->c_str(), (char*) NULL);
            exit(69);
        }
        //opening writing pipe
        newWritingFD  = open(pipeToWrite.c_str(), O_WRONLY);
        writeFD[previousChildIndex] = newWritingFD;
        //replacing old with the new child id in the ids array
        childrenIDs[previousChildIndex] = newPid;
        getChildCountries(previousChildIndex, &childCountries);
        sendText(bufferSize, newWritingFD, childCountries);
        changeBlockState(newReadingFD);
        //keep receiving statistics till empty message appears
        do{
            receiveText(bufferSize, newReadingFD, &receivedText);
            TextToPrint = receivedText;
            //received all statistics from index-th child
            if(gotEmptyText(receivedText))
            {
                break;
            }else
            {
                printStatistics(TextToPrint);
            }
        }while(!gotEmptyText(receivedText));
        kill(newPid, SIGUSR2);
    }
    return;
}

void initializeSigaction()
{
    sigfillset(&handlerMask);
    progExitAction.sa_handler = progExitHandler;
    //there two actions need child ids
    //sa_sigaction used to get siginfo struct with child id 
    newStatsAction.sa_sigaction = newStatsHandler;
    newStatsAction.sa_flags = SA_SIGINFO;
    deadChildAction.sa_sigaction = deadChildHandler;
    deadChildAction.sa_flags = SA_SIGINFO;

    //our handler will ignore all signals
    progExitAction.sa_mask = handlerMask;
    newStatsAction.sa_mask = handlerMask;
    deadChildAction.sa_mask = handlerMask;

    // sigaddset(&blocked_signals, SIGINT);
    // sigaddset(&blocked_signals, SIGQUIT);
    sigaddset(&blocked_signals, SIGUSR1);

    //all the extra signal actions are valid instantly
    //only sigchild has default behaviour (ignoring) till getting queries

    sigaction(SIGUSR1, &newStatsAction, NULL);
    sigaction(SIGINT, &progExitAction, NULL);
    sigaction(SIGQUIT, &progExitAction, NULL);

    return;
}

//changes the signal blocking for a specific child
void changeChildAllowance(int childIndex)
{
    int childPID;
    childPID = childrenIDs[childIndex % activeWorkers];
    //this signal is used to reverse blocking state for signal reception in children
    kill(childPID, SIGUSR2);
}

//closing the pipes corresponding to active workers
void closeActivePipes()
{
    int readingFD, writingFD;
    for(int index = 0; index < activeWorkers; index++)
    {
        readingFD = readFD[index];
        writingFD = writeFD[index];
        close(readingFD);
        close(writingFD);
    }
    return;
}


//closing the pipes that were opened for workers that received no countries
void closeInactivePipes()
{
    int startIndex = activeWorkers;
    int readingFD, writingFD;

    //for each pipe duet corresponding to inactive workers
    for(int index = startIndex; index < numWorkers; index++)
    {
        readingFD = readFD[index];
        writingFD = writeFD[index];
        close(readingFD);
        close(writingFD);
    }
    return;
}

//returns the pipe name corresponding 
string getIndexPipe(int index, string **pipeArray)
{
    return *(pipeArray[index]);
}

bool parentProcess(int pid)
{
    if(pid > 0)
    {
        return true;
    }else
    {
        return false;
    } 
}

bool childProcess(int pid)
{
    if(pid == 0)
    {
        return true;
    }else
    {
        return false;
    }
}

bool unidentifiedProcess(int pid)
{
    if(pid < 0)
    {
        return true;
    }else
    {
        return false;
    }
}

//sending a meta data packet
//containing the total number of workers and the id of the current one
void sendWorkersData()
{
    string totalWorkersText;
    string metaData;
    int serverWorkersNum = min(numWorkers, foldersNum);

    totalWorkersText = to_string(serverWorkersNum);
    totalWorkersText = totalWorkersText + "\n";

    for(int index = 0; index < numWorkers; index++)
    {
        metaData = totalWorkersText + to_string(index) + "\n";
        sendText(bufferSize, writeFD[index], metaData);
    }
    return;
}