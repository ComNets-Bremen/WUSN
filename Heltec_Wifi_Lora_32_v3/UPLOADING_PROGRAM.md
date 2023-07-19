# Uploading Program to ESP32-S3 using VSCode and PyMakr

To get started with uploading your program to the ESP32-S3 microcontroller using VSCode and the PyMakr extension, follow the steps below:

## Prerequisites

- Install Visual Studio Code (VSCode) from the [official website](https://code.visualstudio.com/download)

- Before flashing the firmware, you need to ensure that the necessary drivers for USB to UART communication are installed on your Windows system. You can download the drivers from the Silicon Labs website. [Download](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads).

## Setup

1. Install the PyMakr (Preview) extension in VSCode. You can find it in the VSCode Marketplace using the following link: [PyMakr (Preview) Extension](https://marketplace.visualstudio.com/items?itemName=pycom.pymakr-preview)

2. In the VSCode Explorer tab, locate the "PYMAKR: PROJECTS" section.

3. Click on "Create Project" or the "+" button and select (or create) a folder where you want to set up your project.

4. Provide a project name and choose the "Empty" template.

## Writing the Code

1. In the project folder, you will find two files: `boot.py` and `main.py`. The `boot.py` file contains code that executes before the `main.py` file, while the `main.py` file contains the main code for your program.

2. Write or paste your code into the `main.py` file.

3. If you need to use additional modules, place them in the same folder as your project.

## Uploading the Code

1. In the "PYMAKR: PROJECTS" tab, locate your project under the project name.

2. Click on "Add Devices" and select the device(s) you want to upload the code to. If your device is not listed, make sure it is connected and properly recognized by your computer.

3. After adding the device, use the device's additional drop-down menu ("...") and click on "Stop Script" to ensure the previous program execution is stopped.

4. To upload the code to the device, click on the "Upload" button (up arrow in a cloud) in the PyMakr toolbar.

5. Once the upload is complete, you can hard restart the device by using the device's additional drop-down menu ("...") and clicking on "Hard Reset". This will run the uploaded code on the device.

Note: Make sure to connect your ESP32-S3 device to your computer via USB before attempting the upload.


# Troubleshooting Upload Issues

If you encounter difficulties while uploading your code to the ESP32-S3 device using VSCode and PyMakr, you can try the following steps to resolve the issue:

1. If the upload process seems to be stuck or unresponsive, disconnect the device by clicking the "Unplug" button in the PyMakr toolbar.

2. Reconnect the device by clicking the "Lightning" button in the PyMakr toolbar. This will establish a fresh connection between VSCode and the device.

3. Use the device's additional drop-down menu ("...") and click on "Stop Script" to ensure any previously running code is stopped before attempting the upload again.

4. If the upload still fails, try erasing the device's memory by using the "Erase" option in the device's additional drop-down menu and selecting "empty" as the project template. This will erase the existing program and any stored data on the device.

5. Once the device is erased, re-upload your code using the "Upload" button (up arrow in a cloud) in the PyMakr toolbar.

By following these steps, you can reset the connection, stop any existing program, erase the device's memory, and upload the code again to troubleshoot and resolve upload issues.