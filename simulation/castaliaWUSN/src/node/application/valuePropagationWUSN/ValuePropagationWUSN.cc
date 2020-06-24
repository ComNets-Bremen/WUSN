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

#include "node/application/valuePropagationWUSN/ValuePropagationWUSN.h"


Define_Module(ValuePropagationWUSN);

int recpktseq;
queue<double> sensorqueue;
queue<double> sensorqueue_pktloss;
vector<double> vwcchange;
int i,j;
double vwc_value=0;
int threshold_pkt_count;
double sensed_data_counter;
double change=0;
int retry_flag=0;
int send_buffer=0;
int buffer_flag=0;


void ValuePropagationWUSN::startup()
{

    dataSN = 1;
    totalpacketssent=0;
    pktloss=0;
    savedpackets=0;
    eeprom_w_power=1.1; // 1.1mW write power consumption (EEPROM M9M02-DR)
    eeprom_r_power=0.8;  // 0.8mW read operation power consumption
    i=0,j=0;

    selectmethod=par("selectmethod");
    sendingrate=par("sendingrate");




    if(selectmethod==2)
    {
        setTimer(SEND_BUFFER, sendingrate+.1);
    }

    setTimer(REQUEST_SAMPLE,   sendingrate);
    setTimer(SLEEP_NODE,  sendingrate+1);

}

void ValuePropagationWUSN::timerFiredCallback(int index)
{
    switch (index) {

    case REQUEST_SAMPLE:{
        if(getParentModule()->getIndex()!=0)
        {
            requestSensorReading();
            setTimer(REQUEST_SAMPLE,  sendingrate);
        }
        break;
    }


    case EMPTY_BUFFER:{

        if(sensorqueue.empty()!=1 && vwc_value < (sensethreshold))
        {
            //  ResourceManagerMessage *drawPowerMsgRead =
            //   new ResourceManagerMessage("EEPROM read power consumption message", RESOURCE_MANAGER_DRAW_POWER);

            //  drawPowerMsgRead->setPowerConsumed(eeprom_r_power);
            //  sendDirect(drawPowerMsgRead, resMgrModule, "powerConsumption");
            toNetworkLayer(createRadioCommand(SET_STATE, TX));
            toNetworkLayer(createRadioCommand(SET_TX_OUTPUT,10));
            EV << "Total Packet Sent " << totalpacketssent <<" Queue Size "<< sensorqueue.size() <<"\n";
            notifyWirelessChannelfromApp(vwc_value);
            toNetworkLayer(createGenericDataPacket(sensorqueue.front(), dataSN),"0");
            sensorqueue.pop();
            dataSN++;
            totalpacketssent++;

            buffer_flag=1;


            setTimer(BUFFER_SLEEP_NODE,  0.1+.01);


        }

        setTimer(EMPTY_BUFFER, .1);
        break;
    }

    case SEND_BUFFER:{

        if(sensorqueue.empty()!=1 && send_buffer==1)
        {
            //  ResourceManagerMessage *drawPowerMsg =
            //              new ResourceManagerMessage("Power consumption message", RESOURCE_MANAGER_DRAW_POWER);

            //  drawPowerMsg->setPowerConsumed(eeprom_r_power);
            // sendDirect(drawPowerMsg, resMgrModule, "powerConsumption");

            toNetworkLayer(createRadioCommand(SET_STATE, TX));
            toNetworkLayer(createRadioCommand(SET_TX_OUTPUT,10));
            notifyWirelessChannelfromApp(vwc_value);
            toNetworkLayer(createGenericDataPacket(sensorqueue.front(), dataSN),"0");
            // dataSN++;
            totalpacketssent++;


            if(recpktseq ==(dataSN-2) || recpktseq ==(dataSN-1))
            {

                //  dataSN++;
                counters=0;
                sensorqueue.pop();

            }
            else
            {
                counters=3;        // Retries
                // writelogfile("/home/idrees/Desktop/lostpkt",sensedValue ,1);
            }


            while(counters!=0 && !(recpktseq ==(dataSN-2) || recpktseq ==(dataSN-1)))
            {
                EV << " Retries Attempted " << "\n";
                toNetworkLayer(createRadioCommand(SET_STATE, TX));
                toNetworkLayer(createRadioCommand(SET_TX_OUTPUT,10));
                toNetworkLayer(createGenericDataPacket(sensorqueue.front(), dataSN),"0");
                totalpacketssent++;
                counters--;
                if(counters==0)
                {
                    sensorqueue.pop();
                    // dataSN++;
                    EV << " Packet Lost " << "\n";
                }



            }
        }
        break;
    }

    case BUFFER_SLEEP_NODE:{


        if(getParentModule()->getIndex()!=0)
        {
            toNetworkLayer(createRadioCommand(SET_STATE, SLEEP));
            EV << " Radio is put to sleep "  << "\n";
        }
        if(buffer_flag==1)
        {
            EV << " SLEEP NODE "  << "\n";
            setTimer(BUFFER_SLEEP_NODE,  0.1+.01);
            buffer_flag=0;
        }

        break;
    }


    case SLEEP_NODE:{

        if(getParentModule()->getIndex()!=0)
        {
            toNetworkLayer(createRadioCommand(SET_STATE, SLEEP));
            EV << " Radio is put to sleep "  << "\n";
        }

        EV << " SLEEP NODE "  << "\n";
        setTimer(SLEEP_NODE,  sendingrate);
        break;
    }

    }

}

void ValuePropagationWUSN::fromNetworkLayer(ApplicationPacket * rcvPacket, const char *source, double rssi, double lqi)
{

    double receivedData = rcvPacket->getData();
    recpktseq=rcvPacket->getSequenceNumber();


    EV << "The value: " << receivedData  << " RSSI: " << rssi << " Sender Packet Numeber " << dataSN << " Sequence Number " << recpktseq ;

    totalPackets++;



}

void ValuePropagationWUSN::handleSensorReading(SensorReadingMessage * rcvReading)
{

    double sensedValue = rcvReading->getSensedValue();


    EV << "VWC in app layer " << sensedValue <<"\n";

    sensed_data_counter=sensed_data_counter+1;

    // writelogfile("/home/idrees/Desktop/rssi",sensed_data_counter ,sensedValue,dataSN);

    switch (selectmethod)

    {

    case 1:



        //vwcchange.push_back(sensedValue);


        // EV << "Sent packet sequence numbers " << dataSN << " Received packet sequence numbers "  <<recpktseq << " Total Packets Sent " << totalpacketssent << " Rate of Change "<< change <<"\n";
        // EV << "Sensed VWC value "<< rcvReading->getSensedValue() << " threshold " << sensethreshold  << " Queue Size "<< sensorqueue.size() << " Packet Loss "<< pktloss <<" Threshold Packet count "<< threshold_pkt_count <<"\n";



        if(sensedValue <  sensethreshold && getParentModule()->getIndex()!=0 )
        {
            toNetworkLayer(createRadioCommand(SET_STATE, TX));
            toNetworkLayer(createRadioCommand(SET_TX_OUTPUT,10));
            notifyWirelessChannelfromApp(sensedValue);
            toNetworkLayer(createGenericDataPacket(sensedValue, dataSN),"0");
            totalpacketssent++;
            threshold_pkt_count++;

            if(sensorqueue.size()>5)
            {
                setTimer(EMPTY_BUFFER, .1);
            }

        }

        else if(sensedValue >=  sensethreshold)
        {


            sensorqueue.push(sensedValue);
            EV << "----------------------No Communication----------------- "<< sensedValue;
            toNetworkLayer(createRadioCommand(SET_STATE, SLEEP));
            pktloss=0;
            counters=0;
            savedpackets++;



        }


        if(pktloss>5)
        {
            sensethreshold=sensedValue;
        }



        if(recpktseq ==(dataSN-2) || recpktseq ==(dataSN-1))
        {

            dataSN++;
            counters=0;

        }
        else
        {
            counters=3;        // Retries
            // writelogfile("/home/idrees/Desktop/lostpkt",sensedValue ,1);
        }


        while(counters!=0 && !(recpktseq ==(dataSN-2) || recpktseq ==(dataSN-1)))
        {
            EV << " Retries Attempted " << "\n";
            toNetworkLayer(createRadioCommand(SET_STATE, TX));
            toNetworkLayer(createRadioCommand(SET_TX_OUTPUT,10));
            notifyWirelessChannelfromApp(sensedValue);
            toNetworkLayer(createGenericDataPacket(sensedValue, dataSN),"0");
            totalpacketssent++;
            counters--;
            if(counters==0)
            {
                sensorqueue.push(sensedValue);
                dataSN++;

                EV << " Last Retry " << "\n";
            }

        }



        break;
        //////////////////////////////////////////Without Channel Adaptation ////////////////////////////////////////////////////////////////////////

    case 2:

        if( getParentModule()->getIndex()!=0)
        {



            EV << "Sent packet sequence numbers " << dataSN << " Received packet sequence numbers "  <<recpktseq << " Total Packets Sent " << totalpacketssent  <<"\n";



            if(recpktseq ==(dataSN-2) || recpktseq ==(dataSN-1))
            {

                dataSN++;
                counters=0;

            }
            else
            {
                counters=3;        // Retries
                // writelogfile("/home/idrees/Desktop/lostpkt",sensedValue ,1);
            }


            while(counters!=0 && !(recpktseq ==(dataSN-2) || recpktseq ==(dataSN-1)))
            {
                EV << " Retries Attempted " << "\n";
                toNetworkLayer(createRadioCommand(SET_STATE, TX));
                toNetworkLayer(createRadioCommand(SET_TX_OUTPUT,10));
                notifyWirelessChannelfromApp(sensedValue);
                toNetworkLayer(createGenericDataPacket(sensedValue, dataSN),"0");
                totalpacketssent++;
                counters--;
                if(counters==0)
                {
                    sensorqueue.push(sensedValue);
                    dataSN++;



                    //    ResourceManagerMessage *drawPowerMsg =
                    //                new ResourceManagerMessage("Power consumption message", RESOURCE_MANAGER_DRAW_POWER);

                    //   drawPowerMsg->setPowerConsumed(eeprom_w_power);  // EEPROM read and write power 5mA at 3.3V
                    //   sendDirect(drawPowerMsg, resMgrModule, "powerConsumption");
                    EV << " Last Retry " << "\n";
                }
            }
        }




        break;

    default:
        EV << " No Method Selected " << "\n";

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    }

}




void ValuePropagationWUSN::writelogfile(string filename, double content, double content1, int content2)
{
    ofstream  out;
    out.open(filename.c_str(), ios_base::app);
    out << content << " " << content1 << " " << content2 <<"\n";
    out.close();
}

void ValuePropagationWUSN::finishSpecific()
{



    declareOutput("app packets created");
    collectOutput("app packets created", "", dataSN);

    declareOutput("app packets sent");
    collectOutput("app packets sent", "", totalpacketssent);

    declareOutput("app packets received");
    collectOutput("app packets received", "", totalPackets);



}


