//Written by Meric Melike Softa and Firtina Kucuk on 21.05.2016
//for the 3rd class project of Comp304 - Operating Systems

//This file implements a simple shell interface only supporting the
//commands given in the problem statement.

//This shell interface was implemented based on our code from Project 1, KUSH.

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include "kufs.h"
#include "kufs.cpp"

#define MAX_LINE       80 /* 80 chars per line, per command, should be enough. */

int parseCommand(char inputBuffer[], char *args[]);
char buffer[1024];

int main(void)
{
  char inputBuffer[MAX_LINE]; 	        /* buffer to hold the command entered */
  char *args[MAX_LINE/2 + 1];	        /* command line (of 80) has max of 40 arguments */
  pid_t child;            		/* process id of the child process */
  int status;           		/* result from execv system call*/
  int shouldrun = 1;

	mountKUFS();

  while (shouldrun){            /* Program terminates normally inside setup */
    shouldrun = parseCommand(inputBuffer,args);       /* get next command */

    if (strncmp(inputBuffer, "exit", 4) == 0){
    shouldrun = 0;     /* Exiting from kush*/
		}
    else {

			readKUFS(5, buffer);
		//	printf("%s\n", buffer);

      child = fork();
      if (child == 0) { /* child process */

        char* cmd= args[0]; /* command i.e. first word of the input */

    		if(strcmp(args[0], "ls")==0){

				}
				else if(strcmp(args[0], "cd")==0){

				}
				else if(strcmp(args[0], "md")==0){

				}
				else if(strcmp(args[0], "rd")==0){

				}
				else if(strcmp(args[0], "stats")==0){

				}
				else if(strcmp(args[0], "display")==0){

				}
				else if(strcmp(args[0], "create")==0){

				}
				else if(strcmp(args[0], "rm")==0){

				}

	      else { // if commandFound == 0
	        printf("%s: command not found.\n", cmd);
	      }
	      exit(0);
    }

		else { /* parent process */
			int status;
			int status2;
			waitpid(-1, &status, 0); //wait for the children
			waitpid(0, &status2, 0);
		}
	}
}
return 0;
}

// the following function is directly taken from KUSH project

int parseCommand(char inputBuffer[], char *args[])
{
  int length,		/* # of characters in the command line */
  i,		/* loop index for accessing inputBuffer array */
  start,		/* index where beginning of next command parameter is */
  ct,	        /* index of where to place the next parameter into args[] */
  command_number;	/* index of requested command number */

  ct = 0;

  /* read what the user enters on the command line */
  do {
    printf("kush>");
    fflush(stdout);
    length = read(STDIN_FILENO,inputBuffer,MAX_LINE);
  }
  while (inputBuffer[0] == '\n'); /* swallow newline characters */

  start = -1;
  if (length == 0)
  exit(0);

  if ( (length < 0) && (errno != EINTR) ) {
    perror("error reading the command");
    exit(-1);           /* terminate with error code of -1 */
  }


  for (i=0;i<length;i++) {
    /* examine every character in the inputBuffer */

    switch (inputBuffer[i]){
      case ' ':
      case '\t' :               /* argument separators */
      if(start != -1){
        args[ct] = &inputBuffer[start];    /* set up pointer */
        ct++;
      }
      inputBuffer[i] = '\0'; /* add a null char; make a C string */
      start = -1;
      break;

      case '\n':                 /* should be the final char examined */
      if (start != -1){
        args[ct] = &inputBuffer[start];
        ct++;
      }
      inputBuffer[i] = '\0';
      args[ct] = NULL; /* no more arguments to this command */
      break;

      default :             /* some other character */
      if (start == -1)
      start = i;
    } /* end of switch */
  }    /* end of for */

  args[ct] = NULL; /* just in case the input line was > 80 */

  return 1;

} /* end of parseCommand routine */
