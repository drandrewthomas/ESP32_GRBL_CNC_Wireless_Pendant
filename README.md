# ESP32 GRBL CNC pendantwith Bluetooth data link

I have a small, cheap and cheerful, Chinese CNC machine that I use for routing and LASER cutting. It's really good but one thing it lacked compared to more professional machines was a proper pendant for me to jog and zero the XYZ axes. So, I decided to build one. While thinking about what microcontroller to use it dawned on me that if I used an ESP32 I could combine the pendant functions with a Bluetooth interface for sending G-Code over. After some coding, soldering and 3D printing I ended up with the design shown below.

![Photos of the finished pendant and the circuit](https://github.com/drandrewthomas/ESP32_GRBL_CNC_Wireless_Pendant/blob/master/images/cncpendant.jpg)
