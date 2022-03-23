#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INPUTSIZE 100
#define FIELDSIZE 11
#define FIELDS 5
#define OPERATORS 5
#define GENDERVALUES 4
#define MAXID 999999999
#define IDSIZE 9

struct s {
    char data[FIELDS][FIELDSIZE];
}typedef Person;

struct q {
    int field;
    int operator;
    char value[FIELDSIZE];
    struct q* nextNode;
}typedef Query;

typedef enum { first_name, last_name, ID, gender, age } fields;

char* fieldsNames[FIELDS] = { "first_name", "last_name", "ID", "gender", "age" };
char* operators[OPERATORS] = { "<=", ">=", "=", "<", ">" };
char* genderValues[GENDERVALUES] = { "male", "female", "MALE", "FEMALE" };
char invalidValue[] = "...";
Person** fileData;
Query* queriesList;
int numOfRows;
FILE* file;

//===================================functions prototype======================================
//file functions
int isValidName(char* str);
int isValidID(char* str);
int isValidGender(char* str);
int isValidAge(char* str);
char* copyData(char* filePath);

//sort functions
int compare(Person* first, Person* second);
void merge(Person** array, int left_side, int middle, int right_side);
void mergeSort(Person** array, int left_side, int right_side);

//printing functions
int shouldPrint(Person* person);
void printPerson(Person* person);
void printFile();
void printQuery();

//check query functions
void freeQueryList();
char* checkToken(char* query);
char* parseQuery(char* input);

//add person functions
char* writeToFile(char* filePath);
char* updateMemory(Person* person);
int validateInput(char* input, Person* person);
void freeFileData();

//============================================file functions========================================
int isValidName(char* str) {
    if (!str) { return 0; }
    int i;
    char c;
    for (i = 0, c = str[i]; c != '\0'; c = str[++i]) {
        if (i > FIELDSIZE || !isalpha(c)) { return 0; }
    }
    return 1;
}
//--------------------------------------------------------------------------------------------
int isValidID(char* str) {
    if (!str) { return 0; }
    int i;
    long number;
    char c;
    for (i = 0, c = str[i]; c != '\0'; c = str[++i]) {
        if (i > FIELDSIZE || !isdigit(c)) { return 0; }
    }
    if (i != IDSIZE) { return 0; }
    number = atol(str);
    if (number<1 || number>MAXID) { return 0; }

    return 1;
}
//--------------------------------------------------------------------------------------------
int isValidGender(char* str) {
    if (!str) { return 0; }
    int i;
    char c;
    for (i = 0, c = str[i]; c != '\0'; c = str[++i]) {
        if (i > FIELDSIZE || !isalpha(c)) { return 0; }
    }
    for (i = 0; i < GENDERVALUES; i++) {
        if (strcmp(str, genderValues[i]) == 0) {
            return 1;
        }
    }
    return 0;
}
//---------------------------------------------------------------------------------------------
int isValidAge(char* str) {
    if (!str) { return 0; }
    int i, number;
    char c;
    for (i = 0, c = str[i]; c != '\0'; c = str[i++]) {
        if (i > FIELDSIZE || !isdigit(c)) { return 0; }
    }
    number = atoi(str);
    if (number < 0 || number>120) { return 0; }

    return 1;
}
//------------------------------------------------------------------------------------------------
//copy file to memory. the file will be represented as array of pointers to Person structs.
//return NULL in success or char* that describe the error. 
char* copyData(char* filePath) {

    if ((file = fopen(filePath, "r")) == NULL) {
        return "error in open file";
    }

    int i, currentRow = -1;
    char line[FIELDS * FIELDSIZE], * token;
    Person* person;
    int(*isValid[])(char*) = { isValidName, isValidName, isValidID, isValidGender, isValidAge };//array of isValid functions

    numOfRows = 0;
    while (fgets(line, FIELDS * FIELDSIZE, file) != NULL) { //find number of rows for allocate enough space
        numOfRows++;
    }

    if ((fileData = (Person**)calloc(numOfRows + 1, sizeof(Person*))) == NULL) {//allocate array of pointers (pointer=row)
        return "error in allocate array of Person*";
    }

    //read data, allocate person for each row. if some field is incorrect, insert illegaleValue
    rewind(file);
    while ((fgets(line, FIELDS * FIELDSIZE, file)) != NULL) {

        if ((person = (Person*)calloc(1, sizeof(Person))) == NULL) {
            return "error in allocate Person";
        }

        token = strtok(line, ",\n ");
        for (i = 0; i < FIELDS; i++) {
            (isValid[i](token)) ? strcpy(person->data[i], token) : strcpy(person->data[i], invalidValue);
            token = strtok(NULL, ",\n ");
        }
        fileData[++currentRow] = person;
    }

    if ((fclose(file)) != 0) {
        return "error in close file";
    }

    return NULL;
}
//========================================sort functions=====================================
//compares two persons
int compare(Person* first, Person* second) {
    return strcmp(first->data[last_name], second->data[last_name]);
}
//-------------------------------------------------------------------------------------------
//merges sub arrays to sorted one
void merge(Person** array, int left_side, int middle, int right_side)
{
    int i = left_side, j = middle + 1, k = 0;
    Person** temp_array = (Person**)malloc(((right_side - left_side) + 1) * sizeof(Person*));
    while (i <= middle && j <= right_side) {
        if ((compare(array[i], array[j])) < 1) {//if array [i] <= array[j]
            temp_array[k] = array[i];
            i = i + 1;
        }
        else {
            temp_array[k] = array[j];
            j = j + 1;
        }
        k = k + 1;
    }
    while (i <= middle) { //if values are left in the left array copy them to temp_array
        temp_array[k] = array[i];
        k = k + 1;
        i = i + 1;
    }
    while (j <= right_side) { //if values are left in the right array copy them temp_array
        temp_array[k] = array[j];
        k = k + 1;
        j = j + 1;
    }
    //this for copies the values from the temp array to the original array
    for (i = left_side, k = 0; i <= right_side; i++, k++) {
        array[i] = temp_array[k];
    }
    free(temp_array);
}
//-------------------------------------------------------------------------------------------
//sort given Persons* array by merge sort
void mergeSort(Person** array, int left_side, int right_side)
{
    int middle;
    if (left_side < right_side) {
        middle = (left_side + right_side) / 2;
        mergeSort(array, left_side, middle); //call mergesort on the left array
        mergeSort(array, middle + 1, right_side); //call mergesort on the right array
        merge(array, left_side, middle, right_side);
    }
}
//==================================printing functions========================================
//return 1 if given person meets the queries' requirements and should be printed. 0 if not
int shouldPrint(Person* person) {

    Query* currentNode = queriesList;
    char* queryValue, * op, * endpoint;
    int field, personAge, queryAge;

    while (currentNode) { //go over all queries in list. return 0 if one of the person's fileds doesn't match the query

        field = currentNode->field; op = operators[currentNode->operator]; queryValue = currentNode->value;

        //all operators are legal only in "age" field, so first check if query's field is age.
        //if field is age, check if the age is match the operator. return 0 if not
        if (field == age) {

            personAge = strtol(person->data[age], &endpoint, 10);
            queryAge = strtol(queryValue, &endpoint, 10);

            if (personAge < queryAge) {
                if (!strcmp(op, "=") || !strcmp(op, ">") || !strcmp(op, ">=")) {
                    return 0;
                }
            }
            if (personAge == queryAge) {
                if (!strcmp(op, "<") || !strcmp(op, ">")) {
                    return 0;
                }
            }
            if (personAge > queryAge) {
                if (!strcmp(op, "=") || !strcmp(op, "<") || !strcmp(op, "<=")) {
                    return 0;
                }
            }
        }

        else if ((strcmp(op, "="))) { return 0; } //if fiels isn't "age" and operator isn't "=", return 0

        else if ((strcmp(person->data[field], queryValue)) != 0) { return 0; } //return 0 if person value different of query value

        currentNode = currentNode->nextNode;//continue to next query
    }
    return 1;
}
//--------------------------------------------------------------------------------------------
//print given person (row)
void printPerson(Person* person) {
    int i;
    for (i = 0; i < FIELDS; i++) {
        printf("%s ", person->data[i]);
    }
    printf("\n");
}
//--------------------------------------------------------------------------------------------
//print all file
void printFile() {
    printf("\n");
    int i;
    for (i = 0; i < numOfRows; i++) {
        printPerson(fileData[i]);
    }
}
//--------------------------------------------------------------------------------------------
//print all persons (file's rows) that meets the query's requirements
void printQuery() {
    int i;
    for (i = 0; i < numOfRows; i++) {
        if (shouldPrint(fileData[i])) {
            printPerson(fileData[i]);
        }
    }
}
//======================================check query functions================================
//free memory allocation of queries list
void freeQueryList() {
    Query* temp;
    while (queriesList) {
        temp = queriesList->nextNode;
        free(queriesList);
        queriesList = temp;
    }
}
//--------------------------------------------------------------------------------------------
//check if query token is valid (in form of: <field><operator><value>) and save it. return NULL if valid, error message if not
char* checkToken(char* query) {

    char* tokenField = NULL, * tokenOperator = NULL, * tokenValue = NULL;
    int field, operator, i;

    //check if token's field name is correct (one of the file columns). return 0 if not
    for (i = 0; i < FIELDS; i++) {
        tokenField = strstr(query, fieldsNames[i]);
        if (tokenField == query) { field = i; break; }
    }
    if (tokenField == NULL) { return "invalid field name"; }


    //check if operator in token is correct (=,<,>,<=,>=). return 0 if not
    for (i = 0; i < OPERATORS; i++) {
        tokenOperator = strstr(query, operators[i]);
        if (tokenOperator == query + strlen(fieldsNames[field])) { operator=i; break; }
    }
    if (tokenOperator == NULL) { return "invalid operator"; }


    //check if value in token is not empty. return 0 if so
    if ((tokenValue = query + strlen(fieldsNames[field]) + strlen(operators[operator])) == NULL) { return "invalid value"; }


    //if came here, the query is correct. create new Query from the token
    Query* temp = (Query*)malloc(sizeof(Query));
    if (!temp) { return "error in allocation memory"; }
    temp->field = field;
    temp->operator = operator;
    strcpy(temp->value, tokenValue);

    //insert query to list
    temp->nextNode = queriesList;
    queriesList = temp;

    return NULL;
}
//--------------------------------------------------------------------------------------------
//parse query. save if correct. return NULL if query should be prined or error message if not
char* parseQuery(char* input) {

    char *message, *token = strtok(input, " ");

    while (1) { //check query and continue parsing it while "and" entered

        if ( (message = checkToken(token)) != NULL) { //check if query valid. return the error message if not.
            return message;
        }

        token = strtok(NULL, " ");

        if (!token) { return NULL; }//if query finished and should be printed, return NULL to main loop

        if (strcmp(token, "and")) {//if query continues but without "and". return error message to main loop.
            return "enter valid command";
        }

        token = strtok(NULL, " ");//continue parsing input if token is valid and continues with "and".
    }
}
//====================================add person functions=======================================
//write data in memory to file. return NULL in success or char* that describe the error. 
char* writeToFile(char* filePath) {

    int i, j;

    if ((file = fopen(filePath, "w")) == NULL) {
        return "error in open file";
    }

    for (i = 0; i < numOfRows - 1; i++) {

        for (j = 0; j < FIELDS - 1; j++) {
            fputs(fileData[i]->data[j], file);
            fputc(',', file);
        }
        fputs(fileData[i]->data[age], file);
        fputc('\n', file);
    }

    for (j = 0; j < FIELDS - 1; j++) {
        fputs(fileData[i]->data[j], file);
        fputc(',', file);
    }
    fputs(fileData[i]->data[age], file);

    if ((fclose(file)) != 0) {
        return "error in close file";
    }

    return NULL;
}
//-----------------------------------------------------------------------------------------------
//insert given person (whether new or updated) to the data in memory. 
//return NULL in success or error message
char* updateMemory(Person* person) {

    int i, updateIndex = numOfRows; //by defult will update at the end of the array

    for (i = 0; i < numOfRows; i++) { //if person exist, the location to update is person's location
        if (!(strcmp(fileData[i]->data[ID], person->data[ID]))) {
            updateIndex = i;
            break;
        }
    }

    if (updateIndex == numOfRows) { //allocate new person if person isn't exist
        if ((fileData[numOfRows] = (Person*)malloc(sizeof(Person))) == NULL) {
            return "error in allocating new Person";
        }
        numOfRows++;
    }

    for (i = 0; i < FIELDS; i++) {
        strcpy(fileData[updateIndex]->data[i], person->data[i]);
    }
    return NULL;
}
//-----------------------------------------------------------------------------------------------
//parse and validate input. if correct, insert values to given person and return 1. return 0 if input isn't valid
int validateInput(char* input, Person* person) {

    int i;
    char* token, * temp, * fieldName, * operator,* value;
    int(*isValid[])(char*) = { isValidName, isValidName, isValidID, isValidGender, isValidAge };//array of isValid functions
    
    if ( !(token = strtok(NULL, " ")) ) { return 0; }

    //check if all person fields are exist and correct. save their value if so 
    for (i = 0; i < FIELDS; i++) {
        token = strtok(NULL, " ");

        temp = strstr(token, fieldsNames[i]);
        if (temp == NULL || temp != token) { return 0; }
        fieldName = fieldsNames[i];

        operator = strstr(token + strlen(fieldName), "=");
        if (operator == NULL || operator != token + strlen(fieldName)) { return 0; }

        value = token + strlen(fieldName) + strlen("=");
        if (value == NULL) { return 0; }

        if (!isValid[i](value)) { return 0; }//check if value is correct. each field has validate function of his own

        strcpy(person->data[i], value); //put parsed value In the appropriate field
    }

    return 1; //if came here, the input is valid. return 1
}
//=========================================main functions============================================
//free allocated memory
void freeFileData() {

    int i;
    for (i = 0; i < numOfRows; i++) {
        free(fileData[i]);
    }
    free(fileData);
}
//---------------------------------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    if (argc != 2) { printf("run program correctly with file path"); return 1; }//check arguments to program

    char input[INPUTSIZE], copiedInput[INPUTSIZE], * token, * message;
    Person person;

    while (1) {//while user continues inserting new data, save file and read again it 

        if ((message = copyData(argv[1])) != NULL) { //copy data from file to allocated memory
            printf("%s\n", message);
            return 1;
        }
        printf("copied data\n");
        mergeSort(fileData, 0, numOfRows - 1); //sort data array

        printFile(); //print all file first

        //program main loop. scan input from user until quit or insert data. if input is correct query, print it
        while (1) {

            printf("\n%c", 26);
            gets(input);
            strcpy(copiedInput, input);
            token = strtok(copiedInput, " ");

            if (strcmp(token, "quit") == 0) { //quit program if token is "quit"
                printf("quiting program...\n");
                freeFileData();
                return 0;
            }

            else if (strcmp(token, "set") == 0) {

                if (validateInput(input, &person)) { //validate input. send person to put values in.
                    if ((message = updateMemory(&person)) != NULL) { //update memory with new details.
                        printf("%s\n", message);
                    }
                    if ((message = writeToFile(argv[1])) != NULL) { //write data in memory to file.
                        printf("%s\n", message);
                        return 1;
                    }
                    printf("file updated. new file is:");
                    break;
                }
                printf("enter valid person\n");
            }

            else if (token) { //if came here, input isn't "set" or "quit". parse the query, print if correct
               
                if ( (message = parseQuery(input)) != NULL ) { //if query isnt correct, print message.
                    printf("%s\n", message);
                }
                else {
                    printQuery(); //if query is correct, print the query's results
                }
                freeQueryList(); //free query list of previous input
            }
        }
        freeFileData();
    }
    return 0;
}
//====================================================================================================