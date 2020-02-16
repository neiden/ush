
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
#include "argparse.h"
#include "builtin.h"

/* CONSTANTS */

/* PROTOTYPES */
/* Process Line
 * line  The command to process
 *
 * The parameter line is interpreted as a command name.  This function creates a
 * new process that executes that command.
 */
void processline (char *line);


/*Expand
 * Nonfunctional; ideally the program would iterate through orig (the input line) and
 * copy every value into new, excepting the character '$', '{', and '}'. The variables
 * for expanding would be then be iterated over and added as the next values for new. 
 * The enums signify from which source new is taking new values from:
 *              OUT         - new[idx] = *orig
 *              START, BET, - new waits for environment variable to be found before iterating over.
 * 
 */
int expand(char*  orig, char* new, int newsize);

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
/*Stripping function
 * 
 * Helper method to remove a given character from a string. Uses memmove to leave no 
 * space behind from the removed index. 
 */
void strip(char* line, char n){
    int len = strlen(line);
    for(int i = 0; i < len; i++){
        if(line[i] == n){
            memmove(&line[i], &line[i+1], len - i);
        }
    }
}



int expand(char* orig, char* new, int newsize){
    enum {START, BET, OUT} state = OUT;
    char buffer[newsize];
    char* var;
    char* ptr;
    int idx = 0;
    int flag = 0;
    new = buffer;




    while(*orig != '\0'){

        if(*orig == '$') {
            state = START;
        }

        if(state == OUT){
            new[idx] = *orig;
            ++idx;
        }

        if(*orig == '{' && state == START){
            state = BET;
        }
        if(*orig == '}'){
            if(state == BET){
                strip(ptr, '}');

                var = getenv(ptr);
                while(*var != '\0'){
                    new[idx] = *var;
                    ++idx;
                    ++var;
                }
                
                state = OUT;
            }
        }

        ++orig;
        if(state == BET && flag == 0){
            ptr = orig;
            ++flag;
        }


}
    orig = new;
}

/* Process Line
 *
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 */
void processline (char *line)
{
    assert(line != NULL);
    char* buffer = "";
    expand(line, line, 1024);
    int argcp = 0;
    char** args = argparse(line, &argcp);



    pid_t cpid;
    int   status;
    if(argcp != 0) {

        if(!builtin(args, argcp)) {


            cpid = fork();
            switch (cpid) {
                case -1:
                    perror("fork");
                    break;

                case 0:
                    execvp(args[0], args);
                    perror("exec");
                    exit(EXIT_FAILURE);

                default:
                    wait(&status);
                    break;
            }
        }


     
    }

    free(args);

}
