#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h> 
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <pthread.h>
#include <time.h>
#include <iostream>
#include <string> 
#include <errno.h>
#include <sys/ipc.h>
using namespace std;

//Function signatures
int getFirstChoice();
int getSecondChoice();
int getThirdChoice();
void prepareDataTransmissionChannel();
void deleteFifoFile();
void createChildrenProcesses();

//Global variables
int Choice1, Choice2, Choice3;
const char* FifoFile = "/fifofile.fifo";
const char* ThisFileName = "Project.cpp";
const char* ReadDataFile = "Project.cpp";
int N = 50;

int Pipe_filedes[2];
int SharedMemoryID;
pthread_mutex_t Lock1, Lock2;
int Thread1Res = 0;
const char* Thread2DestinationFile = "./Output.txt";

int main()
{
    cout<<"Hello user!,"<<endl<<"Please follow the instructions below:"<<endl<<endl; //Print welcome message

    //Get user inputs
    Choice1 = getFirstChoice();
    Choice2 = getSecondChoice();
    Choice3 = getThirdChoice();

    prepareDataTransmissionChannel(); //Prepare data transmission channel based on user input
    createChildrenProcesses(); //Create children processes

    wait(NULL); //Wait for first child process to end
    wait(NULL); //Wait for second child process to end if it exists else this will immediately return -1

    if(Choice2 == 1)
    {
        deleteFifoFile(); //Try delete the fifo file, when it's not needed anymore
    }
}

//Function to get the first input
int getFirstChoice()
{
    int choice = -1; //Variable for the choice
    string temp; //Temporary variable

    do{
        //Print instructions
        cout<<"Please choose number of children proccesses."<<endl;
        cout<<"Possible choices: "<<endl;
        cout<<"\t1 - One child process"<<endl;
        cout<<"\t2 - Two child process"<<endl;
        cin>>temp; //Get user input
        cout<<endl;

        if(temp[0] != '1' && temp[0] != '2') //Check if user input is invalid
        {
            cout<<"ERROR: incorrect input"<<endl<<endl; //output error if invalid
        }
        else
        {
            choice = temp[0] - '0'; //If user input was valid, turn the inputs first character to int
        }
        
    } while(choice == -1); //if user input was not valid, try again

    return choice; //return the result
}

//Function to get the second input
int getSecondChoice()
{
    int choice = -1; //Variable for the choice
    string temp; //Temporary variable

    do{
        //Print instructions
        cout<<"Please choose the communication method."<<endl;
        cout<<"Possible choices: "<<endl;
        cout<<"\t1 - FIFO"<<endl;
        cout<<"\t2 - PIPE"<<endl;
        cout<<"\t3 - Shared Memory"<<endl;
        cin>>temp; //Get user input
        cout<<endl;

        if(temp[0] != '1' && temp[0] != '2' && temp[0] != '3') //Check if user input is invalid
        {
            cout<<"ERROR: incorrect input"<<endl<<endl; //output error if invalid
        }
        else
        {
            choice = temp[0] - '0'; //If user input was valid, turn the inputs first character to int
        }
        
    } while(choice == -1); //if user input was not valid, try again

    return choice; //return the result
}

//Function to get the second input
int getThirdChoice()
{
    int choice = -1; //Variable for the choice
    string temp; //Temporary variable

    do{
        //Print instructions
        cout<<"Please choose the data source."<<endl;
        cout<<"Possible choices: "<<endl;
        cout<<"\t1 - Read data from file"<<endl;
        cout<<"\t2 - Generate random values"<<endl;
        cin>>temp; //Get user input
        cout<<endl;

        if(temp[0] != '1' && temp[0] != '2') //Check if user input is invalid
        {
            cout<<"ERROR: incorrect input"<<endl<<endl; //output error if invalid
        }
        else
        {
            choice = temp[0] - '0'; //If user input was valid, turn the inputs first character to int
        }
        
    } while(choice == -1); //if user input was not valid, try again

    return choice; //return the result
}

void deleteFifoFile()
{
    execl("/bin/rm", "rm", FifoFile, NULL);
}

//Function to initialize fifo channel
void initFifo()
{
    if(mkfifo(FifoFile, 0664)) //Try to create fifo file and check if failed
    {
        cout<<"Error while creating FIFO file"<<endl; //Print an error message
        exit(EXIT_FAILURE); //Exit with code failure
    }
}

//Function to initialize pipe channel
void initPipe()
{
    int status = pipe(Pipe_filedes); //Try to create pipe

    if(status == -1) //if failed
    {
        cout<<"Error while creating PIPE"<<endl; //print an error message
        exit(EXIT_FAILURE); //exit with code failure
    }
}

//Function to initialize shared memory
void initSharedMemory()
{
    key_t key = ftok(ThisFileName, 1); //Try to create shared memory key

    if(key == -1) //if failed
    {
        cout<<"Error while creating the shared memory key"<<endl; //print an error message
        exit(EXIT_FAILURE); //exit with code failure
    }

    SharedMemoryID = shmget(key, N * sizeof(char), IPC_CREAT | 0660); // Try to create shared memory

    if(SharedMemoryID == -1) //if failed
    {
        cout<<"Error while creating the shared memory"<<endl; //print an error message
        exit(EXIT_FAILURE); //exit with code failure
    }
}

//Function to prepare the data transmission channel
void prepareDataTransmissionChannel()
{
    switch(Choice2) //Based on user choice
    {
        case 1:
            initFifo(); //Initialize fifo
            break;
        case 2:
            initPipe(); //Initialize pipe
            break;
        case 3:
            initSharedMemory(); //Initialize shared memory
            break;
    }
}

//Function to read data from file
void readDataFromFile(char* data)
{
    FILE* file = fopen(ReadDataFile, "r"); //Open File

    if(file == NULL) //If couldn't open file
    {
        cout<<"Error while opening file to read data"<<endl; //print error
        exit(EXIT_FAILURE); //exit with failure code
    }

    for(int i = 0; i < N; i++) //Loop through N characters
    {
        char nextCh = (char) fgetc(file); //Read one character from a file

        if(nextCh == EOF) //Check if for some reason End Of File is reached (maybe file to read is shorter than N character)
        {
            data[i] = '\0';
            break; //break in such case.
        }

        data[i] = nextCh; //if not EOF put the character previously read into the memory
    }
    data[N] = '\0'; //Add end character

    fclose(file); //Close file
}

//Function to generate random data
void generateRandomData(char* data)
{
    srand(time(NULL)); //Seed of random

    for(int i = 0; i < N; i++) //Loop through all the elements
    {
        data[i] = (random() % 26) + 97; //Generate random int in range [97, 122]
    }

    data[N] = '\0'; //add end character
}

//Attach a shared memory
char* attachSharedMemory()
{
    char* memory = (char*) shmat(SharedMemoryID, NULL, 0); //Try to attach the shared memory

    if(memory == (char*) (-1)) //if failed
    {
        cout<<"Error while attaching the shared memory"<<endl; //print an error message
        exit(EXIT_FAILURE); //exit with code failure
    }
    
    return memory;
}

//Detach the shared memory
void detachSharedMemory(char* memory)
{
    int status = shmdt(memory);
    if(status == -1) //Try to detach the shared memory and check if failed
    {
        cout<<"Error while detaching the shared memory"<<endl; //if failed print an error message
        exit(EXIT_FAILURE); //exit with code failure
    }
}

//Send data through fifo channel
void sendDataThroughFifo(char* data)
{
    int writeChannel = open(FifoFile, O_WRONLY); //Try to open Write channel of fifo
    
    if(writeChannel == -1) //if failed
    {
        cout<<"Failed to open FIFO Write channel"<<endl;
        exit(EXIT_FAILURE); //exit with failure code
    }

    if(write(writeChannel, data, N) < 0) //Try to write data to the channel and check if failed
    {
        cout<<"Error while writing data into FIFO Write Channel"<<endl; //If failed print error message;
        exit(EXIT_FAILURE); //Exit with failure code
    }

    close(writeChannel); //close the fifo channel
}

//Send data through PIPE channel
void sendDataThroughPipe(char* data)
{
    if(close(Pipe_filedes[0]) == -1) //Try to close the unused read stream
    {
        cout<<"Error while closing the unused FIFO read stream"<<endl; //If failed print error message
        exit(EXIT_FAILURE); //exit with failure code
    }

    if(write(Pipe_filedes[1], data, N) < 0) //Try to write data to stream
    {
        cout<<"Failed to write data to the FIFO stream"<<endl; //If failed print error message
        exit(EXIT_FAILURE); //exit with failure code
    }

    if(close(Pipe_filedes[1]) == -1) //Try to close thewrite stream
    {
        cout<<"Error while closing the FIFO write stream"<<endl; //If failed print error message
        exit(EXIT_FAILURE); //exit with failure code
    }
}

//Send data through Shared Memory channel
void sendDataThroughSharedMemory(char* data)
{
    char* memory = attachSharedMemory(); //attach the shared memory

    for(int i = 0; i < N; i++)
    {
        if(data[i] == '\0') //If the current character is the end character
        {
            break; //then brake
        }

        memory[i] = data[i]; //write current character into the shared memory
    }

    detachSharedMemory(memory); //Detach the shared memory
}

//Function to send data
void sendData(char* data)
{
    switch(Choice2) //Based on user choice 
    {
        case 1:
            sendDataThroughFifo(data); //Send data through fifo
            break;
        case 2:
            sendDataThroughPipe(data); //Send data through pipe
            break;
        case 3:
            sendDataThroughSharedMemory(data); //Send data through shared memory
            break;
    }  
}

//Function for reader process
void readerProcess()
{
    char data[N + 1]; //Data container

    switch(Choice3) //Based on user choice
    {
        case 1:
            readDataFromFile(data); //Read data from file
            break;
        case 2:
            generateRandomData(data); //Randomly generate data
            break;
    }
    
    sendData(data);
}

//Read data from fifo channel
void readDataFromFifo(char* data)
{
    int readChannel = open(FifoFile, O_RDONLY); //Try to open Read channel of fifo
    
    if(readChannel == -1) //if failed
    {
        cout<<"Failed to open FIFO read channel"<<endl;
        exit(EXIT_FAILURE); //exit with failure code
    }

    if(read(readChannel, data, N) < 0) //Try to read data from the channel and check if failed
    {
        cout<<"Error while reading data from FIFO read Channel"<<endl; //If failed print error message;
        exit(EXIT_FAILURE); //Exit with failure code
    }

    close(readChannel); //close the fifo channel
}

//Read data from pipe channel
void readDataFromPipe(char* data)
{
    if(close(Pipe_filedes[1]) == -1) //Try to close the unused write stream
    {
        cout<<"Error while closing the unused FIFO write stream"<<endl; //If failed print error message
        exit(EXIT_FAILURE); //exit with failure code
    }

    if(read(Pipe_filedes[0], data, N) < 0) //Try to read data from the stream
    {
        cout<<"Failed to read data from the FIFO stream"<<endl; //If failed print error message
        exit(EXIT_FAILURE); //exit with failure code
    }

    if(close(Pipe_filedes[0]) == -1) //Try to close the read stream
    {
        cout<<"Error while closing the FIFO read stream"<<endl; //If failed print error message
        exit(EXIT_FAILURE); //exit with failure code
    }
}

//Read data from shared memory
void readDataFromSharedMemory(char* data)
{
    char* memory = attachSharedMemory(); //Attach the shared memory

    while(memory[0] == 0);

    for(int i = 0; i < N; i++)
    {
        if(memory[i] == '\0') //If the current character is the end character
        {
            break; //then brake
        }

        data[i] = memory[i]; //write current character from the shared memory to data
    }

    detachSharedMemory(memory); //detach the shared memory
}

//function to receive data
void receiveData(char* data)
{
    switch(Choice2) //Based on user choice 
    {
        case 1:
            readDataFromFifo(data); //receive data through fifo
            break;
        case 2:
            readDataFromPipe(data); //receive data through pipe
            break;
        case 3:
            readDataFromSharedMemory(data); //receive data through shared memory
            break;
    }  
}

//Function to initialize mutex
void initMutexLock(pthread_mutex_t* lock)
{
    int status = pthread_mutex_init(lock, NULL); //Initialize mutex lock

    if(status != 0) //Check if initialization of the lock failed
    {
        cout<<"Error while initializing mutex lock for pthread"<<endl; //Output error message
        exit(EXIT_FAILURE); //Exit with failure code
    }
}

//Function for thread 1
void* thr1Code(void* arg)
{
    pthread_mutex_lock(&Lock1); //Lock

    char* arr = (char*) arg; //Cast arg to char*

    for(int i = 0; i < N; i++) //Loop through the array
    {
        if(arr[i] >= 'i' && arr[i] <= 's') //Check if current character is between i and s
        {
            Thread1Res += 1; //if it is, increate by the Thread1Res by 1
        }
    }

    pthread_mutex_unlock(&Lock1); //unlock
    pthread_exit(NULL); //exit thread
}

//Function for thread 2
void* thr2Code(void* arg)
{
    pthread_mutex_lock(&Lock2); //Lock

    char* arr = (char*) arg; //Cast arg to char*

    FILE* file = fopen(Thread2DestinationFile, "w");

    if(file != NULL) //if file opened successfully
    {
        for(int i = 0; i < N; i++) //Loop through the array
        {
            fputc(arr[i], file);
        }

        fclose(file); //Close the file
    }
    else //if failed to open file
    {
        cout<<"Error while opening the file to write"<<endl; //print error message
    }

    pthread_mutex_unlock(&Lock2); //unlock
    pthread_exit(NULL); //exit thread
}

//Function for destination process
void destinationProcess()
{
    char data[N]; //Data container
    receiveData(data); //Receive data from the user chosen channel

    pthread_t thr1, thr2; //Thead identification variables
    
    initMutexLock(&Lock1); //Initialize Mutex Lock for thread 1
    initMutexLock(&Lock2); //Initialize Mutex Lock for thread 2

    pthread_create(&thr1, NULL, thr1Code, (void*) &data); //Create thread 1
    pthread_create(&thr2, NULL, thr2Code, (void*) &data); //Create thread 2
    
    pthread_join(thr1, NULL); //Wait for thread 1 to finish
    pthread_join(thr2, NULL); //Wait for thread 2 to finish

    pthread_mutex_destroy(&Lock1); //Destroy the mutex lock for thread 1
    pthread_mutex_destroy(&Lock2); //Destroy the mutex lock for thread 2

    cout<<"Number of character between i and s: "<<Thread1Res<<endl;
}

//Function to create children processes
void createChildrenProcesses()
{
    //Spawn destination child process
    switch(fork())
    {
        case -1: //If failed 
            cout<<"Error while creating destination child process"<<endl; //Print an error message
            exit(EXIT_FAILURE); //exit with failure code.
            break;
        case 0: //If current instance is destination child process
            destinationProcess(); //execute the destination code
            exit(EXIT_SUCCESS); //Exit with success code;
            break;
    }

    if(Choice1 == 1) //If User chose 1 child process
    {
        readerProcess(); //Run reader code in the main process
    }
    else //If user chose 2 children processes
    {
        //Spawn reader child process
        switch(fork())
        {
            case -1: //If failed 
                cout<<"Error while creating destination child process"<<endl; //Print an error message
                exit(EXIT_FAILURE); //exit with failure code.
                break;
            case 0: //if current isntace is reader child process
                readerProcess(); //Execute reader process code
                exit(EXIT_SUCCESS); //Exit with success code;
                break;
        }
    }
}