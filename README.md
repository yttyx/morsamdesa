# morsamdesa

![](/images/samdesa.png)

morsamdesa is a program, written to run on a Raspberry Pi, that periodically retrieves RSS feeds from news websites, extracts the headlines from them, and renders them as Morse code.

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

![Sample configuration file](morsamdesa.conf).

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

Grant access to the GPIO pins so that root privileges are not required by morsamdesa

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

Download the morsamdesa source zip file from this page. Extract the files.

    unzip -o morsamdesa.zip

Build

    make all

### Run morsamdesa

    chmod 755 run
    ./run

Use `alsamixer` to set the master volume

## Build photos

Located ![here](https://imgur.com/a/c2PNVbd).



