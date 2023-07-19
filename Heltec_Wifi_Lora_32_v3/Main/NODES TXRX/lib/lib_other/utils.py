import machine, ubinascii, uhashlib
from time import time

def get_node_id(hex=False):
    node_id = ubinascii.hexlify(uhashlib.sha1(machine.unique_id()).digest()).decode("utf-8")[-8:] # 4 bytes unsigned int
    if hex:
        return node_id
    else:
        return int(node_id, 16)

def wait(previous_time, interval) -> bool:
    """ Returns True or False if the interval time has passed
    previous_time: the time which has passed and you want to be comparing to
    interval: the interval you want to be between now and previous time 
    """
    return time() - previous_time >= interval

def csv_from_array(*args) -> str:
    """ Returns a string of comma seperated values from the array provided
    *args: should be in (header: str, array: list) format

    Example: csv_from_array(("header1", [1, 2, 3]), ("header2", [4, 5, 6]))
    """
    if len(args) % 2 != 0:
        raise ValueError("Arguments must be in (header: str, array: list) format")
    
    csv = ""

    column = 0
    row_lengths = []
    row_max_length = 0
    count_of_args = len(args)

    for header, array in args:
        csv += header
        if column < count_of_args - 1:
            csv += ","
        else:
            csv += "\n"
        column += 1

        row_lengths.append(len(array))

    row_max_length = max(row_lengths)

    for row in range(row_max_length):
        column = 0
        for header, array in args:
            if row < len(array):
                csv += str(array[row])
            if column < count_of_args - 1:
                csv += ","
            else:
                csv += "\n"
            column += 1

    return csv