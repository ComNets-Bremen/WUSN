/****************************************************************************
 *  Copyright: National ICT Australia,  2007 - 2010                         *
 *  Developed at the ATP lab, Networked Systems research theme              *
 *  Author(s): Yuri Tselishchev                                             *
 *  This file is distributed under the terms in the attached LICENSE file.  *
 *  If you do not find this file, copies can be found by writing to:        *
 *                                                                          *
 *      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia             *
 *      Attention:  License Inquiry.                                        *
 *                                                                          *  
 ****************************************************************************/

#ifndef _VWCPHYSICALPROCESS_H_
#define _VWCPHYSICALPROCESS_H_

#define SIMTIME_STEP 0.01

#include "helpStructures/CastaliaModule.h"
#include "physicalProcess/PhysicalProcessMessage_m.h"

#include <random>

using namespace std;

typedef struct {
	simtime_t time;
	double x;
	double y;
} sourceSnapshot;

class vwcPhysicalProcess: public CastaliaModule {
 private:
	bool printDebugInfo;

	double vwc_t;
	int randvwcin;
	double vwc_seed;
	int scenario;

    double sensingrate;
    double days_1;    // For Ngaoundere scenario
    double days_2;    // For Ngaoundere scenario
    double days_3;    // For Ngaoundere scenario
    double days_4;    // For Ngaoundere scenario
    double days_5;    // For Ngaoundere scenario

	sourceSnapshot **sources_snapshots;	// N by M array, where N is numSources and, M is the 
										// maximum number of source snapshots. A source 
										// snapshot is a tuple (time, x, y, value)
	const char *description;

	double defaultValue;
	double *valuesTable;

 protected:
	virtual void initialize();
	virtual void handleMessage(cMessage * msg);
	virtual void finishSpecific();

	double calculateScenario(int node_id);
	double randnum (double a, double b);

};

#endif
