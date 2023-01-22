/* ADA10DOF Header File
	Austin H. Dial
	4/22/2016
	
	Special thanks to:
	Derek Molloy, School of Electronic Engineering, Dublin City University
	www.eeng.dcu.ie/~molloyd/
*/

#ifndef ADA10DOF_H_

#define ADA10DOF_H_
#define ADA10DOF_I2C_BUFFER 0x80

/* ADA10_RANGE enumeration
	Relates the Linear Acceleration measurement range to integer
	settings for ease of configurability.
	
	Data settings obtained from page 10 of the LSM303 datasheet.
	2.1 Sensor characteristics table 3.
*/
enum ADA10_RANGE {		//	FS bit
	RANGE_2G	= 0,	//	00
	RANGE_4G	= 1,	//	01
	RANGE_8G 	= 2,	//	10
	RANGE_16G	= 3		//	11
};

/* ADA10 BANDWIDTH enumeration
	Relates the bandwidth settings to integer values for ease
	of configurability.
	
	Data settings obtained from page 25 of the LSM303 datasheet.
	7.1.1 CTRL_REG1_A (20h) table 20. Data rate configuration
	
	Be advised that CTRL_REG1_A is located in register 0x20 as
	decribed on page 23 of the LSM303 datasheet section 6 Register
	mapping table 17. Register address map.
	
	Table 18. CTRL_REG1_A register
	ODR3	ODR2	ODR1	ODR0	LPen	Zen	Yen	Xen
	
	Where:
		LPen	- Low power mode enable
		Zen		- Z-axis enable
		Yen		- Y-axis enable
		Xen		- X-axis enable
	
	The CTRL_REG1_A default value is 0000 0111 (Table 19.) This
	means that the chip by defualt is in power down mode according
	to ODR[3:0] value of 0000 and the LPen is set to normal and
	all axes are enabled. It will be necessary to set the ODR[3:0]
	to desires values to obtain the desired frequency.
*/
enum ADA10_BANDWIDTH {	// ODR3	  ODR2	 ODR1	ODR0
	BW_PD	 	= 0,	// 0	  0		 0 		0
	BW_1HZ 		= 1,	// 0	  0		 0		1
	BW_10HZ 	= 2,	// 0	  0		 1		0
	BW_25HZ 	= 3,	// 0	  0		 1		1
	BW_50HZ 	= 4,	// 0	  1		 0		0
	BW_100HZ 	= 5,	// 0	  1		 0		1
	BW_200HZ 	= 6,	// 0	  1		 1		0
	BW_4OOHZ 	= 7,	// 0	  1		 1		1
	BW_LP		= 8,	// 1	  0		 0		0
	BW_SP		= 9		// 1	  0		 0		1
	
	/* NOTES
		PD	- Power down mode
		LP	- Low power mode only (1620Hz)
		SP	- Has two different frequencies based on the mode
			  Normal 		 (1344Hz)
			  Low-power mode (5376Hz)
			  
		Understand that adjusting the bandwidth does not in turn
		adjust the device mode however adjusting the mode when the
		bandwidth is a non-Hz option will possibly change the Hz.
		Unless otherwise mentioned the frequencies will be the same
		for both low-power and normal modes.
	*/
};

/* ADA10_MODECONFIG enumeration
	Relates the two modes available on the chip to integer
	values for ease of configurability.
	
	Data settings obtained from page 16 of the LSM303 datasheet.
	Functionality table 8. Accelerometer operating mode selection.
*/
enum ADA10_MODECONFIG {	// LPen		HR	
	MODE_LOW_NOISE = 0,	// 1		0
	MODE_LOW_POWER = 3	// 0		1
};

/* ADA10Accelerometer class definition



*/
class ADA10DOFAccelerometer {

	private:
		int I2CBus, I2CAddress;					// The current bus and device address
		char dataBuffer[ADA10DOF_I2C_BUFFER];	// Define buffer of maximum allowable size

		int accelerationX;						// Private acceleration on X-axis
		int accelerationY;						// Private acceleration on Y-axis
		int accelerationZ;						// Private acceleration on Z-axis

		double pitch;							// See later functions
		double roll;							// See lates functions

		// Page 11. Section 2.2 Temperature sensor characteristics table 4.
		float temperature;						// Ranged -40C to +85C
		
		ADA10_RANGE range;						// Private range setting
		ADA10_BANDWIDTH bandwidth;				// Private bandwidth setting
		ADA10_MODECONFIG modeConfig;			// Private modeConfig setting

		int  convertAcceleration(int msb_addr, int lsb_addr);	// Converts binary acceleration into integer
		int  writeI2CDeviceByte(char address, char value);		// Writes the given value to the given I2C address
		void calculatePitchAndRoll();							// Uses local data to find pitch and roll

	public:
		void ADA10Accelerometer(int bus, int address);	// Calls class, honestly
		void displayMode(int iterations);				// OPERATION UNKNOWN

		int  readFullSensorState();						// Refreshes the sensor data
		
		//--------------------R/W Data--------------------//
	
		// Range interactions
		int setRange(ADA10_RANGE range);				  // Writes the range for the g-force
		ADA10_RANGE getRange();							  // Reads the g-force range
	
		// Bandwidth interactions
		int setBandwidth(ADA10_BANDWIDTH bandwidth);	  // Writes the bandwidth for the data refresh
		ADA10_BANDWIDTH getBandwidth();					  // Reads the bandwidth setting
	
		// ModeConfig interactions
		int setModeConfig(ADA10_MODECONFIG mode);		  // Writes the mode for the device setting
		ADA10_MODECONFIG getModeConfig();				  // Reads the current device mode
	
		// Temperature read
		float getTemperature();							  // Reads the temperature return from the device

		// Return private accelerations
		int getAccelerationX() { return accelerationX; }  // Publically returns private attribute accelerationX
		int getAccelerationY() { return accelerationY; }  // Publically returns private attribute accelerationY
		int getAccelerationZ() { return accelerationZ; }  // Publically returns private attribute accelerationZ

		// Return private pitch and roll
		float getPitch() { return pitch; }  			  // Pitch in degrees
		float getRoll() { return roll; }  				  // Roll in degrees
		
		//--------------------R/W Data--------------------//

		// virtual ~BMA180Accelerometer();	// OPERATION UNKNOWN
};

#endif /* BMA180ACCELEROMETER_H_ */