#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>

#define MSG_SIZE 8

int main() {
    const char* ffname = "/ff.fifo";

    if (mkfifo(ffname, 0664))
    {
        perror("Error while creating the FIFO channel");
    }

    switch (fork())
    {
        case -1:
            perror("Error, fork() returned -1, while creating child process 1");
            break;
        case 0:
            {
                int ff1 = open(ffname, O_WRONLY);

                if(ff1 == -1)
                {
                    perror("Error while opening FIFO file from process 1");
                }

                char msg[MSG_SIZE];

                for(int i = 0; i < MSG_SIZE; i++)
                {
                    msg[i] = 'a' + 2 * (i % 5);
                }

                if(write(ff1, msg, MSG_SIZE) < 0)
                {
                    perror("Error while writing message to FIFO channel from process 1");
                }

                std::cout<<"Child process 1:"<<"\tid: "<<getpid()<<"\tSuccessfully sent message: "<<msg<<std::endl;

                if(close(ff1) == -1)
                {
                    perror("Error while closing FIFO channel from process 1");
                }
            }
            break;
        default:
            switch (fork())
            {
                case -1:
                    perror("Error, fork() returned -1, while creating child process 2");
                    break;
                case 0:
                    {
                        int ff2 = open(ffname, O_RDONLY);

                        if(ff2 == -1)
                        {
                            perror("Error while opening FIFO file from process 2");
                        }

                        char msg[MSG_SIZE];

                        if(read(ff2, msg, MSG_SIZE) < 0)
                        {
                            perror("Error while reading message from FIFO channel from process 2");
                        }

                        if(close(ff2) == -1)
                        {
                            perror("Error while closing FIFO channel from process 2");
                        }

                        std::cout<<"Child process 2:"<<"\tid: "<<getpid()<<"\tSuccessfully recevied message: "<<msg<<std::endl;
                        execl("/bin/rm", "rm", ffname, NULL);
                    }
                    break;
            }
    }

    exit(0);
}
