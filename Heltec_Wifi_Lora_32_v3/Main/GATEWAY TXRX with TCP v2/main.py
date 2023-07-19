print("reading main.py")

# imports
from lib.lib_other.utils import get_node_id
from lib.lib_other.board_config import nodes_config
from lib.lib_led.led_config import blink

from lib.lib_lora.datahandler import DataHandler
from lib.lib_lora.lora_config import *

from lib.lib_oled.oled_config import initialize_oled

from lib.lib_ap.ap_config import *

import _thread, time, gc ; gc.enable()

# oled initialization
oled = initialize_oled()

# node id
NODE_ID  = get_node_id()
NODE_NAME = nodes_config[NODE_ID]["name"]
oled.new_text("{}/{}".format(NODE_NAME, NODE_ID) , 0, 0)

# WiFi connection / socket initialization
try:
    ssid, password = "N10", "12345678"
    wifi, wifiifconfig = connect_to_wifi(ssid, password)

    wifi_socket_main, _ = start_socket_tcp(ip='', port=80) ; wifi_socket_main.settimeout(0.1)
except:
    wifi, wifiifconfig, wifi_socket_main = None, None, None

essid, epassword = "ESP32-AP", "123456789"
ap, apifconfig = start_ap(essid, epassword)

ap_socket, _ = start_socket_tcp(ip='192.168.4.1', port=1234) ; ap_socket.settimeout(0.1)

oled.fill_rect(0, 10, 128, 20, 0); oled.text(wifiifconfig if wifiifconfig != None else apifconfig, 0, 10); oled.show()

# lora initialization
lora = initialize_lora()
BLOCKING_TIMEOUT = 1000 # ms

# empty list for msg collection for data collection
msg_collection = []

# packet_payload parser
dh = DataHandler(b"01001001011011100110011100100000")

""" SERVER HANDLER THREAD """
alive = True

def server_fn():
    global wifi_socket_main, msg_collection, ssid, wifiifconfig
    global alive
    try:
        while alive:
            server_handler(wifi_socket=wifi_socket_main, ap_socket=ap_socket, oled=oled, _msg_collection=msg_collection, SSID=ssid, IP=(wifiifconfig if wifiifconfig is not None else apifconfig))
    except KeyboardInterrupt:
        alive = False
        _thread.exit()

_thread.start_new_thread(server_fn, ())

while True:
    """ LORA RECEPTION AND TRANSMISSION """

    # receive packet
    packet, _ = lora.recv(timeout_en=True, timeout_ms=BLOCKING_TIMEOUT)

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
            blink()
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

    """ UPDATE IFCONFIG IN OLED """

    oled.fill_rect(0, 10, 128, 20, 0); oled.text(wifiifconfig if wifiifconfig != None else apifconfig, 0, 10); oled.show()
    
    print(gc.mem_free())
    """ SERVER HANDLER """
    # server_handler(wifi_socket=wifi_socket_main, ap_socket=ap_socket, data = www.homepage(msg_collection, ssid, wifiifconfig if ifconfig != None else apifconfig), oled=oled, msg_collection=msg_collection)