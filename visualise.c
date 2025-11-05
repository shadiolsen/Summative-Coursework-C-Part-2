/*=================================================
    Standard Libraries
===================================================*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h> //As Int/Long Long limits are too big for program
#include <errno.h> //Only for Long Long error checking
#include <assert.h> //For testing
#include <float.h> //To get range for double

/*=================================================
    Function Helpers
===================================================*/

//Using strtoll (so all data types can use this function) instead of atoi as it allows for error checking
int convertStringToNumber(const char *start, long long min, unsigned long long max, long long *longNumericData, int sign) {
    
    // This is so e.g ./visualise char 08 will not work
    if (start[0] == '0' && start[1] != '\0'){
        return 0;
    } 

    char *endPointer;
    errno = 0; //I avoid global variables but this is necessary as long long has the largest range in C for numbers
    //Otherwise temp will not notice numbers above 2^63 and below 2^63 -1

    if (sign) {
        long long temp = strtoll(start, &endPointer, 10);
        if (*endPointer != '\0' || temp < min || temp > (long long)max || errno == ERANGE) //Variable limits so function can be reused
            return 0; //Fail
        *longNumericData = temp;
    } else {
        unsigned long long temp = strtoull(start, &endPointer, 10); //Convert to long long with strtoll + retrieve numbers in base 10
        if (*endPointer != '\0' || temp > max || errno == ERANGE)
            return 0; //Fail
        *longNumericData = (long long)temp;
    }
    return 1; //Success
}

//This is less DRY but more readable for double which acts differently from the other types
int convertDoubleToNumber(const char *start, double *numericDoubleData) {
    
    // This is so e.g ./visualise double 08 will not work
    if (start[0] == '0' && start[1] != '\0'){
        return 0;
    } //No need for errno as im not including long double

    char *endPointer;
    double temp = strtod(start, &endPointer); //Use strtod instead for double (No base 10 for double)
    
    if (*endPointer != '\0' || temp < -DBL_MAX || temp > DBL_MAX) { //Maximum -ve and +ve for double
        return 0; //Fail
    }
    *numericDoubleData = temp; //No need for casting now as im only doing double
    return 1; //Success
}

//DRY code, defined with the unsigned long long so each data type can be converted
void printBinaryBits(unsigned long long value, int bits, char *buffer){
    int position = 0;//Use to move up the buffer positions

    for (int i= (bits - 1); i >= 0 ; i--) { //Backwards for loop MSB to LSB for shifting
        char currentBit = ((value >> i) & 1) + '0';  //'0' has ASCII value 48

        // so if it's 0 + '0' = 48 in ASCII which stays 0
        // but if it's 1 + '0' = 49 in ASCII which becomes 1

        //If testing then store in buffer, if not testing then print
        if (buffer) { 
            buffer[position++] = currentBit; //Increment position in buffer to store each bit
        } else {
            printf("%c", currentBit); //Print out each bit in order
        }
        
        if (i % 4 == 0 && i != 0) { //0 % 4 = 0, so include i != 0 so there's no extra space
            if (buffer) { 
                buffer[position++] = ' '; //Adds space to buffer for each nibble
            } else {
                printf(" "); //Outputs space for each nibble
                }
            }
        }
        if (buffer) { //At the end of the loop if it's testing add a null terminator
            buffer[position] = '\0'; 
        } else {
            printf("\n"); //If not then just print a new line
        }
    }

/*=================================================
    Binary Conversion Function
===================================================*/

void signedCharToBinary(signed char value){
    //range -128 to +127
    printBinaryBits((unsigned char)value, 8, NULL); //Use casting to prevent sign extension
}

void intToBinary(int value){
    // range -2bil to +2bil
    printBinaryBits(value, 32, NULL); //Takes value, number of bits, and a buffer depending on testing
}

void longlongToBinary(long long value){
    // range -2^63 to 2^63-1
    printBinaryBits(value, 64, NULL);// NULL means that we're going to print 
}

void unsignedCharToBinary(unsigned char value){
    //range 0 to 255
    printBinaryBits(value, 8, NULL); //Use helper function for DRY code
}

void unsignedIntToBinary(unsigned int value){
    // range -2bil to +2bil
    printBinaryBits(value, 32, NULL);
}

void doubleToBinary(double value, char *buffer){
    //range 10^-308 to 10^308

    union { //Allows a value to read in converted and read different forms 
        double doubleNo; 
        unsigned long long unsignedLongLongNo;
    } converter; //Variable to store value before conversion

    converter.doubleNo = value; //Place value as the doubleNo in the variable, which can now be access be ULL
    unsigned long long binaryNo = converter.unsignedLongLongNo; //for readability
    printBinaryBits(binaryNo, 64, buffer);
}



/*=================================================
    Unit Testing 
===================================================*/

void testSignedChar(void){
    char buffer[10];// 8 bits + space + null terminator
    
    //Normal Cases
    printBinaryBits((unsigned char) 7, 8, buffer); //Call function to get buffer of 7
    assert(strcmp(buffer, "0000 0111") == 0); //If buffer is identical to it's number then it passes

    printBinaryBits((unsigned char) -7, 8, buffer); 
    assert(strcmp(buffer, "1111 1001") == 0); 

    printBinaryBits((unsigned char) 32, 8, buffer); 
    assert(strcmp(buffer, "0010 0000") == 0); 

    //Extreme Cases
    printBinaryBits((unsigned char) -128, 8, buffer); 
    assert(strcmp(buffer, "1000 0000") == 0); 

    printBinaryBits((unsigned char) 127, 8, buffer); 
    assert(strcmp(buffer, "0111 1111") == 0); 

    //Erroneous Errors
    long long numericData; //Redeclare here as "numericData" was declared after this function was called in main

    assert(convertStringToNumber("128", -128, 127, &numericData, 1) == 0); //If the function fails then it means data was out of limits
    assert(convertStringToNumber("-129", -128, 127, &numericData, 1) == 0);//Which means function is working as data is indeed out of limits
    assert(convertStringToNumber("egg", -128, 127, &numericData, 1) == 0);

}

void testIntToBinary(void){
    char buffer[40]; //32 bits + spaces + null terminator

    // Normal Cases
    printBinaryBits(7, 32, buffer);
    assert(strcmp(buffer, "0000 0000 0000 0000 0000 0000 0000 0111") == 0);

    printBinaryBits(-7, 32, buffer);
    assert(strcmp(buffer, "1111 1111 1111 1111 1111 1111 1111 1001") == 0);

    printBinaryBits(32, 32, buffer);
    assert(strcmp(buffer, "0000 0000 0000 0000 0000 0000 0010 0000") == 0);

    // Extreme Cases
    printBinaryBits(-2147483648, 32, buffer);
    assert(strcmp(buffer, "1000 0000 0000 0000 0000 0000 0000 0000") == 0);

    printBinaryBits(2147483647, 32, buffer); 
    assert(strcmp(buffer, "0111 1111 1111 1111 1111 1111 1111 1111") == 0);

    //Erroneous Errors
    long long numericData; //Redeclare here as "numericData" was declared after this function was called in main

    assert(convertStringToNumber("2147483648", -2147483648, 2147483647, &numericData, 1) == 0);
    assert(convertStringToNumber("-2147483649", -2147483648, 2147483647, &numericData, 1) == 0);
    assert(convertStringToNumber("egg", -2147483648, 2147483647, &numericData, 1) == 0);
}

void testLong(void){
    char buffer[80]; // 64 bits + 15 spaces + null terminator = 80

    // Normal Cases
    printBinaryBits(7LL, 64, buffer);
    assert(strcmp(buffer, "0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0111") == 0);

    printBinaryBits((unsigned long long)(-7LL), 64, buffer);
    assert(strcmp(buffer, "1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1001") == 0);

    printBinaryBits(32LL, 64, buffer);
    assert(strcmp(buffer, "0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0010 0000") == 0);

    // Extreme Cases
    printBinaryBits((unsigned long long)(-9223372036854775807LL - 1LL), 64, buffer); 
    assert(strcmp(buffer, "1000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000") == 0);

    printBinaryBits(9223372036854775807LL, 64, buffer); 
    assert(strcmp(buffer, "0111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111") == 0);

     //Erroneous Errors
    long long numericData; //Redeclare here as "numericData" was declared after this function was called in main

    assert(convertStringToNumber("-9223372036854775809", LLONG_MIN, LLONG_MAX, &numericData, 1) == 0); //If the function fails then it means data was out of limits
    assert(convertStringToNumber("9223372036854775808", LLONG_MIN, LLONG_MAX, &numericData, 1) == 0);//Which means function is working as data is indeed out of limits
    assert(convertStringToNumber("egg", LLONG_MIN, LLONG_MAX, &numericData, 1) == 0);
}

void testUnsignedChar(void){
    char buffer[10];

    //Normal Cases
    printBinaryBits((unsigned char) 7, 8, buffer); //Call function to get buffer of 7
    assert(strcmp(buffer, "0000 0111") == 0); //If buffer is identical to it's number then it passes

    printBinaryBits((unsigned char) -7, 8, buffer); 
    assert(strcmp(buffer, "1111 1001") == 0); 

    printBinaryBits((unsigned char) 32, 8, buffer); 
    assert(strcmp(buffer, "0010 0000") == 0); 

    //Extreme Cases
    printBinaryBits((unsigned char) 0, 8, buffer); 
    assert(strcmp(buffer, "0000 0000") == 0); 

    printBinaryBits((unsigned char) 255, 8, buffer); 
    assert(strcmp(buffer, "1111 1111") == 0); 

    //Erroneous Errors
    long long numericData; //Redeclare here as "numericData" was declared after this function was called in main

    assert(convertStringToNumber("-100", -128, 127, &numericData, 0) == 0); //If the function fails then it means data was out of limits
    assert(convertStringToNumber("256", -128, 127, &numericData, 0) == 0);//Which means function is working as data is indeed out of limits
    assert(convertStringToNumber("egg", -128, 127, &numericData, 0) == 0);
}

void testUnsignedInt(void){
    char buffer[40];

    // Normal Cases
    printBinaryBits(7, 32, buffer);
    assert(strcmp(buffer, "0000 0000 0000 0000 0000 0000 0000 0111") == 0);

    printBinaryBits(-7, 32, buffer);
    assert(strcmp(buffer, "1111 1111 1111 1111 1111 1111 1111 1001") == 0);

    printBinaryBits(32, 32, buffer);
    assert(strcmp(buffer, "0000 0000 0000 0000 0000 0000 0010 0000") == 0);

    // Extreme Cases
    printBinaryBits(0, 32, buffer);
    assert(strcmp(buffer, "0000 0000 0000 0000 0000 0000 0000 0000") == 0);

    printBinaryBits(4294967295, 32, buffer); 
    assert(strcmp(buffer, "1111 1111 1111 1111 1111 1111 1111 1111") == 0);

    //Erroneous Errors
    long long numericData; //Redeclare here as "numericData" was declared after this function was called in main

    assert(convertStringToNumber("4294967296", 0, 4294967295, &numericData, 0) == 0);
    assert(convertStringToNumber("-2147483649", 0, 4294967295, &numericData, 0) == 0);
    assert(convertStringToNumber("egg", 0, 4294967295, &numericData, 0) == 0);
}

void testDouble(void){
    char buffer[80]; // 64 bits + 15 spaces + null terminator = 80

    // Normal Cases
    doubleToBinary(5.5, buffer);
    assert(strcmp(buffer, "0100 0000 0001 0110 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000") == 0); //1st bit sign, next 11 bits exponent, rest is the bits for the mantissa

    doubleToBinary(-5.5, buffer);
    assert(strcmp(buffer, "1100 0000 0001 0110 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000") == 0);


    doubleToBinary(100, buffer);
    assert(strcmp(buffer, "0100 0000 0101 1001 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000") == 0);

    // Extreme Cases
    doubleToBinary(-DBL_MAX, buffer); //DBL_MIN for unsigned, -DBL_MAX for signed
    assert(strcmp(buffer, "1111 1111 1110 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111") == 0);

    doubleToBinary(DBL_MAX, buffer); 
    assert(strcmp(buffer, "0111 1111 1110 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111 1111") == 0);

    //Erroneous Errors
    double numericData; //Redeclare here as "numericData" was declared after this function was called in main

    assert(convertDoubleToNumber("-1e309", &numericData) == 0); //-1e309 is outside of range of -1.7e308
    assert(convertDoubleToNumber("1e309", &numericData) == 0); //+1e309 is outside of range of +1.7e308
    assert(convertDoubleToNumber("egg", &numericData) == 0);
}

/*=================================================
    Main Function
===================================================*/

int main(int argc, char *argv[]){ //Take in type and data
    char *type;
    char *data;

    if (argc == 1) {
        //Run all unit tests
        testSignedChar();
        testIntToBinary();
        testUnsignedChar();
        testLong();
        testUnsignedInt();
        testDouble();
        printf("All tests pass.\n");
        return 0;

    } else if (argc == 4) { //Using this to combine e.g "Unsigned Int" from 2 arguements into 1
        char combineArgs[100]; 
        strcpy(combineArgs, argv[1]);
        strcat(combineArgs, " ");
        strcat(combineArgs, argv[2]); //Copy and append both arguements into my buffer

        type = combineArgs;
        data = argv[3];

    } else if (argc == 5) { //Using this to combine e.g "Unsigned Int" from 2 arguements into 1
        char combineArgs[100]; 
        strcpy(combineArgs, argv[1]); //Combine 3 arguements instead of 2 previously
        strcat(combineArgs, " ");
        strcat(combineArgs, argv[2]);
        strcat(combineArgs, " ");
        strcat(combineArgs, argv[3]); 

        type = combineArgs;
        data = argv[4];

    } else if (argc != 3) {
        printf("Input error.\n");
        return 0;

    } else {
    type = argv[1];
    data = argv[2];

    }

    long long numericData; //Use long long for reusability
    double numericDoubleData;
    //Assign each arguement to variable


    if (strcmp(type, "signed char") == 0 || strcmp(type, "char") == 0 ) { //clang and gcc both see char == signed char
        if (!convertStringToNumber(data, -128, 127, &numericData, 1)) {
        printf("Input error.\n");
        return 0;
        }
        signedCharToBinary(numericData);

    } else if (strcmp(type, "int") == 0) { 
        if (!convertStringToNumber(data, (INT_MIN), (INT_MAX), &numericData, 1)) {
        printf("Input error.\n");
        return 0;
        }
        intToBinary(numericData);

    } else if (strcmp(type, "long") == 0 || strcmp(type, "long long") == 0 || strcmp(type, "signed long long") == 0) { //As we are on Linux long == long long
        if (!convertStringToNumber(data, (LLONG_MIN), (LLONG_MAX), &numericData, 1)) {
        printf("Input error.\n");
        return 0; // longNumericData now contains the valid number
        }
        longlongToBinary(numericData); 

    }  else if (strcmp(type, "unsigned char") == 0) { // unsigned char 45
        if (!convertStringToNumber(data, 0, 255, &numericData, 0)) {
        printf("Input error.\n");
        return 0; // numericData now contains the valid number
        }
        unsignedCharToBinary(numericData);

    } else if (strcmp(type, "unsigned int") == 0) { 
        if (!convertStringToNumber(data, (0), (UINT_MAX), &numericData, 0)) {
        printf("Input error.\n");
        return 0; 
        }
        unsignedIntToBinary(numericData);

    } else if (strcmp(type, "double") == 0) { //64 bits
         if (!convertDoubleToNumber(data, &numericDoubleData)) {
        printf("Input error.\n");
        return 0; 
        }
        char buffer[80];
        doubleToBinary(numericDoubleData, buffer);
        printf("%s\n", buffer);

    } else {
        printf("Type entered is not listed\n");
    }
    return 0;

}

