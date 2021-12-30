/**
  * File:    shell.h
  *
  * Author:  Ellah Nzikoba
  * Date:    Spring 2021
  *
  * Summary of File:
  *
  *   Header file for the minishell program, methods implemented in shell.c
  *
  */

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#ifndef MINISHELL_SHELL_H
#define MINISHELL_SHELL_H

#define BUFFER_SIZE 80
#define CWD_SIZE 256
#define MAX_COMMANDS 25

typedef struct Node{
    char command[85];
    struct Node* next;
}Node_t;

typedef struct SinglyLinkedList{
    int total_commands;
    Node_t* head;
} sll_t;

typedef struct Shell {
    int total_commands;
    struct SinglyLinkedList* commands;
} shell_t;

void sigint_handler(int sig);

sll_t* init_linked_list();
int add_to_linked_list(sll_t* linked_list, char* command);
void print_linked_list(sll_t* linked_list);
void free_linked_list(sll_t* linked_list);

shell_t* init_shell(sll_t* linked_list);
void get_command(char *command);
void parse_command(char *command, char *argv[]);
int do_command( char *args[]);

void exit_command(shell_t *shell);
int help_command();
int game_command();
int cd_command(char *command);
int history_command(shell_t* shell);
int tab_command();
int wd_command();
int pipe_function(char *command);

#endif //MINISHELL_SHELL_H
