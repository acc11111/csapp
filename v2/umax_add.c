#include<stdio.h>
#include<limits.h>

int main(){
	unsigned int a = UINT_MAX;
	a = a + 1;
	printf("%u",a);
	return 0;
}
