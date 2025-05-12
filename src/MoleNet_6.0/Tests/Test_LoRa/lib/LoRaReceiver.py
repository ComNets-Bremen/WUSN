def receive(lora):
    print("LoRa Receiver")

    while True:
        if lora.received_packet():
            lora.blink_led()
            print('something here')
            payload = lora.read_payload()
            print(payload)
