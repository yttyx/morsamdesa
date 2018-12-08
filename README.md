# morsamdesa

![](/images/samdesa.png)

morsamdesa is a application, designed to run on a Raspberry Pi, that periodically retrieves RSS feeds from news websites, extracts the headlines from them, and renders them as Morse code.

## Features

### Audio
* Carrier wave (CW) or telegraph sounder output
* CW timbre is configurable
* Background noise can be added to simulate atmospheric radio interference

### Visual
* LED output via GPIO
* When not sending Morse, periodically indicates whether audio output is enabled (double flash) or muted (single flash)

### Remote control
* Supports FLIRC remote control
* Mute / Unmute / Replay last message / Play next unplayed message

### RSS
* Multiple RSS feeds can be configured
* Filters can be used to suppress headlines which are not of interest

### MP3 file generation
* Takes a file containing a list of words and generates an MP3-format file with those words rendered in Morse

## Configuration

[Sample configuration file](morsamdesa.conf)

## Installation

This procedure assumes a base installation of Raspbian Stretch lite. It has only been tested on a Raspberry PI model B, rev 2.

### Install morsamdesa pre-requisites

Install libconfig

    sudo apt-get install libconfig++-dev

Install libcurl

    sudo apt-get install libcurl4-openssl-dev

Install pulseaudio

    sudo apt-get install pulseaudio
    sudo git apt-get install libpulse-dev

Install libxml

    sudo get-apt-get install libxml2-dev

Install lame

    sudo apt-get install libmp3lame-dev

Grant access to the GPIO pins so that morsamdesa can be run without root privilege

    sudo chown root.gpio /dev/mem
    sudo chmod g+rw /dev/mem
    sudo chmod g+rw /dev/memown root.gpio

Grant access to the FLIRC dongle (used for remote control)

Add user to plugdev group

    sudo usermod -a -G plugdev <username>

Create a file `/etc/udev/rules.d/51-flirc.rules` containing the single line:

    KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="plugdev"

Reboot. The permissions on /dev/hidraw* device should appear as follows:

    0 crw-rw-r-- 1 root plugdev ... hidraw0
    0 crw-rw-r-- 1 root plugdev ... hidraw1

Create a directory in which to build morsamdesa, and make it current

    mkdir ~/morsamdesa
    cd ~/morsamdesa

Download the morsamdesa source zip file from [this page](https://github.com/yttyx/morsamdesa) and extract the files

    unzip -o morsamdesa.zip

Build

    make all

### Run morsamdesa

    chmod 755 run
    ./run

Use `alsamixer` to set the master volume

## Build photos

Located [here](https://imgur.com/a/c2PNVbd)

## Hardware links

[Adafruit Mono 2.5W Class D Audio Amplifier - PAM8302](https://shop.pimoroni.com/products/adafruit-mono-2-5w-class-d-audio-amplifier-pam8302)

[FLIRC V2 - RPi USB XBMC IR Remote Receiver](https://www.modmypi.com/raspberry-pi/communication-1068/ir-infrared-1074/flirc-rpi-usb-xbmc-ir-remote-receiver)

[Mini 21 Button IR Remote Control](https://www.modmypi.com/raspberry-pi/communication-1068/ir-infrared-1074/flirc-xmbc-ir-remote-control)

[Edimax EW-7811Un N150 Wireless USB Adapter Nano](https://www.amazon.co.uk/dp/B003MTTJOY/ref=pe_3187911_189395841_TE_dp_2)

The TOSLink transmitter was purchased from RS Components

## Morse code resources

[Wikipedia](https://en.wikipedia.org/wiki/Morse_code)

[International Morse code standard](https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-E.pdf)

[Learn CW online](https://lcwo.net/)
