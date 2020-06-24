//********************************************************************************
//*  Copyright: National ICT Australia,  2007 - 2010                             *
//*  Developed at the ATP lab, Networked Systems research theme                  *
//*  Author(s): Yuriy Tselishchev                                                *
//*  This file is distributed under the terms in the attached LICENSE file.      *
//*  If you do not find this file, copies can be found by writing to:            *
//*                                                                              *
//*      NICTA, Locked Bag 9013, Alexandria, NSW 1435, Australia                 *
//*      Attention:  License Inquiry.                                            *
//*                                                                              *
//*******************************************************************************/

package physicalProcess.vwcPhysicalProcess;

// The physical process module simulates a physical process that could be measured/sampled
// by a sensing device on the nodes. Different sensing devices (e.g temperature, pressure,
// light, acceleration) would be represented by distinct PhysicalProcess modules. A node
// simply ask the process for a sample which is returned as soon as it is calculated.

simple vwcPhysicalProcess like physicalProcess.iPhysicalProcess {
 parameters:
	bool collectTraceInfo = default (false);

			
	double vwc_t = default (1.0);		// in meters per second
	int randvwcin = default(1); 
	double vwc_seed = default(1);
	int scenario = default(46); // 44 for Ngaoundere scenario, 45 for varying vwc, 46 for no rain
	double sensingrate = default(60); // in seconds
    double days_1= default(60);    // For Ngaoundere scenario
    double days_2= default(90);    // For Ngaoundere scenario
    double days_3= default(240);    // For Ngaoundere scenario
    double days_4 = default(270) ;    // For Ngaoundere scenario
    double days_5 = default(300) ;    

 	
	string description = default ("Volumetric Water Content");

 gates:
	output toNode[];
	input fromNode[];
}
