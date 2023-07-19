import network, socket, time

def connect_to_wifi(ssid: str, password: str):
    """ Connect to a wifi network using the ssid and password provided
    ssid: wifi network name
    password: wifi network password """

    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    
    while not wlan.isconnected():
        wlan.connect(ssid, password)
        time.sleep(3)

    print("WIFI Connected : ", wlan.ifconfig()[0])
    return wlan, wlan.ifconfig()[0]

def start_ap(essid: str, password: str):
    """ Start an access point with the essid and password provided
    essid: access point name
    password: access point password """

    ap = network.WLAN(network.AP_IF)
    ap.active(True)
    ap.config(essid=essid, password=password)

    print("Access Point: ", ap.ifconfig()[0])
    return ap

def start_socket_tcp(port: int = 80, max_connections: int = 5):
    """ Start a TCP socket on the port provided.
    returns the socket and the port
    port: port number to start the socket on
    max_connections: maximum number of connections to accept """

    sckt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sckt.bind(('', port))
    sckt.listen(max_connections)
    print('listening on', port)
    return sckt, port

with open("lib/lib_ap/homepage.html", "r") as f:
    homepage = str(f.read())

def server_handler(socket: socket, data : str = homepage, content_type: str = "text/html"):
    """ Send data to a socket
    socket: socket to send data to
    data: data to send to the socket
    content_type: type of data to send to the socket 'text/csv', 'text/html' or 'text/plain' """

    conn, addr = socket.accept()
    print('Got a connection from %s' % str(addr))

    request = conn.recv(1024)
    request = str(request)
    print('Content = %s' % request)

    # led_on = request.find('/?led=on')
    # led_off = request.find('/?led=off')

    conn.send('HTTP/1.1 200 OK\n')
    conn.send('Content-Type: {}\n'.format(content_type))
    conn.send('Connection: close\n\n')
    conn.sendall(data)
    conn.close()