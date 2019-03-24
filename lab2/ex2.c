#include <unistd.h>  
#include <stdio.h>   
int main ()   
{   
    pid_t pid; 
    pid=fork();   
    if (pid < 0)   
        printf("error in fork!");   
    else if (pid == 0) {  
        execl("/usr/bin/vi","vi",NULL);
    }  
    else {  
        while(1){}
    }  
    return 0;  
}