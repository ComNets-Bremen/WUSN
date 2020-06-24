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

#include "vwcPhysicalProcess.h"



Define_Module(vwcPhysicalProcess);

long int x=1296000;   // period/2 for Ngaoundere and varying rain scenario
long int y=0;


void vwcPhysicalProcess::initialize()
{


    randvwcin = par("randvwcin");
    vwc_seed=par("vwc_seed");
    scenario=par("scenario"); // Scenario selection
    sensingrate=par("sensingrate");
    days_1=par("days_1");    // For Ngaoundere scenario
    days_2=par("days_2");    // For Ngaoundere scenario
    days_3=par("days_3");    // For Ngaoundere scenario
    days_4=par("days_4");    // For Ngaoundere scenario
    days_5=par("days_5");    // For Ngaoundere scenario

}

void vwcPhysicalProcess::handleMessage(cMessage * msg)
{
    switch (msg->getKind()) {
    case PHYSICAL_PROCESS_SAMPLING: {
        PhysicalProcessMessage *phyMsg = check_and_cast < PhysicalProcessMessage * >(msg);
        // int nodeIndex = phyMsg->getSrcID();
        // int sensorIndex = phyMsg->getSensorIndex();

        // get the sensed value based on node location
        phyMsg->setValue(calculateScenario(phyMsg->getSrcID()));

        EV << "Volumetric water content check 1" << calculateScenario(phyMsg->getSrcID()) <<"\n";


        // Send reply back to the node who made the request
        send(phyMsg, "toNode", phyMsg->getSrcID());
        return;
    }



    default: {
        throw cRuntimeError(":\n Physical Process received message other than PHYSICAL_PROCESS_SAMPLING");
    }
    }
}







double vwcPhysicalProcess::calculateScenario(int node_id)
{


    double retVal =0.0f;



    switch (scenario)
    {
    case 44:{

        int period=2592000;
        double down_slope=.5*period;
        double up_slope=period-down_slope;

        double rand_vwc;
        double startingvalue=10.0;
        double peak=1000.0-startingvalue;
        double vwcvarmin,vwcvarmax;

        if(randvwcin==1)
        {
            vwcvarmin=-0.005;
            vwcvarmax=0.005;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);

        } else if(randvwcin==2)
        {
            vwcvarmin=-0.01;
            vwcvarmax=0.01;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==3)
        { vwcvarmin=-0.015;
        vwcvarmax=0.015;
        rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==4)
        {
            vwcvarmin=-0.02;
            vwcvarmax=0.02;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==5)
        {
            vwcvarmin=-0.025;
            vwcvarmax=0.025;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        }



        //retVal=startingvalue+(peak*(((x % period)<=up_slope)*(1-1/up_slope*(x % period))) + peak*((up_slope<(x % period))*(1/down_slope*((x % period)-up_slope))))+rand_vwc;



        if(simTime() > 0 && simTime() <= days_1 * 86400 )
        {
            retVal= .02+rand_vwc;    // No Rain during this period
        }
        else if(simTime() > days_1 * 86400 && simTime() <= days_2 * 86400)
        {

            retVal=startingvalue+(peak*(((x % period)<=up_slope)*(1-1/up_slope*(x % period))) + peak*((up_slope<(x % period))*(1/down_slope*((x % period)-up_slope))))+rand_vwc;

            x=x+sensingrate;       //  VWC increasing during this period


        }
        else if(simTime() > days_2 * 86400 && simTime() <= days_3 * 86400)
        {
            retVal= .15+rand_vwc;    //  VWC remains constant and above communication threshold during this period
        }
        else if(simTime() > days_3 * 86400 && simTime() <= days_4 * 86400)
        {

            retVal=startingvalue+(peak*(((x % period)<=up_slope)*(1-1/up_slope*(x % period))) + peak*((up_slope<(x % period))*(1/down_slope*((x % period)-up_slope))))+rand_vwc;

            x=x+sensingrate;        // VWC starts decreasing during this period
        }
        else if(simTime() > days_5 * 86400 && simTime() <= 31540000)
        {
            retVal= .02+rand_vwc;    // No Rain during this period
        }





        //     EV << "Volumetric water content" << retVal<< "Random number" <<rand_vwc<<"\n";

        x=x+sensingrate;


        //    return retVal;



    }


    case 45:{

        int period=2592000;   // one month (30x24x60x60 seconds) (VWC increase for first 15 days. then decreases for last 15 days)
        double down_slope=.5*period;
        double up_slope=period-down_slope;

        double rand_vwc;
        double startingvalue=10.0;                 // 1% volumetric water content
        double peak=1000.0-startingvalue;           // 100% volumetric water content
        double vwcvarmin,vwcvarmax;

        if(randvwcin==1)
        {
            vwcvarmin=-0.005;
            vwcvarmax=0.005;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);

        } else if(randvwcin==2)
        {
            vwcvarmin=-0.01;
            vwcvarmax=0.01;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==3)
        { vwcvarmin=-0.015;
        vwcvarmax=0.015;
        rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==4)
        {
            vwcvarmin=-0.02;
            vwcvarmax=0.02;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==5)
        {
            vwcvarmin=-0.025;
            vwcvarmax=0.025;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        }



        retVal=startingvalue+(peak*(((x % period)<=up_slope)*(1-1/up_slope*(x % period))) + peak*((up_slope<(x % period))*(1/down_slope*((x % period)-up_slope))))+rand_vwc;

        EV << "Volumetric water content" << retVal<< "Random number" <<rand_vwc<<"\n";

        x=x+sensingrate;


        return retVal;



    }


    case 46:{

        double rand_vwc;
        double vwcvarmin,vwcvarmax;

        if(randvwcin==1)
        {
            vwcvarmin=-0.005;
            vwcvarmax=0.005;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);

        } else if(randvwcin==2)
        {
            vwcvarmin=-0.01;
            vwcvarmax=0.01;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==3)
        { vwcvarmin=-0.015;
        vwcvarmax=0.015;
        rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==4)
        {
            vwcvarmin=-0.02;
            vwcvarmax=0.02;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        } else if(randvwcin==5)
        {
            vwcvarmin=-0.025;
            vwcvarmax=0.025;
            rand_vwc=uniform(vwcvarmin,vwcvarmax,vwc_seed);
        }



        // retVal=.02 + rand_vwc;  // 2% VWC
        retVal=5.0;



        x=x+sensingrate;


        // return retVal;



    }


    }



    EV << "Volumetric water content" << retVal <<"\n";
    return retVal;
}

double vwcPhysicalProcess::randnum (double a, double b)
{
    static std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution (a,b);
    return distribution(generator);
}

void vwcPhysicalProcess::finishSpecific()
{


}
