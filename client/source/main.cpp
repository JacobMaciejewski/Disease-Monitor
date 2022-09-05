#include "../headers/mainAux.h"

#define MAXMSG 512

// arguments given by the user
int numThreads;
int servPort;
string *queryFile;
string *servIP;

// query counter
int currentQuery;

// file stream mutex
void *makeRequest(void*);
pthread_mutex_t stream_mutex;
pthread_mutex_t print_mutex;


int main(int argc, char *argv[])
{
    pthread_t *threadPool;
    //initializing stream and print mutex
    pthread_mutex_init(&stream_mutex, NULL);
    pthread_mutex_init(&print_mutex, NULL);

    if (argc != 9)
    {
        cout << "Invalid number of arguments!" << endl;
        exit(1);
    }
    //getting the user arguments
    getArguments(argc, argv);
    //initializing the query counter
    currentQuery = 1;
    //opening the file from main thread
    ifstream file(*queryFile);

    //initializing client thread, each one will take care of one query
    threadPool = (pthread_t*) malloc(sizeof(pthread_t) * numThreads);
    for(int index = 0; index < numThreads; index++)
    {
        pthread_create(&(threadPool[index]), NULL, makeRequest, &file);
    }

    for(int index = 0; index < numThreads; index++)
    {
        pthread_join(threadPool[index], NULL);
    }
  
    free(threadPool);
    delete queryFile;
    delete servIP;
}

void *makeRequest(void *arg) 
{ 

    ifstream *queryFile = (ifstream*) arg;
    string line;
    string messageToSend;
    string queryAnswered;
    int newSocket;
    //checking if we got to the end of file
    bool endOfFile;

    do{
        pthread_mutex_lock(&stream_mutex);
        getline(*queryFile, line);
        endOfFile = queryFile->eof();
        pthread_mutex_unlock(&stream_mutex);

        if(!endOfFile)
        {
            getProperQuery(line, &messageToSend);
            //open a new connection with server
            newSocket = establishCommunicationSocket(servIP, servPort, true);

            //getting a new socket for communication with server
            sendText(SOCKET_BUFFER_SIZE, newSocket, messageToSend);
            receiveText(SOCKET_BUFFER_SIZE, newSocket, &queryAnswered);

            //printing the answer to the query
            pthread_mutex_lock(&print_mutex);
            cout << endl << "Query: " << messageToSend;
            cout << "Answer: " << queryAnswered << endl;
            pthread_mutex_unlock(&print_mutex);
            // shutdown(newSocket, SHUT_RDWR);
            close(newSocket);
        }

    }while(!endOfFile);


    return NULL;
} 