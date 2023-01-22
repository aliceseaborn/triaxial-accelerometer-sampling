/* Adafruit 10-DOF Header File
	Austin H. Dial
	4/20/2016

	Contains all of the necessary functions and definitions
	to carry out the communication between I2C devices on
	a higher logical level. In other words it allows this
	stuff to not crowd the actual data processing unit with
	its lower level processes.
*/


#include "ADA10DOFAccelerometer.h"
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <stropts.h>
#include <unistd.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>

#include <iostream>
using namespace std;

// Define variables to register address according to the datasheet
#define ACC_X_LSB	0x28	// Map X-axis acceleration LSB to address 0x02
#define ACC_X_MSB	0x29	// ...
#define ACC_Y_LSB	0x2A	// ...
#define ACC_Y_MSB	0x2B	// ...
#define ACC_Z_LSB	0x2C	// ...
#define ACC_Z_MSB	0x2C	// ...
#define TEMP		0x31	// TEMP_OUT_H_M
#define RANGE		0x01	// CRB_REG_M
#define BANDWIDTH	0x20	// CTRL_REG1_A
#define MODE_CONFIG	0x20	// CTRL_REG1_A

#define MAX_BUS 0x80

/* NOTES
	The 0x20 resisters are used twice which ought not be possible, this
	could result in critical system crashes of even corruption and there
	-fore further research is diffinitively needed before operation.
*/

/* ADA10DOFAccelerometer function
		Defines an instance of the accelerometer class and
	takes attributes bus for the I2C bus and address for
	the address of the device itself.
	Upon instantiation the class calls the function 
	ReadSensorState() to load the buffer.
*/ 
void ADA10DOFAccelerometer::ADA10Accelerometer(int bus, int address) {
	I2CBus = bus;						// Set the attribute I2CBus of the class equal to the argument of the function
	I2CAddress = address;				// ... for address
	readFullSensorState();				// Call ReadFullSensorState
}

/* calculatePitchAndRoll function
	Calculates the pitch and roll of the device using
	the acceleration attributes.
*/
void ADA10DOFAccelerometer::calculatePitchAndRoll() {

	double accelerationXSquared = this->accelerationX * this->accelerationX;
	double accelerationYSquared = this->accelerationY * this->accelerationY;
	double accelerationZSquared = this->accelerationZ * this->accelerationZ;
	this->pitch = 180 * atan(accelerationX/sqrt(accelerationYSquared + accelerationZSquared))/M_PI;
	this->roll = 180 * atan(accelerationY/sqrt(accelerationXSquared + accelerationZSquared))/M_PI;
}

/* readFullSensorState function
	Refreshes the I2C bus information and 
*/
int ADA10DOFAccelerometer::readFullSensorState(){

   //cout << "Starting BMA180 I2C sensor state read" << endl;
    char namebuf[MAX_BUS];							// namebuffer of maximum allowed size
   	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);		// Writes the string literal with the bus sent to it to namebuf
    int file;									// File of type int
    if ((file = open(namebuf, O_RDWR)) < 0){						// 
            cout << "Failed to open BMA180 Sensor on " << namebuf << " I2C Bus" << endl;// 
            return(1);									// 
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){					// 
            cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;		// 
            return(2);									// 
    }

    // According to the BMA180 datasheet on page 59, you need to send the first address
    // in write mode and then a stop/start condition is issued. Data bytes are
    // transferred with automatic address increment.
    char buf[1] = { 0x00 };								// 
    if(write(file, buf, 1) !=1){							// 
    	cout << "Failed to Reset Address in readFullSensorState() " << endl;		// 
    }

    int numberBytes = ADA10DOF_I2C_BUFFER;						// 
    int bytesRead = read(file, this->dataBuffer, numberBytes);				// 
    if (bytesRead == -1){								// 
    	cout << "Failure to read Byte Stream in readFullSensorState()" << endl;		// 
    }
    close(file);									// 

    if (this->dataBuffer[0]!=0x03){
    	cout << "MAJOR FAILURE: DATA WITH BMA180 HAS LOST SYNC!" << endl;
    }

   // cout << "Number of bytes read was " << bytesRead << endl;
   /// for (int i=0; i<8; i++){
    //       printf("Byte %02d is 0x%02x\n", i, dataBuffer[i]);
    //
    //cout << "Closing BMA180 I2C sensor state read" << endl;

    this->accelerationX = convertAcceleration(ACC_X_MSB, ACC_X_LSB);
    this->accelerationY = convertAcceleration(ACC_Y_MSB, ACC_Y_LSB);
    this->accelerationZ = convertAcceleration(ACC_Z_MSB, ACC_Z_LSB);
    this->calculatePitchAndRoll();
    //cout << "Pitch:" << this->getPitch() << "   Roll:" << this->getRoll() <<  endl;
    return 0;
}

int ADA10DOFAccelerometer::convertAcceleration(int msb_reg_addr, int lsb_reg_addr){
//	cout << "Converting " << (int) dataBuffer[msb_reg_addr] << " and " << (int) dataBuffer[lsb_reg_addr] << endl;;
	short temp = dataBuffer[msb_reg_addr];
	temp = (temp<<8) | dataBuffer[lsb_reg_addr];
	temp = temp>>2;
	temp = ~temp + 1;
//	cout << "The X acceleration is " << temp << endl;
	return temp;
}

void ADA10DOFAccelerometer::displayMode(int iterations){

	for(int i=0; i<iterations; i++){
		this->readFullSensorState();
		printf("Rotation (%d, %d, %d)", accelerationX, accelerationY, accelerationZ);
	}
}

//  Temperature in 2's complement has a resolution of 0.5K/LSB
//  80h is lowest temp - approx -40C and 00000010 is 25C in 2's complement
//  this value is offset at room temperature - 25C and accurate to 0.5K

float ADA10DOFAccelerometer::getTemperature(){

	int offset = -40;  // -40 degrees C
	this->readFullSensorState();
	char temp = dataBuffer[TEMP]; // = -80C 0b10000000  0b00000010; = +25C
	//char temp = this->readI2CDeviceByte(TEMP);
	//this->readFullSensorState();
    //char temp = dataBuffer[TEMP];
	int temperature;
	if(temp&0x80)	{
		temp = ~temp + 0b00000001;
		temperature = 128 - temp;
	}
	else {
		temperature = 128 + temp;
	}
	this->temperature = offset + ((float)temperature*0.5f);
	//cout << "The temperature is " << this->temperature << endl;
	//int temp_off = dataBuffer[0x37]>>1;
	//cout << "Temperature offset raw value is: " << temp_off << endl;
	return this->temperature;
}

ADA10_RANGE ADA10DOFAccelerometer::getRange(){
	this->readFullSensorState();
	char temp = dataBuffer[RANGE];
	//char temp = this->readI2CDeviceByte(RANGE);  //bits 3,2,1
	temp = temp & 0b00001110;
	temp = temp>>1;
	//cout << "The current range is: " << (int)temp << endl;
	this->range = (ADA10_RANGE) temp;
	return this->range;
}

int ADA10DOFAccelerometer::setRange(ADA10_RANGE range){
	//char current = this->readI2CDeviceByte(RANGE);  //bits 3,2,1
	this->readFullSensorState();
	char current = dataBuffer[RANGE];
	char temp = range << 1; //move value into bits 3,2,1
	current = current & 0b11110001; //clear the current bits 3,2,1
	temp = current | temp;
	if(this->writeI2CDeviceByte(RANGE, temp)!=0){
		cout << "Failure to update RANGE value" << endl;
		return 1;
	}
	return 0;
}

ADA10_BANDWIDTH ADA10DOFAccelerometer::getBandwidth(){
	this->readFullSensorState();
	char temp = dataBuffer[BANDWIDTH];   //bits 7->4
	//char temp = this->readI2CDeviceByte(BANDWIDTH);  //bits 7,6,5,4
//	cout << "The value of bandwidth returned is: " << (int)temp << endl;
	temp = temp & 0b11110000;
	temp = temp>>4;
//	cout << "The current bandwidth is: " << (int)temp << endl;
	this->bandwidth = (ADA10_BANDWIDTH) temp;
	return this->bandwidth;
}

int ADA10DOFAccelerometer::setBandwidth(ADA10_BANDWIDTH bandwidth){
	//char current = this->readI2CDeviceByte(BANDWIDTH);  //bits 7,6,5,4
	this->readFullSensorState();
    char current = dataBuffer[BANDWIDTH];   //bits 7->4
	char temp = bandwidth << 4; //move value into bits 7,6,5,4
	current = current & 0b00001111; //clear the current bits 7,6,5,4
	temp = current | temp;
	if(this->writeI2CDeviceByte(BANDWIDTH, temp)!=0){
		cout << "Failure to update BANDWIDTH value" << endl;
		return 1;
	}
	return 0;
}

ADA10_MODECONFIG ADA10DOFAccelerometer::getModeConfig(){
	//char temp = dataBuffer[MODE_CONFIG];   //bits 1,0
	//char temp = this->readI2CDeviceByte(MODE_CONFIG);  //bits 1,0
	this->readFullSensorState();
    char temp = dataBuffer[MODE_CONFIG];
	temp = temp & 0b00000011;
	//cout << "The current mode config is: " << (int)temp << endl;
	this->modeConfig = (ADA10_MODECONFIG) temp;
	return this->modeConfig;
}

int ADA10DOFAccelerometer::writeI2CDeviceByte(char address, char value){

    cout << "Starting BMA180 I2C sensor state write" << endl;
    char namebuf[MAX_BUS];
   	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
    int file;
    if ((file = open(namebuf, O_RDWR)) < 0){
            cout << "Failed to open BMA180 Sensor on " << namebuf << " I2C Bus" << endl;
            return(1);
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){
            cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;
            return(2);
    }

    // need to set the ctrl_reg0 ee_w bit. With that set the image registers change properly.
    // need to do this or can't write to 20H ... 3Bh
    // Very Important... wrote a 0x10 to 0x0D and it worked!!!
    //   char buf[2];
    //     buf[0] = BANDWIDTH;
    //     buf[1] = 0x28;
    //     buf[2] = 0x65;
    //  if ( write(file,buf,2) != 2) {
    //	  cout << "Failure to write values to I2C Device " << endl;
    //  }

    char buffer[2];
    	buffer[0] = address;
    	buffer[1] = value;
    if ( write(file, buffer, 2) != 2) {
        cout << "Failure to write values to I2C Device address." << endl;
        return(3);
    }
    close(file);
    cout << "Finished BMA180 I2C sensor state write" << endl;
    return 0;
}

/* TRASHED FUNCTION
char BMA180Accelerometer::readI2CDeviceByte(char address){

  //  cout << "Starting BMA180 I2C sensor state byte read" << endl;
    char namebuf[MAX_BUS];
   	snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2CBus);
    int file;
    if ((file = open(namebuf, O_RDWR)) < 0){
            cout << "Failed to open BMA180 Sensor on " << namebuf << " I2C Bus" << endl;
            return(1);
    }
    if (ioctl(file, I2C_SLAVE, I2CAddress) < 0){
            cout << "I2C_SLAVE address " << I2CAddress << " failed..." << endl;
            return(2);
    }

    // According to the BMA180 datasheet on page 59, you need to send the first address
    // in write mode and then a stop/start condition is issued. Data bytes are
    // transferred with automatic address increment.
    char buf[1] = { 0x00 };
    if(write(file, buf, 1) !=1){
    	cout << "Failed to Reset Address in readFullSensorState() " << endl;
    }

    char buffer[1];
    	buffer[0] = address;
    if ( read(file, buffer, 2) != 2) {
        cout << "Failure to read value from I2C Device address." << endl;
    }
    close(file);
   // cout << "Finished BMA180 I2C sensor state read" << endl;
    return buffer[0];
}*/