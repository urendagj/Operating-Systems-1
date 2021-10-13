/**********************************************************************
** * Program: Assignment 5 One Time Pads
** * Author: Jacob Urenda Garibay
** * Date: December 1, 2020
** * Description: Performs the client side encryption
** * Input: Uses the encrypted message provided by the server
** * Output: Encrypted message is sent to the destination signified by user
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
#define BUFFER (int)100000

// Error function used for reporting issues
void error(const char* msg) {
    perror(msg);
    exit(0);
}

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address,
    int portNumber,
    char* hostname) {

    // Clear out the address struct
    memset((char*)address, '\0', sizeof(*address));

    // The address should be network capable
    address->sin_family = AF_INET;
    // Store the port number
    address->sin_port = htons(portNumber);

    // Get the DNS entry for this host name
    struct hostent* hostInfo = gethostbyname(hostname);
    if (hostInfo == NULL) {
        fprintf(stderr, "CLIENT: ERROR, no such host\n");
        exit(0);
    }
    // Copy the first IP address from the DNS entry to sin_addr.s_addr
    memcpy((char*)&address->sin_addr.s_addr,
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

/*This function allows us to find the size of the file
* We can use this to get the length of the key
* Code derived from: https://www.geeksforgeeks.org/c-program-find-size-file/
*/
long int findSize(char file_name[])
{
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r");

    // checking if the file exist or not 
    if (fp == NULL) {
        printf("File Not Found!\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    // calculating the size of the file 
    long int res = ftell(fp);

    // closing the file 
    fclose(fp);

    return res;
}


int main(int argc, char* argv[]) {
    int socketFD, portNumber, charsWritten, charsRead;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER];
    char messagelengthchar[BUFFER];
    char keylengthchar[BUFFER];
    char message[BUFFER];
    char key[BUFFER];
    FILE* FD;
    FILE* Getkey;
    // Check usage & args
    if (argc < 3) {
        fprintf(stderr, "USAGE: %s hostname port\n", argv[0]);
        exit(0);
    }

    // Create a socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("CLIENT: ERROR opening socket");
    }

    // Set up the server address struct
    setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

    //set up and create the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        error("CLIENT: ERROR opening socket");
    }

    //Check to see if our connection to the server has failed
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        error("CLIENT: ERROR connecting");
    }
    //Open the file specified by the client in read mode
    FD = fopen(argv[1], "r");                                               
    //Make sure that opening the file was successful
    if (FD == NULL)                                                          
    {
        error("Failed");
    }
    //Store the file into the message array
    fgets(message, BUFFER, FD);                                            
    //Open the file specified by the user (client)
    Getkey = fopen(argv[2], "r");                                               
    //Make sure we are actually able to open the file 
    if (Getkey == NULL)                                                           
    {
        error("Failed");
    }
    //Stores the key into our array 
    fgets(key, BUFFER, Getkey);                                                  

    //Get the length of our file
    long filelen = findSize(argv[1]);
    //Ge the length of our key
    long keylen = findSize(argv[2]);
    //Store our long type variable into a messagecharlength and convert to a char
    sprintf(messagelengthchar, "%d", filelen);
    //Store our long type variable into a keylengthchar and convert to a char
    sprintf(keylengthchar, "%d", keylen);
    //checking to see if the length of the file is greater than the key
    if (filelen > keylen) {
        printf("Error: key is too short\n");
        error("Error: key is too short\n");
    }


/************************************************************************************************************************
*									***Send Messag Length***
* Sending all of the information will require multile itterations so we must break it into parts
* First, we will send the length of the message, since that is the first piece of information recieved by the server
* we also send a message of validation to check the connection status between server and client
*************************************************************************************************************************/
    
    //we ask the server for confirmation
    charsWritten = send(socketFD, messagelengthchar, strlen(messagelengthchar), 0);

    //Otherwise we were not able to write to the socket
    if (charsWritten < 0) {
        error("CLIENT: ERROR writing to socket");
    }
    //If we see that what we have written is less than the length of the message, then we know we still have more data to send
    if (charsWritten < strlen(messagelengthchar)) {
        printf("CLIENT: WARNING: Not all data written to socket!\n");
    }
    //Clear the buffer for reuse
    memset(buffer, '\0', BUFFER);

    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
    if (charsRead < 0) {
        error("CLIENT: ERROR reading from socket");
    }

    //Check to see that we have a valid connection, if it does not equal our marker then connection is not valid
    if (buffer[0] != '+') {
        error("ERROR not a valid connection established between client and server");
    }
    //Check to see if we were not able to read from the socket
    if (charsRead < 0) {
        error("CLIENT: ERROR reading from socket");
    }


/************************************************************************************************************************
*									***Send Key Length***
* Next we will send the length of our key, since that is the next thing the server is looking to recieve
*************************************************************************************************************************/
    //Send the message to the server(keylength) and write
    charsWritten = send(socketFD, keylengthchar, strlen(keylengthchar), 0);
    //checking for error writing to the socket
    if (charsWritten < 0) {
        error("CLIENT: ERROR writing to socket");
    }

    //check to see if data has transferred across socket
    if (charsWritten < strlen(keylengthchar)) {
        error("CLIENT: WARNING: Not all data written to socket!\n");
    }

    //Reset the buffer for use again
    memset(buffer, '\0', BUFFER);

    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);

    //Check for error when reading from socket
    if (charsRead < 0) {
        error("CLIENT: ERROR reading from socket");
    }

/************************************************************************************************************************
*									***Send Message***
* Next we will send our message to the server, as this is what the server will be looking to recieve
*************************************************************************************************************************/

    //This point is where we send the actual message from the user
    charsWritten = send(socketFD, message, strlen(message), 0);

    //checking for error writing to the socket
    if (charsWritten < 0) {
        error("CLIENT: ERROR writing to socket");
    }

    // Checks to make sure all the data has transferred across socket
    if (charsWritten < strlen(message)) {
        error("CLIENT: WARNING: Not all data written to socket!\n");
    }

    //clear the buffer
    memset(buffer, '\0', BUFFER);

    //Read data from the socket, leaving \0 at the end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);

    //Check for error when reading from socket
    if (charsRead < 0) {
        error("CLIENT: ERROR reading from socket");
    }

/************************************************************************************************************************
*									***Send Key***
* Next we will send the length of our key, since that is the next thing the server is looking to recieve
*************************************************************************************************************************/
    //Send the key to the server and write 
    charsWritten = send(socketFD, key, strlen(key), 0);                           
    //Otherwise we have failed to write
    if (charsWritten < 0)                                                          
    {
        error("CLIENT: ERROR writing to socket");
    }
    //If charswritten is less than the legth of the key, we know we still have data to send
    if (charsWritten < strlen(key))                                                
    {
        error("CLIENT: WARNING: Not all data written to socket!\n");
    }
    //Clear the buffer for reuse
    memset(buffer, '\0', BUFFER);

    //Get data from the server, leaving a \0 at the end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);                               
    //Otherwise we were not able to read from the socket
    if (charsRead < 0)                                                            
    {
        error("CLIENT: ERROR reading from socket");
    }
    
/************************************************************************************************************************
*									***Send the encrypted message***
* Next we will send the length of our key, since that is the next thing the server is looking to recieve
*************************************************************************************************************************/
    //Reset the buffer for reuse
    memset(buffer, '\0', BUFFER);

    // Read data from the socket, leaving \0 at end
    charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);                               
    //Otherwise we could not retrieve information from the socket
    if (charsRead < 0){
        error("CLIENT: ERROR reading from socket");
    }
    //Display the contents of our buffer
    printf("%s", buffer);
    //Decriment the length to see if we have reached the end of the fileinfo
    filelen -= strlen(buffer);                                              
    //If we have not, then we must go retrieve the rest of the data
    for (int i = 0; filelen > 0; i++ ) {
        //Check when the contents of the file are emppty, so we know when to break out
        if (strlen(buffer) == 0)                                                    
        {
            break;
        }
        else
        {
            // Clear out the buffer again for reuse
            memset(buffer, '\0', BUFFER); 
            // Read data from the socket, leaving \0 at end
            charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);                            
            // Checks for an error when reading from the socket
            if (charsRead < 0)                                                       
            {
                error("CLIENT: ERROR reading from socket");
            }
            // Decrements the length
            filelen -= strlen(buffer);                                          
            //Display the contents of the buffer
            printf("%s", buffer);                                                     
        }
    }
    //Close the socket
    close(socketFD);                                                             

    return 0;
}


