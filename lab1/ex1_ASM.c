#include <stdio.h>
#include <unistd.h>

int main (){
	pid_t pid;
	
	asm volatile(
		"mov $0,%%ebx\n\t"
		"mov $0x14,%%eax\n\t"
		"int $0x80\n\t"
		"mov %%eax,%0\n\t"
		:"=m"(pid)
	);

	printf("%d\n", pid);
	return 0;
}