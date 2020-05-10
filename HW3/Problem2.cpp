//Include Libraries 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
using namespace std;

//Signatures of functions
key_t createKey(const char*, int);
int createOrGetSharedMemory(key_t, int);
pid_t createChildProcess();
char* attachSharedMemory(int);
void detachSharedMemory(char*);
void p2Code(int);


//global variables
const char* FileToRead = (char*) "Problem2.cpp"; //File to read the characters from (For the p2 child process)

int main() 
{
    key_t key = createKey("Problem2.cpp", 1); //Create key

    pid_t child1 = -1; //Variable for child process

    child1 = createChildProcess(); //Create child process 1
    if(child1 == 0) //if the current process is the child porcess 1
    {
        int shmid = createOrGetSharedMemory(key, 50); //Create shared memory

        pid_t child2 = createChildProcess(); //Create child process 2
        if(child2 == 0) //if the current process is the child process 2
        {
            p2Code(shmid); //execute function for the child process 2
            exit(0); //exit with success code
        }

        exit(0); //exit with success code
    }

    wait(NULL); // Wait for the child process 1 to end.

    //This code will be executed by only the parent process (P)
    int shmid = createOrGetSharedMemory(key, 50); //Here the shared memory should already be created by the P1, thus this will only retrieve its id.
    char* memory = attachSharedMemory(shmid); //Attach the shared memory

    while(memory[49] == 0); //Wait while the child process 2 populates the memory with chars from file

    int count = 0; //Variable in which we will save the number of character 'a' in the shared memory
    
    //Iterate all 50 characters of the shared memory
    for(int i = 0; i < 50; i++)
    {
        if(memory[i] == 'a') //check if this element of the shared memory is 'a'
        {
            count ++; //increase if its true.
        }
        else if(memory[i] == '\0') //check if end of the character sequence is reached
        {
            break; //if that's the case, break the for loop
        }
    }

    detachSharedMemory(memory); //detach the shared memory

    cout<<"Number of character 'a' in the memory is: "<<count<<endl; //Output the number of 'a' in the shared memory

    return 0;
}

//Function to create the key
key_t createKey(const char* file, int keys)
{
    key_t key = ftok(file, 1); //create the key

    if(key == -1) //check if there was an error during key creation
    {
        cout<<"Failed to create a key!"<<endl; //print an error message
        exit(-1); //exit with failed code.
    }

    return key; //if successful return the key
}


//Function to create the shared memory
int createOrGetSharedMemory(key_t key, int N)
{
    int id = shmget(key, N * sizeof(char), IPC_CREAT | 0660); //create the shared memory

    if(id == -1) //check if shared memory creation failed
    {
        cout<<"Error while creating shared memory"<<endl; //print an error message
        exit(-1); //exit with failed code
    }

    return id; //if successful return the shared memory id
}

//Function to create the child process
pid_t createChildProcess()
{
    pid_t pid = fork(); //duplicate the current process - thus create the child process 

    if(pid == -1) //check if failed
    {
        cout<<"Error while creating child process"<<endl; //print an error message
        exit(-1); //exit with code failed
    }

    return pid; //if successful return pid
}

//Function to attach the shared memory
char* attachSharedMemory(int id)
{
    char* memory = (char*) shmat(id, NULL, 0); //Attach the shared memory

    if(memory == (char*)(-1)) //Check if unsuccessful
    {
        cout<<"Can't attach shared memory"<<endl; //print an error message
        exit(-1); //exit with status failed
    }

    return memory; //if successful return the memory pointer
}

//Function to detach the shared memory
void detachSharedMemory(char* memory)
{
    int status = shmdt(memory);
    if(status == -1) //Try to detach and check if failed
    {
        cout<<status<<endl;
        cout<<"Error while detaching the shared memory"<<endl; //print an error message
        exit(-1); //exit with status code failed
    }
}
//Function for the code for the child porcess 2
void p2Code(int shmid)
{
    char* memory = attachSharedMemory(shmid); //Attach the shared memory

    FILE* file = fopen(FileToRead, "r"); //Open the file to read from.

    //Read the first 50 character of the file
    for(int i = 0; i < 50; i++)
    {
        char nextCh = (char) fgetc(file); //Read one character from a file

        if(nextCh == EOF) //Check if for some reason End Of File is reached (maybe file to read is shorter than 50 character)
        {
            memory[i] = '\0';
            break; //break in such case.
        }

        memory[i] = nextCh; //if not EOF put the character previously read into the memory
    }

    fclose(file); //close file

    detachSharedMemory(memory); //detach the shared memory
}