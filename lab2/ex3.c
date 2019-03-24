#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
int main() {
    pid_t p1, p2, p3, p4, p5;
    while ((p1 = fork()) == -1);
    if (!p1) {
        while ((p2 = fork()) == -1);
        if (!p2) {
            while ((p4 = fork()) == -1);
            if (!p4) {
                while (1){
                    printf("Node p4 is p2's child with pid %d, it's parent pid %d.\n", getpid(), getpid());
//getpid()输出当前进程id，getppid()输出当前进程的父进程的id。
                }
            }
            else{
                while ((p5 = fork()) == -1);
                if (!p5) {
                    while (1){
                        printf("Node p5 is p2's child with pid %d, it's parent pid %d.\n", getpid(), getppid());
                    }
                }
                else { ; }
            }
        while (1) {
            printf("Node p2 is p1's child with pid %d, it's parent pid %d.\n", getpid(), getppid());
        }
    }
        else { 
            while ((p3 = fork()) == -1);
                if (!p3) {
                while (1){
                    printf("Node p3 is p1's child with pid %d, it's parent pid %d.\n", getpid(), getppid());
                }
                }
        }
    while (1) {
        printf("Node p1 is a parent with pid %d, it's parent pid %d.\n", getpid(), getppid());
        }
}
    else { ; }
return 0;
}
