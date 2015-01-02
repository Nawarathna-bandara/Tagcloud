/*=================================================================================================================================================
 * CO314 : Systems and Network Programming Lab - Project I - Iteration 3
 * Group 16 : E10/049 - E10/170 
 *=================================================================================================================================================
 * Important notes-
 * 
 * 		- text file of excluded words is hard coded as "english.stop" and should be in the same directory this file in.
 * 		- names of the directories took as command line arguments
 * 		- it is assumed that the maximum number of words in one directory is 100000.
 * 
 * ==============================================================================================================================================*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
/*=================================================================================================================================================*/
#define MAX_WORDS_IN_DIRECTORY 100000
#define HASH_TABLE_SIZE 10000
#define SIZE_OF_WORD 128
/*=================================================================================================================================================*/
typedef struct node *NODE;                                             /* structure for hashtable */
struct node{
	char data[SIZE_OF_WORD];
	int freq;
	NODE next;
};
/*=================================================================================================================================================*/
int hash(char data[]);
NODE create_node(char data[],int frequency);
void insert(NODE hashtable[],char key[],int frequncy);
void print_table(NODE hashtable[]);
int search_key(NODE hashtable[],char key[]);
/*=================================================================================================================================================*/
int main(int argc, char *argv[]){

	if(argc<2){
		puts("please enter directories as command line arguments");
		return 0;
		}
	
    int i,pid,pipes[argc-1][2],fd,count,tempf,n,offset=0 ;
    char cmd[256];
    char path[64];
    char temps[SIZE_OF_WORD];
    char *tempbuffer = malloc(MAX_WORDS_IN_DIRECTORY);                  
    char *mainbuffer = malloc(MAX_WORDS_IN_DIRECTORY*argc);             
    
    
    /* since the directories are given as command line arguments (argc-1) is the number of directories. we create a loop with (argc-1) iterations*/
    for(i=0; i<argc-1;i++){
		
		/* create a pipe for every directory to communicate with parent */
		if(pipe(pipes[i])){
			perror("error Creating pipe");
		return -1;
		}
		
		/*create a new prcess*/
		pid = fork();
		/* parent should wait until child finish his work */
		wait(0);
		
		/* ============================== children's work =============================================================================*/
		
        if(pid==0){
			
			/* to preprocess the html files we use shell commands. all the neccessary commands will execute once using pipes.
			 * first we store the command in string variable and then use 'system()' function to execute them 
			 * preprocessing steps
			 * 		- merge all html files in current directory to a single file using 'cat'
			 * 		- move tags to seperate lines so its easy to 'awk' to filter content between body tags
			 * 		- remove html escapes
			 * 		- extract the content between body tags
			 * 		- remove html tags
			 * 		- count the frequencies of each word using 'wordfrequency' executable file (from iteration 1)
			 * final frequency count will written to a file called 'result' so every directory has their own 
			 * 'result' file containing frequency counts of that directory*/
			 
			 /*all the above commands stored in cmd string variable*/
			sprintf(cmd,"cat %s/*.html | sed 's/<[^>]*>/\\n&\\n/g' | sed 's/&[a-zA-Z0-9#;]*//g' | awk '/<body/{a=1;next}/<\\/body>/{a=0}a' | sed 's/<[^>]*>//g' | ./wordfrequency english.stop > %s/result",argv[i+1],argv[i+1]);
			/* clall 'cmd' inside system for html preprocessing */
			system(cmd);
			
			/* since we dont read from child, close the read end of the pipe */
			close(pipes[i][0]);
			
			/*store the path of the result file in 'path' variable*/
			sprintf(path,"%s/result",argv[i+1]);
			
			/* open the result file*/
			fd = open(path, O_RDONLY);
			
			/*read from the result file and store them in temporary buffer*/
			count=read(fd,tempbuffer,MAX_WORDS_IN_DIRECTORY);
			
			/*write the content in temporay buffer to pipe */
			write(pipes[i][1],tempbuffer,count);
			
			/* close the write end of the pipe */
			close(pipes[i][1]);
			
			/*exit from child*/
            exit(1);
        }
	}
	
	/* ============================== parent's work =============================================================================*/
	if(pid  > 0){  
		
		/*parent has to read from all the pipe childs wrote. so we create a loop to iterate through pipes */
		for(i=0;i<argc-1;i++){
			
			/* close the write ends */
			close(pipes[i][1]);
			
			/* read from the red end of the pipe */
			read(pipes[i][0],tempbuffer,MAX_WORDS_IN_DIRECTORY);
			
			/* add the data in tempbuffer to mainbuffer. data fromeach pipe will add in main buffer */
			strcat(mainbuffer,tempbuffer);
			
			/* close the read end */
			close(pipes[i][0]);
		}
		
		/* hashtable for store final result */
		NODE hashtable[HASH_TABLE_SIZE]={NULL};

		tempf=1;
		
		/* scan the main buffer and insert the words and frequencies to hashtable. when similar words found, their frequencies will add up*/
		while(sscanf(mainbuffer+offset, "%s %d%*c%n",temps,&tempf, &n)==2){
			insert(hashtable,temps,tempf);
			offset+=n;
			}
		
		/* print the inl result to the standerd output */
		print_table(hashtable);
		
		/* remove the result files created in the middle of the process*/
		system("rm */result ");
	}
	
    return 0;
}
/*=================================================================================================================================================
 * following functions are for hashtable operations taken from iteration 1 code. we changed the functions slightly so the frequencies for similar 
 * words will add when they inserted to the hashtable. since the functions are explained in iteration 1 code we didn't explain them here.
==================================================================================================================================================*/
/*==========================================================================================
 hash function for hashtable
 ========================================================================================*/
int hash(char data[]){
   unsigned int hash = 0,i;
   for(i = 0; i < strlen(data);i++){
      hash+=data[i]*(383/(i+1));
  }
   return hash % HASH_TABLE_SIZE;
}

/*===========================================================================================
 *  search for a key value in hashtable and return the frequency value
 * ======================================================================================= */
int search_key(NODE hashtable[],char key[]){
	
	int index=hash(key);
	
	NODE current_node=hashtable[index];
	
	while(current_node){
		if(!strcmp(current_node->data,key))
			return current_node->freq;
		current_node=current_node->next;
	}
	return 0;
}
/*============================================================================================
 *  function to create a node in hashtable with the given key value and frequency 
 * =========================================================================================*/

NODE create_node(char data[],int frequency){
	
	NODE new_node=malloc(sizeof(struct node));
	strcpy(new_node->data,data);
	new_node->freq=frequency;
	new_node->next=NULL;
	return new_node;
}
/*===========================================================================================
 function to insert a node to hashtable 
===========================================================================================*/
void insert(NODE hashtable[],char key[],int frequncy){
	
	int index=hash(key);
	if(!hashtable[index]){
		hashtable[index]=create_node(key,frequncy);
		return;
		}
	NODE current_node=hashtable[index];
	while(current_node->next!=NULL){
		if(!strcmp(current_node->data,key)){
			current_node->freq+=frequncy;
			return;
		}
		current_node=current_node->next;
	}
	if(!strcmp(current_node->data,key)){
		current_node->freq+=frequncy;
		return;
		}
	current_node->next=create_node(key,frequncy);
}

/*===========================================================================================
function to print the hashtable to the standerd output 
* ==========================================================================================*/
void print_table(NODE hashtable[]){
	int i; 
	NODE current_node;
	for(i=0;i<HASH_TABLE_SIZE;i++){
		if(!hashtable[i]){
			continue;}
		current_node=hashtable[i];
		while(current_node!=NULL){
			printf("%s\t%d\n",current_node->data,current_node->freq);
			current_node=current_node->next;
		}
	}
}
/*==================================================================================================================================================*/
