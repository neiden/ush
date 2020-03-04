
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
#include <math.h>
#include <fcntl.h>

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




    signal(SIGINT, SIG_IGN);
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


int getDigits(int num){
    int count = 0;
    while(num > 0){
        num /= 10;
        ++count;
    }
    return count;
}

int expand(char* orig, char* new, int newsize){
    enum { VARBET, START, CMDBET, OUT} state = OUT;
    char* var = orig;
    char* final = orig;
    char* value;
    int varIdx = 0;
    int newIdx = 0;
    int expanded = 0;

    if(*orig == '\0'){
        return "";
    }





    while(*orig != '\0'){

        if(*orig == '$' && state == OUT) {
            new[newIdx] = *orig;
            ++newIdx;
            state = START;
        }

        else if(*orig == '$' && state == START){
            --newIdx;
            int value = getpid();
            int digits = getDigits(value) - 1;
            while(value > 0){
                new[newIdx] = (value / (int)pow(10, digits)) + '0';
                value %= (int)pow(10, digits);
                --digits;
                ++newIdx;
            }
            ++orig;
            state = OUT;
        }

        else if((*orig != '{' && *orig != '(') && state == START){
            state = OUT;
        }
        else if(*orig == '(' && state == START){
            var = orig;
            state = CMDBET;
        }

        else if(*orig == '{' && state == START){
            var = orig;
            state = VARBET;
        }

        else if(*orig == ')' && state == CMDBET){
            var[varIdx] = '\0';
            ++var;
            --newIdx;

            char buffer[newsize];
            char argc = 0;
            char** args = argparse(var, &argc);
            int status;
            int fd[2];
            if(pipe(fd) == -1){
                perror("pipe");
                exit(1);
            }

            pid_t cid = fork();
            if(cid == -1){
                perror("fork");
                exit(1);
            }
            else if (cid == 0){
                if(dup2(fd[1], STDOUT_FILENO) == -1){
                    perror("dup2");
                    exit(1);
                }
                close(fd[1]);
                close(fd[0]);
                execvp(args[0], args);
                perror("exec");
                exit(1);
            }
            else{
                close(fd[1]);
                for(int i = 0; (i = read(fd[0], buffer, newsize)) > 0;){
                    write(STDOUT_FILENO, value, i);
                }

                close(fd[0]);
                wait(&status);

            }



            char buff[1024];
            if(value != NULL) {
                expand(value, buff, 1024);

                for (int i = 0; i < strlen(value); i++) {
                    new[newIdx] = value[i];
                    ++newIdx;
                }
                expanded = 1;
            }
            else{
                printf("Failed to execute command %s", var);
            }

            varIdx = 0;
            state = OUT;

        }

        else if(*orig == '}' && state == VARBET){
            var[varIdx] = '\0';
            ++var;
            --newIdx;

            value = getenv(var);
            char buff[1024];
            if(value != NULL) {
                expand(value, buff, 1024);

                for (int i = 0; i < strlen(value); i++) {
                    new[newIdx] = value[i];
                    ++newIdx;
                }
                expanded = 1;
            }
            else{
                printf("No value associated with variable %s\n", var);
            }
            varIdx = 0;
            state = OUT;
        }

        if(state == VARBET || state == CMDBET){
            ++varIdx;
        }

        if(state == OUT){
            if(*orig != '\0'){
                new[newIdx] = *orig;
                ++newIdx;
            }
        }

        ++orig;


    }

    if(state == VARBET || state == CMDBET){
        fprintf(stderr,"Mistmatched bracket error");
    }

    new[newIdx] = '\0';
    int count = 0;
    for(;count < newIdx; count ++){
        final[count] = new[count];
    }

    final[count] = '\0';



    return expanded;
}


/* Process Line
 *
 * Note the three cases of the switch: fork failed, fork succeeded and this is
 * the child, fork succeeded and this is the parent (see fork(2)).
 */
void processline (char *line)
{

    assert(line != NULL);
    char new[1024];
    expand(line, new, 1024);
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
		            signal(SIGINT, SIGINT);
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


