#include "auxiliarry.h"
#define SERVER_BACKLOG 500

using namespace std;

//takes a flag as an arguments and returns its non blocking version
int removeNonBlocking(int flag)
{
    return (flag & ~O_NONBLOCK);
}

//takes a flag as an arguments and returns its blocking version
int addNonBlocking(int flag)
{
    return (flag | O_NONBLOCK);
}

//returns the number of rows in given text
int numOfRows(string givenText)
{
    return (count(givenText.begin(), givenText.end(), '\n'));
}

//returns the number of folders, files etc. contained within given directory
int getNumberOfEntitiesIn(string givenDirectory)
{
    int numOfEntities = 0;
    DIR *dir;
    struct dirent *dent;
    const char* directory = givenDirectory.c_str();
    dir = opendir((const char*)directory);

    if(dir!=NULL){
        while((dent=readdir(dir))!=NULL){
            if(dent->d_name[0] != '.'){
                numOfEntities++;
            }
            
        }
    }
    closedir(dir);
    return numOfEntities;
}

//checks the block state of file desc
//if non blocking -> block and vice versa
void changeBlockState(int fileDesc)
{
    int curr_flag;
    curr_flag = fcntl(fileDesc, F_GETFL);

    if(curr_flag & O_NONBLOCK)
    {
        fcntl(fileDesc, F_SETFL, curr_flag & !O_NONBLOCK);
    }else
    {
        fcntl(fileDesc, F_SETFL, curr_flag & O_NONBLOCK);
    }  
}

//sets opposite block state for all given file descriptors
void changeBlockStates(int numOfFiles, int *FD_Array)
{
    for(int i = 0; i < numOfFiles; i++)
    {
        changeBlockState(FD_Array[i]);
    }
    return;
}

//prints the flags of each FD in the given array
void printStreamFlags(int numOfFiles, int *FD_Array)
{
    for(int i = 0; i < numOfFiles; i++)
    {
        printFDFlags(FD_Array[i]);
    }
    return;
}

//prints the glags of the given fd
void printFDFlags(int fileDesc)
{

    cout << "FD[" << fileDesc << "] with flag: " << fcntl(fileDesc, F_GETFL) << endl;
    return;
}

//used to send given string as a message
//string may be broken to substrings due to buffer size
//end of message implied by newline
void sendMessage(int bufferSize, int FileDesc, string givenString)
{
    string message, toSendMessage;
    int packetsNum, messageSize, packetSize;
    char buf[bufferSize];

    int sentBytes, newSentBytes;

    //message end with new line
    message = givenString;
    messageSize = message.length();
    packetsNum = messageSize / bufferSize;

    string tmpStr = string();
    for(int currPacket = 0; currPacket < packetsNum; currPacket++)
    {
        packetSize = min(messageSize, bufferSize);
        //getting packet size bytes to produce partial message
        toSendMessage = message.substr(0, packetSize);

        //making sure all the bytes have been sent
        sentBytes = 0;
        strncpy(buf, toSendMessage.c_str(), bufferSize);

        while(sentBytes < bufferSize)
        {
            newSentBytes = write(FileDesc, buf + sentBytes, bufferSize - sentBytes);
            sentBytes = sentBytes + newSentBytes;
        }

        //still some message to send
        message = message.substr(packetSize);
        tmpStr = tmpStr.append(toSendMessage);
    }

    // cout << "Last packet to send! " << endl;
    if(!message.empty())
    {
        strncpy(buf, message.c_str(), bufferSize);
        //new line at the end implies end of message
        buf[bufferSize - 1] = '\n';
        // printf("Last Message sending buffer: %s\n", buf);
        // cout << "Sent " << write(FileDesc, buf, bufferSize) << " bytes!" << endl;
        // write(FileDesc, buf, bufferSize);
        sentBytes = 0;
        // strncpy(buf, toSendMessage.c_str(), bufferSize);
        while(sentBytes < bufferSize)
        {
            newSentBytes = write(FileDesc, buf + sentBytes, bufferSize - sentBytes);
            sentBytes = sentBytes + newSentBytes;
        }
        tmpStr.append(message);
    }
    // cout << "Sent the whole " << tmpStr << endl << endl;
    return;
}

//checks if we got the whole message
//last packet end with end line
void gotAllPackets(bool *endOfMessage, int bufferSize, char *buf)
{
    *endOfMessage = (buf[bufferSize - 1] == '\n');
}

//returns as its third parameter the size of the final packet
void getPacketSize(int bufferSize, char *buf, int *packetSize)
{
    for(int i = 0; i < bufferSize; i++)
    {
        //found the end of the message
        if(buf[i] == '\n')
        {
            *(packetSize) = i + 1;
            return;
        }
    }

    // cout << "No newline, problem with received packet!" << endl;
    return;
}

void receiveMessage(int bufferSize, int fileDesc, string *outputMessage)
{
    int lastPacketSize;
    char buf[bufferSize];
    bool endOfMessage;
    string message, packet, tmp_string;
    int index = 0;

    int receivedBytes, currentReceivedBytes;


    do{
        // cout << "Process [" << getpid() << "] starts receiving packet" << endl;

        receivedBytes = 0;
        while(receivedBytes < bufferSize)
        {
            currentReceivedBytes = read(fileDesc, buf + receivedBytes, bufferSize - receivedBytes);
            receivedBytes = receivedBytes + currentReceivedBytes;
        }

        // cout << "Received " << read(fileDesc, buf, bufferSize) << " bytes!" << endl;

        gotAllPackets(&endOfMessage, bufferSize, buf);

        //get the part of buffer corresponding to last packet
        if(endOfMessage)
        {
            // cout << "[" << getpid() << "] received last packet!" << endl;
            //getting the size of the last message
            getPacketSize(bufferSize, buf, &lastPacketSize);
            if(lastPacketSize == 1)
            {
                // cout << "[" << getpid() << "]" << " Packet size 1 [LAST]: " << endl;
                packet = "\n";
                // cout << "#" << packet << "#" << endl;
            }else
            {
                tmp_string = buf;
                // cout << "[" << getpid() << "]" << " Packet bigger than 1 [LAST]: " << endl;
                packet = tmp_string.substr(0, lastPacketSize);
                // cout << "#" << packet << "#" << endl;
            }
            
            message = message + packet;
        }else //simply add the contents of buffer to final message
        {
            packet = buf;
            packet = packet.substr(0, bufferSize);
            // cout << "[" << getpid() << "]" << " Packet[NOT LAST]: " << endl;
            // cout << "#" << packet << "#" << endl;
            message = message + packet;
        }
        // cout << "Process [" << getpid() << "] message till now:" << endl;
        // cout << "Process [" << getpid() << "] message till now:" << message << endl;
    }while(!endOfMessage);
    // cout << "Process [" << getpid() << "] received message the whole message: " << endl;
    // cout << "#" << message << "#" << endl;
    //return the received message
    (*outputMessage) = message;
}

//receives a text with commands, entries (messages) seperated by newline
//keeps receiving messages till $ character is given
void receiveText(int bufferSize, int fileDesc, string *outputText)
{
    string message;
    string emptyString = "";
    string partialText = string();
    // cout << "Process[" << getpid() << "] starts receiving text" << endl;
    do{
        receiveMessage(bufferSize, fileDesc, &message);
        //got to the end of the text

        if(lastMessage(message) || lastByte(partialText, message))
        {
            *(outputText) = emptyString;
            break;
        }else
        {
            //adding the newly received message to the output text
            partialText = partialText + message;
        }
        
    }while(!lastMessage(message) && !lastByte(partialText, message));
    // cout << "Process[" << getpid() << "] received this text: " << "#" << partialText << "#" << endl;
    // cout << "Process[" << getpid() << "] done receiving text" << endl;
    *(outputText) = partialText;
    // cout << "received: " << partialText << endl;
    return;
}

//sends a group of messages (commands, entries etc), each taking one line
//sends a dollar sign as a message indicating the end of the text
void sendText(int bufferSize, int FileDesc, string givenText)
{
    int messageSize = 0;
    string currentMessage;
    //adding the dollar sign, indicating the end of the text
    givenText = givenText.append("$\n");
    //searching for newline, indicating text contains another message
    while((messageSize = givenText.find("\n")) != -1)
    {
        currentMessage = givenText.substr(0, messageSize + 1);
        // cout << "Message sent: " << currentMessage << endl;
        //sending the message
        sendMessage(bufferSize, FileDesc, currentMessage);
        givenText = givenText.substr(messageSize + 1);
    }
    return;
}

//sends an empty message to given file
void sendEmptyText(int bufferSize, int fileDesc)
{
    string emptyString = "";

    sendText(bufferSize, fileDesc, emptyString);
    return;
}

//initializes the group array (fills it with zeros, allocates memory)
void getGroupArray(int groups, int **groupCases)
{
    (*groupCases) = (int*) malloc(sizeof(int) * groups);

    for(int index = 0; index < groups; index++)
    {
        (*groupCases)[index] = 0;
    }
    return;
}

//adds a new line to the end of the string, turning it into a message
void addNewLine(string *query)
{
    (*query) = (*query) + "\n";
    return;
}

//removes newline from given string/query
void removeNewLine(string *query)
{
    int newlineIndex;
    newlineIndex = query->find("\n");
    (*query) = query->substr(0, newlineIndex);
    return;
}

// //returns the first argument of a query, its name
void getQuery(string *line, string *query)
{
    *query = line->substr(0, line->find(" "));
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

//takes a path as input and returns the file name (aka date)
string getDate(string path)
{
    int slashIndex;
    string date;

    slashIndex = path.find('/');
    date = path.substr(slashIndex + 1);
    slashIndex = date.find('/');
    date = date.substr(slashIndex + 1);
    return date;
}

//returns the index-th word in the given line
//countring starts from 1
string getIthWord(string *givenString, int index)
{
    string tempString;
    istringstream iss (*givenString);
    while(index-- > 0 && (iss >> tempString));
    return tempString;
}

//takes a text as input
//returns the first message (row) with newline symbol
//given text is being cut into pieces!
int popMessage(string *givenText, string *message)
{
    int newlineIndex;

    newlineIndex = (*givenText).find("\n");
    *(message) = (*givenText).substr(0, newlineIndex + 1);
    *givenText = (*givenText).substr(newlineIndex + 1);
    return (*message).length();
}

//same behaviour as popMessage, but message has no new line
int popMessageAsString(string *givenText, string *message)
{
    int newlineIndex;
    newlineIndex = (*givenText).find("\n");
    *(message) = (*givenText).substr(0, newlineIndex);
    *givenText = (*givenText).substr(newlineIndex + 1);
    return (*message).length();
}

//received an empty message
//user sent an "", so received message is equal to $\n
bool gotEmptyText(string givenString)
{
    return (givenString.empty());
}

//checks if the last packet shows end of message
//edge case for buffer size = 1
bool singletonEnd(int bufferSize, char *buf)
{
    return (buf[0] == '\n');
}

//checks if given message indicates end of text
bool lastMessage(string givenMessage)
{
    return (givenMessage == "$\n");
}

//extra case for one byte connection
bool lastByte(string message, string lastByte)
{
    return (message == "$" && lastByte == "\n");
}

int establishCommunicationSocket(string *info, int port, bool addressGiven)
{
    int newSocket; 
    struct sockaddr_in servaddr;

    // socket create and varification 

    check(newSocket = socket(AF_INET, SOCK_STREAM, 0), "Unable to open a communication socket!");

    bzero(&servaddr, sizeof(servaddr));

    //assign IP, PORT 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr(info->c_str()); 
    servaddr.sin_port = htons(port); 

    //initializing connection through the new socket
    //avoiding big congestion by sleeping in case of busy server
    while(connect(newSocket, (sockaddr *) &servaddr, sizeof(servaddr)) == -1)
    {
        sleep(3);
    }
    // check(connect(newSocket, (sockaddr *) &servaddr, sizeof(servaddr)), "Unable to connect to target!");

    return newSocket;
}

//initializes a socket for communication with given info (ip address or host name)
//target port given as a paremeter, returns the file descriptor of the established socket
// int establishCommunicationSocket(string *info, int port, bool addressGiven)
// {
//     //struct containing info about the address of communication target
//     struct hostent* rem;
//     struct sockaddr_in target;
//     struct in_addr givenAddress;
//     struct sockaddr *targetptr = (struct sockaddr *)&target;
//     //new socket
//     int newSocket;
//     //used for socket reuse
//     int opt = 1;

//     check(newSocket = socket(AF_INET, SOCK_STREAM, 0), "Unable to open a communication socket!");

//     //target ip address given
//     if(addressGiven)
//     {
//         inet_aton(info->c_str(), &givenAddress);
//         //initializing address struct
//         rem = gethostbyaddr(&givenAddress ,sizeof(givenAddress), AF_INET);
//     }else// address name has been given
//     {
//         rem = gethostbyname(info->c_str());
//     }

//     target.sin_family = AF_INET; /* Internet domain */
//     memcpy(&target.sin_addr, rem->h_addr, rem->h_length);
//     target.sin_port = htons(port); /* target port */
//     //initializing connection through the new socket
//     check(connect(newSocket, targetptr, sizeof(target)), "Unable to connect to target!");

//     return newSocket;  
// }

//established a listening socket for reception of new connections by the server
int establishListeningSocket(int port)
{
    int listeningSocket;
    struct sockaddr_in server, client;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;
    int opt = 1;

    check(listeningSocket = socket(AF_INET, SOCK_STREAM, 0), "Unable to open a listening socket!");
    check(setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(&opt)), "SETSOCKOPT!");

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    check(bind(listeningSocket, serverptr, sizeof(server)), "Unable to bind new listening socket!");
    check(listen(listeningSocket, SERVER_BACKLOG), "Unable to listen to new socket!");

    return listeningSocket;
}


int getRandomListeningSocket(int port, int *actualPort)
{
    int listeningSocket;
    socklen_t sockLength;
    struct sockaddr_in server, client;
    struct sockaddr *serverptr = (struct sockaddr *)&server;
    struct hostent *rem;
    int opt = 1;

    check(listeningSocket = socket(AF_INET, SOCK_STREAM, 0), "Unable to open a listening socket!");
    check(setsockopt(listeningSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(&opt)), "SETSOCKOPT!");

    server.sin_family = AF_INET; /* Internet domain */
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); /* The given port */

    check(bind(listeningSocket, serverptr, sizeof(server)), "Unable to bind new listening socket!");
    check(listen(listeningSocket, SERVER_BACKLOG), "Unable to listen to new socket!");


    sockLength = sizeof(server);
    getsockname(listeningSocket, (struct sockaddr *)&server, &sockLength);

    //getting a new random port
    (*actualPort) = htons(server.sin_port);

    return listeningSocket;
}

//checks if communication function failed
//if so, prints the error in the stderror
int check(int exp, const char *msg)
{
    if (exp == SOCKETERROR)
    {
        perror(msg);
        exit(1);
    }
    return exp;
}


void getPeerIP(int peerFD, string *peerIP)
{
    struct sockaddr_in workerAddr;
    uint16_t port;
    char addressBuffer[INET_ADDRSTRLEN];
    struct in_addr workerIPAddr = workerAddr.sin_addr;
    socklen_t peerLen;
    
    //string containing the ip of the peer
    string realPeerIP;

    peerLen = sizeof(workerAddr);
    check(getpeername(peerFD, (struct sockaddr *) &workerAddr, &peerLen), "Unable to get worker IP through socket!");

    struct sockaddr_in *s = (struct sockaddr_in *)&workerAddr;
    port = ntohs(s->sin_port);
    //buffer now contains the ip of the peer
    inet_ntop(AF_INET, &s->sin_addr, addressBuffer, sizeof(addressBuffer));

    realPeerIP = string(addressBuffer);

    (*peerIP) = realPeerIP;
    return;
}

//waits till client (request or worker type) terminates its socket
//so local client socket can be closed as well
void closeRequestSocket(int givenClientSocket)
{
    char localBuffer[SOCKET_BUFFER_SIZE];

    //wait till end of file is read
    while(read(givenClientSocket, localBuffer, SOCKET_BUFFER_SIZE));
    //client closed his side of the connection, proceed to close local socket
    // shutdown(givenClientSocket, SHUT_RDWR);
    check(close(givenClientSocket), "Unable to close connection with client socket!");
    return;
}