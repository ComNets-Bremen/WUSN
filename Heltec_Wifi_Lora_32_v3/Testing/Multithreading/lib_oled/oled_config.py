from lib_other.board_config import heltecV3
from machine import SoftI2C, reset
import ssd1306

def new_text(self, msg: str, x: int = 0, y: int = 0):
    """ Display text at once, skipping using fill(0), ... , show() """
    # Clear the display
    self.fill(0)

    # Try to decode the message as UTF-8 if it is bytes
    try:
        msg = msg.decode('utf-8')
    except:
        pass
    
    print(msg)
    # Display the message in chunks of 16 characters on different lines
    self.text(msg[:16], x, y)
    if len(msg) > 16-x:
        self.text(msg[16-x:], 0, y+10)
    elif len(msg) > 32-x:
        self.text(msg[32-x:], 0, y+20)
    elif len(msg) > 48-x:
        self.text(msg[48-x:], 0, y+30)
    else:
        self.text(msg[60-x:], 0, y+40)

    # Show the updated display
    self.show()

ssd1306.SSD1306.new_text = new_text

def initialize_oled():
    """ start the OLED display """
    
    heltecV3['i2c_rst'].value(0)
    heltecV3['i2c_rst'].value(1)

    try:
        i2c = SoftI2C(scl=heltecV3['disp_scl'], sda=heltecV3['disp_sda'])
        display = ssd1306.SSD1306_I2C(heltecV3['disp_width'], heltecV3['disp_height'], i2c)
    except OSError:
        reset()

    display.new_text("Initializing...", 0, 0)
    return display

def display_msg(send_or_receive: str, num: int, msg: int, to_from_node: int, oled, nodes_config: dict, is_ack=False, other=''):
    """ 
     ### \# | sent | to
     or
     ### \# | recv | from

     send_or_receive: specify 'send' or 'receive'
     int num: sent or received msgs count
     msg: the msg transmitting or receving
     to_from_node: node number you are receiving the msg from or sending to
     oled: oled driver defined, SSD1306
     nodes_config: configuration dictionary with node names
     is_ack: to display acknowledgement confirmation
    """
    
    variables = {
        "send" : {
            'send_recv' : "sent",
            'to_from' : "to"
        },
        "receive" : {
            'send_recv' : "recv",
            'to_from' : "from"
        }
    }

    x1 = 0
    x2 = 30
    x3 = 64
    x4 = 80

    oled.text("# |", x1, 20)
    oled.text(variables[send_or_receive]['send_recv'], x2, 20) ; oled.text("|", x3, 20)
    oled.text(variables[send_or_receive]['to_from'], x4, 20)

    def disp(y):
        oled.fill_rect(0,y, 128, y+5, 0)
        oled.text(str(num), x1, y) 
        oled.text(str(msg), x2, y)

        if not is_ack and other is (''):
            to_from = str(nodes_config[to_from_node]['name'])
        elif is_ack:
            to_from = "ACK"
        elif other is not (''):
            to_from = other
        oled.text(to_from, x4, y)
    
    if num % 3 == 1:
        disp(30)
    if num % 3 == 2:
        disp(40)
    if num % 3 == 0:
        disp(50)
    
    oled.show()