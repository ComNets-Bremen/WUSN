This manual worked on Manjaro Linux, no guarantee for other distros/operations systems.
# Installing Micropython on STM32 board

1. Install micropython unix from https://github.com/micropython/micropython
    1. Clone repository

        ```
        git clone https://github.com/micropython/micropython
        ```
    2. Build micropython

        ```
        $ cd ports/unix
        $ make submodules
        $ make
        ```
    3. check installation

        ```
        $ ./micropython
        ```
    4. Run complete testsuite

        ```
        $ make test
        ```
2. Install STM32 version
    1. Build:

        ```
        $ cd ports/stm32
        $ make submodules
        $ make
        ```
    2. Install pyusb if not done already

        ```
        $ pip3 install pyusb
        ```
    3. put board in DFU mode
        1. Either by running the command `machine.bootloader()` in REPL
        2. Or by pulling BOOT0 to 3.3V and reset the board
    4. Flash the code via USB DFU to your device

        ```
        $ make BOARD=NUCLEO_L073RZ deploy-stlink
        ```
    5. If 4. doesn’t work:
        1. install STM32CubeProgrammer
        2. press connect and go to Erasing & Programming then install /micropython/ports/stm32/build-NUCLEO_L073RZ/firmware.bin  which you built in step 4

# Running Code

1. Connect board to PC
2. Find the path the Board is connected to
    1. usually it is at ttyACM0
    2. sudo dmesg | grep tty often helps, the entire list of devices can be found using /sys/class/tty
3. For testing purposes you can install rshell
    1. run rshell -p /dev/ttyACM0 (adjust the path name accordingly)
    2. enter the command repl
    3. now you can run some commands for testing, for example:
        1. Hello World

            ```
            print(„Hello world“)
            ```
        2. Toggle LED:

            ```
            >>> import pyb
            >>> pyb.LED(1).on()
            >>> pyb.LED(1).off()
            ```
4. Now configure your IDE i.e. VS Code by installing the pymakr extension
    1. If you have troubles, try adding yourself to the dialout group

        ```
        sudo usermod -a -G dialout <your-username>
        ```
5. If 4 doesn’t work, files can be uploaded via rshell by using the command

    ```
    cp filename.py /pyboard
    ```


