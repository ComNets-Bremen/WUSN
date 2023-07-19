import network, socket, time
from . import webpages as www

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
    return ap, ap.ifconfig()[0]

def start_socket_tcp(ip: str = '', port: int = 80, max_connections: int = 5):
    """ Start a TCP socket on the port provided.
    returns the socket and the port
    ip: ip address to start the socket on
    port: port number to start the socket on
    max_connections: maximum number of connections to accept """

    sckt = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sckt.bind((ip, port))
    sckt.listen(max_connections)
    print('listening on', port)
    return sckt, port

def server_handler(wifi_socket: socket, ap_socket: socket, oled = None, _msg_collection : list = [[0, 0, 0, 0, 0, 0, 0]], SSID = None, IP=None):
    """ Send data to a socket
    wifi_socket: wifi socket to send data to
    ap_socket: access point socket to send data to
    data: data to send to the socket
    content_type: type of data to send to the socket 'text/csv', 'text/html' or 'text/plain' """

    global ssid, wifiifconfig, wifi_socket_main
    conn_list = []

    try:
        wifiifconfig = wifiifconfig
    except:
        wifiifconfig = '192.168.4.1'

    # connection from client
    try:
        wifi_conn, addr = wifi_socket.accept(); conn_list.append(wifi_conn)
        print('Got a connection from %s via wifi' % str(addr))
    except:
        pass

    try:
        ap_conn, addr = ap_socket.accept(); conn_list.append(ap_conn)
        print('Got a connection from %s via the local server' % str(addr))
    except:
        pass
    
    for conn in conn_list:
        if conn:
            request = conn.recv(1024)
            request = str(request)
            print('Content = %s' % request)

            # actions if wifi config page is requested

            if "/ " in request:
                data = www.homepage(message_collection=_msg_collection, SSID=SSID, IP=wifiifconfig)
                content_type = "text/html"

            """if "/wifi" in request:
                data = www.wifiPage()
                content_type = "text/html"

            if "wifi?ssid=" in request:
                ssid = request.split("?ssid=")[1].split("&")[0]
                password = request.split("&password=")[1].split(" ")[0]
                _ , wifiifconfig = connect_to_wifi(ssid, password)

                # restart socket
                wifi_socket_main = start_socket_tcp(ip=str(wifiifconfig))
                time.sleep(1)

                oled.fill_rect(0, 10, 128, 20, 0); oled.text(wifiifconfig, 0, 10); oled.show()

                data = www.processingPage() """

            if "/ml" in request:
                data = www.mlPage(_msg_collection)
                content_type = "text/html"

            # connection from client management

            try:
                conn.send('HTTP/1.1 200 OK\n')
                conn.send('Content-Type: {}\n'.format(content_type))
                conn.send('Connection: close\n\n')
                conn.sendall(data)
                conn.close()
            except:
                pass
    try:
        del request, data, conn_list
    except:
        pass