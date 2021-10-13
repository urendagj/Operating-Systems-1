#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


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

	while ((nread = getline(&currLine,&len, file)) != -1) {
		
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

			if (atof(p1->Rateval) < atof(p2->Rateval)) {
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


//Allows for the ratings of each movie to be displayed
void printrating(struct movie* node) {
	struct movie* temp = node->next;
	while (temp != NULL) {
		float data = atof(temp->Rateval);
		printf("%.1f,\n" , data);
		temp = temp->next;
	}
}

/*findearliest is used to find the oldest movie release year in our given csv file
* I used this function to set ranges for my forloops used in part 2
*/
int findearliest(struct movie* head) {
	int minvalue = atoi(head->releaseyear);
	struct movie* temp = head->next;
	while (temp != NULL) {
		if (atoi(head->releaseyear) > atoi(temp->releaseyear)){
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
			maxval= atoi(temp->releaseyear);
		}
		head = head->next;
		temp = temp->next;
	}
	return maxval;
}

/// <summary>
// This function add_delimiter was provided in the Piazza forums by Grigori Barbachov
/// </summary>
/// This function essentially allows us to add the delimiter ';' so we can correctly identify the substing we need for comparisons 
char* add_delimiter(char* src)
{
	char* result = (char*)malloc(sizeof(char) * (strlen(src) + 3));
	strcpy(result, ";");
	strcat(result, src);
	strcat(result, ";");
	return result;
}


/*I read on the piazza forum that this method could have been done with tokens, but coming from C++ I had trouble, so I went with this method instead
* This function was made to go along with the add_delimiter function. We search the language data of the struct
*	and if it contains either '[' or ']', we replace that char with our needed delimeter ';'
* I have used this method for previous Cs classes, but I am curious how this would be done with tokens.
*/
void langformat(struct movie* head) {
	int start = findearliest(head);
	int end = findlatest(head);
	//Traverse linked list
		while (head != NULL){
			//Traverse to the end of the languages data
			for (int j = 0; j < strlen(head->Languages); j++) {
			//If we find an opening bracket, change the bracket to our delimiter for substring ';'
			if (head->Languages[j] == ']') {
					head->Languages[j] = ';';
					break;
				}
			//If we find opening bracket, replace it with delimiter for substring ';'
				else if (head->Languages[j] == '[') {
					head->Languages[j] = ';';
				}

			}
			//make sure we don't reach out of bounds
			if (head->next == NULL) {
				break;
		}
			head = head->next;
		}
}


/*This function allows for the user to find the movies released within a specific eyar
* If no movies are released, then it prompts the user with no data found
*/
void printmovieinyear(struct movie* list) {
	int error = 1;
	int movieyear = 0;
	int moviecount = 0;
	struct movie* temp;
	while (error != 0) {
		temp = list;
		//prompt and read in data
		printf("Enter the year you want to see movies: "); scanf("%d", &movieyear);
		//Traverse list
			while (list != NULL) {
				int year = atoi(list->releaseyear);
				//If movie at year found, print
				if (year == movieyear) {
					printf("%s \n", list->Title);
					moviecount++;
				}
				list = list->next;
			}
			error = 0;
		//If no movies are found tell the user
		if(moviecount == 0){
			printf("\nNo data for movies released in the year %d \n \n", movieyear);
		}
	}
	list = temp;
}


/*Prints the movies found for
*/
void printmoviesinlang(struct movie* list, struct movie* temp) {
	char targetlang[200];
	char notfound[200];
	char* choices;
	//struct movie* temp;
	printf("Enter the language you want to view movies for: "); scanf("%s", &targetlang);
	strcpy(notfound, targetlang);
	strcpy(targetlang, add_delimiter(targetlang));
	langformat(list);
	int count = 0;
	//traverses the linked list
	while (list != NULL) {
		choices = strstr(list->Languages, targetlang);
		//If our substring is found
		if (choices) {
			printf("%s %s \n", list->releaseyear, list->Title);
			count++;
		}
		//continue on
		list = list->next;
	}
	//If no movies found in specific lang
	if (count == 0) {
		printf("No data about movies released in %s \n", notfound);
	}

	list = temp;
}


int main(int argc, char *argv[])
{
	if (argc < 2) {
		printf("You must provide the name of the file to process \n");
		printf("Example Usage: ./movie_sample_1.csv \n");
		return EXIT_FAILURE;
	}
	
	int input = 0;
	int count = 0;
	
	struct movie* list = fileparse(argv[1]);
	count = countnumlines(list);
	
	printf("Processed file %s ", argv[1]);
	printf("and parsed for data for% d movies \n", count);
	while(input != 4){
	struct movie* temp = list;
	//int error = 1;
	printf("1. Show movies released in a specified year\n2. Show highest rated movie for each year\n3. Show the title and year of release of all movies in a specific language\n4. Exit from the program\n");
	printf("Enter a choice from 1 to 4: "); scanf("%d", &input);

	if (input == 1) {
		printmovieinyear(list);
	}
	
	else if (input == 2) {
		
		//get the length of the file
		int filelength = countnumlines(list);
		//sort ratings from highest to lowest
		bubbleSort(&list, filelength);
		struct movie* temp;
		temp = list;
		//get lowest range
		int a = findearliest(list);
		//get highest range
		int b = findlatest(list);
		//traverse for the beggining movie release year to the last movie year released
		for (int i = a; i <= b; i++) {
			//traverse for the length of the file
			for (int j = 0; j < filelength; j++) {
				//If we find that movie released, we know it's the highest because of how we sorted the ratings
				if (atoi(list->releaseyear) == i) {
					float rateval = atof(list->Rateval);
					printf("%s %s %.1f \n", list->releaseyear, list->Title, rateval);
					break;
				}
				list = list->next;

			}

			list = temp;
		}

	}

	else if (input == 3) {
		printmoviesinlang(list,temp);
	
	}
	

	else if (input == 4) {
		break;
	}
	
		

	}

	return EXIT_SUCCESS;
	



}