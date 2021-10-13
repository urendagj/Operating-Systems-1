#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

#define PREFIX "movies_"

/* struct for movie information*/
struct movie
{
	char* Title;
	char* releaseyear;
	char* Languages;
	char* Rateval;
	struct movie* next;


};

/* Function derived from students.c in provided example code: https://repl.it/@cs344/studentsc#main.c
* creates a movie struct with the data in the current line
*/
struct movie* getmovie(char* curr) {
	struct movie* movie = malloc(sizeof(struct movie));
	//used as a perameter to save in the token
	char* savetoken;

	//First token is to get the data for the Title
	char* token = strtok_r(curr, ",", &savetoken);
	movie->Title = calloc(strlen(token) + 1, sizeof(char));
	//copy contents of the movie title into repsective location
	strcpy(movie->Title, token);

	//Second token to get the release year of the movie
	token = strtok_r(NULL, ",", &savetoken);
	movie->releaseyear = calloc(strlen(token) + 1, sizeof(int));
	strcpy(movie->releaseyear, (token));

	//Third token is to get the languages for the movie
	token = strtok_r(NULL, ",", &savetoken);
	movie->Languages = calloc(strlen(token) + 1, sizeof(char));
	strcpy(movie->Languages, token);

	//fourth token is for the movies rating value
	token = strtok_r(NULL, ",", &savetoken);
	movie->Rateval = calloc(strlen(token), sizeof(char));
	strcpy(movie->Rateval, token);

	movie->next = NULL;

	return movie;


}

/*Function derived from students.c in provided example code: https://repl.it/@cs344/studentsc#main.c
* Returns a linked list of movies by parsing the data from each line of the sepcified file
*/
struct movie* fileparse(char* filePath) {
	FILE* file;
	//open the file in read mode
	file = fopen(filePath, "r");

	//display error message in case csv file fails to open
	if (file == NULL) {
		perror("The file was unable to open.");
		exit(1);
	}


	char* token;
	char* currLine = NULL;
	size_t nread;
	size_t len = 0;

	//we create the head of the linked list here
	struct movie* head = NULL;
	//we create the tail of the linked list here
	struct movie* tail = NULL;
	getline(&currLine, &len, file);

	while ((nread = getline(&currLine, &len, file)) != -1) {

		//Get the movie that corresponds to the current line
		struct movie* newmovie = getmovie(currLine);

		//Checking to see if it's the first node in the linked list
		if (head == NULL) {
			//set head to be the first node in the linked list
			//both head and tail set to the Newnode
			head = newmovie;
			tail = newmovie;
		}
		else {
			//We have found that this is not the first node
			//add the node to our linked list and advance the tail to the next node
			tail->next = newmovie;
			tail = newmovie;
		}


	}
	free(currLine);
	fclose(file);
	return head;



}

/* function derived from student.c from example code provided: https://repl.it/@cs344/studentsc#main.c
* Print data for the given movie
*/
void printmovie(struct movie* movies) {
	printf("%s,%s,%s,%s\n", movies->Title,
		movies->releaseyear,
		movies->Languages,
		movies->Rateval);
}

/*function derived from student.c from example code provided: https://repl.it/@cs344/studentsc#main.c
* Print the linked list of movies
*/
void printmovielist(struct movie* movielist) {

	while (movielist != NULL) {
		printmovie(movielist);
		movielist = movielist->next;


	}



}
//Function was derived from geeksforgeeks: https://www.geeksforgeeks.org/bubble-sort-for-linked-list-by-swapping-nodes/
//This function allows us to perform the swapping of nodes needed in bubble sort
//Changes made to this function include using my struct instead of the default
struct movie* swap(struct movie* ptr1, struct movie* ptr2) {
	struct movie* tmp = ptr2->next;
	ptr2->next = ptr1;
	ptr1->next = tmp;
	return ptr2;
}

//Function was derived from geeksforgeeks: https://www.geeksforgeeks.org/bubble-sort-for-linked-list-by-swapping-nodes/
//I decided to use bubblesort in the assignment because I could sort the ratings from highest to lowest.
//This would allows for the first instance of that year to also be the highest rated movie for that year.
void bubbleSort(struct movie** head, int count) {
	struct movie** h;
	int i, j, swapped;
	for (i = 0; i <= count; i++) {
		h = head;
		swapped = 0;
		for (j = 0; j < count - i - 1; j++) {
			struct movie* p1 = *h;
			struct movie* p2 = p1->next;

			if (atoi(p1->releaseyear) > atoi(p2->releaseyear)) {
				//update the linked list after swapping 
				*h = swap(p1, p2);
				swapped = 1;
			}
			h = &(*h)->next;
		}
		//break if the loop ended without any swap
		if (swapped == 0)
			break;
	}
}
//Allows for the length of the file to be accounted for: i.e movies_sample_1.csv contains 24 movies
int countnumlines(struct movie* head) {
	int count = 0;
	while (head != NULL) {
		head = head->next;
		count++;
	}
	return count;
}

/*findearliest is used to find the oldest movie release year in our given csv file
* I used this function to set ranges for my forloops used in part 2
*/
int findearliest(struct movie* head) {
	int minvalue = atoi(head->releaseyear);
	struct movie* temp = head->next;
	while (temp != NULL) {
		if (atoi(head->releaseyear) > atoi(temp->releaseyear)) {
			minvalue = atoi(temp->releaseyear);
		}
		head = head->next;
		temp = temp->next;
	}
	return minvalue;
}

/*findlatest is used to find the newest movie release year in our given csv file
* I used this function to set ranges for my forloops used in part2
*/
int findlatest(struct movie* head) {
	int maxval = atoi(head->releaseyear);
	struct movie* temp = head->next;
	while (temp != NULL) {
		if (maxval < atoi(temp->releaseyear)) {
			maxval = atoi(temp->releaseyear);
		}
		head = head->next;
		temp = temp->next;
	}
	return maxval;
}

/*This function allows us the create the name needed for our directories being made
* the function assings the directory name to your_onid.movies.random so in my case it would be like urendagj.movies.5467
* Initialization for rand documentation can be found here: https://www.tutorialspoint.com/c_standard_library/c_function_srand.htm
*/
void createdirectoryname(char dirname[256]) {
	time_t t;
	int random = 0;
	//Seed rand()
	srand((unsigned)time(&t));
	random = rand() % 10000;
	char Dirname[256];
	//Add our random number to the end of our directory name
	sprintf(Dirname,"urendagj.movies.%d",random);
	//copy contents
	strcpy(dirname, Dirname);
	
}

/*This function is used to actually create a directory whenever needed
* We use the createdirectoryname function to get the name of our directory and assign that name to the directory being created
* This function also allows for the filepath to be created
*/
void  createdirectory(char filepath[256], char dirname[256]) {
	//Get the name for the directory
	createdirectoryname(dirname);
	struct stat st = { 0 };
	//create our directory
	mkdir(dirname, 0750);
	//Assign our file path
	sprintf(filepath, "%s/", dirname);



}

/*This function will initiate once the user has selected the option 1 in the prompt
* From here the user will be able to choose from three options: 1.Pick the largest file, 2.Pick the smallest file, 3.Specify a file name to be processed
*/
void options() {
	int choice = 0;
	int error = 1;
	while (error != 0) {
		printf("Which file you want to process \n");
		printf("Enter 1 to pick the largest file\n");
		printf("Enter 2 to pick the smallest file\n");
		printf("Enter 3 to specify the name of a file\n");
		printf("Enter a choice from 1 to 3: ");
		scanf("%d", &choice);
		if (choice == 1) {
			option1();
			error = 0;
		}

		else if (choice == 2) {
			option2();
			error = 0;
		}

		else if (choice == 3) {
			error = option3();
			
		}

	}
}

/*This will be the initial screen the user is greeted with when running the program
* The user can either choose to 1. Select a file to process or 2. Exit the program
*/
void prompt() {
	int error = 1;
	int choice = 0;
	while (error != 0) {
		printf("1. Select file to process \n");
		printf("2. Exit the program \n");
		printf("Enter a choice from 1 to 2 \n");
		scanf("%d", &choice);

		if (choice == 1) {
			options();
		}
		else if (choice == 2) {
			error = 0;
			//return EXIT_SUCCESS;
			
		}
	
	}
	
}



/*If option 1 is selected, the program will proceed to look for the largest csv file in our current directory
* After the largest csv file has been selected, the program will proceed to create a designated directory
* Text files according to years will also be made with the movies released in that year being placed inside the file
* Code in this function has samples from: https://canvas.oregonstate.edu/courses/1818570/pages/exploration-directories?module_item_id=20000705
*/
void option1() {
	// Open the current directory
	DIR* currDir = opendir(".");
	struct dirent* aDir;
	struct stat dirStat;
	struct movie* list;
	char* csv;
	char entryName[256];
	char dirname[256];
	char filepath[256];
	
	// Go through all the entries
		while ((aDir = readdir(currDir)) != NULL) {
			csv = strstr(aDir->d_name, ".csv");
			//checks to see if the file is a csv file
			if (csv) {
				if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {

					// Get meta-data for the current entry
					stat(aDir->d_name, &dirStat);
					int max = dirStat.st_size;
					//checks for largest file
					if (dirStat.st_size >= max) {
						max = dirStat.st_size;
						memset(entryName, '\0', sizeof(entryName));
						strcpy(entryName, aDir->d_name);
					}
				}
			}
		}
	// Close the directory
	closedir(currDir);
	list = fileparse(entryName);
	int csvlength = countnumlines(list);
	//sort the movies from earliest to latest release
	bubbleSort(&list, csvlength);
	createdirectory(filepath, dirname);
	
	printf("Now processing the chosen file named %s \n", entryName);
	printf("Created the directory with name %s \n", dirname);
	//Parse the entire file
	while (list != NULL) {
		//Get the path to create the needed text file
		sprintf(filepath, "%s/%s.txt", dirname,list->releaseyear);
		//Create our text file
		int file_descriptor = open(filepath, O_RDWR| O_APPEND | O_CREAT, 0640);

		if (file_descriptor == -1) {
			printf("open() failed on \"%s\"\n", filepath);
			perror("Error");
			exit(1);
		}
		//write our data to the file
		int howmany = write(file_descriptor, strcat(list->Title,"\n"), strlen(list->Title) + 1);
		close(file_descriptor);
		list = list->next;
	}
	//prompt();
	free(csv);
	free(aDir);

}

/*If option 2 is selected, the program will proceed to look for the smallest csv file in our current directory
* After the smallest csv file has been selected, the program will proceed to create a designated directory
* Text files according to years will also be made with the movies released in that year being placed inside the file
* Code in this function has elements from: https://canvas.oregonstate.edu/courses/1818570/pages/exploration-directories?module_item_id=20000705
*/
void option2() {
	DIR* currDir = opendir(".");
	struct dirent* aDir;
	struct stat dirStat;
	char* csv;
	char entryName[256];
	int min = dirStat.st_size;
	char dirname[256];
	char filepath[256];
	struct movie* list;
	// Go through all the entries
	while ((aDir = readdir(currDir)) != NULL) {
		csv = strstr(aDir->d_name, ".csv");
		//Check to see if the file is a csv file
		if (csv) {
			//Make sure that our file matches our set prefix
			if (strncmp(PREFIX, aDir->d_name, strlen(PREFIX)) == 0) {
				// Get meta-data for the current entry
				stat(aDir->d_name, &dirStat);
				//Assign min
				if(min <= 0) {
					min = dirStat.st_size;
				}
				//Check to see if our file is the smallest
				if (dirStat.st_size <= min) {
					min = dirStat.st_size;		
					memset(entryName, '\0', sizeof(entryName));
					strcpy(entryName, aDir->d_name);
	
				}
			}
		}
	}
	// Close the directory
	closedir(currDir);
	list = fileparse(entryName);
	int csvlength = countnumlines(list);
	//Sort our movies from earliest released to latest release
	bubbleSort(&list, csvlength);
	createdirectory(filepath, dirname);
	printf("Now processing the chosen file named %s \n", entryName);
	printf("Created the directory with name %s \n", dirname);
	//Travese the entire file
	while (list != NULL) {
		//get our needed filepath to create the text file
		sprintf(filepath, "%s/%s.txt", dirname, list->releaseyear);
		//Create the text file and set permissions 
		int file_descriptor = open(filepath, O_RDWR | O_APPEND | O_CREAT, 0640);
		if (file_descriptor == -1) {
			printf("open() failed on \"%s\"\n", filepath);
			perror("Error");
			exit(1);
		}
		//write to the text files
		int howmany = write(file_descriptor, strcat(list->Title, "\n"), strlen(list->Title) + 1);
		close(file_descriptor);
		list = list->next;
	}
	//prompt();
	free(csv);
	free(aDir);
}

/*This function will allow the user to choose a specified file based on their input (filename)
*/
int option3() {
	//Open our current directory
	DIR* currDir = opendir(".");
	struct dirent* aDir;
	struct stat dirStat;
	int file;
	char* csv;
	char entryName[256];
	int tracker = 0;
	char dirname[256];
	char filepath[256];
	struct movie* list;
	printf("Enter the complete file name: "); scanf("%s", &entryName);
	// Go through all the entries
	while ((aDir = readdir(currDir)) != NULL) {
		csv = strstr(aDir->d_name, ".csv");
		file = strcmp(entryName, aDir->d_name);
		//If the file is in fact a csv file continue on
		if (csv) {
			//Check to see if the input matches a file in the directory
			if (strncmp(entryName, aDir->d_name, strlen(entryName)) == 0) {

				// Get meta-data for the current entry
				stat(aDir->d_name, &dirStat);
				
					memset(entryName, '\0', sizeof(entryName));
					strcpy(entryName, aDir->d_name);
					//Increment to see if we have found anything
					tracker++;
				

			}
		}
	}
	//If we have not found anything then we warn the user and reprompt
	if (tracker == 0) {
		printf("The file %s was not found. Try again \n", entryName);
		return 1;
		//options();
	}
	else {
		// Close the directory
		closedir(currDir);
		list = fileparse(entryName);
		int csvlength = countnumlines(list);
		//Sort our movies from earliest release year to latest release year
		bubbleSort(&list, csvlength);
		createdirectory(filepath, dirname);
		printf("Now processing the chosen file named %s \n", entryName);
		printf("Created the directory with name %s \n", dirname);
		//Traverse the entire csv file
		while (list != NULL) {
			//Get the filepath needed to create our text files
			sprintf(filepath, "%s/%s.txt", dirname, list->releaseyear);
			//Create our text files based on movie year
			int file_descriptor = open(filepath, O_RDWR | O_APPEND | O_CREAT, 0640);
			if (file_descriptor == -1) {
				printf("open() failed on \"%s\"\n", filepath);
				perror("Error");
				exit(1);
			}
			//Write movie year entries to the text file
			int howmany = write(file_descriptor, strcat(list->Title, "\n"), strlen(list->Title) + 1);
			close(file_descriptor);
			list = list->next;
		}
		//prompt();
		free(csv);
		free(aDir);
		return 0;
	}
	
}


int main() {
	
	prompt();
		


	return 0;
	




}