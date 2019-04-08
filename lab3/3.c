#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

char buf[10];
char print;

int in = 0, out = 0;
int items = 0, spaces = 10;
pthread_mutex_t mutex ;
pthread_cond_t count1 = PTHREAD_COND_INITIALIZER;  // 缓冲区不满
pthread_cond_t count2 = PTHREAD_COND_INITIALIZER;
void *worker1( void *arg ) {
        //printf("this is worker1");
        while(1){       
            pthread_mutex_lock( &mutex );  
            while( items==10 ) {
                pthread_cond_wait( &count1, &mutex );
            }
            //printf("this is real worker1");
            char temp;
            scanf("%c",&temp);
            buf[in]=temp;
            in = ( in + 1 ) % 10;
            items++;spaces--;
            pthread_cond_signal( &count2 );
            pthread_mutex_unlock( &mutex );
       
    }
    pthread_exit( NULL );
}

void *worker2( void *arg ) {
    while(1){    
            //printf("this is worker2");
            pthread_mutex_lock( &mutex );
            while( spaces==10 ) {
                pthread_cond_wait( &count2, &mutex );
            }
            //printf("this is real worker2");
            //sleep(3);
            print=buf[out];
            out = ( out + 1 ) % 10;
            items--;spaces++;
            printf( "%c", print);
            pthread_cond_signal( &count1 );
            pthread_mutex_unlock( &mutex );
    }
    pthread_exit( NULL );
}

int main() {
    pthread_mutex_init(&mutex, NULL);
    pthread_t pro, con;
    pthread_create( &pro, NULL, worker1, NULL );
    pthread_create( &con, NULL, worker2, NULL );
    pthread_join( pro, NULL );
    pthread_join( con, NULL );
    return 0;
}