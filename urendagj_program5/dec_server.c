/**********************************************************************
** * Program: Assignment 5 One Time Pads
** * Author: Jacob Urenda Garibay
** * Date: December 1, 2020
** * Description: Performs the server side decryption
** * Input: Gets message specified from the command line
** * Output: Decrypted message is returned to the client
**********************************************************************/

/***********************************************************
* All Includes for needed libraries
************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

//Global variable to be used when allocating size for arrays
#define BUFFER (int)100000


// Error function used for reporting issues
void error(const char* msg) {
	perror(msg);
	exit(1);
}


// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address,
	int portNumber) {

	// Clear out the address struct
	memset((char*)address, '\0', sizeof(*address));

	// The address should be network capable
	address->sin_family = AF_INET;
	// Store the port number
	address->sin_port = htons(portNumber);
	// Allow a client at any address to connect to this server
	address->sin_addr.s_addr = INADDR_ANY;
}

/*Necessary conversion for encrypting the message
* In order to complete the encryption, we need to do various manipulations with integers and chars
* This function will convert a char to an int
* Code cited: https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language
*/
int chartoint(char n) {
	//this is the last element for A - Z, so we can just return it's value known 
	if (n == ' ') {
		return 26;
	}
	else {
		//Otherwise we subtract our current char from our starting position to get it's integer value
		return (n - 'A');
	}
}


/*Necessary conversion for encrypting the message
* In order to complete the encryption, we will need to perform various manipulations to get our correct encryption
* This function will convert a integer value to a char
* This function is the exact same idea as chartoint we just flip the logic
* Code cited: https://www.quora.com/How-do-I-convert-character-value-to-integer-value-in-c-language
*/
char intTochar(int x) {
	if (x == 26) {
		//We know that if x == 26, then it will be the last item in the array which is the ' '
		return ' ';
	}
	else {
		//Addition to our int value from our starting point will give us the correct int value
		return (x + 'A');
	}
}


/*This function is what the server will use to decrypt the message that was send by the user
* This function will recieve an encrypted message to be someting similar to: DSFGG DGFDGFD GFD GFDDDFFGD
* We then take than encoded message and decrypt it to be something similar to: THE RED GOOSE FLIES AT MIDNIGHT STOP
*/
char* decrypt(char* message, char* key) {
	char n;
	int i;


	for (i = 0; message[i] != '\n'; i++) {
		if ((message[i] < 'A' && message[i] > 'Z') && message[i] != ' ') {
			fprintf(stderr, "otp_enc_d error: input contains bad characters\n");
			exit(1);
		}
		n = (chartoint(message[i]) - chartoint(key[i])) % 27;
		if (n < 0) {
			n += 27;
		}
		message[i] = intTochar(n);
	}
	strcat(message, "\n");
	message[i + 1] = '\0';
	return message;
}




int main(int argc, char* argv[]) {
	/***************************************************************
	* Declare our needed contents for the file
	****************************************************************/
	int connectionSocket, charsRead, messagelen, keylen, status;
	char buffer[BUFFER];
	struct sockaddr_in serverAddress, clientAddress;
	socklen_t sizeOfClientInfo = sizeof(clientAddress);
	pid_t spawn_pid;

	// Check usage & args
	if (argc < 2) {
		fprintf(stderr, "USAGE: %s port\n", argv[0]);
		exit(1);
	}

	// Create the socket that will listen for connections
	int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket < 0) {
		error("ERROR opening socket");
	}

	// Set up the address struct for the server socket
	setupAddressStruct(&serverAddress, atoi(argv[1]));

	// Associate the socket to the port
	if (bind(listenSocket,
		(struct sockaddr*)&serverAddress,
		sizeof(serverAddress)) < 0) {
		error("ERROR on binding");
	}

	// Start listening for connetions. Allow up to 5 connections to queue up
	listen(listenSocket, 5);

	while (1) {
		//Declare our buffers that will be used to store and send information that is encrypted back to the client
		char message[BUFFER] = "";
		char key[BUFFER] = "";

		// Accept the connection request which creates a connection socket
		connectionSocket = accept(listenSocket,
			(struct sockaddr*)&clientAddress,
			&sizeOfClientInfo);
		//we were not able to establish a connection, return an error
		if (connectionSocket < 0) {
			error("ERROR on accept");
		}

		//here is where we create a new process for work to be done
		spawn_pid = fork();

		//If the pid is < 0, then we know the fork failed and we mut return an error
		if (spawn_pid < 0) {
			error("Error forking");
			exit(1);
		}

		//If the pid == 0, then we know we are the child process and we can begin to do our work
		if (spawn_pid == 0) {

/************************************************************************************************************************
*									***Get Message Length ***
* Getting all of the necessary information is going to be broken up into seperate steps
* First, we will be getting the length of the message, so we must recieve input from the client
* we also send a message of validation to check the connection status between client and server
*************************************************************************************************************************/
			//clear the buffer to make sure that it is ready to be used
			memset(buffer, '\0', BUFFER);
			// Read the client's message from the socket
			charsRead = recv(connectionSocket, buffer, BUFFER, 0);
			//If our value for charsread
			if (charsRead < 0) {
				error("ERROR reading from socket");
			}
			//We need to get the length of the message, so we can use our buffer to do so
			messagelen = atoi(buffer);

			//Send a confirmation char for valid connection
			charsRead = send(connectionSocket, "-", 1, 0);
			//Otherwise we could not write 
			if (charsRead < 0) {
				error("ERROR writing to socket");
			}

/************************************************************************************************************************
*									***Get Key Length ***
* We move onto the next step in getting all of the information, which is getting the length of the key
* The client will give us our key length, which we will be using to send an encrypted output back to the user
*************************************************************************************************************************/
			//reclear the buffer so that it is ready to recieve new input from the user
			memset(buffer, '\0', BUFFER);

			// Read the client's message from the socket
			charsRead = recv(connectionSocket, buffer, BUFFER, 0);
			//If the chars read is < 0, then we know that we were not able to successfully read
			if (charsRead < 0) {
				error("ERROR reading from socket");
			}
			//We now use the buffer to get the input length, which will be our keylength
			//We also convert to be able to use the value for encrypting
			keylen = atoi(buffer);

			//We send a message back to the user, letting them know that we have successfuly retrieved the length of the key
			charsRead = send(connectionSocket, "I am the server, keylength recieved", 35, 0);

			//Otherwise if charsread < 0, we know that reading was unsuccessful
			if (charsRead < 0) {
				error("ERROR writing to socket");
			}

/************************************************************************************************************************
*									***Get The message ***
* We move onto the next step in getting all of the information, which is getting the length of the key
* The client will give us our key length, which we will be using to send an encrypted output back to the user
*************************************************************************************************************************/
			//Clear the buffer for reuse
			memset(buffer, '\0', BUFFER);

			//We read to get the message sent from the client
			charsRead = recv(connectionSocket, buffer, BUFFER, 0);

			//If charsread < 0, then we know that reading was unsuccessful
			if (charsRead < 0) {                                                         // Checks for an error when reading from the socket

				error("ERROR reading from socket");
			}

			//Get the message from the buffer into our actual variable we will use to send 
			strcat(message, buffer);
			//Decriment our message length
			messagelen -= strlen(buffer);

			//Checking to see while contents are still avalible
			for (int i = 0; messagelen > 0; i++) {
				if (strlen(buffer) == 0) {
					//at this point there is nothing in the buffer and we can break
					break;
				}
				else { // otherwise we have to retrieve the rest of the data
					memset(buffer, '\0', BUFFER);
					//Get the rest of the message
					charsRead = recv(connectionSocket, buffer, BUFFER, 0);
					//Check to see if we had an error reading
					if (charsRead < 0) {
						error("ERROR reading to socket");
					}
					//Decriment message legnth
					messagelen -= strlen(buffer);
					//Add contents to our message
					strcat(message, buffer);
				}
			}
			//We send a message to the client letting them know that they have successfully had their message recieved 
			charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
			//Otherwise there was an error sending
			if (charsRead < 0) {
				error("ERROR writing to socket");
			}
/************************************************************************************************************************
*									***Get the Key***
* We move onto the next step in getting all of the information, which is getting the length of the key
* The client will give us our key length, which we will be using to send an encrypted output back to the user
*************************************************************************************************************************/
			//clear the buffer for reuse
			memset(buffer, '\0', BUFFER);
			//Read the client message from the socket
			charsRead = recv(connectionSocket, buffer, BUFFER, 0);
			//Otherwise we had an error reading 
			if (charsRead < 0) {
				error("ERROR reading to socket");
			}
			//Get the key from the buffer
			strcat(key, buffer);
			//Decriment the length of the key
			keylen -= strlen(buffer);

			//Checking to see while there are still contents to get for the key
			for (int i = 0; keylen > 0; i++) {
				//If there is nothing in the buffer, we can exit
				if (strlen(buffer) == 0) {
					break;
				}
				else { //Otherwise we will need to retrieve the rest of the data
					//Reset the buffer to get more information (Key)
					memset(buffer, '\0', BUFFER);
					//We read the data from the socket to get our Key
					charsRead = recv(connectionSocket, buffer, BUFFER, 0);
					//Otherwise we know there was an error recieving input from the socket
					if (charsRead < 0) {
						error("ERROR reading to socket");
					}
					//Decriment the length of the key so we can see if we still have content left 
					keylen -= strlen(buffer);
					//Copy the recieved contents into our key
					strcat(key, buffer);

				}
			}
			//We send a mesage notifying the clinet of success
			charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
			//Otherwise we notify an error has occured
			if (charsRead < 0) {
				error("ERROR writing to socket");
			}
/************************************************************************************************************************
*									***Send the message and encrypt***
* We move onto the next step in getting all of the information, which is getting the length of the key
* The client will give us our key length, which we will be using to send an encrypted output back to the user
*************************************************************************************************************************/
			//We encrypt the message that we are going to send to the client
			char* test = decrypt(message, key);
			//We then send the encrypted message back to the client
			charsRead = send(connectionSocket, test, strlen(test), 0);
			//Otherwise we have an error
			if (charsRead < 0) {
				error("ERROR writing to socket");
			}
			//Checking to see that we have actually transfered all of the data
			if (charsRead < strlen(message)) {
				printf("Not all data has been written to the socket! \n");
			}
		}



/************************************************************************************************************************
*									***Parent Process***
* We wait for all work to be finished by the child before terminating
*************************************************************************************************************************/
		else {
			//Parent waits for the child process to finish execution
			spawn_pid = waitpid(-1, &status, WNOHANG);

		}

/*************************************************************************************************************************/





				// Close the connection socket for this client
		close(connectionSocket);

	}

	// Close the listening socket
	close(listenSocket);
	return 0;


}

