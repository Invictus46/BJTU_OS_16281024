#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

sem_t* sem1 = NULL;
sem_t* sem4 = NULL;
sem_t* sem2 = NULL;

int main(){
    pid_t p,p_pid;
    sem1 = sem_open("1", O_CREAT, 0666, 0);
    sem4 = sem_open("4", O_CREAT, 0666, 0);
    sem2 = sem_open("2", O_CREAT, 0666, 0);
    p_pid = getpid();
    p = fork();
    p = fork();
    if (p>0){
        if(getpid() != p_pid){
            p=fork();
            if(p==0){
                sem_wait(sem1);
                sem_wait(sem4);
                printf("I am the process 4\n");
            }
            else{
                printf("I am the process 1\n");
                sem_post(sem2);
            }
        }
    }
    else if (p==0){
        if(getppid()!=p_pid){
            sem_wait(sem2);
            printf("I am the process 3\n");
            sem_post(sem2);
            sem_post(sem4);
        }
        else{
            sleep(1);
            sem_wait(sem2);
            printf("I am the process 2\n");
            sem_post(sem2);
            sem_post(sem1);
        }
    }
    sleep(1);
    sem_close(sem1);
    sem_close(sem4);
    sem_close(sem2);
    sem_unlink("1");
    sem_unlink("4");
    sem_unlink("2");
    return 0;
}