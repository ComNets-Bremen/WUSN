
import network, time, random, ufirebase

ap = network.WLAN(network.STA_IF)
ap.active(True)

while (not ap.isconnected()):
    ap.connect("PRIME-ROG-STRIX", "eiscreamburger")
    time.sleep(5)


print("Wi-Fi connected")
print("IP address:", ap.ifconfig()[0])

ufirebase.setURL("https://apptest2-81a0a-default-rtdb.europe-west1.firebasedatabase.app/")

# Example data
def data_to_firebase():
    data = random.randint(0, 100)
    print(data)
    return data

while True:
    ufirebase.addto("Data", data_to_firebase())
    print("Data sent to Firebase")
    time.sleep(5)
