"""
Utilities for the wolf firmware

~Jens Dede <jd@comnets.uni-bremen.de>~
Under dev by KDU team
"""

import machine, ubinascii, uhashlib
from lib.board_config import nodes_config
from time import time

def get_node_id(hex=False):
    node_id = ubinascii.hexlify(uhashlib.sha1(machine.unique_id()).digest()).decode("utf-8")[-8:] # 4 bytes unsigned int
    if hex:
        return node_id
    else:
        return int(node_id, 16)

def get_this_config():
    myId = int(get_node_id())
    if myId in nodes_config:
        return nodes_config[myId]
    return None

def wait(previous_time, interval) -> bool:
    """ Returns True or False if the interval time has passed
    previous_time: the time which has passed and you want to be comparing to
    interval: the interval you want to be between now and previous time 
    """
    return time() - previous_time >= interval