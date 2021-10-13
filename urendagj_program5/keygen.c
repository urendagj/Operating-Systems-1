/**********************************************************************
** * Program: Assignment 5 One Time Pads
** * Author: Jacob Urenda Garibay
** * Date: December 1, 2020
** * Description: Generates a key for encryption and decryption use
** * Input: Use argv[] to get specified commands from the user to execute
** * Output: Random key ranging from A - Z with ' ' inclusive
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




int main(int argc, char* argv[]) {
    /*This keygen file is what we will be using to generate a random key of letters
    * we will store these random instances of letters to get our key
    */
    if (argc != 2) {
        printf("Not enough arguments");
    }

    srand(time(NULL));
    /*Initialize all of our parameters
    * we set the length of the key to be the length of the argument + 1
    * Our key will randomly be chosen from A - Z with ' ' also inclusive
    */
    int length;
    length = atoi(argv[1]);
    char key[length + 1];
    char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    char random;

    //Itterate through the length of our key
    for (int i = 0; i < length; i++) {
        //get our random letter
        random = alphabet[rand() % 27];
        //Assign that random letter to our key
        key[i] = random;
    }
    //set a null termination char at the end of our aray
    key[length] = '\0';
    //Display the key
    printf("%s\n", key);
    
    
    return 0;
    

}
