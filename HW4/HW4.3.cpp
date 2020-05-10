#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <iostream>
using namespace std;

//Function signatures
void randomCharacters(char*, int);
void initMutexLock(pthread_mutex_t*);
void* thr1Code(void*);
void* thr2Code(void*);

//Global variables
int ArrLen = 100;
pthread_mutex_t Lock1, Lock2;
int Thr1Res = 0, Thr2Res = 0;

int main()
{
    char arr[ArrLen + 1] = { 0 }; // Assign zeroes to the array
    randomCharacters(arr, ArrLen); //Initialize random char array
    cout<<"The generated random string:"<<endl<<arr<<endl<<endl; //print the generated random character sequence

    pthread_t thr1, thr2; //Thead identification variables
    
    initMutexLock(&Lock1); //Initialize Mutex Lock for thread 1
    initMutexLock(&Lock2); //Initialize Mutex Lock for thread 2

    pthread_create(&thr1, NULL, thr1Code, (void*) &arr); //Create thread 1
    pthread_create(&thr2, NULL, thr2Code, (void*) &arr); //Create thread 2
    
    pthread_join(thr1, NULL); //Wait for thread 1 to finish
    pthread_join(thr2, NULL); //Wait for thread 2 to finish

    pthread_mutex_destroy(&Lock1); //Destroy the mutex lock for thread 1
    pthread_mutex_destroy(&Lock2); //Destroy the mutex lock for thread 2

    cout<<"The number of character 'q' is: "<<Thr1Res<<endl;
    cout<<"The number of m*n (3 character sequence) is: "<<Thr2Res<<endl; //Output the results

    exit(EXIT_SUCCESS); //Exit with success code
}

//Function to generate random array of chars
void randomCharacters(char* arr, int N)
{
    srand(time(NULL)); //Seed of random

    for(int i = 0; i < N; i++) //Loop through all the elements
    {
        arr[i] = (random() % 26) + 97; //Generate random int in range [97, 122]
    }
    arr[N] = '\0'; //add the end character
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

//Function for thread1
void* thr1Code(void* arg)
{
    pthread_mutex_lock(&Lock1); //Lock

    char* arr = (char*) arg; //Cast arg to char*

    for(int i = 0; i < ArrLen; i++) //Loop through the array
    {
        if(arr[i] == 'q') //Check if current character is 'q'
        {
            Thr1Res += 1; //if it is, increate by the Thr1Res by 1
        }
    }

    pthread_mutex_unlock(&Lock1); //unclock
    pthread_exit(NULL); //exit thread
}

//Function for thread 2
void* thr2Code(void* arg)
{
    pthread_mutex_lock(&Lock2); //Lock

    char* arr = (char*) arg; //Cast arg to char*

    for(int i = 2; i < ArrLen; i++) //Loop through the array
    {
        if(arr[i] == 'n' && arr[i - 2] == 'm') //Find a m*n three character sequence
        {
            Thr2Res += 1; //If found, increase Thr2Res by 1
        }
    }

    pthread_mutex_unlock(&Lock2); //unlock
    pthread_exit(NULL); //exit thread
}