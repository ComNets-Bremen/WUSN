""" Wolf application by Jens Dede
Under development by KDU group 2023"""

print("reading main.py")

from lib.lib_communication.lora_config import initialize_lora
from lib.lib_oled.oled_config import initalize_oled, display_msg

from sys import exit
import time, random

from lib.lib_communication.utils import get_node_id, get_this_config, wait
import lib.lib_communication.datahandler as datahandler, lib.lib_communication.ufirebase as ufirebase

from lib.lib_led.led import blink
from lib.board_config import nodes_config

#initialize oled
oled = initalize_oled() ; blink() ; oled.new_text("oled initailized", 0, 0)

#initialize lora
lora = initialize_lora()
if lora is None:
    oled.new_text("lora initailization failed", 0, 0)
    exit()

#ENCRYPTION KEY
aeskey = b"01001001011011100110011100100000"
dh = datahandler.DataHandler(aeskey)

#set the NODE configuration
NODE_ID = get_node_id()
nodeCfg = get_this_config()
if nodeCfg is None:
    oled.new_text("Node not configured", 0, 0)
    exit()

oled.fill(0) #display node info on screen
try: #Sniffer?
    IS_GATEWAY = nodeCfg["is_gateway"]
    if IS_GATEWAY:
        oled.text("Receiving msgs", 0, 10)
except:
    IS_GATEWAY = False

if not IS_GATEWAY:
    oled.text("Random Numbers", 0, 10)

(oled.text(nodeCfg['name'] + (' | gateway' if IS_GATEWAY else ''), 0, 0))#; oled.text(str(get_node_id()), 0, 10)
oled.show()

#sending - variables
num_pkg_sent = 0
payload_sent_at = time.time() #Number of Packages sent for now
msg_send_wait_interval = random.randint(1, 2) #seconds
RECEIVER_NODE_ID = 3753907262 #alpha

#receiving - variables
num_received_pkg = msg_received_at = 0 #Number of Packets Received
received_msg = packet = msg_sequence = None
MSG_RECEIVE_WAIT_INTERVAL = 0 #seconds
BLOCKING_TIMEOUT = 1000 #ms
i_got_msgs_from = [] #list of msgs received from a node
sequence_collection = {} #list of msgs received from a node

#acknowledgement - variables
waiting_for_ack = False #waiting for ack from receiver
no_ack_receive_interval = 1 #seconds
old_ack = []

#retry count
retry = 0


while True:

    """ SENDING MSGS """
    if wait(payload_sent_at, msg_send_wait_interval) and not waiting_for_ack and not IS_GATEWAY:
        msg = random.randint(1, 99)
        num_pkg_sent += 1
        seq = num_pkg_sent

        payload = dh.sendEncMsg(msg, RECEIVER_NODE_ID, seq) #to alpha
        lora.send(payload)
        waiting_for_ack = True

        [blink() for i in range(0,2)]

        display_msg("send", num_pkg_sent, msg, RECEIVER_NODE_ID, oled, nodes_config)
        print("{}. sent \"{}\" to {}".format(num_pkg_sent, msg, nodes_config[RECEIVER_NODE_ID]['name']))
    
        payload_sent_at = time.time()

    if waiting_for_ack and wait(payload_sent_at, no_ack_receive_interval):
        retry += 1
        print(retry)
        payload = dh.sendEncMsg(msg, RECEIVER_NODE_ID, seq, retry_count=retry) #to alpha
        lora.send(payload)

        print("{}. Resent \"{}\" to {}".format(num_pkg_sent, msg, nodes_config[RECEIVER_NODE_ID]['name']))
        
        payload_sent_at = time.time()


    """ 
    /\_____/\
   /  o   o  \
  ( ==  ^  == )
   )         (          KATZE
  (           )
 ( (  )   (  ) )
(__(__)___(__)__)

        """
    
    """ RECEIVING MSGS """

    if wait(msg_received_at, MSG_RECEIVE_WAIT_INTERVAL):
        received_msg, err = lora.recv(timeout_en=True, timeout_ms=BLOCKING_TIMEOUT) #receiving raw msg

        try:
            if received_msg is not (b''):
                packet = dh.receiverEncPacket(bytearray(received_msg)) #parsing raw msg into readable information
                [blink() for i in range(0,1)]
        except ValueError or AttributeError:
            pass
        except Exception as e:
            print("Error while parcing received msg into a packet", Exception, e)
            pass


        try:
            if packet is not None and not (b''):
                #identifying packet info
                parsed_packet_info = {"receiver": packet.get_receiver(),
                                    "sender": packet.get_sender(),
                                    "sequence": packet.get_sequence(),
                                    "is_ack": packet.get_is_ack(),
                                    "value" : None,
                                    "retry_count": packet.get_retry_count(),
                                    "rssi" : None,
                                    "snr" : None
                                    }

                #getting an acknowledgement
                if parsed_packet_info["receiver"] == NODE_ID and parsed_packet_info["is_ack"] \
                        and msg is not None and parsed_packet_info["sequence"] == seq: #receiving acknowledgement

                    waiting_for_ack = False
                    retry = 0
                    print("ACK received from {} for the msg {}".format(parsed_packet_info["sender"], parsed_packet_info["sequence"]))
                    display_msg("send", num_pkg_sent, msg, RECEIVER_NODE_ID, oled, nodes_config, is_ack=True)

                    # time.sleep(random.randint(0, 10)) #wait a random amount of time before sending the next msg
                    ack = packet = received_msg = payload = msg = seq = payload_recovery = None

                #getting a message
                if not parsed_packet_info["is_ack"]: #if not acknowledgement
                    parsed_packet_info["value"] = packet.get_params() #add value
                else:
                    parsed_packet_info["value"] = None

                if parsed_packet_info["sender"] not in i_got_msgs_from: #if node not in list
                    i_got_msgs_from.append(int(parsed_packet_info["sender"]))
                    sequence_collection[i_got_msgs_from[-1]] = []

                if IS_GATEWAY: #run only in gateway nodes

                    ack = packet.create_ack() #send acknowledgement
                    if ack:
                        lora.send(dh.encrypt(ack))
                        print("Sent ACK to {} for the msg {} in the sequence {}".format(parsed_packet_info["sender"], parsed_packet_info["value"], parsed_packet_info["sequence"]))
                        this_ack = [parsed_packet_info["sender"], parsed_packet_info["value"], parsed_packet_info["sequence"]]

                        if this_ack == old_ack:
                            time.sleep(random.randint(0, 10)) #wait a random amount of time before sending the next msg
                        
                        old_ack = this_ack
                    try:
                        #index of the sender node in the dictionary
                        sender_index = i_got_msgs_from.index(parsed_packet_info["sender"])
                        #ID of the sender node
                        sender_ID = i_got_msgs_from[sender_index]
                    except:
                        pass

                    if parsed_packet_info["sequence"] not in sequence_collection[sender_ID]: #if msg not in list
                        sequence_collection[sender_ID].append(parsed_packet_info["sequence"])
                        if len(sequence_collection[sender_ID]) > 100:
                            sequence_collection[sender_ID].pop(0)
                        
                        packet.set_rssi(lora.getRSSI())
                        packet.set_snr(lora.getSNR())

                        parsed_packet_info["rssi"] = packet.get_rssi()
                        parsed_packet_info["snr"] = packet.get_snr()
                        
                        print("\n\n\nReceived:\n", packet, sep='')
                        num_received_pkg += 1

                        print("\nTotal received packets:", num_received_pkg)

                        msg = parsed_packet_info["value"]
                        display_msg("receive", num_received_pkg, msg, parsed_packet_info["sender"], 
                                    oled=oled, nodes_config=nodes_config)
                        print("{}. received msg \"{}\" from node {}".format(num_received_pkg, msg, parsed_packet_info["sender"]))

                        #send msg to firebase
                        ufirebase.setURL("https://wusn-test1-default-rtdb.europe-west1.firebasedatabase.app/")
                        ufirebase.addto("Data", parsed_packet_info)

        except Exception as e:
            print(Exception, e)
            pass

    
        msg_received_at = time.time()