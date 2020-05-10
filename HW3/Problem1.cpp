//Include Libraries 
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <iostream>
using namespace std;

//Signatures of functions
key_t createKey(const char*, int);
int createSharedMemory(key_t, int);
pid_t createChildProcess();
void child1Code(int);
void child2Code(int);


int main() 
{
    key_t key = createKey("Problem1.cpp", 1); //Create key
    int shmid = createSharedMemory(key, 100); //Create shared memory

    pid_t child1 = -1, child2 = -1; //Variables for children processes

    child1 = createChildProcess(); //Create child process 1
    if(child1 == 0) //if the current process is the child porcess 1
    {
        child1Code(shmid); //execute function for child process 1
        exit(0); //exit with success code
    }
    
    child2 = createChildProcess(); //Create child process 2
    if(child2 == 0) //if the current process is the child process 2
    {
        child2Code(shmid); //execute function for the child process 2
        exit(0); //exit with success code
    }

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
int createSharedMemory(key_t key, int N)
{
    int id = shmget(key, N * sizeof(int), IPC_CREAT | 0660); //create the shared memory

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
int* attachSharedMemory(int id)
{
    int* memory = (int*) shmat(id, NULL, 0); //Attach the shared memory

    if(memory == (int*)(-1)) //Check if unsuccessful
    {
        cout<<"Can't attach shared memory"<<endl; //print an error message
        exit(-1); //exit with status failed
    }

    return memory; //if successful return the memory pointer
}

//Function to detach the shared memory
void detachSharedMemory(int* memory)
{
    if(shmdt(memory) == -1) //Try to detach and check if failed
    {
        cout<<"Error while detaching the shared memory"<<endl; //print an error message
        exit(-1); //exit with status code failed
    }
}

//Function for the code for the child process 1
void child1Code(int shmid) 
{
    int* memory = attachSharedMemory(shmid); //Attach the shared memory

    srand(time(NULL)); //Seed of random

    //Populate the shared memory with random ints in range [100, 200]
    //Iterate from 99 to 0, because the child process 2 checks for the element with index 0 to be sure that this process ended population process.
    for(int i = 99; i >= 0; i--) 
    {
        memory[i] = (random() % 101) + 100; // Generate random int between [0, 100] and add 100 to remap the range to [100, 200]
    }

    detachSharedMemory(memory); //Detach the shared memory
}

//Function for the code for the child porcess 2
void child2Code(int shmid)
{
    int* memory = attachSharedMemory(shmid); //Attach the shared memory

    while(memory[0] == 0); //Wait while the child process 1 populates the memory with random ints

    int count = 0; //variable to count the number of occurances of divisible ints on 5 in the shared memory

    //count the divisible on 5 ints in the shared memory
    for(int i = 0; i < 100; i++)
    {
        if(memory[i] % 5 == 0) //check if the current element is divisible on 5
        {
            count++; //if it is, increate the count by one
        }
    }

    detachSharedMemory(memory); //detach the shared memory

    cout<<"Number of multiples of 5s is "<<count<<endl; //output the result of counting.
}