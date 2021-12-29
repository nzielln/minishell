//
// Created by Ellah Nzikoba on 12/29/21.
//

//Libraries
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#include "shell.h"
#include "shell.c"

//MAIN=========================================================================
//Handles inputs, calls/manages built in commands
int main(){
    //Signal handler for Contro-C
    signal(SIGINT, sigint_handler);

    //stores commands from stdin
    char *command = malloc(sizeof(char)*BUFFER_SIZE);
    //stores command from parse
    char *argv[5];

    while(1) {
        get_command(command);

        //if less than 2 letters is input, try again
        if (strlen(command) <= 1 ) {
            printf("Not enough commands. Try again!\n");
        } else {

            //if has "|" go to piping function
            if (strstr(command, "|") != NULL) {
                pipe_function(command);

                //if "help"
            } else if (strcmp(command, "help\n") == 0) {
                help_shell(command);

                //if "help"
            } else if (strcmp(command, "exit\n") == 0) {
                exit_shell(command);

                //if "cd"
            } else if (strstr(command, "cd") != NULL) {
                cd_shell(command);

                //if "game"
            } else if (strcmp(command, "game\n") == 0) {
                game_shell();

            } else {

                //normal command
                parse_command(command, argv);
                do_command(argv);
            }
        }
    }

    return 0;
}
