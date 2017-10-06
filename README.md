# MTP Group B

Codebase for the MTP subject in Fall 2017. The goal is to successfully transmit and receive a 1MB file with two battery-powered independent transceivers.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

We need git and cmake:
```
$ sudo apt install git cmake 
```
And then, we need to enable the SPI kernel module for the raspberry:
```
$ sudo raspi-config
```
Under **Advanced** > **Enable SPI Kernel Module**.

### Installing

The main requisite is the [RF24](https://tmrh20.github.io/RF24/) library.
We'll start by installing the BCM2835 library (GPIO):
```
$ wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.52.tar.gz
$ tar xf bcm2835-1.52.tar.gz
$ cd bcm2835-1.52
$ ./configure
$ make
$ sudo make check
$ sudo make install
```

Then, we need the MRAA library: 

```
$ cd ~/
$ git clone https://github.com/intel-iot-devkit/mraa.git
$ cd mraa
$ mkdir build
$ cd build
$ cmake .. -DBUILDSWIGNODE=OFF
$ sudo make install
```
To finish the MRAA installation, we have to edit the following file:
```
$ nano /etc/ld.so.conf 
```
and add this at the end:
```
/usr/local/lib/arm-linux-gnueabihf 
``` 
We apply the changes with
```
$ sudo ldconfig
```
Finally, we have to install RF24:
```
$ cd ~/
$ git clone https://github.com/nRF24/RF24.git
$ cd RF24/
$ ./configure --driver=MRAA
$ sudo make install -B
```
And we're done!

## Compiling code

To compile including this library, with your file.cpp in the same folder as the RF24 folder:
```
$ g++ -Wall -L RF24/ -lrf24 file.cpp -o output
```
Remember that all RF24 code needs to be run as root:
```
sudo ./output
```

## Raspbian image
You can also find a Raspbian image with everything already prepared in the #software channel in [Slack](https://mtp-project.slack.com).
