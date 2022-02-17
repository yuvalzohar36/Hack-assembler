#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//define declarations
#define FILE_NAME 20
#define ASM_LEN_FILE 300
#define COMP_LEN 6
#define DEST_LEN 3
#define JUMP_LEN 3
#define INSTRUCTION_LEN 16
#define MAX_FILE_EXSTENTION_LENGTH 4
#define ASM_EXS_LEN 3

typedef struct instruction { //struct for managing instruction fields
	char instructionType;
	char symbol[ASM_LEN_FILE+1];
	char dest[DEST_LEN + 1];
	char comp[COMP_LEN + 1];
	char jump[JUMP_LEN + 1];
} instruction;

typedef struct compTables { //struct for managing the table of Comp
	char binValue[7];
	char order[4];
	char Morder[4];
}compTables;

typedef struct destTables {//struct for managing the table of Dest
	char binValue[4];
	char order[4];
}destTables;

typedef struct jumpTables { //struct for managing the table of Jump
	char binValue[4];
	char order[4];
}jumpTables;

//function declarations
FILE* openFile(char* file_name, char* mode);
char* removeComment(char* asm_line);
char inctructionType(char* asm_line);
char* dest(char* asm_line, destTables* destTables_arr);
char* jump(char* asm_line, jumpTables* jumpTables_arr);
char* comp(char* asm_line, compTables* compTables_arr, int* is_M);
void convertToBinary(int number, char* string);
void initInstruction(char* instruction_str);
void writeInstruction(instruction instruction, FILE* HACK_file, int is_M);
void initCompTables(compTables* compTables_arr);
void initDestTables(destTables* destTables_arr);
void initJumpTables(jumpTables* jumpTables_arr);
void manageFile(FILE* asm_file, FILE* hack_output, int* is_M);
void updateExstenstionName(char* file_name);
char* removeSpaces(char* asm_line);

int main() {
	char destOrd[4], compOrd[4], jumpOrd[4], file_name[FILE_NAME + MAX_FILE_EXSTENTION_LENGTH + 1], read_mode[3] = "rt", write_mode[3] = "wt";
	printf("Please enter the assembly file name with the extension .asm: ");
	scanf("%s", file_name); //get the user file name input
	FILE* asm_file = openFile(file_name, read_mode); //open the asm file with read mode
	updateExstenstionName(file_name);
	FILE* hack_output = openFile(file_name, write_mode); // open/create the .hack output file with write mode
	int is_M = 0;
	manageFile(asm_file, hack_output, &is_M);
	fclose(asm_file);
	fclose(hack_output);
	return 0;
}

void updateExstenstionName(char* file_name) {
	char* p_asm = file_name + strlen(file_name) - ASM_EXS_LEN;
	strcpy(p_asm, "hack");// "delete" the asm extenstion and puts hack instead
}

void manageFile(FILE* asm_file, FILE* hack_output, int* is_M) {
	char asm_line[ASM_LEN_FILE + 1];
	instruction cur_instruction;
	compTables compTablesArr[18];
	destTables destTablesArr[8];
	jumpTables jumpTablesArr[8];
	initCompTables(compTablesArr); //initalize the compTables array(of structs)
	initDestTables(destTablesArr); //initalize the destTables array(of structs)
	initJumpTables(jumpTablesArr); //initalize the jumpTables array(of structs)
	while (fgets(asm_line, ASM_LEN_FILE, asm_file) != NULL) { //while there is still lines in the filem
		strcpy(asm_line, removeComment(asm_line)); //remove all the comments from the line
		strcpy(asm_line, removeSpaces(asm_line));
		if (strlen(asm_line) > 0) { //while there isn't empty line
			if (inctructionType(asm_line) == 'A') { //in case of A instruction type
				cur_instruction.instructionType = 'A'; //update the instruction Type in the struct
				strcpy(cur_instruction.symbol, asm_line + 1);
			}
			else { //in case of C instruction
				cur_instruction.instructionType = 'C'; //update the C instruction type in the struct
				strcpy(cur_instruction.symbol, asm_line); //extract and assign the symbol
				strcpy(cur_instruction.dest, dest(asm_line, destTablesArr)); //extract and assign the dest field
				strcpy(cur_instruction.jump, jump(asm_line, jumpTablesArr)); //extract and assign the jump field
				strcpy(cur_instruction.comp, comp(asm_line, compTablesArr, is_M)); //extract and assign the comp field
			}
			writeInstruction(cur_instruction, hack_output, *is_M); //write the instruction to the output file
			*is_M = 0;
		}
	}
}

void initCompTables(compTables* compTables_arr) {
	int i;
	char compBinArr[18][7] = { "101010", "111111", "111010", "001100", "110000", "001101", "110001","001111",
		"110011", "011111","110111","001110", "110010", "000010", "010011", "000111", "000000", "010101" };//the Comp binary array
	char a1_arr[18][4] = { "0", "1", "-1", "D", "A", "!D", "!A", "-D","-A","D+1","A+1","D-1","A-1","D+A","D-A","A-D","D&A","D|A" };//the Comp array(a=1)
	char a0_arr[18][4] = { "","","","", "M","","!M","","-M","","M+1","","M-1","D+M","D-M","M-D","D&M","D|M" };//the Comp array(a=0)
	for (i = 0; i < 18; i++) {//updating the compTables array(of structs) with the above arrays
		strcpy(compTables_arr[i].binValue, compBinArr[i]); //assign the binary value
		strcpy(compTables_arr[i].order, a1_arr[i]); //assign the a=1 order
		strcpy(compTables_arr[i].Morder, a0_arr[i]); //assign the a=0 order
	}
}

void initDestTables(destTables* destTables_arr) {
	char destBinArr[8][4] = { "000","001","010","011","100","101","110","111" }; //the Dest binary array
	char order_arr[8][4] = { "","M","D","DM","A","AM","AD","AMD" }; //the Dest array
	int i;
	for (i = 0; i < 8; i++) { //updating the destTables array(of structs) with the above arrays
		strcpy(destTables_arr[i].binValue, destBinArr[i]); //assign the binary value
		strcpy(destTables_arr[i].order, order_arr[i]); //assign the order 
	}
}

void initJumpTables(jumpTables* jumpTables_arr) {
	char jumpBinArr[8][4] = { "000","001","010","011","100","101","110","111" };//the Jump binary array
	char order_arr[8][4] = { "","JGT","JEQ","JGE","JLT","JNE","JLE","JMP" };//the Jump array
	int i;
	for (i = 0; i < 8; i++) {//updating the jumpTables array(of structs) with the above arrays
		strcpy(jumpTables_arr[i].binValue, jumpBinArr[i]); //assign the binary value
		strcpy(jumpTables_arr[i].order, order_arr[i]); //assign the order
	}
}

char* removeSpaces(char* asm_line) { //remove spaces from the line
	char trimmed_asm_line[ASM_LEN_FILE + 1];
	int i, j = 0;
	for (i = 0; asm_line[i] != '\0'; i++)
		if (asm_line[i] != ' ' && asm_line[i] != '\t') // if tab or spacess
			trimmed_asm_line[j++] = asm_line[i];
	trimmed_asm_line[j] = '\0';
	return trimmed_asm_line;
}

char* removeComment(char* asm_line) {
	char* p_to_slash = strstr(asm_line, "//"); //find comment in the line
	if (p_to_slash) {
		*p_to_slash = '\0';
	}
	if (asm_line[strlen(asm_line) - 1] == '\n') //fixed the fgets ends with '\n'
		asm_line[strlen(asm_line) - 1] = '\0';
	return asm_line;
}

FILE* openFile(char* file_name, char* mode) {
	FILE* file = fopen(file_name, mode); //open the file with given filename and mode
	if (file == NULL) { //in case of error while opening the file, close everything
		printf("Error: cannot open the file\n");
		exit(1);
	}
	return file;
}

char inctructionType(char* asm_line) {
	if (asm_line[0] == '@')
		return 'A';//return the instructionType
	return 'C';
}
char* jump(char* asm_line, jumpTables* jumpTables_arr) {
	int i;
	char* p_comp = strstr(asm_line, ";"); //find the ';' position(address) if exists, otherwise NULL
	if(p_comp ==NULL) // in case there is no ';'
		return jumpTables_arr->binValue;
	if(*(p_comp+1)=='\0') //if there is nothing after ';'
		return jumpTables_arr->binValue;
	for (i = 1; i < 8; i++) //find the jump in the jumpTables array and than return the binary value
		if (!(strcmp(p_comp+1, (jumpTables_arr + i)->order)))
			return (jumpTables_arr + i)->binValue; //return the appropriate binary value
}

char* comp(char* asm_line, compTables* compTables_arr, int* is_M) {
	int i, j = 0, index = 0, end_index = strlen(asm_line);
	char comp[COMP_LEN + 1];
	for (i = 0; i < strlen(asm_line); i++)  //find the '=' (if exists)
		if (asm_line[i] == '=')
			index = i + 1;
	for (i = index; i < strlen(asm_line); i++) //find the ';' (if exists)
		if (asm_line[i] == ';')
			end_index = i;
	for (i = index; i < end_index; i++) { // "moving" between the '=' and ';'
		comp[j] = asm_line[i]; //fill the comp field
		if (comp[j] == 'M')
			*is_M = 1;
		j++;
	}
	comp[j] = '\0';
	for (i = 0; i < 18; i++) //find the comp in the compTables array and than return the binary value
		if (!(*is_M) && !strcmp((compTables_arr + i)->order, comp) || (*is_M) && !strcmp((compTables_arr + i)->Morder, comp))
			return (compTables_arr + i)->binValue;
}

char* dest(char* asm_line, destTables* destTables_arr) {
	int i = 0;
	char dest[DEST_LEN + 1];
	while (i < DEST_LEN && asm_line[i] != '=') { //find the '=' if exists
		dest[i] = asm_line[i];
		i++;
	}
	dest[i] = '\0';
	if (asm_line[i] != '=' && i == DEST_LEN) //in casethere is no '='
		return destTables_arr->binValue; //return the null value
	for (i = 1; i < 8; i++) //find the dest in the destTables array and than return the binary value
		if (!(strcmp(dest, (destTables_arr + i)->order)))
			return (destTables_arr + i)->binValue; //find the dest in the destTables array and than return the binary value
}

void initInstruction(char* instruction_str) {
	int i;
	for (i = 0; i < INSTRUCTION_LEN; i++)
		instruction_str[i] = '0'; //filling all the instruction string with '0'
	instruction_str[INSTRUCTION_LEN] = '\0';
}

void writeInstruction(instruction instruction, FILE* HACK_file, int is_M) {
	char instructionLine[INSTRUCTION_LEN + 1];
	initInstruction(instructionLine); //filling all the instruction string with '0'
	if (instruction.instructionType == 'A') //in case of A instruction
		convertToBinary(atoi(instruction.symbol), instructionLine); //convert to binary the NUMBER
	else { //in case of C instruction
		instructionLine[0] = '1'; //3 firsts cells are '1' in C instruction
		instructionLine[1] = '1';
		instructionLine[2] = '1';
		instructionLine[3] = '0' + is_M; //if M is involved via the Table in the lecture
		instructionLine[4] = '\0';
		strcat(instructionLine, instruction.comp); //append the Comp field
		strcat(instructionLine, instruction.dest); //append the Dest field
		strcat(instructionLine, instruction.jump); //append the Jump field
	}
	fprintf(HACK_file, "%s\n", instructionLine); //write to the file the instruction in binary
}

void convertToBinary(int number, char* string) {
	int i;
	for (i = 1; number > 0; i++) {
		string[strlen(string) - i] = '0' + (number % 2); //convert to binary (to the end of the string)
		number = number / 2;
	}
}