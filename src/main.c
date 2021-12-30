/**
  * File:    main.c
  *
  * Author:  Ellah Nzikoba
  * Date:    Spring 2021
  *
  * Summary of File:
  *
  *   main entry for the minishell program.
  *
  */

// Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>

// Header Files
#include "shell.h"

//MAIN=========================================================================
//Handles inputs, calls/manages built in commands
int main(){
    //Signal handler for Contro-C
    signal(SIGINT, sigint_handler);

    //stores commands from stdin
    char *command = malloc(sizeof(char)*BUFFER_SIZE);
    sll_t* linked_list = init_linked_list();
    shell_t* shell = init_shell(linked_list);
    int status;

    //stores command from parse
    char *argv[5];

    do {
        get_command(command);

        //if less than 2 letters is input, try again
        if (strlen(command) <= 1 ) {
            printf("Not enough for a command. Try again!\n");
        } else {
            if (add_to_linked_list(shell->commands, command) == 1) {
                shell->total_commands += 1;
            }

            //if has "|" go to piping function
            if (strstr(command, "|") != NULL) {
                status = pipe_function(command);

                //if "help"
            } else if (strcmp(command, "help\n") == 0) {
                status = help_command();

                //if "help"
            } else if (strcmp(command, "exit\n") == 0) {
                exit_command(shell);

                //if "cd"
            } else if (strstr(command, "cd") != NULL) {
                status = cd_command(command);

                //if "game"
            } else if (strcmp(command, "game\n") == 0) {
                status = game_command();

                //if "history"
            } else if (strcmp(command, "history\n") == 0) {
                status = history_command(shell);

                //if "tab"
            } else if (strcmp(command, "tab\n") == 0) {
                status = tab_command();

                //if "wd"
            } else if (strcmp(command, "wd\n") == 0) {
                status = wd_command();

            } else {

                //normal command
                parse_command(command, argv);
                status = do_command(argv);
            }
        }
    } while(status);

    return 0;
}
