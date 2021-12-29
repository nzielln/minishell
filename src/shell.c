// Modify this file for your assignment
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>

#define BUFFER_SIZE 80

//SIGINT=======================================================================
//Signal handler, exits the program when Control-C is pressed
void sigint_handler(int sig){
	write(1,"\nMini-Shell Terminated!\n", 24); //Writes prompt to shell
	exit(0);
}

//GET==========================================================================
//Prints prompt and reads input from stdin
void get_command(char *command) {
	printf("mini-shell> ");
	fgets(command, BUFFER_SIZE, stdin);
	return;
}

//PARSE========================================================================
//Takes output of get_command and parses it into an array, returns an array with
//to string, the first and second argument
void parse_command(char *command, char *argv[]) {
	int tok = 0;

	argv[tok] = strtok(command, " ");

	//for "echo", save everything else as secong argument
	if (strcmp(argv[tok], "echo") == 0) {
		argv[1] = strtok(NULL, "");
	} else {

	while (argv[tok] != NULL) {
		argv[++tok] = strtok(NULL, " ");
		}
	}
}

//DO COMMAND===================================================================
//Executes non_piping commands that are passed in through an array
//Uses a fork and then returns to parent (shell) when done
int do_command( char *args[]){
	int c_pid;
	int i = 0;
	int c_status;
	c_pid = fork();

	if(c_pid == 0) {
		while(args[i] != NULL) {
			args[i][strcspn(args[i], "\n")] = 0; //remove "\n" from arguments, if any
			++i;
		}

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

//=============================BUILT IN COMMANDS===============================
//HELL=========================================================================
//Exits shell with "exit" is read from stdin
void exit_shell(char *command) {
	printf("You've exited the Mini-Shell.\n");
	exit(0);

}

//HELP=========================================================================
//Prints out information about the shell, mainly the built in commands
void help_shell(char *command) {
	char *commands[2][4] = {{"help", "exit", "cd", "game"},
	{"To dxplains the built in commands offered by the Mini-Shell",
	"To terminates and closes the Mini-Shell", "To change directories",
	"To start a new guessing game on the Mini-Shell"}};

	printf("\nWelcome to the Mini-Shell!\n\nIn addition to standard commands, "
	"here's are some of the built in commands I offer. \n\n");
	for(int i = 0; i < 4; ++i) {
		printf("[%s]\t%s\n", commands[0][i], commands[1][i]);
	}
	printf("\nType a command to get started!");
	printf("\n------------------------------------------------------"
	"-----------------------------------\n");

}

//GAME=========================================================================
//Starts fork to play a guessing game. When user exits (or game ends) goes back
//parent (i.e main shell)
void game_shell(){
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

}

//CD===========================================================================
//Changes directories
void cd_shell(char *command) {
	char *argv[3];

	parse_command(command, argv);
	argv[1][strcspn(argv[1], "\n")] = 0; //remove "\n" if any

	chdir(argv[1]);
}

void pipe_function(char *command) {
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

return;
}

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
