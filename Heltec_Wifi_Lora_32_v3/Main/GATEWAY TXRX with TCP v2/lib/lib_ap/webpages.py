def homepage(message_collection: list = [[0, 0, 0, 0, 0, 0, 0]], SSID: str = "", IP: str = "" ):
    """ Generate the homepage with the message collection data
    message_collection: list of message data tuples (sender, receiver, seq, msg, retry_count, rssi, snr) """

    with open("lib/lib_ap/HTML/homepage.html", "r") as f:
        html_content = str(f.read())

    rows = ""
    for msg_data in message_collection:
        sender, receiver, seq, msg, retry_count, rssi, snr = msg_data
        index = message_collection.index(msg_data) + 1
        rows += f"""
            <tr>
                <td>{index}</td>
                <td>{sender}</td>
                <td>{receiver}</td>
                <td>{seq}</td>
                <td>{msg}</td>
                <td>{retry_count}</td>
                <td>{rssi}</td>
                <td>{snr}</td>
            </tr>
        """

    # Replace the placeholder in the HTML file with the generated table rows
    html_content = html_content.replace("$ROWS$", rows)
    html_content = html_content.replace("$SSID$", SSID)
    html_content = html_content.replace("$IP$", IP)

    return html_content

def wifiPage():
    """ Generate the wifi settings page """

    with open("lib/lib_ap/HTML/wifi.html", "r") as f:
        html_content = str(f.read())

    return html_content

def processingPage():
    """ Generate the processing page """

    with open("lib/lib_ap/HTML/processing.html", "r") as f:
        html_content = str(f.read())

    return html_content

def mlPage(message_collection: list = [[0, 0, 0, 0, 0, 0, 0]]):
    """ Machine Learner Model Page """

    with open("lib/lib_ap/HTML/ml.html", "r") as f:
        html_content = str(f.read())

    html_content = html_content.replace("$MSG_COLLECTION$", str(message_collection))

    return html_content