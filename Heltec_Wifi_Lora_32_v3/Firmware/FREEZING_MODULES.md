# Freezing Modules in ESP32-S3 Firmware

To achieve better memory efficiency, you can rebuild the firmware for the ESP32-S3 microcontroller with necessary modules frozen in. Freezing modules involves pre-processing Python modules so that they are included directly in the MicroPython firmware, resulting in reduced memory usage and faster loading and interpretation by MicroPython. 

Please note that the following instructions are specifically for Ubuntu-based Linux systems.

## Installing ESP-IDF

1. Open a terminal and execute the following command to install the required dependencies:
   ```
   sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 -y
   ```

2. Create a directory named `esp` in your home directory and navigate into it:
   ```
   mkdir -p ~/esp
   cd ~/esp
   ```

3. Clone the ESP-IDF repository recursively using the following command:
   ```
   git clone --recursive https://github.com/espressif/esp-idf.git
   ```

4. Navigate into the `esp-idf` directory:
   ```
   cd ~/esp/esp-idf
   ```

5. Run the installation script:
   ```
   ./install.sh
   ```

6. Source the environment variables by executing the following command:
   ```
   . ./export.sh
   ```
   Note: You might need to **run only this command every time you open a new terminal session** after the first installation of ESP-IDF.

## Building new firmware

1. Clone the MicroPython repository recursively:
   ```
   cd ~
   git clone --recursive https://github.com/micropython/micropython.git
   ```

2. Copy the Python modules you want to freeze into the `~/micropython/ports/esp32/modules` directory.

3. Navigate to the `mpy-cross` directory:
   ```
   cd ~/micropython/mpy-cross
   ```

4. Clean the previous build artifacts:
   ```
   make clean
   ```

5. Build the mpy-cross tool:
   ```
   make
   ```

6. Navigate to the `esp32` directory within the MicroPython repository:
   ```
   cd ~/micropython/ports/esp32
   ```

7. Clean the previous build artifacts:
   ```
   make BOARD=GENERIC_S3 clean
   ```

8. Update the submodules:
   ```
   make BOARD=GENERIC_S3 submodules
   ```

9. Build the firmware for the ESP32-S3 with frozen modules:
   ```
   make BOARD=GENERIC_S3
   ```

10. After a successful build, you can find the new `firmware.bin` file in the `~/micropython/ports/esp32/GENERIC_S3` directory.

## Usage

You can flash the newly built firmware onto your ESP32-S3 board using the appropriate flashing tool, following the manufacturer's instructions.

Please refer to the MicroPython documentation for further details on using the frozen modules in your MicroPython applications.

## Additional

[Installing ESP-IDF on Linux and MAC](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html)