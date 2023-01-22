/* Bus Read Test
	Austin H. Dial
	4/26/2016
*/

#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

/*--------------------GLOBALS--------------------*/
int read_count = 0;
int target_count = 0;
int refresh_rate = 10;

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

const int ENTRY_SIZE = (sizeof(ENTRY)/sizeof(entry));	// The number of entries for ENTRY declared below

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
		fprintf(fpOUT, "%d\t0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",	// Print formatted string to data file
			current->VALUE.time_sig,							// Print entry number
			current->VALUE.X_L, current->VALUE.X_H,				// Print X_L X_H
			current->VALUE.Y_L, current->VALUE.Y_H,				// ...
			current->VALUE.Z_L, current->VALUE.Z_H);			// ...
			
		current = current->NEXT;										// Advance current to the next entry
	}
	
	fclose (fpOUT);	// Close the date file
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
	int i;								// Instantiate iteration counter
	for (i = 0; i < target_count; i++){	// For every element in target count
		BUS_READ(1, 0x19);				// Read device ID 0x19 on bus 1
		LSAVE();						// Save data from RAWData
		usleep(usleep_value);			// Wait before recall
	}
	PDUMP();	// Save local data into formatted file
	
	return 0;	// TERMINATE MAIN PROGRAM
}