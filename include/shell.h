//
// Created by Ellah Nzikoba on 12/29/21.
//

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

void get_command(char *command);
void parse_command(char *command, char *argv[]);
int do_command( char *args[]);
void exit_shell(char *command);
void help_shell(char *command);
void game_shell();
void cd_shell(char *command);
void pipe_function(char *command);

#endif //MINISHELL_SHELL_H
