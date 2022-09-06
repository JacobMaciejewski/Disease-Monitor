# Disease-Monitor
Implementation of a Multi-threaded TCP  Server &amp; Client Setup for Disease Data Retrieval following a Custom Communication Protocol 🖥



## Execution & Compilation:

First, a file containing the queries to be posed, has to be produced. It is done by executing the following python script: </br>
`python3 queries.py --queryFile <Target Queries File> --numQueries <Queries Number> --commandsFile <Queries Format File> --countriesFile <Countries List File> --diseasesFile <Diseases List File> --startDate <Start Date: DD-MM-YY> --endDate <End Date: DD-MM-YY>`

In order to compile the source code, type: `make`. It can take a **substantial amount of time**! </br>
After successful compilation, three executable files are produced. They have to be executed in the following order and manner:
* **`whoServer`**
* **`master`**
* **`whoClient`**

## Server:

Set up *Server* by typing: `./whoServer -q <Listening Port> -s <Statistics Port> -w <Threads Number> -b <Buffer Size>` </br>

Server has a **Listening Port**, which is used to receive queries from the Client Process. It also owns a **Statistics Port**, that is used
to receive the statistics from the Worker Processes. The number of **Total Threads** that handle the reception and response
to the posed queries is specified. Finally, user gives the size of the **Circular Buffer**, that is used to store information about the incoming
query requesters. Server attempts to establish communication with the Worker Processes and listens to both ports for workers' positive response, client
queries, workers' answers and sends them to the client processes.

## Master:

Set up *Master* by typing: `./master -w <Workers Number> -b <Buffer Size> -i <Input Directory> -s <Server IP>  -p <Server Port>` </br>

Master Process initializes the requested number of Workers Processes, which now behave as independent servers. Sends them all the necessary information,
allowing them to traverse the **Disease Database** and store its copy locally in efficient, custom data structures. Subsequently, Master Process is only
responsible for safe worker termination and memory deallocation.

## Client:

Set up *Client* by typing: `./whoClient -q <Query File> -w <Threads Number> -sp <Server Port> -sip <Server IP>` </br>

Client initializes a group of processes that read queries from the produced file, parse them to the server and print the received answer.


### Further Information:

We include a **readME** PDF file that contains an extensive description of the project set up, explanation of design choices and data structures.
This PDF is written in Greek!

*This project is part of the course: System Programming , Spring of 2020. University of Athens, DiT.*
