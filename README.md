# ESP32 GRBL CNC pendant with Bluetooth data link

## Introduction

I have a small, cheap and cheerful, Chinese CNC machine that I use for routing and LASER cutting. It's really good but one thing it lacked compared to more professional machines was a proper pendant for me to jog and zero the XYZ axes. So, I decided to build one. While thinking about what microcontroller to use it dawned on me that if I used an ESP32 I could combine the pendant functions with a Bluetooth interface for sending G-Code over. After some coding, soldering and 3D printing I ended up with the design shown below.

![Photos of the finished pendant and the circuit](https://github.com/drandrewthomas/ESP32_GRBL_CNC_Wireless_Pendant/blob/master/images/cncpendant.jpg)

## Building the pendant

One of the difficulties with a project like this is that there are a wide range of ESP32 modules available. So, it's not possible to give exact construction details. However, I thought I'd provide a few hints based on my experience designing it. The first is that all of the connections for switches, the OLED display and the rotary encoder module are in the Arduino code. It just involves running wires from those to the ESP32 board. To make life simple I wrote the code to use internal pullup resistors for the buttons. However, not all pins on all ESP32 boards have them, so if you have problems with a button not working you can add a 10k pullup resistor to it (you can see I had to do that with one button in the images folder).

The buzzer is a little more complicated as I used a 5V active one to get enough volume for it to be heard over the CNC machine. As it's 5V, and ESP32s are 3.3V, I used a 2n2222A transistor, with a 220 ohm resistor into it from the ESP32, for switching on and off, which works well. Another difficulty is the serial connection. Obviously you need to connect 5V and ground to the ESP32 (making sure to connect 5V to Vin, not 3.3V) and wire the Tx and Rx serial connections together between the CNC controller and ESP32. However, the connections vary between different GRBL CNC controller boards, so you'll need to sort that out for yourself based on which board you have.

As for the case, it just involves 3D printing the parts provided in the '3d files' folder. You may need to adapt it for your use and for variations between ESP32 boards, so I've included the OpenSCAD file too. The photos should give enough info for you to lay out a piece of stripboard (a.k.a. veroboard) with the OLED display and buttons to fit the case openings. To mount the rotary encoder module I cut a hole in some stripboard and fixed the encoder to the back. The case includes holes in the corners that you can glue M3 nuts into to bolt the top and bottom of the case together. For the dial I decided to LASER cut some 1.5mm thick cherry wood, although you could change the OpenSCAD design to remove the need for that. However, if you decide to cut it there's an SVG included to help you out.

## Parts list

The exact parts needed will depend on how you decide to build the project, but the list below is what I used, in case you find it useful:

* 1x 0.96 Inch SSD1306 OLED I2C Display 128x64 pixels

* 5x PCB mounting push switches

* 1x Arduino rotary encoder module brick on PCB

* 1x ESP32 board (I used an ESP32 DOIT DevKit v1)

* 1x 220-ohm resistor

* 1x 2n2222A transistor

* 1x 5V mini active buzzer

* 4x M3 nuts and bolts

* 10 k-ohm pullup resistors for push switches where needed

## Using the pendant

When you power up the pendant (usually through your cable to the CNC controller board) it'll show a splash screen and soon after start in operating mode (as in the photo above). That means it will pass through any commands sent between your PC/tablet and the CNC controller. Basically that means it'll act just like any other wireless or wired way of sending G-Code to your CNC. To connect via Bluetooth you'll need to know the pendants' identifier, which is 'CNCESP32'.

From the operating mode you can use the OK button to access a menu of other functions: jogging mode, zeroing mode and focusing mode. The modes are quite easy to change or add to in the code if they don't suit your needs. Use the rotary encoder dial to scroll through them. The OK button then lets you enter that mode. In jogging mode you can choose the axis to move using the XYZ buttons and turn the dial clockwise and counter-clockwise to move the selected axis. For coarser, faster, movement press the rotary encoder dial down while turning, as the encoder has a switch built in. Coarser movement goes three times further than normal jogging, so please be careful until you get the hang of it.

In zeroing mode you can press an XYZ button and the pendant will tell the CNC controller to zero that axis at the current position. To zero all axes just press the XYZ buttons one at a time, waiting for a confirmation beep in between. The final mode is the focusing one, which is only of use if you have a LASER module. Basically it turns on the LASER at a low power level: I set the value for my 5W dode-LASER so please adjust it to suit your own LASER so as to minimise injury and damage risks. The first time you enter this mode the LASER will turn on, allowing you to focus it or to go back to jogging mode to set your origin position with a visible LASER-dot. The second time will turn the LASER off, although it'll also turn off automatically if you exit back to operating mode (all modes and menus can be exited with the BACK button).

## And finally!

If you'd like to read my blog-post about this project [please click here to visit it](https://cncmakerzone.co.uk/make-a-grbl-cnc-pendant-with-a-bluetooth-data-link).
