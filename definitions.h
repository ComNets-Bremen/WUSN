/*
Definitions for MoleNet program
*/

#define NETWORKID     	100  			//the same on all nodes in the network
#define GATEWAYID     	1         //ID of Gateway
#define FREQUENCY   		RF69_433MHZ //Frequency to be used for communication
#define ENCRYPTKEY    	"sampleEncryptKey"	//key for encryption
                                  //exactly the same 16 characters/
																	//bytes on all nodes!

#define	YYP		1   //position of year in payload
#define MM		3   //position of month in payload
#define DD		5   //position of day in payload
#define HPOS	8   //position of hour in payload
#define MPOS	10  //position of minute in payload
#define WPOS	13  //start position for VWC value in payload
#define TPOS	19  //start position for temperature value in payload
#define CHK		24  //position of checksum in payload
#define SPOS	26  //position of success indicator in payload


