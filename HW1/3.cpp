#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MSG_SIZE 8

int main() {
    int fds[2][2];

    if (pipe(fds[0]) == -1)
    {
        perror("Error while creating the pipe-0 channel!");
    }

    if (pipe(fds[1]) == -1)
    {
        perror("Error while creating the pipe-1 channel!");
    }

    switch (fork())
    {
        case -1:
            perror("Error, fork() returned -1, while creating child process 1");
            break;
        case 0:
            if(close(fds[0][0]) == -1)
            {
                perror("Error while closing the read stream of the process 1 pipe-0");
            }

            if(close(fds[1][1]) == -1)
            {
                perror("Error while closing the write stream of the process 1 pipe-1");
            }

            char msg1[MSG_SIZE], msg2[MSG_SIZE];

            //Write
            for(int i = 0; i < MSG_SIZE; i++)
            {
                msg1[i] = 'a' + (i % 5);
            }

            if(write(fds[0][1], msg1, MSG_SIZE) < 0)
            {
                perror("Error while writing message to the process 1 pipe-0");
            }

            if(close(fds[0][1]) == -1)
            {
                perror("Error while closing the write stream of the process 1 pipe-0");
            }

            //Read
            if(read(fds[1][0], msg2, MSG_SIZE) < 0)
            {
                perror("Error while reading message from the process 1 pipe-1");
            }

            if(close(fds[1][0]) == -1)
            {
                perror("Error while closing the read stream of the process 1 pipe-1");
            }

            std::cout<<"Process 1, with PID: "<<getpid()<<"\tSuccessfully sent the message: "<<msg1<<"\tSuccessfully received a message: "<<msg2<<std::endl;
            break;
        default:
            switch (fork())
            {
                case -1:
                    perror("Error, fork() returned -1, while creating child process 2");
                    break;
                case 0:
                    if(close(fds[0][1]) == -1)
                    {
                        perror("Error while closing the write stream of the process 2 pipe-0");
                    }

                    if(close(fds[1][0]) == -1)
                    {
                        perror("Error while closing the read stream of the process 2 pipe-1");
                    }

                    char msg1[MSG_SIZE], msg2[MSG_SIZE];

                    //Write
                    for(int i = 0; i < MSG_SIZE; i++)
                    {
                        msg1[i] = 'z' - (i % 5);
                    }

                    if(write(fds[1][1], msg1, MSG_SIZE) < 0)
                    {
                        perror("Error while writing message to the process 2 pipe-1");
                    }

                    if(close(fds[1][1]) == -1)
                    {
                        perror("Error while closing the write stream of the process 2 pipe-1");
                    }

                    //Read
                    if(read(fds[0][0], msg2, MSG_SIZE) < 0)
                    {
                        perror("Error while reading message from the process 2 pipe-0");
                    }

                    if(close(fds[0][0]) == -1)
                    {
                        perror("Error while closing the read stream of the process 2 pipe-0");
                    }

                    std::cout<<"Process 2, with PID: "<<getpid()<<"\tSuccessfully sent the message: "<<msg1<<"\tSuccessfully received a message: "<<msg2<<std::endl;
                    break;
            }
    }
    exit(0);
}
