#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#define MAX_LINE 80

void setup (char inputBuffer[], char *args[], int *background)
{
	int length, 	/* # of chars in command line */
	    i,		/* loop index for buffer array*/
	    start,	/* index where beginning of next command parameter*/
	    ct;		/* index of where to place the next parameter*/

	ct = 0;
	// read what the user enters on the next line
	length = read(STDIN_FILENO, inputBuffer, MAX_LINE);

	start = -1;
	if (length == 0)
	{
		exit(0);	//exit then ^d
	}

	if (length < 0) 
	{
		perror("error reading the command");
		exit (-1);
	}

	//examine every char in inputBuffer
	for (i = 0; i < length; i ++)
	{
		switch (inputBuffer[i])
		{
			case ' ':
			case '\t':	//seperte arguments
				if(start != -1)
				{
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				start = -1;
				break;	
			case '\n':	
				if(start != -1)
				{
					args[ct] = &inputBuffer[start];
					ct++;
				}
				inputBuffer[i] = '\0';
				args[ct] = NULL;	//no more arguments in the line
				break;		
			case '&':
				*background = 1;
				inputBuffer[i] = '\0';
				break;
			default:
				if (start == -1)
				{
					start = i;	
				}

		}	
	}
	args[ct] = NULL;
}

int main(void)
{
	char inputBuffer[MAX_LINE];	// buffer for command entered
	int background;			// equals 1 if the command is followed by a &
	char *args[MAX_LINE/2 + 1];	// cli arguments
	pid_t pid;
	while (1) 
	{
		background = 0;
		printf(" COMMAND->");
		fflush(stdout);
		setup(inputBuffer, args, &background);

		//fork a new process
		pid = fork();


		//error has occured	
		if (pid < 0)		
		{
			fprintf(stderr, "Fork Failed");
			return 1;
		}

		//child process
		else if(pid == 0)
		{
			execvp(inputBuffer, args);
		}

		//parent process
		else
		{

			//wait if background is 0
			if (background == 0)
			{
				wait(NULL);
			}

			//run setup if background isnt 0
			else
			{
				setup (inputBuffer, args, &background);
			}
		}
	}
	return 0;
}
