#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <pwd.h>
#include <stdbool.h>

char* delimeter = " \n";
char Input[512];
char* token;
char* Argument[512];
char* Inputname = NULL;
char* Outputname = NULL;
char* savePtr;
char* lineinput = NULL;
char buffer[2048];
char* string = "";

int foreground = 0;
int background = 0;
int count;
int spawnPid;
int status = 0;
int childstatus = 0;
int file_descriptor;
int backgroundarr[512];
int countbg = 0;
int exitval = 0;


struct sigaction act;
struct sigaction SIGINT_action = { 0 };
struct sigaction SIGTSTP_action = { 0 };

//This function is used in order to catch the SIGTSTP signal
//Foreground-only mode will be toggeled
void SIGTSTPcatcher(int signo) {

	//If background process is allowed, enter foreground only mode and prompt 
	if (foreground == 0) {
		//background = 0;
		char* prompt = "Entering foreground-only mode (& is now ignored)\n";
		write(1, prompt, 52);
		fflush(stdout);
		foreground = 1;
		//background = 1;
	}
	else {
		
		char* prompt = "Exiting foreground-only mode\n";
		write(1, prompt, 29);
		fflush(stdout);
		//foreground = 1;
		//background = 0;
		foreground = 0;
	}
}

void Endbgprocess() {
	for (int i = 0; i < 512; i++) {
		if (spawnPid == 0) {//we are the parent, we can just exit no processes needed to be terminated
			status = 0;
		}
		else {//processes will be terminated
			kill(backgroundarr[i], SIGTERM);
		}
	}
}

//reset array and files for next command
void reset() {
	//background = 0;
	for (int i = 0; i < count; i++) {
		Argument[i] = NULL;
	}
	Outputname = NULL;
	Inputname = NULL;
}

void builtincmds() {
	if (Argument[0] == NULL || !(strncmp(Argument[0], "#", 1))) {
		//if comment or null - do nothing
		;
	}

	
	//Checking for the cd command, which indicates the user wants to change directories
	else if (strcmp(Argument[0], "cd") == 0) {
		if (Argument[1] == NULL) {
			//We have no commands besides cd, we then know we must navigate to the home directory

			chdir(getenv("HOME"));
		}
		else {
			chdir(Argument[1]);
		}
	}

	//We are checking to see if the input is the exit command, so if so it should be in the first position in the array
	else if (strcmp(Argument[0], "exit") == 0) {
		Endbgprocess();
		status = 0;
		exit(0);
		exitval = 1;
	}

	//check termination status
	//code infulenced from canvas modules
	else if (strcmp(Argument[0], "status") == 0) {
		//Print status
		if (WIFEXITED(status)) {//print exit status
			printf("Exit status: %d\n", WEXITSTATUS(status));
		}
		else { //else print terminating signal
			printf("Terminating signal %d\n", status);
		}
	}
}


int main() {

	// signal code heavily referenced from in class powerpoint
	
	// SIGINT handling - default is ignore
	SIGINT_action.sa_handler = SIG_IGN;
	sigaction(SIGINT, &SIGINT_action, NULL);

	// SIGTSTP handling - foreground only mode
	SIGTSTP_action.sa_handler = SIGTSTPcatcher;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags = SA_RESTART;
	sigaction(SIGTSTP, &SIGTSTP_action, NULL);

	while (!exitval) {
		background = 0;
		printf(": "); //Assignment said to flush to avoid complications
		fflush(stdout);
		fflush(stdin);
		//begin by reading the line
		ssize_t size = 0;
		if (!(getline(&lineinput, &size, stdin))) { //We have ended up reaching the end of the string (NUll termination)
			return 0;
		}
		

		count = 0;
		token = strtok(lineinput, "  \n");
		while (token != NULL) {
			//Check for the Input file denoted by <
			if (strcmp(token, "<") == 0) {
				//Checking for input file
				//We assign the Inputname
				token = strtok(NULL, delimeter);
				Inputname = strdup(token);

				//Get the next argument
				token = strtok(NULL, delimeter);
			}
			else if (strcmp(token, ">") == 0) {
				//Checking for Output file
				//We assign Output file name
				token = strtok(NULL, delimeter);
				Outputname = strdup(token);
				token = strtok(NULL, delimeter);
			}
			else if (strcmp(token, "&") == 0) {
				//there is a command in the background
				//token[count] = NULL;
				//background = 1;
				if (foreground) {
					background = 0;
				}
				else {
					background = 1;
				}
				break;
			}
			else {
				//Stor argument/command into the array 
				Argument[count] = strdup(token);

				//Access the next token
				token = strtok(NULL, delimeter);

				//Count keeps track of the amount of arguments
				count++;
			}
			//Add a Null Termination Char at the end of the array
			Argument[count] = NULL;



		}

		//$$ expansion
		//https://www.geeksforgeeks.org/snprintf-c-library/ - code referenced
		for (int i = 0; i < count; i++) {
			for (int j = 0; j < strlen(Argument[i]); j++) { // looks for the $$
				if (Argument[i][j] == '$' && Argument[i][j + 1] == '$') {
					Argument[i][j] = '\0'; // replaces the $$ with NULLS
					// appending to make files with process IDs
					snprintf(buffer, 2048, "%s%d", Argument[i], getpid());
					Argument[i] = buffer;
				}
			}
		}
		//printf("this is the string: %s \n ", buffer);
		
		//Checks to see if we have a pound sign(#), if so we do nothing and move on
		
		builtincmds();

		externalcmds();

		//else {

		


		//}
	
		/*background = 0;
		for (int i = 0; i < count; i++) {
			Argument[i] = NULL;
		}
		Outputname = NULL;
		Inputname = NULL;*/
		reset();


		spawnPid = waitpid(-1, &status, WNOHANG);
		while (spawnPid > 0) {
			//Print that process is complete
			printf("background pid complete: %d\n", spawnPid);

			if (WIFEXITED(status)) { //If the process ended successfully
				printf("Exit status: %d\n", WEXITSTATUS(status));
			}
			else { //If the process was terminated by a signal
				printf("Terminating signal: %d\n", status);
			}

			spawnPid = waitpid(-1, &status, WNOHANG);
		}
		

	}
	//fflush(stdin);
	return 0;

}

void externalcmds() {
	//No built in command has been called, therefore we must move on and create a new process for non built-in commands
	spawnPid = fork();
	switch (spawnPid) {

	case -1: //If the pid is below 0, we know that the fork has failed
		printf("Fork has failed\n");
		fflush(stdout);
		status = 1;
		break;
	case 0: //If we get a PID value of 0, then we know that we are in fact the child

		if (!background) {
			SIGINT_action.sa_handler = SIG_DFL;
			SIGINT_action.sa_flags = 0;
			sigaction(SIGINT, &SIGINT_action, NULL);
		}
		//sigaction(SIGTSTP, &ignore_action, NULL);
		if (background == 0) {//foreground mode
			//sigaction(SIGINT, &SIGINT_action, NULL);

			if (Inputname != NULL) { //Input
				file_descriptor = open(Inputname, O_RDONLY);
				if (file_descriptor == -1) {
					printf("open() failed on %s \n", Inputname);
					perror("Error");
					fflush(stderr);
					status = 1;
				}
				else if (dup2(file_descriptor, 0) == -1) { //We redirect unless we get an error
					fprintf(stderr, "dup2 error");
					fflush(stderr);
					status = 1;
					exit(1);
				}
				close(file_descriptor);
			}
			if (Outputname != NULL) { //Outputfile
				file_descriptor = open(Outputname, O_WRONLY | O_CREAT | O_TRUNC, 0666);
				if (file_descriptor == -1) {
					fprintf(stderr, "File %s cannot be opened for output \n", Outputname);
					fflush(stderr);
					status = 1;
					exit(1);
				}
				else if (dup2(file_descriptor, 1) == -1) {
					fprintf(stderr, "dup2 error");
					fflush(stderr);
					status = 1;
					exit(1);
				}
				close(file_descriptor);
			}
		}
		else { //Otherwise we are autmatically a background process
			//Specified in the assignment, we redirect to /dev/null for background processes
			file_descriptor = open("/dev/null", O_RDONLY);
			if (file_descriptor == -1) {
				fprintf(stderr, "/dev/null could not be opened for input \n");
				fflush(stderr);
				status = 1;
				exit(1);
			}
			else if (dup2(file_descriptor, 0) == -1) { //redirect unless error
				fprintf(stderr, "Open error");
				fflush(stderr);
				status = 1;
				exit(1);
			}
			close(file_descriptor);
		}
		//code derived from canvas moduel
		if (execvp(Argument[0], Argument)) { //execute
			//Command not recognized error, exit
			fprintf(stderr, "Command not recognized: %s\n", Argument[0]);
			fflush(stdout);
			status = 1;
			exit(1);
		}
		// should never get here
		fprintf(stderr, "invalid command\n");
		fflush(stderr);
		status = 1;
		exit(1);

	default: //This is the parent process
		//Check to see if we are the background process:
		if (background == 1) {
			//We need to add the background processes to the array to keep track for cleanup
			backgroundarr[countbg] = spawnPid;
			//make sure we don't overwrite and increment the array
			countbg++;
			//now we can show the pid of the background process while not in foreground mode
			printf("background pid: %d\n", spawnPid);
			fflush(stdout);
			//background = 0;
		}
		else {// We are in foreground mode
			//we must now wait for the child process to terminate before continuing
			pid_t pidOutput = waitpid(spawnPid, &status, 0);
			//If the process has been terminated, then we can print out the termination signal
			if (WIFSIGNALED(status) != 0) {
				printf("Terminated by signal: %d \n", WTERMSIG(status));
				fflush(stdout);
			}

			//foreground = 1;
		}


	}
	
		
	
		

}




