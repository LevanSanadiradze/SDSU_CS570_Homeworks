#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MSG_SIZE 8

int main() {
    int fds[2];

    if (pipe(fds) == -1)
    {
        perror("Error while creating the pipe channel!");
    }

    switch (fork())
    {
        case -1:
            perror("Error, fork() returned -1, while creating child process 1");
            break;
        case 0:
            if(close(fds[0]) == -1)
            {
                perror("Error while closing the read stream of the process 1");
            }

            char msg[MSG_SIZE];
            for(int i = 0; i < MSG_SIZE; i++)
            {
                msg[i] = 'a' + (i % 5);
            }

            if(write(fds[1], msg, MSG_SIZE) < 0)
            {
                perror("Error while writing message from the process 1");
            }

            if(close(fds[1]) == -1)
            {
                perror("Error while closing the write stream of the process 1");
            }

            std::cout<<"Process 1, with PID: "<<getpid()<<std::endl<<"\tSuccessfully sent the message: "<<msg<<std::endl;
            break;
        default:
            switch (fork())
            {
                case -1:
                    perror("Error, fork() returned -1, while creating child process 2");
                    break;
                case 0:
                    if(close(fds[1]) == -1)
                    {
                        perror("Error while closing the write stream of the process 2");
                    }

                    char msg[MSG_SIZE];
                    if(read(fds[0], msg, MSG_SIZE) < 0)
                    {
                        perror("Error while reading message from the process 2");
                    }

                    if(close(fds[0]) == -1)
                    {
                        perror("Error while closing the read stream of the process 2");
                    }

                    std::cout<<"Process 2, with PID: "<<getpid()<<std::endl<<"\tSuccessfully received a message: "<<msg<<std::endl;
                    break;
            }
    }
    exit(0);
}
