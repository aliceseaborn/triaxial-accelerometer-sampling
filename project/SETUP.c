/* TESTING GROUND
	Austin H. Dial
	4/24/2016
	
	The following code is a testing ground for I2C operations 
	including the reading, writing and processing of I2C data.
	The first step will be to access the actual data bus and 
	then to store the data from said bus locally for processing.
*/

#include <sys/ioctl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

/*--------------------GLOBALS--------------------*/
bool debugging = false;
bool safe_mode = false;
int i;

/* ENS - Enable mapping
	0 Low-power mode
	1 X-axis enable
	2 Y-axis enable
	3 Z-axis enable
*/
int ENS[4];



/* USER_SETUP function
	Prompts the user for the device frequency setting and then
	determines the correct ODR[3:0] values from there to send
	to the command prompt in the CTRL_REG1_A_SETUP function.
*/
int USER_SETUP (void){
	int ret;
	
	int RefRate;
	printf("Select the refresh rate.\n");
	printf("\t0\tPower-down mode\n\t1\t1 Hz\n");
	printf("\t2\t10 Hz\n\t3\t25 Hz\n");
	printf("\t4\t50 Hz\n\t5\t100 Hz\n");
	printf("\t6\t200 Hz\n\t7\t400 Hz\n");
	printf("\t8\t1344 Hz\n>: ");
	scanf("%d", &RefRate);
	
/* Here lies the capability to configure Axis enables, INCOMPLETE
	char All_axes[100];
	printf("\nDo you want all axes enabled? (y/n) >: ");
	scanf("%s", All_axes);
	
	// Test user input and configure from there
	if (strcmp(All_axes, "y") == 0){	// Enable all axes
		ENS[0] = 0;	// Low-power enable 	- OFF
		ENS[1] = 1;	// X-axis enable		- ON
		ENS[2] = 1;	// Y-axis enable		- ON
		ENS[3] = 1;	// Z-axis enable		- ON
	}
	else if (strcmp(All_axes, "n") == 0){
		printf("Configuring axes.");
		
		ENS[0] = 0	// The low-power setting will be disengaged by default
		
		// Capture X-axis from user
		printf("Enable X-axis? (1\0) >: ");	// Prompt for X-axis
		scanf("%d", &ENS[1]);				// Capture X-axis enable
		
		// Capture Y-axis from user
		printf("Enable Y-axis? (1\0) >: ");	// Prompt for Y-axis
		scanf("%d", &ENS[2]);				// Capture Y-axis enable
		
		// Capture Z-axis from user
		printf("Enable X-axis? (1\0) >: ");	// Prompt for Z-axis
		scanf("%d", &ENS[3]);				// Capture Z-axis enable
	}
	else{								// FALL THROUGH
		printf("Error: Unexpected input.\n");
		exit(1);
	}
	printf("Configuration complete.\n")
*/

	/*	Now that we have the binary we need to find the HEX value
		but there are lots of possibilities.
	*/
	switch(RefRate){
		case 0:				// Power-down	0000 0000
			ret = 0x00;		// All zero	for ODR and Enables
			break;
		case 1:				// 1 Hz			0001 0111
			ret = 0x17;		// HEX Equivalent
			break;
		case 2:				// 10 Hz		0010 0111
			ret = 0x27;		// HEX Equivalent
			break;
		case 3:				// 25 Hz		0011 0111
			ret = 0x37;		// HEX Equivalent
			break;
		case 4:				// 50 Hz		0100 0111
			ret = 0x47;		// HEX Equivalent
			break;
		case 5:				// 100 Hz		0101 0111
			ret = 0x57;		// HEX Equivalent
			break;
		case 6:				// 200 Hz		0110 0111
			ret = 0x67;		// HEX Equivalent
			break;
		case 7:				// 400 Hz		0111 0111
			ret = 0x77;		// HEX Equivalent
			break;
		case 8:				// 1344 Hz		1001 0111
			ret = 0x97;		// HEX Equivalent
			break;
		default:												// Unrecognized ODR setting
			printf("Error: ODR Configuration unrecognized.\n");	// Inform user of error
			exit(1);											// Exit with error
	}
	
	// Return the set return value
	return ret;
}

/* CTRL_REG1_A_SETUP function
	The first step in setting up the accelerometer is the 
	configuration of the CTRL_REG1_A to activate the device
	and define the refreshing rate of the data. This requires
	using the i2cset tool which takes arguments for the i2c
	bus, chip address, data-address, and the desired value
	of that data-address. To accomplish this task we use
	the the sprintf to print to a character string before
	sending it to system() as if is were a user command.
	
		Bus:	 1
		Address: 0x19

	REF: http://linux.die.net/man/8/i2cset
*/
void CTRL_REG1_A_SETUP (int i2c_bus, int dev_addr, int data_addr, int CTRL_value){
	
	/* Check that the bus is correct
		The BeagleBoneBlack has i2c-0 and i2c-1 configured
		by default. The i2c-2 bus may be configured but it
		requires work outside the scope of this program.
	*/
	switch(i2c_bus){
		case 0:
		case 1:
			/*No need for action*/
			break;
		case 2:
			printf("WARNING: The specified bus may not be set up.\n");
			printf("Press CTRL-C to cancel the process. \n");
			int i;
			for (i = 5; i > 0; i--){printf("%d.", i); sleep(1);}
			break;
		default:
			printf("Error: The specified bus does not exist.\n");
			exit(1);
			break;
	}
	
	/* Check that the data_addr is correct
		Although this program can theoreticallt write to
		any specified bus, dev_addr, and data-addr without
		error it is not recommended to use i2cset with data
		addresses between the range of 0x50 and 0x57 as it
		could easily destroy the device.
	*/
	char HEX[8];
	sprintf(HEX, "%#x", data_addr);
	if (strcmp(HEX, "0x50") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}		// 0x50 RESERVED ADDRESS
	else if (strcmp(HEX, "0x50") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else if (strcmp(HEX, "0x51") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else if (strcmp(HEX, "0x52") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else if (strcmp(HEX, "0x53") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else if (strcmp(HEX, "0x54") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else if (strcmp(HEX, "0x55") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else if (strcmp(HEX, "0x56") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else if (strcmp(HEX, "0x57") == 0){printf("Error: Specified data-address \"%s\" is dangerous", HEX); exit(1);}	// ...
	else{/*No need for action*/}

	/*	First we need to write to the CTRL_REG1_A register to
		set the device mode to normal from the default power
		down 0000 setting.
	*/
	char command[50];	// Command to be send as an argument of system();
	int n;				// Saves the returnes string size from sprintf
	
	// Form command and store sprintf output in n for buffer checking
	n = sprintf(command, "i2cset -y %d %#x %#x %#x", i2c_bus, dev_addr, data_addr, CTRL_value);
	
	// If the sprintf return is greater than the size of command
	if(n >= sizeof(command)){
		printf("Error: Command string has caused buffer overflow.\n");	// Inform error
		printf("Process cancelled to protect the device.\n");			// ...
		exit(1);														// Exit with error
	}
	else{/*No need for action*/}
	
	
	if (debugging){printf("%s\n", command);}	// print the command string is the user is verbose
	else{/*No need for action.*/}				// otherwise continue regardless
	
	// If safe mode is on DO NOT send command to the system
	if (safe_mode){printf("Same mode prevented system call.\n");}
	else if (!safe_mode){	// Safe_mode is off, send command to system
		system(command);	// Execute system command
	}
	else{printf("Error: You broke logic, congratulations.\n");}
}

int main(int argc, char *argv[]){
	if (argc > 1){								// If the user sends arguments
		for (i = 0; i < argc; i++){					// Scan all arguments
			if (strcmp(argv[i], "-safe") == 0){		// If one of them is -safe
				safe_mode = true;					// Set safe mode on
			}
			if (strcmp(argv[i], "verbose") == 0){	// If the user sends verbose
				debugging = true;					// Set global debugging to true
			}
			else{/*No need for action*/}			// Otherwise just continue
		}
	}
	else{/*No need for action*/}

	
	
	int USER_FREQ = USER_SETUP();				// Setup the device and ask for the frequency settings
	CTRL_REG1_A_SETUP(1, 0x19, 0x20, USER_FREQ);	// Send frequency setting are argument

	return 0;
}