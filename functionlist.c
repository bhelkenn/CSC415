#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define HW "Hello World!"

int main() {
	char str[1]; //buffer input
	int choice = 0; //integer representation of str
	pid_t pid;
    
	while (choice != 3) {
		printf("1. Hello World!\n");
		printf("2. List files\n");
		printf("3. Exit\n");
		printf("Please select: ");
		choice = atoi(fgets(str, 5, stdin));
		fflush(stdout);
    
		if (choice == 1 || choice == 2) {
			pid = fork();
			
			if (pid < 0) {
				fprintf(stderr, "Fork Failed");
				return 1;
			}
			else if (pid == 0) {
				if (choice == 1) execlp("/bin/echo", "echo %d", HW, NULL); // hello world
				else if (choice == 2) execlp("/bin/ls", "ls", NULL); // list files
			}
			else wait(NULL);
		}
		else if (choice == 3) { // exit program
			// do nothing - program will exit out of while loop and terminate
		}
    else printf("Invalid choice!");
  }
	return 0;
}                                                                                              