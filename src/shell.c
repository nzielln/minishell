/**
  * File:    shell.c
  *
  * Author:  Ellah Nzikoba
  * Date:    Spring 2021
  *
  * Summary of File:
  *
  *   Source code for the minishell program, methods defined in shell.h
  *
  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <sys/stat.h>

// Header Files
#include "shell.h"

//SIGINT========================================================================
/**
 * Function: sigint_handler
 * Parameters:
 *      (int) sig - a signal
 *
 * Purpose: Exits the program when Control-C is pressed
 */
void sigint_handler(int sig){
    write(1,"\nMini-Shell Terminated!\n", 24); //Writes prompt to shell
    exit(0);
}

//=============================LINKED LIST COMMANDS=============================
//INIT LINKED LIST==============================================================
/**
 * Function: init_linked_list
 * Parameters:
 *      None
 *
 * Purpose: Initiates the linked list struct
 */
sll_t* init_linked_list(){
    sll_t* ll = malloc(sizeof(sll_t));
    ll->head = NULL;
    ll->total_commands = 0;

    return ll;
}

//ADD TO LINKED LIST [FRONT]====================================================
/**
 * Function: add_to_linked_list
 * Parameters:
 *      (sll_t) linked_list - a sll_t, singly-linked-list, struct
 *      (char*) command - a char pointer to a command
 *
 * Purpose: Adds a new command to the front of the linked list
 */
int add_to_linked_list(sll_t* linked_list, char* command){
    if (linked_list == NULL) {
        return -1;
    } else if (linked_list->total_commands == MAX_COMMANDS) {
        return 0;
    }

    Node_t* node = malloc(sizeof(Node_t));
    stpcpy(node->command, command);
    node->next = NULL;

    //If it's the first item to be added, make it the head
    if (linked_list->head == NULL) {
        linked_list->head = node;
        linked_list->total_commands++;
        return 1;
    } else {
        //Otherwise, add to beginning of linked list
        node->next = linked_list->head;
        linked_list->head = node;

        linked_list->total_commands++;
        return 1;
    }
}

//PRINT LINKED LIST=============================================================
/**
 * Function: print_linked_list
 * Parameters:
 *      (sll_t) linked_list - a sll_t, singly-linked-list, struct
 *
 * Purpose: Prints all items in the linked list
 */
void print_linked_list(sll_t* linked_list) {
    Node_t* node = linked_list->head;
    while(node != NULL) {
        printf(" >>>>>> %s", node->command);
        node = node->next;
    }
}

//FREE LINKED LIST==============================================================
/**
 * Function: free_linked_list
 * Parameters:
 *      (sll_t) linked_list - a sll_t, singly-linked-list, struct
 * Purpose: Frees allocated memory in the single-linked-list struct and all the
 * nodes in the list
 */
void free_linked_list(sll_t* linked_list) {
    Node_t* node = linked_list->head;
    while(node != NULL) {
        Node_t* temp = node->next;
        free(node);
        node = temp;
    }

    free(node);
    free(linked_list);
}

//INIT_SHELL====================================================================
/**
 * Function: init_shell
 * Parameters:
 *      (sll_t) linked_list - a sll_t, singly-linked-list, struct that holds all
 *                            the commands
 * Purpose: Initiates the shell struct
 */
shell_t* init_shell(sll_t* linked_list) {
    shell_t* shell = malloc(sizeof(shell_t));
    shell->total_commands = linked_list->total_commands;
    shell->commands = linked_list;

    return shell;
}

//=============================HANDLE COMMANDS==================================
/**
 * Function: get_command
 * Parameters:
 *      (char*) command - a char pointer to a command
 * Purpose: Prints prompt and reads input from stdin
 */
void get_command(char *command) {
    printf("mini-shell> ");
    fgets(command, BUFFER_SIZE, stdin);
}

//PARSE=========================================================================
/**
 * Function: init_linked_list
 * Parameters:
 *      (char*) command - a char pointer to a command
 *      (char*) argv[] - a char pointer to array of command line arguments
 *
 * Purpose: Takes output of get_command and parses it into an array, returns an array with
 *          to string, the first and second argument
 */
void parse_command(char *command, char *argv[]) {
    int tok = 0;
    strtok(command, "\n"); // remove "\n" from command

    argv[tok] = strtok(command, " ");

    //for "echo", save everything else as second argument
    if (strcmp(argv[tok], "echo") == 0) {
        argv[1] = strtok(NULL, "");
    } else {

        while (argv[tok] != NULL) {
            argv[++tok] = strtok(NULL, " ");
        }
    }
}

//DO COMMAND====================================================================
/**
 * Function: do_command
 * Parameters:
 *      (char*) argv[] - a char pointer to array of command line arguments
 *
 * Purpose: Executes non_piping commands that are passed in through an array
 *          Uses a fork and then returns to parent (shell) when done
 */
int do_command( char *args[]){
    int c_pid;
    int c_status;
    c_pid = fork();

    if(c_pid == 0) {
        //if exec isn't successful, print message
        if (execvp(args[0], args) == -1) {
            printf("Command not found--Did you mean something else?\n");
        }

        execvp(args[0], args);
        exit(0); //exit child process

    } else {
        waitpid(c_pid, &c_status, 0); //waits for child to finish
    }

    return 1;
}

//=============================BUILT IN COMMANDS================================
//HELP==========================================================================
/**
 * Function: exit_command
 * Parameters:
 *      (shell_t) shell - shell struct that holds all command information
 *
 * Purpose: Exits shell with "exit" is read from stdin and free all allocate memory
 */
void exit_command(shell_t *shell) {
    printf("You've exited the Mini-Shell.\n");

    //Free all allocated memory
    free_linked_list(shell->commands);
    free(shell);
    exit(0);
}

//HELP==========================================================================
/**
 * Function: init_linked_list
 * Parameters:
 *      None
 *
 * Purpose: Prints out information about the shell, mainly the built in commands
 */
int help_command() {
    char *commands[2][7] = {{"help", "exit", "cd", "game", "history", "tab", "wd"},
                            {"To explain the built in commands offered by the Mini-Shell",
                                     "To terminate and closes the Mini-Shell", "To change directories",
                                                   "To start a new guessing game on the Mini-Shell",
                                                           "To display all previous commands a user has typed in",
                                                                   "To list all runnable programs",
                                                                          "To print current working directory"}};

    printf("\nWelcome to the Mini-Shell!\n\nIn addition to standard commands, "
           "here's are some of the built in commands I offer. \n\n");
    for(int i = 0; i < 7; ++i) {
        printf("[%s]\t%s\n", commands[0][i], commands[1][i]);
    }
    printf("\nType a command to get started!");
    printf("\n------------------------------------------------------"
           "-----------------------------------\n");

    return 1;
}

//GAME==========================================================================
/**
 * Function: game_command
 * Parameters:
 *      None
 *
 * Purpose: Starts fork to play a guessing game. When user exits (or game ends) goes back
 *          parent (i.e main shell)
 */
int game_command(){
    //opens a new child process to star guessing game
    int g_id;
    int g_status;

    g_id = fork(); //forks into new child process

    if (g_id == 0) {
        int data[5];
        int games = 5;
        int guesses = 0;

        srand(time(NULL));
        int number = 1 + rand() % 10;
        printf("\nPick a number between 1 & 10.\nEnter \"quit\" to end game.\n\n");

        while (games > 0) {
            char in_put[15];
            int guess;

            printf("Make a guess: ");
            scanf("%s", in_put);

            if (strcmp(in_put, "quit") == 0) {
                exit(0); //if "quit" is entered, return to shell (parent)
            }

            guess = atoi(in_put); //if not quit; parse from str to int

            if (guess == number) {

                printf("You got it!\n");
                ++guesses;
                data[5 - games] = guesses;
                --games;
                guesses = 0;

                if (games > 0) {
                    number = 1 + rand() % 10;
                    printf("\nPick a number between 1 and 10.\n");
                }

            } else if (guess < number) {

                printf("That's too low!\n");
                ++guesses;

            } else if (guess > number) {

                printf("That's too high!\n");
                ++guesses;

            } else {

                break;
            }
        }

        printf("\nHere are your results!\n");
        for (int i = 0; i < 5; i++) {
            printf("Game %d took you %d guesses.\n", i, data[i]);
        }
        exit(0); //when games ends, return to parent

    } waitpid(g_id, &g_status, 0);

    return 1;
}

//CD============================================================================
/**
 * Function: cd_command
 * Parameters:
 *      (char*) command - a char pointer to a command
 *
 * Purpose: Changes directories
 */
int cd_command(char *command) {
    char *argv[3];

    parse_command(command, argv);
    argv[1][strcspn(argv[1], "\n")] = 0; //remove "\n" if any

    chdir(argv[1]);

    return 1;
}

//PIPE==========================================================================
/**
 * Function: pipe_function
 * Parameters:
 *      (char*) command - a char pointer to a command
 *
 * Purpose: Hanles a pipe function using forks
 */
int pipe_function(char *command) {
    int pipe_count = 0;
    int i = 0;
    char *args[2]; //to store the two sides of the piping functions

    //counts how many "|" are in the command
    while(command[i] != '\0'){
        if ( command[i] == '|') {
            ++pipe_count;
        }
        ++i;
    }

    //if more than one "|", prints error message
    if (pipe_count > 1) {
        printf("Command not found--Did you mean something else?");
    }

    //removes "\n"
    args[0] = strtok(command, "|");
    args[0][(strcspn(args[0], "\n"))] = 0;

    args[1] = strtok(NULL, " ");
    args[1][(strcspn(args[1], "\n"))] = 0;

    //Parses the commands and creates new arg arrays
    char *pros_1[3];
    char *pros_2[3];
    pros_1[2] = NULL;
    pros_2[2] = NULL;

    parse_command(args[0], pros_1);
    parse_command(args[1], pros_2);

    //file descriptors and piping for first process
    int pfd[2];
    pipe(pfd);

    int p_id;
    int p_id_2;

    int p_status;
    int p_status_2;

    p_id = fork(); //first fork()

    if (p_id == -1) {
        printf("Pipe Function Fork failed\n");
        exit(EXIT_FAILURE);
    }

    if (p_id == 0) {

        close(STDOUT_FILENO);

        dup2(pfd[1], STDOUT_FILENO);

        close(pfd[1]);
        close(pfd[0]);

        execvp(pros_1[0], pros_1);
        exit(0);

    } else {

        p_id_2 = fork(); //second fork()

        if (p_id_2 == 0) {

            close(STDIN_FILENO);
            dup2(pfd[0], 0);
            close(pfd[1]);
            close(pfd[0]);

            execvp(pros_2[0], pros_2);
            exit(0);
        }

        close(pfd[0]);
        close(pfd[1]);

        //waits for both process to finish
        waitpid(p_id, &p_status, 0);
        waitpid(p_id_2, &p_status_2, 0);
    }

    return 1;
}

//HISTORY=======================================================================
/**
 * Function: history_command
 * Parameters:
 *      (shell_t) shell - shell struct that holds all command information
 *
 * Purpose: Prints all commands user has called so far, from most recent
 */
int history_command(shell_t* shell) {
    printf("\n>>>>Command History from most recent.\n");
    print_linked_list(shell->commands);
    printf("\n");
    return 1;
}

//TAB===========================================================================
/**
 * Function: tab_command
 * Parameters:
 *      None
 *
 * Purpose: Prints all executable files in the current working directory
 */
int tab_command() {
    //Get current working directory
    char path[PATH_MAX+BUFFER_SIZE];

    //Open directory and read files
    DIR* directory;
    struct dirent *de;
    struct stat sb;

    directory = opendir(".");
    while (directory) {
        de = readdir(directory);
        if (!de) {
            break;
        } else {
            //Get file full path name
            getcwd(path, PATH_MAX);
            strcat(path, "/");
            strcat(path, de->d_name);

            //Check if file is executable
            if(!stat(path, &sb)) {
                if(S_ISREG(sb.st_mode) && sb.st_mode & 0111)
                    printf("%s\n", de->d_name);
            }
        }
    }

    return 1;
}

//WD============================================================================
/**
 * Function: init_linked_list
 * Parameters:
 *      None
 *
 * Purpose: Prints current working directory
 */
int wd_command(){
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);

    printf("%s\n", path);

    return 1;
}

