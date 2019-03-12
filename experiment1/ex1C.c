#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	write(1,"hello world\n",11);
	return 0;
}