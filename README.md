# Arduino-LoRa-Sniffer
Allows you to use an Arduino to capture LoRa radio network traffic and transfer to a PC for viewing or saving to disk.

This Arduino project contains only an INO file to get started allows you to operate as a "promiscuous" node, receiving all frames on a LoRa radio network.  The sketch then sends the captured frames in either a **verbose** or **delimited** format to the serial port for transfer to the host workstation, where the packets can be viewed or saved to disk (requires a terminal application with save to disk capabilities).

For save to disk, you can use any terminal application you like, but not the Arduino IDE Serial Monitor.  Unfortunately, the Arduino IDE Serial Monitor is not currently capable of saving the serial port stream.  Internally we use and can recommend Tera Term; Tera Term has numerous features: including automatic serial port detection and write to disk.

## Notes:
* This only works for LoRa clear-text networks, not for LoRaWAN networks protected by message encryption.  
* Any frequency LoRa should work, as long as the same frequency is used for both the "sniffer" and the monitored network.
