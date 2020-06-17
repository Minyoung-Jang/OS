// If the comments are hard to read because of the color,
// type ':set background=dark'

/***

	This program reads texts and switches the case of the characters (lower case <-> upper case) until the user types "quit".

	Example)
		Input a text: Welcome!
		convered text = wELCOME!
		Input a text: God bless you!
		convered text = gOD BLESS YOU!
		Input a text: quit
		Bye!

	It launches a child process and communicates with it through two ordinary pipes, one to send the original text and the other to receive the converted text.

	Complete the program using ORDINARY PIPES by following the instructions.
	DO NOT use other IPC such as shared memory or message queue.

	The main creates two pipes and a child process.
	
	Then, the parent process repeats the followings until the user types "quit".
		Read a text line from the user.
		Send the text to the child through pipe.
		Receive and display the converted text.

	The child process repeats the followings until it receives "quit" from the parent. 
		Read a text line from the parent through one pipe.
		Convert all upper case characters to the corresponding lower case characters and vice versa.
		Write the converted text to the parent through the other pipe.

	Hint) To read a text line from the user, use the following code.
			printf("Input a text: ");
			fgets(src, BUFFER_SIZE - 1, stdin);	
			int len = strlen(src) - 1;
			src[len] = 0;				// trim '\n'

	Note! Before the parent terminates, there the child processing MUST terminate.
		You can check whether the child is still running by check currently running processes by 'ps -al'.

***/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define TRUE 1
#define FALSE 0

#define READ_END 0
#define WRITE_END 1

#define BUFFER_SIZE 256

void SwitchCase(int in[], int out[]);

int main()
{
	int in[2];		// pipe from parent to child
	int out[2];		// pipe from child to parent

	//	TO DO: Create two ordinary pipes
    if(pipe(in) == -1 || pipe(out) == -1){
        fprintf(stderr, "Pipe failed");
        return 1;
    }

	//  TO DO: Create child process
    pid_t child_pid;

    child_pid = fork();

    if(child_pid < 0){
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    
    if(child_pid > 0){
        char buffer[BUFFER_SIZE];
        int exitcode;
        usleep(5000);
        while(1){
            printf("Input a text: ");
            fgets(buffer, BUFFER_SIZE - 1, stdin);	
            int len = strlen(buffer) - 1;
            buffer[len] = 0;
            write(in[WRITE_END], buffer, strlen(buffer)+1);
            if(strcmp(buffer, "quit") == 0)
			    break;
            read(out[READ_END], buffer, BUFFER_SIZE);
            printf("converted = %s\n", buffer);
        }
        wait(&exitcode);
        close(in[WRITE_END]);
        close(out[READ_END]);
        fflush(stdout);
        printf("Bye!\n");
    }
    else{
        SwitchCase(in, out);
    }

    return 0;

	// On parent process,
		// Repeats the followings until the user types "quit".
			// Read a text line from the user.
			// Send the text to the child through pipe.
			// Receive and display the converted text.

		// Wait for the child to terminate
		// Deallocate pipes
		// Print a good-bye message


	// On child process call SwitchCase(in, out);
}

void SwitchCase(int in[], int out[])
{
    usleep(1);
    fflush(stdout);
    char buffer[BUFFER_SIZE];
    while(1){        
        read(in[READ_END], buffer, BUFFER_SIZE);
        usleep(1);
        if(strcmp(buffer, "quit") == 0)
			break;
        for(int i=0; buffer[i]; i++){
            if((buffer[i]>=97)&&(buffer[i]<=122)){
                buffer[i] = buffer[i]-97+65;
            }else if((buffer[i]>=65)&&(buffer[i]<=90)){
                buffer[i] = buffer[i]-65+97;
            }
        }
        int len = strlen(buffer);
        buffer[len] = 0;
        write(out[WRITE_END], buffer, strlen(buffer)+1);
	}
	// TO DO: Implement the following algorithm
	// Repeats the followings until it receives "quit" from the parent. 
		//	Receive a text line from the parent through pipe in.
		//	Convert all upper case characters to the corresponding lower case characters and vice versa.
		// 		You may use isupper(), islower(), toupper(), tolower() functions.
		//		But DO NOT use other IPC such as shared memory or message queue.
		//	Send the converted text to the parent through pipe out.
    close(out[WRITE_END]);
    close(in[READ_END]);
    fflush(stdout);
	// TO DO: deallocate pipes;
}

