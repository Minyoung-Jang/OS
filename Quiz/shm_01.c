// If the following instructions are hard to read on vim because of the font color, type ':set background=dark'.

/*
	This program creates a child process by fork().
	The parent reads text messages from the user, and then, sends it to the child through a global buffer.
	The child prints the message in the global buffer.
	However, this program does not work correctly.

	To fix the problem, the parent and child should communicate through a shared memory buffer instead of the global buffer.
	Complete this program using System-V shared memory API.
		(shmget(), shmat(), shmdt(), shmctl(), ...)

	You can search Internet to know how to use the System-V shared memory API functions.
	However, it is strictly prohibited to search for the solution.

*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
// struct for message buffer
typedef struct {
	char message[512];			// message buffer
	int filled;					// 1 if the message is filled. 0 otherwise.
} Buffer;

Buffer global_buffer;

void parent(int shm_id);		// function for the parent
void child(int shm_id);			// function for the child

int main()
{
	int shm_id = 0;
	// hint: allocate shared memory block for the shared buffer
	shm_id = shmget(IPC_PRIVATE, sizeof(Buffer), S_IRUSR | S_IWUSR);

	pid_t child_pid = fork();

	if(child_pid < 0){
		printf("Failed to create child process\n");
		exit(-1);
	} else if(child_pid > 0){
		parent(shm_id);
	} else {
		// child
		child(shm_id);
	}

	return 0;
}

void parent(int shm_id)
{
	int exitcode;
	Buffer *buffer = shmat(shm_id, NULL, 0);		// hint: modify this line
	buffer->filled = 0;

	sleep(2);			// wait for the child to start
	printf("[parent] Input a message and I'll send to the child.\n");

	while(1){			// DO NOT print any message in this loop.
		char message[512];
		message[511] = 0;	// for safety

		fgets(message, 512, stdin);
		message[strlen(message) - 1] = 0;	// trim '\n'

		while(buffer->filled)
			usleep(50000);

		strcpy(buffer->message, message);
		buffer->filled = 1;

		if(strcmp(message, "quit") == 0)
			break;
	}

	// hint: put some code here
	wait(&exitcode);
	shmdt(buffer);
	
	printf("[parent] Terminating.\n");
	fflush(stdout);
}

void child(int shm_id)
{
	sleep(1);			// wait for the parent to initialize the shared buffer

	printf("[child] Started\n");
	fflush(stdout);


	Buffer *buffer = shmat(shm_id, NULL, 0);		// hint: modify this line

	while(1){
		while(!buffer->filled)
			usleep(50000);

		if(buffer->message[0]){
			printf("[child] %s\n", buffer->message);
			fflush(stdout);
			buffer->filled = 0;
		}

		if(strcmp(buffer->message, "quit") == 0)
			break;
	}

	// hint: put some code here
	shmdt(buffer);
	shmctl(shm_id, IPC_RMID, NULL);

	printf("[child] Terminating.\n");
	fflush(stdout);
}

