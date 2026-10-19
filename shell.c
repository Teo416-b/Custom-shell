#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_ARGS 100

// Creating a function for tokenization
void tokenize(char *cmd, char **args) {
    int i = 0;
    args[i] = strtok(cmd, " \t"); // sasving the first word of the command
    while (args[i] != NULL) {
        i++;
        args[i] = strtok(NULL, " \t"); // saving the rest words of the command without space or tab
    }
}

int main() {
    char input[1024];
    char *args1[MAX_ARGS];
    char *args2[MAX_ARGS];

    while (1) {
        // Printing '$'
        printf("$ ");
        fflush(stdout);

        // reading the input from user
        if (fgets(input, sizeof(input), stdin) == NULL) {
            // If Ctrl+D is entered the code is terminated
            break; 
        }

        // Removing the \n at the end of the input
        input[strcspn(input, "\n")] = 0;

        // Ingnoring empty lines
        if (strlen(input) == 0) continue;

        // locating Pipe
        char *pipe_pos = strchr(input, '|');

        if (pipe_pos != NULL) {
            
            // deviding the string into two
            *pipe_pos = '\0';
            char *left_cmd = input;
            char *right_cmd = pipe_pos + 1;

            tokenize(left_cmd, args1);
            tokenize(right_cmd, args2);

            // checking if anything is missing from the pipe
            if (args1[0] == NULL || args2[0] == NULL) {
                fprintf(stderr, "Error: command missing before or after pipe.\n");
                continue;
            }

            // creating pipe
            int fd[2];

            if (pipe(fd) < 0) {
                perror("Error during the creation of pipe.\n");
                continue;
            }

            // creating the first child process 
            pid_t pid1 = fork();

            if (pid1 < 0) {
                perror("Error in the fork of the first child.\n");
            } 
            
            else if (pid1 == 0) {
                // code of the first child process 
                dup2(fd[1], STDOUT_FILENO); // output goes to pipe
                close(fd[0]);               // closing fd
                close(fd[1]);
                
                if (execvp(args1[0], args1) < 0) {
                    fprintf(stderr, "Command: %s, not found.\n", args1[0]);
                    exit(1);
                }
            }

            // creating the second child process 
            pid_t pid2 = fork();

            if (pid2 < 0) {
                perror("Error in the fork of the second child.\n");
            } 
            
            else if (pid2 == 0) {
                // code of the second child process
                dup2(fd[0], STDIN_FILENO);  // the input comes from the pipe
                close(fd[0]);               // closing fd
                close(fd[1]);

                if (execvp(args2[0], args2) < 0) {
                    fprintf(stderr, "Command: %s, not found\n", args2[0]);
                    exit(1);
                }
            }

            // Father code
            close(fd[0]); // Father closing the fd
            close(fd[1]);

            // waiting for both child processes to complete
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);

        }
        
        else { //executing simple command without pipe
            
            tokenize(input, args1);

            // If the user only enters 'enter' the '$' is shown again
            if (args1[0] == NULL) continue;

            // exit
            if (strcmp(args1[0], "exit") == 0) {
                break;
            }

            // 
            if (strcmp(args1[0], "cd") == 0) {
                if (args1[1] == NULL) {
                    // User typed cd without a directory
                    fprintf(stderr, "No directory given.\n");
                } 

                else if (chdir(args1[1]) != 0) {
                    // message in case there is an error during changing the directory
                    perror("error during changing the directory");
                }

                continue;
            }

            // Creating child process
            pid_t pid = fork();

            if (pid < 0) {
                // Error during creation
                perror("Error in fork.");
            } 

            else if (pid == 0) {
                // child process code
                if (execvp(args1[0], args1) < 0) {
                    // Printing message in case the command cant be found 
                    fprintf(stderr, "Command: %s, not found\n", input);
                    exit(1); // ending the child process
                }
            } 

            else {
                // waiting for the child process to complete
                wait(NULL);
            }
        }
    }

    return 0;
}