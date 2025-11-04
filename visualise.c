/*=================================================
    Standard Libraries
===================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> //As Int/Long Long limits are too big for program
#include <errno.h> //Only for Long Long error checking

/*=================================================
    Function Helpers
===================================================*/

//Using strtoll (so all data types can use this function) instead of atoi as it allows for error checking
int convertStringToNumber(const char *start, long long min, long long max, long long *numericData) {
    char *endPointer;
    errno = 0; //I avoid global variables but this is necessary as long long has the largest range in C for numbers
    //Otherwise temp will not notice numbers above 2^63 and below 2^63 -1

    unsigned long long temp = strtoull(start, &endPointer, 10); //Convert to long long with strtoll + retrieve numbers in base 10
    
    //If there's no null terminator, input could be non-numeric/wrong type
    if (*endPointer != '\0' || temp < min || temp > max || errno == ERANGE) { //Variable limits so function can be reused
        return 0; //Failed
    }
    *numericData = temp;
    return 1; //Successful return
}

//DRY code, defined with the unsigned long long so each data type can be converted
void printBinaryBits(unsigned long long value, int bits, int sign){

    if (sign) {
        long long signedValue = (long long) value; //Cast value back to a signed integer
        value = *(unsigned long long*) &signedValue; //
    }


    for (int i= (bits - 1); i >= 0 ; i--) { //Backwards for loop MSB to LSB for shifting
        int currentBit = (value >> i) & 1; 
        printf("%d", currentBit); //Print out each bit in order
        if (i % 4 == 0 && i != 0) { //0 % 4 = 0, so include i != 0 so there's no extra space
            printf(" "); //Outputs space for each nibble
        }
    }
    printf("\n");
}

/*=================================================
    Binary Conversion Function
===================================================*/

void unsignedCharToBinary(unsigned char value){
    //range 0 to 255
    printBinaryBits(value, 8, 0); //Use helper function for DRY code
}

void signedCharToBinary(signed char value){
    //range -128 to +127
    printBinaryBits((unsigned char)value, 8, 1); //Use casting to prevent sign extension
}

void intToBinary(int value){
    // range -2bil to +2bil
    printBinaryBits(value, 32, 1);
}

void unsignedIntToBinary(unsigned int value){
    // range -2bil to +2bil
    printBinaryBits(value, 32, 0);
}

void longlongToBinary(long long value){
    // range -2^63 to 2^63-1
    printBinaryBits(value, 64, 1);
}

void floatToBinary(){}
void doubleToBinary(){}
void longdoubleToBinary(){}
void longToBinary(){}


/*=================================================
    Unit Testing 
===================================================*/

void testSignedChar(void){
    //Testing normal values
    signedCharToBinary(7); //0000 0111
    signedCharToBinary(42); //0010 1010
    signedCharToBinary(89); //0101 1010

    //Testing extreme values
    signedCharToBinary(-128); //1000 0000
    signedCharToBinary(127);  //0111 1111

}

void testUnsignedChar(void){
    //Testing normal values
    unsignedCharToBinary(7); //0000 0111
    unsignedCharToBinary(42); //0010 1010
    unsignedCharToBinary(90); //0101 1010

    //Testing extreme values
    unsignedCharToBinary(0); //0000 0000
    unsignedCharToBinary(255);  //1111 1111

}

void testIntToBinary(void){
    //Testing normal values
    intToBinary(7); 
    intToBinary(42); 
    intToBinary(89);

    //Testing extreme values
    intToBinary(10000); 
    intToBinary(-10000);  

}

/*=================================================
    Main Function
===================================================*/

int main(int argc, char *argv[]){ //Take in type and data

    if (argc == 1) {
        //Run all unit tests
        testSignedChar();
        testUnsignedChar();
        printf("All tests pass\n");
        return 0;

    } else if (argc != 3) {
        printf("Input error.\n");
        return 0;
    }

    long long numericData; //Use long long for reusability./vi
    //Assign each arguement to variable
    char *type = argv[1];
    char *data = argv[2];

    if (strcmp(type, "unsigned char") == 0) { // unsigned char 45
        if (!convertStringToNumber(data, 0, 255, &numericData)) {
        printf("Input error.\n");
        return 0;} // numericData now contains the valid number

        unsignedCharToBinary(numericData);

    } else if (strcmp(type, "signed char") == 0 || strcmp(type, "char") == 0 ) { //assuming we're using clang and gcc, char == signed char
        if (!convertStringToNumber(data, -128, 127, &numericData)) {
        printf("Input error.\n");
        return 0;} 
        
        signedCharToBinary(numericData);

    } else if (strcmp(type, "int") == 0) { 
        if (!convertStringToNumber(data, (INT_MIN), (INT_MAX), &numericData)) {
        printf("Input error.\n");
        return 0;} 

        intToBinary(numericData);

    } else if (strcmp(type, "unsigned int") == 0) { 
        if (!convertStringToNumber(data, (0), (UINT_MAX), &numericData)) {
        printf("Input error.\n");
        return 0;} 

        unsignedIntToBinary(numericData);
    
    } else if (strcmp(type, "long") == 0 || strcmp(type, "long long") == 0 || strcmp(type, "signed long long") == 0) { //As we are on Linux long == long long
        if (!convertStringToNumber(data, (LLONG_MIN), (LLONG_MAX), &numericData)) {
        printf("Input error.\n");
        return 0;} // longNumericData now contains the valid number

        longlongToBinary(numericData); 

    } else if (strcmp(type, "float") == 0) { //32 bits
        floatToBinary(numericData);

    } else if (strcmp(type, "double") == 0) { //64 bits
        doubleToBinary(numericData);

    } else if (strcmp(type, "long double") == 0) { //80 bits for Linux
        longdoubleToBinary(numericData);

    } else {
        printf("Type entered is not listed");
    }
    return 0;

}

