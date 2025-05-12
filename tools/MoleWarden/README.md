MoleWarden - The MoleNet Maintanance Tool
=========================================

This tool is kind of proof of concept. It should automatize the repeating tasks
for

- Setting up and testing new devices
- Repeating testing existing devices (returned from test campaigns etc.)
- Install the MicroPython VM
- Keep track of the devices: When have they been tested last time?

Start the script
================

This script is tested using Debian Linux.

1) Create a venv: `python -m venv venv`
2) Activate the venv: `. ./venv/bin/activate`
3) Install the requirements: `pip install -r requirements.txt`
4) Run the script: `./run.py`

Update the requirements.txt
===========================

To update the [requirements.txt](requirements.txt), follow those steps:

1) Make sure pipreqs is installed: `pip install pipreqs`
2) Save the requirements: `pipreqs .`

Ideally, everything should be located in a `venv`, `pipenv` etc.

Authors
=======

- Jens Dede <jd@comnets.uni-bremen.de>, Sustainable Communication Networks,
  University of Bremen
