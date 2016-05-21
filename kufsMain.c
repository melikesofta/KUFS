/**
* KUSH shell interface program
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h> //for dr
#include <fcntl.h>
#include <sys/wait.h>
#include "kufs.h"
#include "kufs.cpp"

#define MAX_LINE       80 /* 80 chars per line, per command, should be enough. */

int parseCommand(char inputBuffer[], char *args[],int *background);

/* methods added by us */
/*finds the path of a command (cmd) and writes it into pathOfCmd.
/ returns 1 if command is found, 0 if not
example: if cmd == ls, sets pathOfCmd to "/bin/ls" */
int findPathOfCommand(char* cmd,char* pathOfCmd);
void changeDirectory(char* path);
int charCounter(char* str,char ch);
void processPaths(char* firstPath[],int l1,char* secondPath[],int l2,char* mypth);
void sendStrInput(char* path,char* sign);
void cd(char* path);

	char buffer[1024];

int main(void)
{
  char inputBuffer[MAX_LINE]; 	        /* buffer to hold the command entered */
  int background;             	        /* equals 1 if a command is followed by '&' */
  char *args[MAX_LINE/2 + 1];	        /* command line (of 80) has max of 40 arguments */
  pid_t child;            		/* process id of the child process */
  int status;           		/* result from execv system call*/
  int shouldrun = 1;

  /*parameters and functions added by us*/
  int commandFound = 0;
  char *args2[MAX_LINE/2 +1]; //args array for the second child
  char path[80];
  char timeStr[20];
  char buf[20];
  const char point[2] = ".";



  while (shouldrun){            /* Program terminates normally inside setup */
    background = 0;
    shouldrun = parseCommand(inputBuffer,args,&background);       /* get next command */

    if (strncmp(inputBuffer, "exit", 4) == 0)
    shouldrun = 0;     /* Exiting from kush*/
    else if(strcmp(args[0],"cd")==0){
      char* path = malloc(sizeof(args[1]));
      path=args[1];
      changeDirectory(path);
    }

    if (shouldrun) {

			mountKUFS();
			readKUFS(1, buffer);
			//printf("%s\n", buffer);

      child = fork();
      if (child == 0) { /* child process */

        char* cmd= args[0]; /* command i.e. first word of the input */
        char pathOfCmd[100]; /* path of command will be saved into this */
        commandFound = findPathOfCommand(cmd,pathOfCmd);
        if(commandFound){
        execv(pathOfCmd,args); //voila
	      }
	      else { // if commandFound == 0
	        printf("%s: command not found.\n", cmd);
	      }
	      exit(0);
    }

  else { /* parent process */
    if(!background){

      int status;
      int status2;
      waitpid(-1, &status, 0); //wait for the children
      waitpid(0, &status2, 0);
    }
  }
}
}
return 0;
}


int findPathOfCommand(char* cmd,char pathOfCmd[]){
  DIR *directory;
  struct dirent *fileName;
  char paths[1000];
  strcpy(paths,getenv("PATH")); // getd the PATH environment variable, and copies it to paths string
  char* path = strtok(paths,":");  //Tokenizes the paths string
  while(path != NULL){
    directory = opendir(path);
    if (directory){ //if the directory exists
      while ((fileName = readdir(directory)) != NULL){
        if(strcmp(fileName->d_name,cmd)==0){
          strcpy(pathOfCmd,path);
          strcat(pathOfCmd,"/");
          strcat(pathOfCmd,cmd);
          return 1;
        }
      }
      closedir(directory);
    }
    path = strtok(NULL, ":");
  }
  return 0;
}


///CD methods

void changeDirectory(char* path){ //Assume that path is tokenizable by /
  char* combinedPWD = malloc(100);
  strcpy(combinedPWD,"/");

  char* pwd = malloc(strlen(getenv("PWD")+1));
  strcpy(pwd,getenv("PWD"));
  int leng1=charCounter(pwd,'/');
  char* parsedPWD[leng1];
  char* home = malloc(strlen(getenv("HOME")+1));
  strcpy(home,getenv("HOME"));
  int leng3 = charCounter(home,'/');
  char* parsedHome[leng3];
  /*
  printf("PWD:%s\n",pwd);
  printf("PATH:%s\n",path);
  printf("HOME:%s\n",home);
  */

  if(path != NULL){
    if(strcmp(path,"/")==0){
      combinedPWD = "/";
    }else{
      int leng2;
      if(*path=='/'){
        if(*(path+strlen(path)-1)=='/'){
          leng2 = charCounter(path,'/')-1;
        }else{
          leng2 = charCounter(path,'/');
        }
      }else{
        if(*(path+strlen(path)-1)=='/'){
          leng2 = charCounter(path,'/');
        }else{
          leng2 = charCounter(path,'/')+1;
        }
      }

      char* parsedPath[leng2];


      //Tokenize pwd
      char* p1 = strtok(pwd,"/");
      int i=0;
      while(p1 != NULL){
        parsedPWD[i]=p1;
        i++;
        p1=strtok(NULL,"/");
      }
      i=0;

      //Tokenize path
      char* p2 = strtok(path,"/");
      while(p2 != NULL){
        parsedPath[i]=p2;
        i++;
        p2=strtok(NULL,"/");
      }
      i=0;

      //Tokenize home
      char* p3 = strtok(home,"/");
      while(p3 != NULL){
        parsedHome[i]=p3;
        i++;
        p3=strtok(NULL,"/");
      }

      int k;
      int size1=0,size2=0,size3=0;
      for(k=0;k<leng1;k++){
        size1+=sizeof(parsedPWD[k]);
      }
      for(k=0;k<leng2;k++){
        size2+=sizeof(parsedPath[k]);
      }
      for(k=0;k<leng3;k++){
        size3+=sizeof(parsedHome[k]);
      }

      if(*path =='/'){ //FULL PATH CASE
        if(parsedPath[0]!=NULL){
          processPaths((char* []){""},1,parsedPath,leng2,combinedPWD);
        }
      }else if(strcmp(parsedPath[0],"~")==0){ //Home slash case
        processPaths(parsedHome,leng3,parsedPath,leng2,combinedPWD);
      }else{
        processPaths(parsedPWD,leng1,parsedPath,leng2,combinedPWD);
      }
    }
  }else{
    combinedPWD = home;
  }

  //Change Directory
  int successCD=chdir(combinedPWD);
  if(successCD==-1){
    printf("cd: %s: No such directory\n",combinedPWD);
  }else if(successCD==0){
    setenv("PWD",combinedPWD,1);
  }
}

int charCounter(char* str,char ch){
  int ctr=0;
  int i=0;
  for(i=0;i<strlen(str);i++){
    if(str[i]==ch){
      ctr++;
    }else{
    }
  }
  return ctr;
}

void processPaths(char* firstPath[],int l1,char* secondPath[],int l2,char* mypth){

  if(secondPath[0]!=NULL){
    if(strcmp(secondPath[0],"~")==0){ // firstPath is the home directory
      char* firstcombinedPath[l1+l2-1];
      int i=0;
      for(i=0;i<l1;i++){
        firstcombinedPath[i]=firstPath[i];
      }
      i=0;
      for(i=1;i<l2;i++){
        firstcombinedPath[l1+i-1]=secondPath[i];
      }
      //since none of the arrays contain "/", we use "/" as an indicator for ignoring that element of the array
      int j;
      strcpy(mypth,"/");
      for(j=0;j<l1+l2-1;j++){
        sendStrInput(mypth,firstcombinedPath[j]);
      }
    }else{
      char* firstcombinedPath[l1+l2];
      int i=0;
      for(i=0;i<l1;i++){
        firstcombinedPath[i]=firstPath[i];
      }
      i=0;
      for(i=0;i<l2;i++){
        firstcombinedPath[l1+i]=secondPath[i];
      }
      //since none of the arrays contain "/", we use "/" as an indicator for ignoring that element of the array
      int j;
      strcpy(mypth,"/");

      for(j=0;j<l1+l2;j++){
        sendStrInput(mypth,firstcombinedPath[j]);
        printf("%s\n", mypth);
        printf("%s\n", firstcombinedPath[j]);
      }
    }
  }else{
    mypth="/";
  }
}

void sendStrInput(char* path,char* sign){
  if(strcmp(sign,".")!=0){
    if(strcmp(sign,"..")==0){
      if(strcmp(path,"/")!=0){
        int i;
        for(i=strlen(path)-1;i>=0 && *(path+i)!='/';i--){
        }
        char* path2 = path;
        strncpy(path2,path,i);
        path2[i]='\0';
        path=path2;
      //  printf("%s\n",path);
      }
    }
    else{
      if(path[strlen(path)-1]!='/'){
        strcat(path,"/");
      }
      strcat(path,sign);
    }
  }
}


/**
* The parseCommand function below will not return any value, but it will just: read
* in the next command line; separate it into distinct arguments (using blanks as
* delimiters), and set the args array entries to point to the beginning of what
* will become null-terminated, C-style strings.
*/


int parseCommand(char inputBuffer[], char *args[],int *background)
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

  /**
  *  0 is the system predefined file descriptor for stdin (standard input),
  *  which is the user's screen in this case. inputBuffer by itself is the
  *  same as &inputBuffer[0], i.e. the starting address of where to store
  *  the command that is read, and length holds the number of characters
  *  read in. inputBuffer is not a null terminated C-string.
  */
  start = -1;
  if (length == 0)
  exit(0);            /* ^d was entered, end of user command stream */

  /**
  * the <control><d> signal interrupted the read system call
  * if the process is in the read() system call, read returns -1
  * However, if this occurs, errno is set to EINTR. We can check this  value
  * and disregard the -1 value
  */

  if ( (length < 0) && (errno != EINTR) ) {
    perror("error reading the command");
    exit(-1);           /* terminate with error code of -1 */
  }

  /**
  * Parse the contents of inputBuffer
  */

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
      if (inputBuffer[i] == '&') {
        *background  = 1;
        inputBuffer[i-1] = '\0';
      }
    } /* end of switch */
  }    /* end of for */

  /**
  * If we get &, don't enter it in the args array
  */

  if (*background)
  args[--ct] = NULL;

  args[ct] = NULL; /* just in case the input line was > 80 */

  return 1;

} /* end of parseCommand routine */
