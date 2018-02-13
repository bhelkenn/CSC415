#include <stdio.h>
#define NAME "Brady"

int main() {
	char* str = "Hello CSC415, my name is";
	char result[50];
	write(1, result, sprintf(result, "%s %s!\n", str, NAME));
	return 0;
}
