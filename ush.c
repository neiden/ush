/* CS 347 -- Mini Shell!  
 * Original author Phil Nelson 2000
 * Modified by Aran Clauson 2019
 */
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>


/* CONSTANTS */ 

/* PROTOTYPES */
/* Process Line
 * line  The command to process
 * 
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.  
 */
void processline (char *line);

int numElements(char* line){
    char* p = line;
    int count = 0;
    while(*p != '\0'){
        if((*p != 32 && (*(p+1) == 32 || *(p+1) == '\0'))){
            ++count;
        }
        ++p;
    }
    return count;
}

char** argparse(char* line){
    
    char n[(strlen(line) + 1)];
    strcpy(n, line);
    int elements = numElements(n) + 1;
    char* x = n;
    int count = 0;
    
    char** args = (char**)malloc(elements*sizeof(char*));
    
    while(*x != '\0'){
        while(*x == 32){
            ++x;
        }
        args[count] = x;
        ++count;
        while(*x != 32 && *x != '\0'){
            ++x
        }
        if(*x != '\0'){
            x[0] = '\0';
            ++x;
        }
    }
    if(*line != '\0'){
        args[count] = x;
    }
}



/* Get Input
 * line     A pointer to a char* that points at a buffer of size *size or NULL.
 * size     The size of the buffer *line or 0 if *line is NULL.
 * returns  The length of the string stored in *line.  
 *
 * This function prompts the user for input.  If the input fits in the buffer
 * pointed to by *line, the input is placed in *line.  However, if there is not
 * enough room in *line, *line is freed and a new buffer of adequate space is
 * allocated.  The number of bytes allocated is stored in *size.  
 */
ssize_t getinput(char** line, size_t* size);

/* Main
 * 
 * This function is the main entry point to the program.  This is essentially
 * the primary read-eval-print loop of the command interpreter.  
 */
int main () {

  char*   line = NULL;
  size_t  size = 0;
  ssize_t len = getinput(&line, &size);

  while(len != -1) {
    processline (line);
    len = getinput(&line, &size);
  }

  if (!feof(stdin))
    perror ("read");

  return EXIT_SUCCESS;		
}

/* Get Input
 * 
 */
ssize_t getinput(char** line, size_t* size) {
  assert(line != NULL && size != NULL);

  fprintf(stderr, "%% ");
  ssize_t len = getline(line, size, stdin);
  
  if(len > 0 && (*line)[len-1] == '\n') {
    (*line)[len-1] = '\0';
    --len;
  }

  return len;
}


/* Process Line
 *
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).  
 */
void processline (char *line)
{
  assert(line != NULL);
  char** args = argparse(line);

  pid_t cpid; 
  int   status;

  cpid = fork();
  switch(cpid) {
  case -1:
    perror("fork");
    break;
    
  case 0:
    execvp(*arg, arg);
    perror("exec");
    exit(EXIT_FAILURE);
    
  default:
    wait (&status);
    break;
  }
  
}

