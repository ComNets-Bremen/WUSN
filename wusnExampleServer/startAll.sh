#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd $DIR
WEBSESSION_NAME="WebserverSession"
SERIAL_NAME="RECEIVER_SESSION"

echo "Existing sessions:"
screen -list

echo "Try to kill all sessions opened by this script..."
echo $WEBSESSION_NAME
screen -S $WEBSESSION_NAME -X quit

echo $SERIAL_NAME
screen -S $SERIAL_NAME -X quit


echo "Starting new sessions..."
echo $WEBSESSION_NAME
screen -d -m -S $WEBSESSION_NAME ./manage.py runserver 0.0.0.0:8080

echo $SERIAL_NAME
screen -d -m -S $SERIAL_NAME ./manage.py SerialListener /dev/ttyUSB0 115200

echo "done."
