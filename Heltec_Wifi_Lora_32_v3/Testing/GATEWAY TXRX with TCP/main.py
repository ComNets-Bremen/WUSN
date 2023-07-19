print("running main.py")

""" GATEWAY """

# imports
from lib.lib_other.utils import get_node_id
from lib.lib_other.board_config import nodes_config

from lib.lib_lora.datahandler import DataHandler
from lib.lib_lora.lora_config import *

from lib.lib_oled.oled_config import initialize_oled

from lib.lib_ap.ap_config import *

# oled initialization
oled = initialize_oled()

# node id
NODE_ID  = get_node_id()
NODE_NAME = nodes_config[NODE_ID]["name"]
oled.new_text("{}, {}".format(NODE_NAME, NODE_ID) , 0, 0)

# WiFi connection
wifi, ifconfig = connect_to_wifi("PRIME-ROG-STRIX", "eiscreamburger")
oled.fill_rect(0, 10, 128, 20, 0); oled.text(ifconfig, 0, 10); oled.show()

# Web Socket
sckt, port = start_socket_tcp()

# lora initialization
lora = initialize_lora()
BLOCKING_TIMEOUT = 1000 # ms

# empty list for msg collection for data collection
msg_collection = []

# packet_payload parser
dh = DataHandler(b"01001001011011100110011100100000")

# main loop
while True:

    """ LORA RECEPTION AND TRANSMISSION """

    # receive packet
    packet, err = lora.recv(timeout_en=True, timeout_ms=BLOCKING_TIMEOUT)

    # parse packet
    try:
        parsedPacket = dh.receiverEncPacket(bytearray(packet)) #parsing raw msg into readable information
    except:
        parsedPacket = None

    if parsedPacket:
        # add rssi and snr to parsedPacket
        parsedPacket.set_rssi(lora.getRSSI()) ; parsedPacket.set_snr(lora.getSNR())

        # add parsedPacket to msg_collection
        if not msgFN2(msg_collection, msgFN1(parsedPacket)):
            msg_collection.append(msgFN1(parsedPacket))

            # print parsedPacket
            print(parsedPacket)
            oled.fill_rect(0, 40, 128, 64, 0); oled.text("from: {}, {}".format(nodes_config[parsedPacket.get_sender()]['name'], parsedPacket.get_params()), 0, 40); oled.show()

        # send ack
        ack = parsedPacket.create_ack()
        if ack:
            lora.send(dh.encrypt(ack))
            print("ACK sent to node {}".format(parsedPacket.get_sender()))

        #debug
        print("[sender    , receiver  ,seq,msg, retry_count, rssi, snr]")
        for eachSublist in msg_collection:
            print(eachSublist)


    """ WEB SERVER """
