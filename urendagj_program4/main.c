#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h> // must link with -lm
#include <string.h>




char* lineinput;


void* replacePlusSigns();
void put1(char* );
void* LineSeparator();
void* get_input();
void* OutputThread();
char* get1();
char* get2();
void put2(char*);
void put3(char*);
char* get3();


char buffer[1000];
char* buffer_1[1000];
int count1 = 0;
int prod_index_1 = 0;
int put_down_1 = 0;
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;

char* buffer_2[1000];
int count2 = 0;
int prod_index_2 = 0;
int put_down_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;

char* buffer_3[1000];
int count3 = 0;
int prod_index_3 = 0;
int put_down_3 = 0;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

//Consumer - Producer interaction for last thread with put3 and get3
void put3(char* put_line_3) {
	pthread_mutex_lock(&mutex_3); // lock mutex
	buffer_3[prod_index_3] = put_line_3; // put line into buffer
	prod_index_3++; // increment put down
	count3++; // increment number of lines still in buff
	pthread_cond_signal(&cond3); // singal buff not empty
	pthread_mutex_unlock(&mutex_3); // unlock mutex
}
char* get3() {
	pthread_mutex_lock(&mutex_3); // lock mutex
	while (count3 == 0) { // wait for buffer to have line
		pthread_cond_wait(&cond3, &mutex_3);
	}
	char* get_line_3 = buffer_3[put_down_3]; // get line
	put_down_3++; // increment pick up
	count3--; // increment number of lines still in buff
	pthread_mutex_unlock(&mutex_3); // unlock the mutex

	return get_line_3;
}





void put2(char* put_line_2) {
	pthread_mutex_lock(&mutex_2); // lock mutex
	buffer_2[prod_index_2] = put_line_2; // put line into buffer
	prod_index_2++; // increment put down
	count2++; // increment number of lines still in buff
	pthread_cond_signal(&cond2); // singal buff not empty
	pthread_mutex_unlock(&mutex_2); // unlock mutex
}
char* get2() {
	pthread_mutex_lock(&mutex_2); // lock mutex
	while (count2 == 0) { // wait for buffer to have line
		pthread_cond_wait(&cond2, &mutex_2);
	}
	char* get_line_2 = buffer_2[put_down_2]; // get line
	put_down_2++; // increment pick up
	count2--; // increment number of lines still in buff
	pthread_mutex_unlock(&mutex_2); // unlock the mutex

	return get_line_2;
}



void put1(char* put_line_1) {
	pthread_mutex_lock(&mutex_1); // lock mutex
	buffer_1[prod_index_1] = put_line_1; // put line into buffer
	prod_index_1++; // increment put down
	count1++; // increment number of lines still in buff
	pthread_cond_signal(&cond1); // singal buff not empty
	pthread_mutex_unlock(&mutex_1); // unlock mutex
}

char* get1() {
	pthread_mutex_lock(&mutex_1); // lock mutex
	while (count1 == 0) { // wait for buffer to have line
		pthread_cond_wait(&cond1, &mutex_1);
	}
	char* get_line_1 = buffer_1[put_down_1]; // get line
	put_down_1++; // increment pick up
	count1--; // increment number of lines still in buff
	pthread_mutex_unlock(&mutex_1); // unlock the mutex

	return get_line_1;
}

/*************************************************************************************
* This function will be handeled by thread 2
* thread 2 will be responsible for replacing every line seperator (aka \n) with a ' '
* Consumer to input - Producer to Plus_Sign
**************************************************************************************/
void* LineSeparator() {
	int signal = 0;
	while (signal == 0) {
		char* data = get1();
		//printf("Recieved |%s|\n", data);
		if (strcmp(data, "STOP\n") == 0) {
			signal = 1;
		}
		else {
			int length = strlen(data);

			//printf("string contents: %s| \n", data);

			for (int i = 0; i < length; i++) {
				//printf("Made it before if");
				if (data[i] == '\n') {
					//printf("Comparison made");
					data[i] = ' ';

				}

			}
			
			//printf("Finished: %s| \n", data);
		}
		put2(data);
	}

	//printf("Thread 1 ending \n");
	return NULL;
}

char* recieveinput() {
	char* line = NULL;
	size_t buff = 1000;

	getline(&line, &buff, stdin); // get user input line
	//printf("Given |%s|\n", line);
	return line;
}

/**********************************************
* This function is going to be run by thread 1
* thread 1 will read in lines of characters from standard input
* This thread will act as a producer
************************************************/
void* get_input() {

	//char* data;
	size_t size = 1000;
	int signal = 0;
	while (signal == 0) {
		
			//printf("Type here: "); 
			//fgets(data, sizeof(data), stdin);
		char* data = (char*)malloc((size + 1) * sizeof(char));
		do {
			data = recieveinput();
			put1(data);
		} while (strcmp("STOP\n", data) != 0);
		signal = 1;
		//fprintf(stdout, ">INPUT DONE\n");
	
		
		/*getline(&data, &size, stdin);
		fflush(stdin);

			put1(data);*/

			//if (strcmp(data, "STOP\n") == 0) {
			//	signal = 1;
			//	//printf("I made it into the stop conditon!");
			//}
	
	}

	return NULL;
}



/*****************************************************************
* This function is going to be run by thread 3
* It will be used to replace any instance of ++ with a single ^
* Must continue running until signal STOP\n recieved
* Consumer to LineSeparator - Producer to OutputThread
******************************************************************/
void* replacePlusSigns() {
	//++ expansion
	int signal = 0;
	while (signal == 0) {
		char* data = get2();
		//printf("Recieved |%s|\n", data);
		if (strcmp(data, "STOP\n") == 0) {
			signal = 1;

		}
		else {
			char tempbuff[1000];
			char tempbuff2[1000];

			//int i = 0;
			int length = strlen(data);
			//printf("Check: |%s|%d|\n", data, length);
			//printf("string contents: %s| \n", data);

			for (int i = 0; i < length; i++) {
				//printf("Made it before if");
				if (data[i] == '+' && data[i + 1] == '+') {
					//printf("Comparison made");
					for (int j = 0; j < i; j++) {
						tempbuff[j] = data[j];
						
					}
					int m = i;
					for (int k = 0; k < (length - i + 2); k++) {
						tempbuff2[k] = data[m + 2];
						m++;
					}
					memset(data, '\0', length);
					strcat(data, tempbuff);
					strcat(data, "^");
					strcat(data, tempbuff2);
					memset(tempbuff, '\0', sizeof(tempbuff));
					memset(tempbuff2, '\0', sizeof(tempbuff2));
					//printf("Working: |%s| \n", data);

				}

			}

			//printf("Finished: |%s| \n\n", data);
			
		}
		put3(data);
	}
	//printf("Thread 2 ending \n");
	return NULL;
}

/*************************************************************************************************
* This function will be run by therad 4
* The output thread will write processed data to standard output as lines of exactly 80 characters 
* Consumer to PlusSign
***************************************************************************************************/
void* OutputThread() {
	int signal = 0;
	while (signal == 0) {
		char* data = get3();
		//printf("Recieved |%s|\n", data);
		if (strcmp(data, "STOP\n") == 0) {
			signal = 1;

		}
		else {
			char tempbuff[1000];
			char tempbuff2[1000];
			//get needed data
			strcat(buffer, data);

			int length = strlen(buffer);
			//change to actual size
			while(length > 80) {
				for (int j = 0; j < 80; j++) {
					tempbuff[j] = buffer[j];
				}
				printf("%s \n", tempbuff);
				for (int k = 0; k < (length - 80); k++) {
					tempbuff2[k] = buffer[k + 80];
				}
				//printf("Remaining chars: |%s| \n\n", tempbuff2);
				memset(buffer, '\0', sizeof(buffer));
				strcat(buffer, tempbuff2);
				//printf("Remaining chars back in buffer: |%s| \n\n", buffer);
				memset(tempbuff, '\0', sizeof(tempbuff));
				memset(tempbuff2, '\0', sizeof(tempbuff2));
				length = length - 80;
			}


		}
	}
	//printf("Thread 3 ending \n");
	return NULL;
}

int main(int argc, char**argv) {

	
	
	//Initialize all of the threads that will needed to be run
	pthread_t Input_Thread;
	pthread_t Line_Separator;
	pthread_t Plus_Sign;
	pthread_t Output_Thread;

	//Allow the treads to perform their intended actions
	pthread_create(&Input_Thread, NULL, get_input, NULL);
	pthread_create(&Line_Separator, NULL, LineSeparator, NULL);
	pthread_create(&Plus_Sign, NULL, replacePlusSigns, NULL);
	pthread_create(&Output_Thread, NULL, OutputThread, NULL);

	//Wait for the threads to terminate
	pthread_join(Input_Thread, NULL);
	pthread_join(Line_Separator, NULL);
	pthread_join(Plus_Sign, NULL);
	pthread_join(Output_Thread, NULL);

	return EXIT_SUCCESS;
	





}