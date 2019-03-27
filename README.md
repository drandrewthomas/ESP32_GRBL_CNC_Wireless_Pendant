# ESP32 GRBL CNC pendant with Bluetooth data link

## Introduction

I have a small, cheap and cheerful, Chinese CNC machine that I use for routing and LASER cutting. It's really good but one thing it lacked compared to more professional machines was a proper pendant for me to jog and zero the XYZ axes. So, I decided to build one. While thinking about what microcontroller to use it dawned on me that if I used an ESP32 I could combine the pendant functions with a Bluetooth interface for sending G-Code over. After some coding, soldering and 3D printing I ended up with the design shown below.

![Photos of the finished pendant and the circuit](https://github.com/drandrewthomas/ESP32_GRBL_CNC_Wireless_Pendant/blob/master/images/cncpendant.jpg)

## Building the pendant

One of the difficulties with a project like this is that there are a wide range of ESP32 modules available. So, it's not possible to give exact construction details. However, I thought I'd provide a few hints based on my experience designing it. The first is that all of the connections for switches, the OLED display and the rotary encoder module are in the Arduino code. It just involves running wires from those to the ESP32 board. To make life simple I wrote the code to use internal pullup resistors for the buttons. However, not all pins on all boards have them, so if you have problems with a button not working you can add a 10k pullup resistor to it (you can see I had to do that with one button in the images folder).

The buzzer is a little more complicated as I used a 5V active one to get enough volume for it to be heard over the CNC machine. As it's 5V, and ESP32s are 3.3V, I used a 2n2222A transistor, with a 220 ohm resistor into it from the ESP32, for switching on and off, which works well. Another difficulty is the serial connection. Obviously you need to connect 5V and ground to the ESP32 (making sure to connect 5V to Vin, not 3.3V) and wire the Tx and Rx serial connections together between the CNC controller and ESP32. However, the connections vary between different GRBL CNC controller boards, so you'll need to sort that out for yourself based on which board you have.

As for the case, it just involves 3D printing the parts provided in the '3d files' folder. You may need to adapt it for your use and for variations between ESP32 boards, so I've included the OpenSCAD file too. The photos should give enough info for you to lay out a piece of stripboard (a.k.a. veroboard) with the OLED display and buttons to fit the case openings. To mount the rotary encoder module I cut a hole in some stripboard and fixed the encoder to the back. The case includes holes in the corners that you can glue M3 nuts into to bolt the top and bottom of the case together. For the dial I decided to LASER cut some 1.5mm thick cherry wood, although you could change the OpenSCAD design to remove the need for that. However, if you decide to cut it there's an SVG included to help you out.

