#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>


int main(){
	pid_t x = 1;
	int pid;

	pid = fork();
	printf("run after the fork()");
	if(pid == 0){
		printf("child");
		exit(0);
	}

	printf("parent");
	
}
