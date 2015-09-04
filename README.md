AXON-UPS
========

## Introduction

AXON-UPS is a sigle-board-computer, which acts as a functional unit.
Its main purpose to provide intelligent UPS functionality to linux based single board computers, such as Raspberry Pi, Banana Pi, Orange Pi, Cubieboard, etc.

This device is a tiny member of the big family of intelligent UPSs that is a wide range of devices from simple desktop variants to the professional server UPSs.
It is developed as a thesis work, but the developer is willing to create a product, that is able to delivers real values to real customers. So the functionalities of the several releases of the product will be collected from potential users of it.
    
This document summerize the whole project, including the requirements specification, the development roadmap and some references to existing alternatives of the product and solutions might worth to study during the design phase.

## Roadmap

1. Definition of the thesis topic

    - Deadline: 01.09.2015

2. Phase I:

    - Scouting, reading literature, collecting ideas, existing solutions, etc.

    - Defining the features, release planning

    - Designing the device, start development:

        - schematics of the circuit
        - breadboard prototype
        - firmware/software
        - prototype testing

    - Deadline:
        dec 15

2. Phase II:
    - Finish development:

        - PCB layout

    - Manufacturing and assembling the PCBs (prototype)
    - Testing
    - Completed documentation, finished thesis work
    - Deliver the working v1.0 product
    - Deadline: június 30

## Next Actions

- Involving potential product end-users into the requirement specification
- Collecting ideas and required features

    - Collecting and studying Universal UPS features, functionalities
    - Study the unix upsd and similar tools

- Defining the scope for v1.0 product

## Requirements

### Mandatory

- AXON-UPS must provide the regulated (configurable) voltage and current to the connected devices:

    - Voltages:
        - 5 VDC
        - 3.3 VDC
    - Current: min. 3A

- Power Outage Detection and management:
    - In case of outage a controlled switch-off procedure must be executed successfully
    - In case of outage the UPS must be able to supply the host computer until that is successfully shut down.
    - Time and mode is configurable
    - Auto-shut down mode:
        - shut down immediately (timing is configurable),
        - keep-alive mode until there is enough power, then shut-down.

- Intentional switch on/off process management:

    - Execute the startup and shut-down process in case of order: switch ON/OFF (mechanical, remote command).

    - via...
        - mechanical switch,
        - remote command.

- In case the shut-down process has been started either intentionally or via power outage, the swithch on/off can be activated only after the ongoing shut-down process successfully finished.

- Display/monitor status:

    - LED,
    - software,
    - configurable.

- USB communication channel with serial protocol:

    - communicates with the host computer,
    - configurability,
    - alert/watch-dog function,
    - log transfer,
    - etc.

- Configurability via
    - firmware
    - jumpers

### Optional

- Supervise and log the outages, brown-outs, 
- Interrupt jumper/connector  to rPi GPIO, etc.
- ROS compatible
- Using solar cells 

## References

- [Raspberry Pi UPS: A Very Simple Solution](http://homediyelectronics.com/projects/raspberrypi/ups/)
- [Belkin Universal UPS protocol description](http://www.mscs.dal.ca/~selinger/ups/belkin-universal-ups.html)
- [Network UPS Tools](https://en.wikipedia.org/wiki/Network_UPS_Tools)
- [V-USB - A software-only implementation of a low-speed USB device for Atmel’s AVR®](https://www.obdev.at/products/vusb/index.html)
- [Controlled Shutdown at Power Failure](https://www.raspberrypi.org/forums/viewtopic.php?f=37&t=50470)
