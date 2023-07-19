from machine import Pin
import lib.lib_communication.packets as packets

heltecV3 = {
    'onboard_button': Pin(0, Pin.IN),
    'led': Pin(35, Pin.OUT),

    #display
    'disp_sda': Pin(17, Pin.OUT, Pin.PULL_UP),
    'disp_scl': Pin(18, Pin.OUT, Pin.PULL_UP),
    'i2c_rst': Pin(21, Pin.OUT),
    'disp_width': 128,
    'disp_height': 64,
    'shutdown_display_after': 120,

    #lora
    'lora_spi_bus': -1,
    'lora_clk': 9,
    'lora_mosi': 10,
    'lora_miso': 11,
    'lora_cs': 8,
    'lora_irq': 14,
    'lora_rst': 12,
    'lora_gpio': 13,

}

nodes_config = {
    3753907262 : { # Alpha
        "name": "alpha",
        "is_gateway" : True,
        "ldr" : False
    },

    2637953060 : { # Beta
        "name": "beta",
        "is_gateway" : False,
        "ldr" : True
    },

    4276759790 : { # Delta
        "name": "delta",
        "is_gateway" : False,
        "ldr" : False
    },
    1119174399 : { # Gamma
        "name": "gamma",
        "is_gateway" : False,
        "ldr" : False,
        "temp" : True
    }
}