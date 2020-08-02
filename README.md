# uvkeysan

## Install Arduino IDE  
Don’t use the app installer. Tested version 1.8.7
[https://www.arduino.cc/en/main/software](https://www.arduino.cc/en/main/software)

## Install Pololu AVR Programmer V2.1 software  
[https://www.pololu.com/docs/0J67/4.1](https://www.pololu.com/docs/0J67/4.1)

Connect the programmer to PC and board and apply power to the board and you should get a green led and 2 flashing orange leds.
Run Pololu config and make a note of the Programming Port.

## In Arduino IDE

### Configure Processor
From the menu : File, Preferences   
In additional Boards Manager URLs enter: http://drazzy.com/package_drazzy.com_index.json  
The press OK.  

From the menu : Tools, Board, Boards Manager... (at the top of the list)
Scroll down and select ATTinyCore by Spence Konde version 1.33 and Install

* From the Tools menu select 
* Board : ATtiny24/44/84
* Chip: ATtiny84
* Clock: 1Mhz internal
* Port: the com port noted in Pololu Config
* Programmer: Atmel STK500 development board (ATTinyCore)

### Install Libraries
From the menu Sketch, Include Library, Manage Libraries...
Search for Bounce2, select the library and press the Install button.
Search for tone, select the library and press the Install button.   

