# DASH7 Python Support
A collection of Python modules, supporting the DASH7 Alliance Protocol in general,
and [OSS-7](https://github.com/MOSAIC-LoPoW/dash7-ap-open-source-stack) in particular.

## Introduction

This repository contains a collection of Python modules that can help when working with the Dash7 Alliance Wireless Sensor and Actuator Network Protocol.

## Installation

We are currently targeting python v2.7.
Use the following commands to get started:

```bash
$ git clone https://github.com/MOSAIC-LoPoW/pyd7a.git
$ cd pyd7a
$ sudo pip install -r requirements.txt
```

You can verify that the installation succeeded by running the unit tests:
```bash
$ make test
*** running all tests
.................................................................................................................................
----------------------------------------------------------------------
Ran 129 tests in 1.064s

OK
```
If all tests ran without any errors, you're good to go.

## Modules

### ALP Parser

A parser/generator for Application Layer Protocol commands. From the specification:

"_ALP is the D7A Data Elements API. It is a generic API, optimized for usage with the D7A Session Protocol. It can be encapsulated in any other communication protocol. ALP defines a standard method to manage the Data Elements by the Application.
Any application action, data exchange method or protocol is mapped into manipulation of D7A Data Elements and their properties by means of ALP Commands._"

### DLL Parser

A parser for D7AP frames as transmitted over the air.

### OSS-7 Serial console interface parser

A parser for frames used by the serial console interface by OSS-7 nodes

### OSS-7 Modem interface

Allows to use a serial connected OSS-7 node as a modem. By sending ALP commands you can access the node's filesystem, or use the node's DASH7 interface to access the filesystem of nodes in the network.

### OSS-7 Modem webgui

Enables you to query the serially attached node and the nodes in the environment through a webgui.

Start using:
```bash
$ PYTHONPATH=. python modem-webgui/modem-webgui.py -d /dev/ttyACM0
 * Restarting with stat
 * Debugger is active!
 * Debugger pin code: 327-070-848
(1929) wsgi starting up on http://0.0.0.0:5000
(1929) accepted ('127.0.0.1', 57740)
Sending command of size  17
> 41 54 24 44 c0 00 0a b4 68 01 00 00 08 01 02 00 0f
< c0 00 21 20 00 00 08 00 0b 57
< 00 00 09 14 18 20 02 00 0f 01 01 67 61 74 65 77 61 39 35 36 31 36 61 38 23 68
connected to /dev/ttyACM0, node UID 0xb570000091418 running D7AP v1.1, application "gatewa" with git sha1 95616a8
starting read thread
modem: 3191882256028696
```

And open the displayed link in a browser.
The GUI is still work in progress.
