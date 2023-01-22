/* PARSE PrettyData.txt from hex to signed int
	Austin H. Dial
	4/27/2016
*/

/* Signed Magnitude Hex to Decimal
	Austin H. Dial
	4/27/2016
*/

#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>

/*--------------------GLOBALS--------------------*/
int read_count = 0;
int target_count = 0;
int refresh_rate = 10;
const int ENTRY_SIZE = 1000;	// The number of entries for ENTRY declared below

/* NOTES ON ENTRY
	Defining how many entries are to explitely exist
	is actually quite challenging. Assuming a refresh_rate
	of 10 calls per given second will be an acceptable
	resolution, the 1000 entries will store data for 
	100 seconds. Unfortunately this means that the data
	will cap after 100 seconds and a system must be
	built which prevents overflow.
*/
struct entry{
	int X_L, X_H;	// X-Axis acceleration data
	int Y_L, Y_H;	// Y-Axis acceleration data
	int Z_L, Z_H;	// Z-Axis acceleration data
	int time_sig;	// The entry number
}ENTRY[1000];	// Arbitrary, see notes
typedef struct entry entry;	// Define type for entry

struct record{				// Linked list architecture
	entry VALUE;			// VALUE sector
	struct record *NEXT;	// NEXT pointer
};
typedef struct record list;	// Define type for list (LL)

list *head, *current;	// Place holders and navigators

/* CHECK_N function
	Takes an integer variable and its maximum size and
	prints an error if the variable is out of range to
	prevent buffer overflow.
*/
void CHECK_N (int variable, int buffer_size){
	if (variable >= buffer_size){
		printf("Error: Potential buffer overflow.\n");	// Inform the user of the error
		exit(1);										// Exit with error
	}
	else {/*No need for action*/}
}

/* BUS_READ function
	Reads the content of the I2C device data bus and stores
	the content into a file for futher analysis.
	
	REF: http://www.cplusplus.com/reference/cstdio/freopen/
*/
void BUS_READ (int bus, int dev_addr){
	char *filename = "RAWData.txt";	// Define the output file name
	freopen(filename, "w", stdout);	// Open the Data.txt file in write mode and send stdout stream to file
	
	// Ensure that the output data file is genuine
	if (freopen(filename, "w", stdout) == NULL){
		printf("Error: %s file not found.\n", filename);	// Inform the user of error
		exit(1);											// Exit with error
	}
	
	
	char command[50];	// Store the literal command for i2cget
	int n;				// Captures sprintf output to check for overflow

	/* X-Axis i2cget command
		OUT_X_L_A	- 0x28
		OUT_X_H_A	- 0x29
	*/
	// LOW
	n = sprintf(command, "i2cget -y %d %#x 0x28", bus, dev_addr);	// Form command
	CHECK_N(n, 50);													// Check for overflow
	system(command);												// X-Axis LOW
	
	// HIGH
	n = sprintf(command, "i2cget -y %d %#x 0x29", bus, dev_addr);	// Form command
	CHECK_N(n, 50);													// Check for overflow
	system(command);												// X-Axis HIGH
	
	/* Y-Axis i2cget command
		OUT_Y_L_A	- 0x2A
		OUT_Y_H_A	- 0x2B
	*/
	// LOW
	n = sprintf(command, "i2cget -y %d %#x 0x2A", bus, dev_addr);	// Form command
	CHECK_N(n, 50);													// Check for overflow
	system(command);												// Y-Axis LOW
	
	// HIGH
	n = sprintf(command, "i2cget -y %d %#x 0x2B", bus, dev_addr);	// Form command
	CHECK_N(n, 50);													// Check for overflow
	system(command);												// Y-Axis HIGH
	
	/* Z-Axis i2cget command
		OUT_Z_L_A	- 0x2C
		OUT_Z_H_A	- 0x2D
	*/
	// LOW
	n = sprintf(command, "i2cget -y %d %#x 0x2C", bus, dev_addr);	// Form command
	CHECK_N(n, 50);													// Check for overflow
	system(command);												// Z-Axis LOW
	
	// HIGH
	n = sprintf(command, "i2cget -y %d %#x 0x2D", bus, dev_addr);	// Form command
	CHECK_N(n, 50);													// Check for overflow
	system(command);												// Z-Axis HIGH
	
	/*	Hypothesis: The stdout stream will be sent to
		the file and we will be able to effectively call
		for certain data areas without having to use C++.
	*/
	
	read_count++;	// Increment the number of reads performed
}

/* LSAVE function
	The workaround demonstrated and ultimately implemented
	in the BUS_READ function is great but upon every call to
	that function is resets the data file and thus we lose
	the data every time. To alter this we will call the LSAVE
	function before calling BUS_READ such that the data may
	undergo a local save which prevents loss between refreshes.
*/
void LSAVE (void){
	char *filename = "RAWData.txt";
	FILE *fpIN;
	fpIN = fopen(filename, "r");
	
	// Check that the file works and can be opened
	if (fopen(filename, "r") == NULL){							// If the file cannot be opened
		printf("Error: Trouble opening %s file.", filename);	// Inform error
		exit(1);												// Exit with incident
	}

	// If this is the first local entry
	if (read_count == 1){
		current = (list *)malloc(sizeof(list));
		
		// X-Axis values
		fscanf(fpIN, "%x\n", &current->VALUE.X_L);	// Save X-Axis low
		fscanf(fpIN, "%x\n", &current->VALUE.X_H);	// Save X-Axis high
		
		// Y-Axis values
		fscanf(fpIN, "%x\n", &current->VALUE.Y_L);	// ...
		fscanf(fpIN, "%x\n", &current->VALUE.Y_H);	// ...
		
		// Z-Axis values
		fscanf(fpIN, "%x\n", &current->VALUE.Z_L);	// ...
		fscanf(fpIN, "%x\n", &current->VALUE.Z_H);	// ...
		
		current->NEXT = NULL;	// Set the next to NULL
		head = current;			// Pass current, now that it's set up, to head
	}
	else{
		current->NEXT = (list *)malloc(sizeof(list));	// Create a new entry and assign it to current->NEXT
		
		// X-Axis values
		fscanf(fpIN, "%x\n", &current->VALUE.X_L);	// Save X-Axis low
		fscanf(fpIN, "%x\n", &current->VALUE.X_H);	// Save X-Axis high
		
		// Y-Axis values
		fscanf(fpIN, "%x\n", &current->VALUE.Y_L);	// ...
		fscanf(fpIN, "%x\n", &current->VALUE.Y_H);	// ...
		
		// Z-Axis values
		fscanf(fpIN, "%x\n", &current->VALUE.Z_L);	// ...
		fscanf(fpIN, "%x\n", &current->VALUE.Z_H);	// ...
		
		current->NEXT = NULL;						// Set the next to NULL
	}
	
	current->VALUE.time_sig = read_count;	// Store element number
		
	current->NEXT = (list *)malloc(sizeof(list));	// Create memory location of list
	current = current->NEXT;						// Advance current by one element of list
	
	fclose(fpIN);	// Close the DATA file
}

/* PDUMP function
	Considering that all of the data is stored locally within
	the linked list it becomes quite easy to dump the data in
	a formatted manner once the read is completed. One simply
	just passed the data to a file in a formatted manner.
*/
void PDUMP (void){
	char *filename = "PrettyData.txt";	// Define the file name in string literal
	FILE *fpOUT;						// Define fpOUT pointer of type FILE
	fpOUT = fopen(filename, "w");		// Open the file in write mode
	
	current = head;	// Set current to head
	
	while (current->NEXT != NULL){								// While there is another record next
		fprintf(fpOUT, "%d\t%d %d %d %d %d %d\n",	// Print formatted string to data file
			current->VALUE.time_sig,							// Print entry number
			current->VALUE.X_L, current->VALUE.X_H,				// Print X_L X_H
			current->VALUE.Y_L, current->VALUE.Y_H,				// ...
			current->VALUE.Z_L, current->VALUE.Z_H);			// ...
			
		current = current->NEXT;								// Advance current to the next entry
	}
	
	fclose (fpOUT);	// Close the date file
}

/* BIN_DEC function
	Takes array arguments as a pointers and converts the binary
	values in the bit arrays to decimals and appends them to the
	matching array.
*/
int BIN_DEC (int *B){
	int i = 0;
	
	// First we need to document which elements are 1 and store
	// the corresponding power in an array.
	int POWERS[8];
	
	// Now collect the values to add later
	for (i = 0; i < 8; i++){
		int temp = 7 - i;
		if (B[i] == 1){POWERS[i] = pow(2, temp);}
		else {POWERS[i] = 0;}
	}
	
	// Now we've stored the values of 2^n for each of the elements
	// in the target array, now simply add them up to get the decimal.
	int RET = 0;
	for (i = 0; i < sizeof(POWERS)/sizeof(int); i++){
		RET += POWERS[i];
	}
	
	return RET;
}

/* CONVERT function
	Takes a hexadecimal representation a two's complement number
	and returns it's decimal equivalent. This proocess may be
	applied to convert the signed hexadecimal accelerometer data
	into a useable/plotable form.
*/
int CONVERT (int hex){
	int ret;	// Return value
	int i = 0;	// Iteration counter
	
	/* Convert Hexadecimal to binary integer array
		The first step will be to create a binary array which may
		represent the hexadecimal input.
	*/
	char hex_str[20];						// Store string of integer
	int n = sprintf(hex_str, "%x", hex);	// Pass integer as string and store in hex_str
	
	hex_str[n+1] = '\0';	// Set last character to NULL
	
	if (strcmp(hex_str, "0x") == 0){	// If string begins with 0x then delete to get the core value
		hex_str[0] = hex_str[2];		// Pass value part 1 to loaction 0
		hex_str[1] = hex_str[3];		// Pass value part 2 to loaction 1
	}
	else{/*No need for action*/}
	
	int BIN[8];
	
	// Find size
	while (hex_str[i] != '\0'){
		i++;
	}
	i--;	// This is the last value before null
	
	int B1[4];	// First set
	int B2[4];	// Second set
	
	// Now populate both sets based on the values
	if (i == 0){		// If only one character exist
         switch(hex_str[0]){
            case '0': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 0;
				break;
            case '1': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 1;
				break;
            case '2': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 0;
				break;
            case '3': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case '4': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case '5': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case '6': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case '7': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case '8': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case '9': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case 'A': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'B': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case 'C': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case 'D': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case 'E': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'F': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case 'a': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'b': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case 'c': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case 'd': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case 'e': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'f': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 1;
				break;
			default:  
				printf("Error: Invalid hexadecimal character.\n"); exit(1);
        }
		
		B2[0] = 0;
		B2[1] = 0;
		B2[2] = 0;
		B2[3] = 0;
	}
	else if (i == 1){	// If two characters exist
		switch(hex_str[0]){
            case '0': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 0;
				break;
            case '1': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 1;
				break;
            case '2': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 0;
				break;
            case '3': 
				B1[0] = 0;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case '4': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case '5': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case '6': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case '7': 
				B1[0] = 0;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case '8': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case '9': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case 'A': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'B': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case 'C': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case 'D': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case 'E': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'F': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case 'a': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'b': 
				B1[0] = 1;
				B1[1] = 0;
				B1[2] = 1;
				B1[3] = 1;
				break;
			case 'c': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 0;
				break;
			case 'd': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 0;
				B1[3] = 1;
				break;
			case 'e': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 0;
				break;
			case 'f': 
				B1[0] = 1;
				B1[1] = 1;
				B1[2] = 1;
				B1[3] = 1;
				break;
			default:  
				printf("Error: Invalid hexadecimal character.\n"); exit(1);
        }
		
		// Check the second character
		switch(hex_str[1]){
            case '0': 
				B2[0] = 0;
				B2[1] = 0;
				B2[2] = 0;
				B2[3] = 0;
				break;
            case '1': 
				B2[0] = 0;
				B2[1] = 0;
				B2[2] = 0;
				B2[3] = 1;
				break;
            case '2': 
				B2[0] = 0;
				B2[1] = 0;
				B2[2] = 1;
				B2[3] = 0;
				break;
            case '3': 
				B2[0] = 0;
				B2[1] = 0;
				B2[2] = 1;
				B2[3] = 1;
				break;
			case '4': 
				B2[0] = 0;
				B2[1] = 1;
				B2[2] = 0;
				B2[3] = 0;
				break;
			case '5': 
				B2[0] = 0;
				B2[1] = 1;
				B2[2] = 0;
				B2[3] = 1;
				break;
			case '6': 
				B2[0] = 0;
				B2[1] = 1;
				B2[2] = 1;
				B2[3] = 0;
				break;
			case '7': 
				B2[0] = 0;
				B2[1] = 1;
				B2[2] = 1;
				B2[3] = 1;
				break;
			case '8': 
				B2[0] = 1;
				B2[1] = 0;
				B2[2] = 0;
				B2[3] = 0;
				break;
			case '9': 
				B2[0] = 1;
				B2[1] = 0;
				B2[2] = 0;
				B2[3] = 1;
				break;
			case 'A': 
				B2[0] = 1;
				B2[1] = 0;
				B2[2] = 1;
				B2[3] = 0;
				break;
			case 'B': 
				B2[0] = 1;
				B2[1] = 0;
				B2[2] = 1;
				B2[3] = 1;
				break;
			case 'C': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 0;
				B2[3] = 0;
				break;
			case 'D': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 0;
				B2[3] = 1;
				break;
			case 'E': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 1;
				B2[3] = 0;
				break;
			case 'F': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 1;
				B2[3] = 1;
				break;
			case 'a': 
				B2[0] = 1;
				B2[1] = 0;
				B2[2] = 1;
				B2[3] = 0;
				break;
			case 'b': 
				B2[0] = 1;
				B2[1] = 0;
				B2[2] = 1;
				B2[3] = 1;
				break;
			case 'c': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 0;
				B2[3] = 0;
				break;
			case 'd': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 0;
				B2[3] = 1;
				break;
			case 'e': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 1;
				B2[3] = 0;
				break;
			case 'f': 
				B2[0] = 1;
				B2[1] = 1;
				B2[2] = 1;
				B2[3] = 1;
				break;
			default:  
				printf("Error: Invalid hexadecimal character.\n"); exit(1);;
        }
	}
	else{				// Unsupported size > 2
		printf("Error: Unsupported hexadecimal size\n");
		exit(1);
	}
	
	// Now combine B1 and B2 into BIN
	
	if (i == 1){		// If there are two character in the hexadecimal
		int j;
		for (j = 0; j < 4; j++){
			BIN[j]	= B1[j];		// Fill set 2
		}
		for (j = 7; j > 3; j--){
			BIN[j]	= B2[j - 4];	// Fill set 1
		}
	}
	else if (i == 0){	// If there is only one character in the hexadecimal
		int j;
		for (j = 0; j < 4; j++){
			BIN[j]	= B2[j];		// Fill set 2
		}
		for (j = 7; j > 3; j--){
			BIN[j]	= B1[j - 4];	// Fill set 1
		}
	}
	
	// For debugging we'll print this value
	printf("%#x = ", hex);
	for (i = 0; i < (sizeof(BIN)/sizeof(int)); i++){
		printf("%d", BIN[i]);
	}
	printf("\n");
	
	/*	Take two's complement
		Now we have a binary array representing the signed hex.
		To complete this process just take the two's complement
		and convert to decimal.
	*/
	int COMP2[8];
	bool neg_bit = false;
	
	
	// Now check if it is negative
	if (BIN[0] == 0){
		neg_bit = false;
	}
	else if(BIN[0] == 1){
		neg_bit = true;
	}
	
	
	// If negative then flip and add 1
	if (neg_bit){
		int j;
		
		// Set COMP2 and BIN equal to each other
		for (i = 0; i < (sizeof(COMP2)/sizeof(int)); i++){
			COMP2[i] = BIN[i];
		}
		
		// Invert all values in array
		for (j = 0; j < (sizeof(COMP2)/sizeof(int)); j++){
			if (COMP2[j] == 0){ COMP2[j] = 1;}
			else {COMP2[j] = 0;}
		}
	
		// Now we need ro securily add 1 to the array and carry
		int carry = 1;
		j = 0;
		do {
			if (COMP2[7 - j] == 1){
				COMP2[7 - j] = 0;
				carry = 1;
			}
			else{
				COMP2[7 - j] = 1;
				carry = 0;
			}

			j++;
		} while (carry == 1);
	}
	
	// If neg_bit is false then simply represent in binary
	else{
		
		// Set COMP2 and BIN equal to each other
		for (i = 0; i < (sizeof(COMP2)/sizeof(int)); i++){
			COMP2[i] = BIN[i];
		}
	}
	
//	For debugging we'll print here
	printf("Unsigned = ");
	for (i = 0; i < (sizeof(COMP2)/sizeof(int)); i++){
		printf("%d", COMP2[i]);
	}
	printf("\n");

	/* Now convert to decimal
		We have the raw binary number now and can simply
		convert to decimal and use that value.
	*/
	
	ret = BIN_DEC(COMP2);	// Get the decimal number equivalent from BIN_DEC
	
	if (neg_bit){ret = ret * -1;}	// If the signed binary was negative then negate
	else{/*No need for action*/}	// Otherwise leave it positive
	
	return ret; // Return the INT equivalent
}

/* PARSE function
	Using the CONVERT function listed above, PARSE takes
	the local Linked list and trades the VALUE sector elements
	from hexadecimal into a signed binary for graphing later.
*/
void PARSE (void){
	
	current = head;	// Start at the beginning of the linked list
	int new;		// Store integer equivalent of current VALUE sector
	
	while (current->NEXT != NULL){
		// X-Axis conversion
		current->VALUE.X_L = CONVERT(&current->VALUE.X_L);
		current->VALUE.X_H = CONVERT(&current->VALUE.X_H);
	
		// Y-Axis conversion
		new = CONVERT(current->VALUE.X_L);
		current->VALUE.X_L = new;
		new = CONVERT(current->VALUE.X_H);
		current->VALUE.X_H = new;
		
		// Z-Axis conversion
		new = CONVERT(current->VALUE.X_L);
		current->VALUE.X_L = new;
		new = CONVERT(current->VALUE.X_H);
		current->VALUE.X_H = new;
		
		current = current->NEXT;
	}
	
	// X-Axis conversion
	current->VALUE.X_L = CONVERT(&current->VALUE.X_L);
	current->VALUE.X_H = CONVERT(&current->VALUE.X_H);
	
	// Y-Axis conversion
	current->VALUE.Y_L = CONVERT(current->VALUE.Y_L);
	current->VALUE.Y_H = CONVERT(current->VALUE.Y_H);
		
	// Z-Axis conversion
	current->VALUE.Z_L = CONVERT(current->VALUE.Z_L);
	current->VALUE.Z_H = CONVERT(current->VALUE.Z_H);	
}

/* PRAM function
	Reads the linked list in memory and prints it to the command
	prompt. Note that this does not read/touch/or otherwise alter
	the solid file system.
*/
void PRAM (void){
	current = head;
	
	while (current->NEXT != NULL){										// While there is another record next
		printf("%d\t%d %d %d %d %d %d\n",	// Print formatted string to data file
			current->VALUE.time_sig,							// Print entry number
			current->VALUE.X_L, current->VALUE.X_H,				// Print X_L X_H
			current->VALUE.Y_L, current->VALUE.Y_H,				// ...
			current->VALUE.Z_L, current->VALUE.Z_H);			// ...
			
		current = current->NEXT;										// Advance current to the next entry
	}
}


int main (int argc, char *argv[]){
	if (argc > 1){										// If the user specifies the time rate
		target_count = refresh_rate * (atoi(argv[1]));	// Multiply the seconds by the number of entries per second
	}
	else{														// If the user has not thrown arguments
		printf("Error: Sample time expected. Try --help.\n");	// Inform the user of the error
		exit(0);												// Exit without error
	}
	
	// Check that the ENTRY_SIZE is adequate
	if (target_count > ENTRY_SIZE){
		printf("Error: The specified sample duration is unsupported. Try --help\n");	// Inform error
		exit(0);																		// Exit without incident
	}
	else{/*No need for action*/}
	
	unsigned int usleep_value = (1000000/refresh_rate);	// Number of microseconds in a second divided by the ticks/second
	
	// REF: http://linux.die.net/man/3/usleep
	int i;									// Instantiate iteration counter
	for (i = 0; i < target_count; i++){		// For every element in target count
		BUS_READ(1, 0x19);					// Read device ID 0x19 on bus 1
		LSAVE();							// Save data from RAWData
		usleep(usleep_value);				// Wait before recall
	}
	PARSE();	// Convert data to decimal
//	PDUMP();	// Save local data into formatted file

	PRAM();
	
	
	// For debugging let's check how PARSE interacts with the linked list
	current = head;
	printf("%x = %d\n",
		&current->VALUE.X_H,
		CONVERT(&current->VALUE.X_H));
	
	printf("HELLO!\n");
	
	
	return 0;	// TERMINATE MAIN PROGRAM
}