# morsamdesa

![](/images/samdesa.png)

morsamdesa is a application, written to run on a Raspberry Pi, that periodically retrieves RSS feeds from news websites, and renders the headlines in Morse code.

## Features

### Audio
* International and American (Railroad) Morse
* Continuous wave (CW): Configurable fundamental and 3 harmonics
* Sparkgap: sample-based, 16 variants
* Telegraph sounder: sample-based, 9 variants
* Double plate sounder: sample-based, 1 variant
* Background noise
* Separate output levels for each audio source 

### Visual
* LED output via GPIO
* Shows whether audio output is muted, when not sending Morse

### Morse command input - light sensitive input, single-letter command
* Mute / Unmute
* Replay last message
* Play next message
* Interrupt current message
* Enable/disable sending of headline mnenonic at transmission start

### RSS support
* Multiple RSS feeds
* Filters can be used to suppress headlines
* A 3 character mnenonic associated with a feed is used to select an audio output, so each RSS source can have a distinctive sound

### Miscellaneous
* MP3 file generation
 
## Installation

This procedure assumes a base installation of Raspbian Stretch Lite (image date 13-Nov-2018). It has been tested only on a Raspberry Pi model B, rev 2.

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

Use `alsamixer` to set the master volume.

## Build photos

Located [here](https://imgur.com/a/c2PNVbd)

## Demonstration sound files

Examples of morsamedesa output in mp3 format. The text is each case is "hello world". For CW output, the character speed is set to 26 WPM; with Farnsworth spacing the effective speed is 16 WPM. For sounder output, the character speed is 12.5 WPM; with Farnsworth spacing the effective speed is 8.2 WPM. 

The demo configuration file used to generate the mp3s is [here](demo.conf).


| Description                                                   | Command                                                                       | Sound File                               |
| :---                                                          | :---                                                                          | :---                                     |
| CW                                                            | `./bin/morsamdesa -c demo.conf -p mp3`                                        | [mp3](demo/cw.mp3)                       |
| CW with harmonics                                             | `./bin/morsamdesa -c demo.conf -p cw_harmonics -p mp3`                        | [mp3](demo/cw_harmonics.mp3)             |
| CW without Farnsworth spacing                                 | `./bin/morsamdesa -c demo.conf -p cw_no_farnsworth -p mp3`                    | [mp3](demo/cw_no_farnsworth.mp3)         |
| CW with a noise 'click' at the start and end of each element  | `./bin/morsamdesa -c demo.conf -p cw_sound_burst -p mp3`                      | [mp3](demo/cw_noise_burst.mp3)           |
| CW with sampled atmospheric noise                             | `./bin/morsamdesa -c demo.conf -p noisy -p mp3`                               | [mp3](demo/cw_noisy.mp3)                 |
| Western Electric telegraph sounder                            | `./bin/morsamdesa -c demo.conf -p sounder_we -p mp3`                          | [mp3](demo/sounder_we.mp3)               |
| Western Electric telegraph sounder without Farnsworth spacing | `./bin/morsamdesa -c demo.conf -p sounder_we -p sounder_no_farnsworth -p mp3` | [mp3](demo/sounder_we_no_farnsworth.mp3) |

## Hardware links

[Adafruit Mono 2.5W Class D Audio Amplifier - PAM8302](https://shop.pimoroni.com/products/adafruit-mono-2-5w-class-d-audio-amplifier-pam8302)

[Edimax EW-7811Un N150 Wireless USB Adapter Nano](https://www.amazon.co.uk/dp/B003MTTJOY/ref=pe_3187911_189395841_TE_dp_2)

The TOSLink transmitter was purchased from RS Components

## Morse code resources

[Wikipedia](https://en.wikipedia.org/wiki/Morse_code)

[International Morse code standard](https://www.itu.int/dms_pubrec/itu-r/rec/m/R-REC-M.1677-1-200910-I!!PDF-E.pdf)

[Learn CW online](https://lcwo.net/)

[A standard for timing using the Farnsworth technique](http://www.arrl.org/files/file/Technology/x9004008.pdf)

