print("running main.py")

# imports
from lib.lib_other.utils import get_node_id, wait
from lib.lib_other.board_config import nodes_config

from lib.lib_lora.datahandler import DataHandler
from lib.lib_lora.lora_config import *

from lib.lib_oled.oled_config import initialize_oled

import time, random

# oled initialization
oled = initialize_oled()

# node id
NODE_ID  = get_node_id()
NODE_NAME = nodes_config[NODE_ID]["name"]
oled.new_text("{}/{}".format(NODE_NAME, NODE_ID) , 0, 0)
oled.text(str(0 if NODE_ID == 3753907262 else (1 if NODE_ID == 2637953060 else (2 if NODE_ID == 4276759790 else (3 if NODE_ID == 1119174399 else None)))), 0, 10); oled.show()

# lora initialization
lora = initialize_lora()
BLOCKING_TIMEOUT = 4000 # ms

# packet_payload parser
dh = DataHandler(b"01001001011011100110011100100000")

# gateway id
RECEIVER_NODE_ID = 3753907262 #alpha

# interval between sending packets
msgSentAt, reqAckSince, SENDING_INTERVAL, RETRY_INTERVAL = random.choice([0, 15, 30, 45]), 0, 15, 5 # seconds

# packet sequence number, retry counter
seq, retry_counter, MAX_RETRY = 0, 0, 10

# sent msg collection
msg_collection = []

# ack
waiting_for_ack = False

# main loop
while True:
    # send packet
    if not waiting_for_ack and wait(msgSentAt, SENDING_INTERVAL):
        # update variables
        seq, retry_counter = seq + 1, 0

        # generate random number
        messege = random.randint(0, 100)

        # create packet
        payload = dh.EncMsg(messege, RECEIVER_NODE_ID, seq)

        # add packet to msg_collection
        msg_collection.append(msgFN3(NODE_ID, RECEIVER_NODE_ID, seq, messege))

        # send packet
        lora.send(payload)

        print("{}. Sent {}".format(seq, messege))
        oled.fill_rect(0,30, 128, 40, 0); oled.text("{}. Sent {}".format(seq, messege), 0, 30); oled.show()

        waiting_for_ack, msgSentAt, reqAckSince = True, time.time(), time.time()

        #debug
        print("[sender    , receiver  ,seq,msg, retry_count]")
        for eachSublist in msg_collection:
            print(eachSublist)

    # receive packet
    packet, err = lora.recv(timeout_en=True, timeout_ms=BLOCKING_TIMEOUT)

    # parse packet
    try:
        parsedPacket = dh.receiverEncPacket(bytearray(packet)) #parsing raw msg into readable information
    except:
        parsedPacket = None

    # check if packet is an ack for us
    if parsedPacket is not None:
        if parsedPacket.get_is_ack():
            if parsedPacket.get_receiver() == NODE_ID:
                if parsedPacket.get_sequence() == seq:
                    print("{}. Received ack for the msg {}".format(seq, messege))
                    oled.fill_rect(0,30, 128, 40, 0); oled.text("{}. Received ack for the msg {}".format(seq, messege), 0, 30); oled.show()
                    
                    waiting_for_ack = False

    # if no ack received, resend packet
    if waiting_for_ack and wait(reqAckSince, RETRY_INTERVAL) :
        if retry_counter < MAX_RETRY:
            payload = dh.EncMsg(messege, RECEIVER_NODE_ID, seq, retry_counter)
            lora.send(payload)

            reqAckSince, retry_counter = time.time(), retry_counter + 1

            print("{}. Resent {}, ({})".format(seq, messege, retry_counter))
            oled.fill_rect(0,30, 128, 40, 0); oled.text("{}. Resent {}, {}".format(seq, messege, retry_counter), 0, 30); oled.show()
        
        # if no ack received after 10 retries, send next packet
        else:
            print("{}. No ack received for the msg {}".format(seq, messege))
            oled.fill_rect(0,30, 128, 40, 0); oled.text("{}. No ack received for the msg {}".format(seq, messege), 0, 30); oled.show()
            
            waiting_for_ack = False

        #update msg_collection
        msg_collection[-1][4] = retry_counter

        #debug
        print("[sender    , receiver  ,seq,msg, retry_count]")
        for eachSublist in msg_collection:
            print(eachSublist)