#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main() {
    pid_t parent = getpid();

    switch (fork())
    {
        case -1:
            perror("Error, fork() returned -1, while creating child process 1");
            break;
        case 0:
            std::cout<<"Child process 1:"<<std::endl<<"\tid: "<<getpid()<<std::endl<<"\tParent id: "<<parent<<std::endl;
            break;
        default:
            switch (fork())
            {
                case -1:
                    perror("Error, fork() returned -1, while creating child process 2");
                    break;
                case 0:
                    std::cout<<"Child process 2:"<<std::endl<<"\tid: "<<getpid()<<std::endl<<"\tParent id: "<<parent<<std::endl;
                    break;
            }
    }
    exit(0);
}
