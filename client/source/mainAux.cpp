#include "../headers/mainAux.h"

// binds the given socket to the given port
int bind_on_port(int sock, short port)
{
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    return bind(sock, (struct sockaddr *)&server, sizeof(server));
}

// initializes the data members of sock address
// corresponding to the target of new connection
void initializeServerAddr(sockaddr_in **targerAddr)
{
    //initializing a hostent struct
    //in order to extract connection's target host name
    struct in_addr myaddress;
    inet_aton(servIP->c_str() , &myaddress);

    struct hostent* rem = gethostbyaddr(( const char *)&myaddress ,sizeof(myaddress), AF_INET);

    //malloc memory for connection's target
    (*targerAddr) = (sockaddr_in *) malloc(sizeof(sockaddr_in *));
    bzero(*targerAddr, sizeof(targerAddr));


    (*targerAddr)->sin_family = AF_INET;
    (*targerAddr)->sin_port = htons(servPort);
    // assigning the target's host name we got through hostent
    memcpy(&((*targerAddr)->sin_addr), rem->h_addr, rem->h_length);

    return;
}

//returns the given arguments through its parameters
void getArguments(int argc, char **argv)
{
    string *numThreadsStr, *servPortStr;

    getArgument("-q", argc, argv, &queryFile);
    getArgument("-w", argc, argv, &numThreadsStr);
    getArgument("-sp", argc, argv, &servPortStr);
    getArgument("-sip", argc, argv, &servIP);

    numThreads = stoi(*numThreadsStr);
    servPort = stoi(*servPortStr);

    delete numThreadsStr;
    delete servPortStr;
    return;
}

// simply connects to the 
void connectToListeningSocket(int *listeningSocket, sockaddr_in *targetAddr)
{
    struct sockaddr *serverptr = (struct sockaddr *) targetAddr;
    check(connect(*listeningSocket, serverptr, sizeof(serverptr)), "Unable to connect to server");

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

//reformat file line
void getProperQuery(string fileLine, string *query)
{
    while(fileLine.back() == ' ')
    {
        fileLine.pop_back();
    }
    
    fileLine = fileLine + "\n";
    (*query) = fileLine;
    return;
}

//opens the given file and sends its query contents to server
//gets the proper answers and prints them
// void sendQueries()
// {
//     ifstream file(*queryFile);
//     string line;
//     string messageToSend;
//     string queryAnswered;

//     if(file.is_open())
//     {
//         while(getline(file, line))
//         { 
//             getProperQuery(line, &messageToSend);
//             cout << endl << "Query: " << messageToSend;
//             sendText(SOCKET_BUFFER_SIZE, *serverCommunicationSocket, messageToSend);
//             receiveText(SOCKET_BUFFER_SIZE, *serverCommunicationSocket, &queryAnswered);
//             cout << "Answer: " << queryAnswered << endl;
//         }
//     }

//     return;
// }