#define NODE_ID 2      
#define NEXTHOP_ID 254
#define SINK_ID 254
#define DELAY_BEFORE_SEND 6000
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
    // Alarm Interrupt Condition, wakes up all hardware components
    if( node.alarmInt ) {
        PRINT_SLEEP_MODE_DEACTIVATED
        currentTime = millis();   
        ifDelayBeforeSending = true;  // Activate SEC
        node.resetAlarm();            // Clear Alarm Interrupt 
    }

    // SEC: Sensor Excitation Condition
    if( (millis() - currentTime) > (node.config[StartupConfig::pos_DELAY_BEFORE_SEND]*1000 ) && ifDelayBeforeSending ) {
        ifDelayBeforeSending = false;         // Deactivate SEC
        ifSensorTimeout = true;               // Activate STC
                
        node.send_typ_RX_TX_INFORMATION(      // Send RF channel info. Packet
            node.config[StartupConfig::pos_GATEWAYID],
            node.config[StartupConfig::pos_SINKID] );        
    }
    
    // New Data received at RFM69 chip
    if( node.receiveDone() ) {        
        node.processReceivedData();   // Process received packet
    }    

    // STC: Sensor timeout Condition
    if( (millis() - currentTime) > DELAY_BEFORE_SLEEP && ifSensorTimeout) {
        ifSensorTimeout = false;                      // Deactivate STC
        node.sleepMode();                             // Go to sleep mode
    }    
}
