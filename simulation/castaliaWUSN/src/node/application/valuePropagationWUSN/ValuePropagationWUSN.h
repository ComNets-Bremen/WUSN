/*******************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2011                            *
 *  Developed at the ATP lab, Networked Systems research theme                 *
 *  Author(s): Athanassios Boulis, Dimosthenis Pediaditakis, Yuriy Tselishchev *
 *  This file is distributed under the terms in the attached LICENSE file.     *
 *  If you do not find this file, copies can be found by writing to:           *
 *                                                                             *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                *
 *      Attention:  License Inquiry.                                           *
 *                                                                             *
 *******************************************************************************/

#ifndef _VALUEPROPAGATION_H_
#define _VALUEPROPAGATION_H_

#include "node/application/VirtualApplication.h"



using namespace std;

enum ValuePropagationTimers {
	REQUEST_SAMPLE = 1,
	EMPTY_BUFFER=2,
	SEND_PACKET=3,
	SLEEP_NODE=4,
	AWAKE_NODE=5,
	SEND_BUFFER=6,
BUFFER_SLEEP_NODE=7

};

class ValuePropagationWUSN: public VirtualApplication {
 private:

    int selectmethod;
    double randomBackoffIntervalFraction;
	int totalPackets;
	double currMaxReceivedValue;
	double currMaxSensedValue;
	int sentOnce;
	double theValue;
	double tempThreshold;
	int dataSN;
	int counters;
	int recipientId;
	int pktloss;
	int totalpacketssent;
	int savedpackets;
	int sendingrate;
	double sensethreshold;
	double eeprom_w_power;  // 1.1mW write power consumption (EEPROM M9M02-DR)
	double eeprom_r_power;  // 0.8mW read operation power consumption

    cMessage *event;




 protected:
	void startup();
	void finishSpecific();
	void fromNetworkLayer(ApplicationPacket *, const char *, double, double);
	void handleSensorReading(SensorReadingMessage *);
	void timerFiredCallback(int);
	void writelogfile(string filename, double content, double content1, int content2);
	void checkbufferdata();
	double channelestimate(double sensorvalue);
};

#endif				// _VALUEPROPAGATION_APPLICATIONMODULE_H_
