Program 4

Command to compile:
gcc -lpthread -o line_processor  main.c --std=gnu99

To run program with manual input:
./line_processor

To run program with input from a file. 
note to enter the specific file name wanted to get input from (example: filename.txt)
command:
./line_processor < filename.txt


To create a text file of the program output with textfile input
Make sure that the source is the .txt file name you want to get input from (example: source.txt)
Also make sure to make a name for the text file your program will output to (example: output.txt)
command:
./line_processor < source.txt > output.txt
