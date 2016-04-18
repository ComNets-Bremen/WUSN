WUSN-Example-Server
===================

Jens Dede <jd@comnets.uni-bremen.de>, ComNets University of Bremen, 2016


This is an example database server based on django. It performs the following
tasks:

- `./manage.py SerialListener /dev/ttyUSB0 115200 ` reads the data from the
  serial line and inserts it into the database
- `./manage.py runserver 0.0.0.0:8080` runs an example developing server to
  display and export these information

There is a lot of work to do...

For testing: Connect the WUSN-GW-Node to the computer and start the
`./startAll.sh` script. You should see all the data on <IP>:8080
