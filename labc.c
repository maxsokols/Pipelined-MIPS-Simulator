#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum { false, true } bool;

//Function forward declarations
void progScanner(char*);
void parser(char*);
void trimInstruction(char*);
bool isAValidCharacter(char);
bool isAValidReg(char);
void extractOpcode(char*, char*);
int extractRegister(char*,int);
bool isRType(char* opcode);
bool isIType(char* opcode);
int regValue(char*);

int main(){
  progScanner("prog1.asy");
  return 0;
}

void progScanner(char* filename){
  char instruction[100];
  FILE *instFile = fopen(filename,"r");
  while(fgets(instruction,100,instFile)){
    parser(instruction);
  }
  fclose(instFile);
}

void parser(char* instruction){
  trimInstruction(instruction);
  char opcode[10];
  extractOpcode(opcode,instruction);
  //R Type as fuck
  if(isRType(opcode)){
    int rs = extractRegister(instruction,0);
    int rt = extractRegister(instruction,1);
    int rd = extractRegister(instruction,2);
  }else if(isIType(opcode)){
    
  }else if(strcmp(opcode,"haltSimulation") == 0){
    //Run it through the pipeline and shut shit down
  }else{
    //Unrecognized, crash the program
    assert(!"Unrecognized instruction");
  }
}

void trimInstruction(char* instruction){
  int stage = 0; //How many times we've gone from not a space to a space
  int newIndex = 0;
  char temp[100];
  int i;
  bool lastCharWasSpace = true;
  for(i = 0; instruction[i] != '\0'; i++){
    if(isAValidCharacter(instruction[i])){ //Is alphanumeric or a dollar sign or a comma
      temp[newIndex++] = instruction[i];
      lastCharWasSpace = false;
    }else{ //Basically is a space
      if(!lastCharWasSpace){
        stage++;
        if(stage == 1){
          temp[newIndex++] = ' ';
        }
      }
      lastCharWasSpace = true;
    }
  }
  temp[newIndex++] = '\0';
  strcpy(instruction,temp);
}

bool isAValidCharacter(char c){
  return isalnum(c) || c == '$' || c == ',';
}

bool isAValidReg(char c){
  return isalnum(c) || c == '$';
}

void extractOpcode(char* opcode, char* instruction){
  int i;
  for(i = 0; instruction[i] != ' '; i++){
    opcode[i] = instruction[i];
  }
  opcode[i] = '\0';
}

int extractRegister(char* instruction, int index){
  int i;
  int regIndex = 0;
  int charIndex = 0;
  bool readOpcode = false;
  char reg[6]; //To be able to hold $zero, which is 5 characters and the null character
  
  for(i = 0; instruction[i] != '\0'; i++){
    if(readOpcode && instruction[i] == ','){
      regIndex++;
    }
    if(readOpcode && isAValidReg(instruction[i]) && index == regIndex){
      reg[charIndex++] = instruction[i];
    }
    if(instruction[i] == ' '){
      readOpcode = true;
    }
  }
  reg[charIndex++] = '\0';
  //
  //Checking if it's a valid register
  if(reg[0] != '$'){
    assert(!"Register didn't start with a dollar sign");
  }
  //Set regValue the string without the dollar sign
  int regVal = regValue(reg+1);
  if(regVal != -1){
    //Store that register in the struct for that instruction
    printf("%d\n", regVal);
  }else{
    //Register is invalid
    assert(!"Register is invalid.");
  }
  return regVal;
}

bool isRType(char* opcode){
  return strcmp(opcode,"add") == 0 || strcmp(opcode,"sub") == 0 || strcmp(opcode,"mult") == 0;
}

bool isIType(char* opcode){
  return strcmp(opcode,"addi") == 0 || strcmp(opcode,"lw") == 0 || strcmp(opcode,"sw") == 0 || strcmp(opcode,"beq") == 0;
}

//Returns the location (0-31) of the register from the name
int regValue(char* c){
  //Register indexes via http://msdn.microsoft.com/en-us/library/ms253512(v=vs.80).aspx
  int regIndex = c[1] - '0';
  switch(c[0]){
    case 'a':
      if(regIndex >= 0 && regIndex <= 3 ){
        return 4 + regIndex;
      }else if(c[1] == 't'){
        return 1;
      }
      break;
    case 'g':
      if(c[1] == 'p'){
        return 28;
      }
      break;
    case 'k':
      if(regIndex >= 0 && regIndex <= 1 ){
        return 26 + regIndex;
      }
      break;
    case 'r':
      if(c[1] == 'a'){
        return 31;
      }
      break;
    case 's':
      if(regIndex >= 0 && regIndex <= 7 ){
        return 16 + regIndex;
      }else if(regIndex == 8){
        return 30;
      }else if(c[1] == 'p'){
        return 29;
      }

      break;
    case 't':
      if(regIndex >= 0 && regIndex <= 7 ){
        return 8 + regIndex;
      }else if(regIndex >= 8 && regIndex <= 9 ){
        return 16 + regIndex;
      }
      break;
    case 'v':
      if(regIndex >= 0 && regIndex <= 1 ){
        return 2 + regIndex;
      }
      break;
    case 'z':
      if(strcmp(c,"zero") == 0){
        return 0;
      }
      break;
  }
  /*If it doesn't match any of those values, we have to check if it uses the $# format
    First we check if all the characters after the dollar sign are numbers
    If they are, then we convert it to a number and check if its between 0 and 31
    Otherwise we just fail it (return -1)
  */
  int i = 0;
  char currentChar = c[i];
  while(currentChar != '\0'){
    if(!isdigit(currentChar)){
      return -1;
    }
    i++;
    currentChar = c[++i];
  }

  //If we've made it here, the string is a number
  regIndex = atoi(c);
  if(regIndex >= 0 && regIndex <= 31){
    return regIndex;
  }
  return -1;
}
