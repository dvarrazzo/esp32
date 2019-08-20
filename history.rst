Notes about hacking on ESP32
============================

2018-07-27 first setup
----------------------

Setting up everything on Ubuntu 16.04.

The hardware `I bought`__ is described as: "JZK ESP-32S ESP32 Development
Board 2.4GHz WiFi + Bluetooth Dual Mode Antenna Module with Ultra Low Power".

.. __: https://www.amazon.co.uk/gp/product/B071JR9WS9

Under the board there is "ESP32 DEVKITV1" written.

Following `the instruction`__ to set up the toolchain from scratch as opposite as
using binaries:

.. __: https://docs.espressif.com/projects/esp-idf/en/latest/get-started/linux-setup-scratch.html

For future use, defining the env vars exported in this snippet into a
``env.sh`` file.

::

    # To use the toolchain
    sudo apt-get install git wget make libncurses-dev flex bison gperf \
        python python-serial

    # To compile the toolchain
    sudo apt-get install gawk gperf grep gettext python python-dev \
        automake bison flex texinfo help2man libtool libtool-bin

    mkdir esp32
    cd esp32
    git clone -b xtensa-1.22.x https://github.com/espressif/crosstool-NG.git
    cd crosstool-NG
    ./bootstrap && ./configure --enable-local && make install
    ./ct-ng xtensa-esp32-elf
    ./ct-ng build       # will take some 15 mins

    # Will need the following to use it
    export PATH="$PATH:`pwd`/builds/xtensa-esp32-elf/bin/"

    cd ..

    # Get the esp-idf framework
    git clone --recursive https://github.com/espressif/esp-idf.git
    export IDF_PATH="`pwd`/esp-idf"

    # plug in the esp32: syslog should tell where it it connects:
    # e.g. /dev/ttyUSB0


Problems flashing
-----------------

Trying an example::

    cp -r  esp-idf/examples/get-started/hello_world/ .
    cd hello_world
    make menuconfig
    make flash

mmm... got communication errors here::

    A fatal error occurred: Failed to connect to ESP32: Invalid head of packet ('\x1b')

or::

    termios.error: (5, 'Input/output error')


But the board is there
----------------------

Looking around for docs__, it seems the board has Lua preinstalled.

.. __: https://startingelectronics.org/articles/ESP32-WROOM-testing/

So trying to reach it::

    sudo apt-get install minicom

Running minicom, pressing the EN button on the board, I receive something,
so it seems we are takling::

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    configsip: 0, SPIWP:0x00
    clk_drv:0x00,q_drv:0x00,d_drv:0x00,cs0_drv:0x00,hd_drv:0x00,wp_drv:0x00
    mode:DIO, clock div:2
    load:0x3fff0008,len:8
    load:0x3fff0010,len:3480
    load:0x40078000,len:7804
    ho 0 tail 12 room 4
    load:0x40080000,len:252
    entry 0x40080034
    I (45) boot: ESP-IDF v2.0-3-gbef9896 2nd stage bootloader
    I (46) boot: compile time 05:59:45
    [...]
    I (2139) wifi: mode : sta (b4:e6:2d:b3:cf:d9) + softAP (b4:e6:2d:b3:cf:da)
    I (2142) wifi: mode : softAP (b4:e6:2d:b3:cf:da)


Flash solution
--------------

Reading this__, got to press the BOOT button while flashing. Now it works as
expected::

    $ make flash
    Flashing binaries to serial port /dev/ttyUSB0 (app at offset 0x10000 )...
    esptool.py v2.3.1
    Connecting........_____.
    Chip is ESP32D0WDQ6 (revision (unknown 0xa))
    Features: WiFi, BT, Dual Core, VRef calibration in efuse
    Uploading stub...
    [...]

    Leaving...
    Hard resetting via RTS pin...

and then::

    $ make monitor
    MONITOR
    --- idf_monitor on /dev/ttyUSB0 115200 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ets Jun  8 2016 00:22:57

    rst:0x1 (POWERON_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
    [...]

    Hello world!
    This is ESP32 chip with 2 CPU cores, WiFi/BT/BLE, silicon revision 1, 4MB external flash
    Restarting in 10 seconds...
    Restarting in 9 seconds...
    Restarting in 8 seconds...
    [...]

.. __: https://esp32.com/viewtopic.php?t=3144

This was a forum from 2016, it says "It's a hardware bug (with Windows & some
dev boards), that we are hoping to fix in the next hardware revision.  In the
meantime, I've asked the documentation team to revise the Getting Started
Guide to mention this workaround."


Minicom notes
-------------

``sudo minicom -s`` will create the file::

    $ cat /etc/minicom/minirc.dfl
    # Machine-generated file - use "minicom -s" to change parameters.
    pu port             /dev/ttyUSB0
    pu rtscts           No

with my config in. (Note: before I had rtscts to Yes, and when micropython
worked in ``make monitor`` I couldn't use it in minicom)

``minicom`` will start communication. Ctrl-A to enter commands::

    |               Main Functions                  Other Functions     |
    | Dialing directory..D  run script (Go)....G | Clear Screen.......C |
    | Send files.........S  Receive files......R | cOnfigure Minicom..O |
    | comm Parameters....P  Add linefeed.......A | Suspend minicom....J |
    | Capture on/off.....L  Hangup.............H | eXit and reset.....X |
    | send break.........F  initialize Modem...M | Quit with no reset.Q |
    | Terminal settings..T  run Kermit.........K | Cursor key mode....I |
    | lineWrap on/off....W  local Echo on/off..E | Help screen........Z |
    | Paste file.........Y  Timestamp toggle...N | scroll Back........B |
    | Add Carriage Ret...U                                              |


Trying micropython
------------------

As there seems being no Lua, trying Python!

Following this__, downloaded micropython__.

.. __: https://boneskull.com/micropython-on-esp32-part-1/
.. __: https://micropython.org/download/#esp32

Installed with the following commands::

    python ${IDF_PATH}/components/esptool_py/esptool/esptool.py \
        --chip esp32 --port /dev/ttyUSB0 --baud 115200 erase_flash

    python ${IDF_PATH}/components/esptool_py/esptool/esptool.py \
        --chip esp32 --port /dev/ttyUSB0 --baud 115200 write_flash \
        -z 0x1000 ~/Downloads/esp32-20180727-v1.9.4-409-g434975def.bin

``make monitor`` works, got the prompt! :D

This turns on and off the blue pin on the board from Python::

    from machine import Pin
    pin = Pin(2, Pin.OUT)
    pin.value(1)
    pin.value(0)

It works, but there is no output: ``print()`` doesn't print a thing, as doesn't
stdout. Minicom works alright (after disabling rtscts).


Blink demo
----------

Copied from examples, like hello_world. No surprise, except needed to change
the led from 5 to 2.


Trying Arduino bridge
---------------------

Using Arduino Core can use the software component developed for Arduino

https://github.com/espressif/arduino-esp32

Interpreting the instructions to install it for the Arduino IDE::

    git clone --recursive https://github.com/espressif/arduino-esp32.git
    cd arduino-esp32/tools
    python get.py

then following the `building with make`__ link::

    git clone https://github.com/plerup/makeEspArduino.git
    cd makeEspArduino
    make -f makeEspArduino.mk CHIP=esp32 DEMO=1 ESP_ROOT=~/esp32/arduino-esp32/ flash

.. __: https://github.com/plerup/makeEspArduino

This things work, running ``minicom`` I can see the program output (the list of
wifi networks available... cool!)::

    scan start
    scan done
    10 networks found
    1: VM479591-2G (-57)*
    2: Virgin Media (-57)*
    [...]

Env vars ``CHIP`` and ``ESP_ROOT`` added to ``env.sh``, so no more needed in
the command line. Also added alias ``espmake`` to run it from a sketch dir.


Trying an Arduino demo
----------------------

Ok, but how about running an Arduino sketch on this thang? By now the arduino
package should be installed::

    mkdir blink-arduino
    cd blink-arduino
    cp /usr/share/arduino/examples/01.Basics/Blink/Blink.ino .
    # change the led to 2
    espmake flash

And it blinks!


2018-07-27 Backwards: Arduino as esp-idf component
--------------------------------------------------

.. __: https://github.com/espressif/arduino-esp32/blob/master/docs/esp-idf_component.md

.. _blink-arduino-esp:

Missed `this bit`__, but that's way more sensible, so let's rework the blink::

    cp -r esp-idf/examples/get-started/blink/ blink-arduino-esp
    cd blink-arduino-esp/
    mkdir components
    cd components
    ln -s ../../arduino-esp32/ arduino
    make menuconfig
    # 4 MB flash, disable several Arduino component don't care, led 2
    make flash

Good, so now I can use arduino components within the esp-idf framework.


Reading an analogue value
-------------------------

Reading analog value from a potentiometer, using Arduino into the esp-idf
framework. See the ``pot`` directory.

Project initialized like in `blink-arduino-esp`_, with some further
customizaton: setting up a C++ source file (or it's impossible to use the
serial), customising the config menu.

Analog value read from pin ``GPIO36`` (selectable from menu). The value print
on serial ranges from 0 to 4095.


Galvanic Stress Response
------------------------

Copying the ``pot`` directory into ``gsr``. Custom variable renamed. See
``diff -u {pot,gsr}/sdkconfig`` if there's any meaningful difference.

The `Grove GSR Sensor`__ can be attached like a potentiometer:

.. __: http://wiki.seeedstudio.com/Grove-GSR_Sensor/

- Black: to GND
- Red: to VDD
- Yellow: to GPIO36
- White: not connected

The Grove sensor has a 4 pins connector with a step that it's not compatible
with the breadboard, so I've cut off the connector from one side and made the
wires somewhat more rigid with some solderer.

Not wearing the sensor, plugging in the monitor. Got tripped by the analog
range of the esp32 being 4x Arduino's one. After the adjustment I got some
form of read, where I could relax up to 360-400, and could "stress" down below
300.


Plotting data
-------------

I've only been dumping number. Displaying them?

Starting from `this example`__, into the ``plot`` dir. Had to ``apt install
python-tk``.

.. __: http://www.toptechboy.com/tutorial/python-with-arduino-lesson-11-plotting-and-graphing-live-data-from-arduino-with-matplotlib/

Wait, first check this__

.. __: https://www.thepoorengineer.com/en/arduino-python-plot/

...or in general google "arduino python plot"...
