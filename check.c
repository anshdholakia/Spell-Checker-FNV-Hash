#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256
static const size_t InitialFNV = 2166136261U;
static const size_t FNVMultiple = 16777619;

typedef struct node{
    char* string;
    struct node* next;
} Node;

typedef struct openTable{
    int size;
    Node** table;
} OpenTable;

Node* initializeNode(char* str){
    Node* entry = (Node*)malloc(sizeof(Node));
    entry->string = (char*)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(entry->string, str);
    entry->string[strlen(str)] = '\0';
    entry->next = NULL;
    return entry;
}

OpenTable* initializeHashTable(int size){
    OpenTable* MainTable = (OpenTable*) malloc(sizeof(OpenTable));
    MainTable->size = size;
    MainTable->table = (Node**) malloc(sizeof(Node*)*size);
    for(int i = 0; i<size; i++){
        MainTable->table[i] = NULL;
    }
    return MainTable;
}


// int hashValue(char* str, int size){
//     long long unsigned total = 0;
//     for(int i = 0; i<(strlen(str)-1); i++){
//         total = total + (str[i]);
//         total = total * 101;
//         // printf("%c -> %llu\n", str[i], total);
//     }
//     // printf("\n");
//     return total % size;
// }

/* FNV Hash */
size_t hashValue(char* str, int size)
{
    size_t hash = InitialFNV;
    for(size_t i = 0; i < strlen(str)-1 ; i++)
    {
        hash = hash ^ (str[i]);       /* xor  the low 8 bits */
        hash = hash * FNVMultiple;  /* multiply by the magic number */
    }
    
    return hash % size;
}

void insertWords(OpenTable* MainTable, char* str,int size){
    int position = hashValue(str,size);
    Node* entryPoint = MainTable->table[position];
    int flag = 0;

    while(entryPoint!=NULL){
        flag = 1;
        if(strcmp(entryPoint->string,str) == 0){
            return;
        }
        if(entryPoint->next == NULL){
            break;
        }
        entryPoint = entryPoint->next;
    }

    if(!flag){
        entryPoint = initializeNode(str);
        MainTable->table[position] = entryPoint;
        return;
    }
    entryPoint->next = initializeNode(str);
    return;
}


int checkWord(OpenTable* MainTable, char* str, int size){
    int position = hashValue(str,size);
    Node* Finderptr = MainTable->table[position];
    while(Finderptr!=NULL){
        if(strcmp(Finderptr->string,str) == 0){
            return 1;
        }
        Finderptr = Finderptr->next;
    }
    return 0;
}

void swap(char* str, int st, int last){
    char t = str[st];
    str[st] = str[last];
    str[last] = t;
}

void printSuggestions(OpenTable* MainTable, char* findsug, int size){
    char* orig = (char*)malloc(sizeof(char)*strlen(findsug));
    int flag = 0;
    strcpy(orig, findsug);

    //Alternating
    char t;

    for(int i = strlen(findsug);i>1; i--){
        char t = findsug[i-2];
        findsug[i-2] = findsug[i-1];
        findsug[i-1] = t;
        if(checkWord(MainTable, findsug, size)){
            printf(" %s", findsug);
            flag = 1;
        }
        strcpy(findsug, orig);
    }
    

    free(orig);

    //Removal
    orig = (char*)malloc(sizeof(char)*(strlen(findsug)-1));
    for(int i =0; i<strlen(findsug)-1; i++){
        orig[i]=findsug[i];
    }
    if(checkWord(MainTable, orig, size)){
        printf(" %s", orig);
        flag = 1;
    }
    for(int i =1; i<strlen(findsug); i++){
        orig[i-1]=findsug[i];
    }
    if(checkWord(MainTable, orig, size)){
        printf(" %s", orig);
        flag = 1;
    }
    free(orig);


    //New letter in front and back
    orig = (char*)malloc(sizeof(char)*(strlen(findsug)+2));
    for(int i =1; i< strlen(findsug)+1; i++){
        orig[i] = findsug[i-1];
    }
    orig[strlen(orig)-1] = '\0';

    for(int j = 97; j<123; j++){
        orig[0]=(char)j;
        if(checkWord(MainTable,orig, size)){
            printf(" %s",orig);
            flag=1;
        }
    }

    for(int i =0; i< strlen(findsug); i++){
        orig[i] = findsug[i];
    }

    for(int j = 97; j<123; j++){
        orig[strlen(findsug)]=(char)j;
        if(checkWord(MainTable,orig, size)){
            printf(" %s",orig);
            flag=1;
        }
    }
    free(orig);
    if(!flag){
        printf(" No suggestions\n");
        return;
    }
    printf("\n");

}


void freeList(Node* list){
    Node* ptr = list->next;
    free(list->string);
    free(list);
    while(ptr!=NULL){
        list=ptr;
        ptr=ptr->next;
        free(list->string);
        free(list);
    }
}

void freeHash(OpenTable* MainTable){
    for(int i =0; i<MainTable->size; i++){
        if(MainTable->table[i]!=NULL){
            freeList(MainTable->table[i]);
        }
    }
    free(MainTable->table);
    free(MainTable);
}

int main(int argc, char **argv)
{
	char *dictionaryFilePath = argv[1]; //this keeps the path to the dictionary file file
	char *inputFilePath = argv[2]; //this keeps the path to the input text file
	char *check = argv[3]; // this keeps the flag to whether we should insert mistyped words into dictionary or ignore
	int numOfWords=0; //this variable will tell us how much memory to allocate

	int insertToDictionary;
	if(strcmp(check,"add")==0)
		insertToDictionary = 1;
	else
		insertToDictionary = 0;
    
	////////////////////////////////////////////////////////////////////
	//read dictionary file
    FILE *fp = fopen(dictionaryFilePath, "r");
    char *line = NULL; //variable to be used for line counting
    size_t lineBuffSize = 0; //variable to be used for line counting
    size_t lineSize; //variable to be used for line counting

    //check if the file is accessible, just to make sure...
    if(fp == NULL)
    {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    //First, let's count number of words in the dictionary.
    //This will help us know how much memory to allocate for our hash table
    while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1){
        numOfWords++;
    }
    //Printing line count for debugging purposes.
    //You can remove this part from your submission.
    //printf("%d\n",numOfWords);
    
    //HINT: You can initialize your hash table here, since you know the size of the dictionary
    OpenTable* MainTable = initializeHashTable(2*numOfWords);
     
    //rewind file pointer to the beginning of the file, to be able to read it line by line.
    fseek(fp, 0, SEEK_SET);

    char wrd[BUFSIZE];
    for (int i = 0; i < numOfWords; i++)
    {
        fscanf(fp, "%s \n", wrd);
        
        // Inserting words into dictionary
        insertWords(MainTable, wrd, 2*numOfWords);
    }
    fclose(fp);

    

	////////////////////////////////////////////////////////////////////
	//read the input text file word by word
    fp = fopen(inputFilePath, "r");
	
	//check if the file is accessible, just to make sure...
	if(fp == NULL)
	{
		fprintf(stderr, "Error opening file\n");
		return -1;
	}

    //HINT: You can use a flag to indicate if there is a misspelled word or not, which is initially set to 1
	int noTypo=1;

	//read a line from the input file
	while((lineSize = getline(&line,&lineBuffSize,fp)) !=-1)
	{
		char *word;
        //These are the delimiters you are expected to check for. Nothing else is needed here.
		const char delimiter[]= " ,.:;!\n";

		//split the buffer by delimiters to read a single word
		word = strtok(line,delimiter); 

		//read the line word by word
		while(word!=NULL)
		{
            // You can print the words of the inpit file for Debug purposes, just to make sure you are loading the input text as intended
			//printf("%s\n",word);

            // HINT: Since this nested while loop will keep reading the input text word by word, here is a good place to check for misspelled words
            int result = checkWord(MainTable, word, 2*numOfWords);           
            
            // INPUT/OUTPUT SPECS: use the following line for printing a "word" that is misspelled.
            //printf("Misspelled word: %s\n",word);
            if(!result){
                noTypo+=1;
                printf("Misspelled word: %s\n",word);              
            
            
                // INPUT/OUTPUT SPECS: use the following line for printing suggestions, each of which will be separated by a comma and whitespace.
                printf("Suggestions:"); //the suggested words should follow
                printSuggestions(MainTable, word, 2*numOfWords);
                if(insertToDictionary){
                    insertWords(MainTable, word, 2*numOfWords);
                }
            
            }
			word = strtok(NULL,delimiter); 
		}


	}
	fclose(fp);
    
    //HINT: If the flag noTypo is not altered (which you should do in the loop above if there exists a word not in the dictionary), then you should print "No typo!"
    if(noTypo==1)
        printf("No typo!\n");
    

    // DON'T FORGET to free the memory that you allocated
    freeHash(MainTable);
    
    
	return 0;
}
