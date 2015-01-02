/*=====================================================================================================
  CO314 : Systems and Network Programming Lab - Project I - Iteration I
  Group 16 : E10/049 - E10/170 - E/08/406
=======================================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define HASH_TABLE_SIZE 10000
#define SIZE_OF_WORD 24
/*=====================================================================================================*/
typedef struct node *NODE;
struct node{
	char data[SIZE_OF_WORD];
	int freq;
	NODE next;
};
/*=====================================================================================================*/
int is_punctuation(char c);
int hash(char data[]);
NODE create_node(char data[],int frequency);
void insert(NODE hashtable[],char key[]);
void print_result(NODE hashtable[]);
void delete_node(NODE hashtable[],char key[]);
int search_key(NODE hashtable[],char key[]);
/*=====================================================================================================*/
int main(int argc,char *argv[]){

	int i=0;
	NODE hashtable[HASH_TABLE_SIZE]={NULL};
	char temps[SIZE_OF_WORD],tempc;
	
	/* get characters one by one from the standerd input and store in 'tempc' until EOF is met */
	while((tempc=getchar())!=EOF){
		
		/* make character lowercase */
		tempc=tolower(tempc);
		
		/* if the character is a punctuation mark,*/
		if(is_punctuation(tempc)){
			
			/* then make last character '\0' to end the word */
			temps[i]='\0';
			
			/* if the word is not empty, then insert it to the hashtable */
			if(!temps[0]==0)
				insert(hashtable,temps); 
				
				/* for start storing new word to temps from next loop */
				i=0;
		}
		/* if the character is not a punctuation mark then insert it to 'temps' and continue the loop */
		else
			temps[i++]=tempc;
	}
	
	/* store the last word before EOF (loop exited without storing the last word)*/
	temps[i]='\0';
	if(!temps[0]==0)
		insert(hashtable,temps);
	
	/* load the stopwords to a file pointer */
	FILE *stopwords_file_pt=fopen(argv[1],"r");
	
	/* scan for words and store them in 'temps' */
	while(fscanf(stopwords_file_pt,"%s",temps)!=EOF){
		
		/* make the stopword lowercase by looping character by character*/
		i=0;
		while(temps[i]!='\0'){
			temps[i]=tolower(temps[i]);
			i++;
			}
			
		/* delete the stopword if it found in the hashtable */
		if(search_key(hashtable,temps)){
			delete_node(hashtable,temps);
		}
	}

	/* close the stop word file */
	fclose(stopwords_file_pt);
	
	/* print the results to the standers output */
	print_result(hashtable);
	
	return 0;
}

/*======================================================================================================
 * 	 function to check wether the given character is a given punctuation mark
 * ===================================================================================================*/
int is_punctuation(char c){
	
	int i;
	/* list of punctuation marks given for seperate the words*/
	char punctuations[]={' ','\t','\r','\n',',','.','?','!',';',':','-','_','"','(',')'};

	for(i=0;i<sizeof(punctuations);i++){
		/* if the character is one of the given punctuation mark, then return 1 */
		if(c==punctuations[i]){
			return 1;
		}
	}
	/* if the character is not a punctuation mark, then return 0 */
	return 0;
}
/*======================================================================================================
 *  hash function for hashtable
 * ===================================================================================================*/
int hash(char data[]){
	
   unsigned int hash = 0,i;
   for(i = 0; i < strlen(data);i++){
      hash+=data[i]*(383/(i+1));
  }
   return hash % HASH_TABLE_SIZE;
}
/*======================================================================================================
 * 	search for a key value in hashtable and return the frequency value
 * ===================================================================================================*/
int search_key(NODE hashtable[],char key[]){
	
	int index=hash(key);

	/* go to the key using the hash value */
	NODE current_node=hashtable[index];
	
	/* go through the chained links until finds the correct node with key*/
	while(current_node){
		if(!strcmp(current_node->data,key))
			return current_node->freq;
		current_node=current_node->next;
	}
	/* if the key is not in the hashtable return 0 */
	return 0;
}
/*======================================================================================================
 *  function to create a node in hashtable wth the given key value and frequency
 * ===================================================================================================*/
NODE create_node(char data[],int frequency){
	
	NODE new_node=malloc(sizeof(struct node));
	strcpy(new_node->data,data);
	new_node->freq=frequency;
	new_node->next=NULL;
	return new_node;
}
/*======================================================================================================
 * 	 function to insert a node to hashtable
 * ===================================================================================================*/
void insert(NODE hashtable[],char key[]){
	
	int index=hash(key);

	/* if the position of the index value is empty, then create a new node with given key and frequency */
	if(!hashtable[index]){
		hashtable[index]=create_node(key,1);
		return;
		}

	/* if already there is a node, (collision)*/
	NODE current_node=hashtable[index];

		/*find the node with same key and increase the frequency value by 1*/
		while(current_node->next!=NULL){
			if(!strcmp(current_node->data,key)){
				current_node->freq++;
				return;
			}
			current_node=current_node->next;
		}
		if(!strcmp(current_node->data,key)){
			current_node->freq++;
			return;
			}

		/* if there is no node with the given key value, then create a new node at the end */
		current_node->next=create_node(key,1);
}
/*======================================================================================================
 	function to delete a node from hashtable
 * ===================================================================================================*/
void delete_node(NODE hashtable[],char key[]){

	int index=hash(key);
	NODE current_node=hashtable[index]->next;
	NODE previous_node=hashtable[index];
	
	/* if there is only one node then that is the key. Then delete it */
	if(hashtable[index]->next==NULL){
		hashtable[index]=NULL;
		return;
	}
	/* else find the key and delete it */
	else if(!strcmp(hashtable[index]->data,key)){
		hashtable[index]=hashtable[index]->next;
	}
	else{
		while(current_node!=NULL){
			if(!strcmp(current_node->data,key)){
				previous_node->next=current_node->next;
				return;
			}
		previous_node=current_node;
		current_node=current_node->next;
		}
	previous_node->next=NULL;
	}
}
/*===========================================================================
	function to print the hashtable to the standerd output
===========================================================================*/
void print_result(NODE hashtable[]){
	
	int i; 
	NODE current_node;
	
	for(i=0;i<HASH_TABLE_SIZE;i++){
		/* if the current position(index i)in hashtable is empty, then continue*/
		if(!hashtable[i]){
			continue;}
		/* loop through the linked nodes and print the key and frequency values */
		current_node=hashtable[i];
		while(current_node!=NULL){
			printf("%s\t%d\n",current_node->data,current_node->freq);
			current_node=current_node->next;
			}
	}
}
/*==========================================================================*/
