#define NODE_ID 254      
#define NEXTHOP_ID 254
#define SINK_ID 254
#define DELAY_BEFORE_SEND 2000
#define DELAY_BEFORE_SLEEP 30000  // 30 seconds

// Include framework header
#include "molenet_sensor_node.h"
MoleNet_Node node;
   
// Global Variables initialization
uint32_t currentTime;
bool ifDelayBeforeSending = false;
bool ifSensorTimeout = false;

void setup()
{  
    // Initialize Node
    node.begin();   //Serial initialized at 19200

    // Hard Coded Configuration
    node.config[StartupConfig::pos_CONTROL_1] = B00011111;
    node.config[StartupConfig::pos_NODEID] = NODE_ID;
    node.config[StartupConfig::pos_GATEWAYID] = NEXTHOP_ID;
    node.config[StartupConfig::pos_SINKID] = SINK_ID;    
    node.config[StartupConfig::pos_DELAY_BEFORE_SEND] = DELAY_BEFORE_SEND;
    node.config[StartupConfig::pos_DELAY_BEFORE_SLEEP] = 0;
    node.applyStartupConfig();  // Apply hard-coded configuration
    node.debugNodeInfo();    
}

void loop()
{   
    mainLoop();
    node.serialCmdLoop();
}

void mainLoop()
{
    // New Data received at RFM69 chip
    if( node.receiveDone() ) {        
        node.processReceivedData();   // Process received packet
    }    
}
