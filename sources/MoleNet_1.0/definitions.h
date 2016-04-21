/******************************************************************************
 * MoleNet - A Wireless Underground Sensor Network
 *
 * Copyright (C) 2016, Communication Networks, University of Bremen, Germany
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>
 *
 * This file is part of MoleNet
 *
 ******************************************************************************/


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


