# RFID + Keypad Access Control System
A two factor door lock built on an Arduino Mega. 
The door only unlocks if both an authorized RFID card and the correct keypad pin is entered in sequence.

## How It Works
1. System waits for an RFID card to be scanned.
2. If the card UID matches the authorized card, it prompts for a pin on the LCD.
3. if the pin matches the authorized pin, the servo motor rotates to 'unlock the door'.
4. The LCD displays the current status at each step: waiting for card, enter code & access granted/denied.

## Hardware Used
- Arduino Mega
- MFRC522 RFID Reader + RFID Card
- 4x4 Matrix Keypad
- 16x2 LCD Display (Parallel, 4-bit)
- SG90 Servo Motor
- Breadboard + Jumper Wires

## Wiring 
- RFID: VCC -> 5V, RST -> pin 49, SDA -> pin 53.
- LCD: RS -> Pin 12, E -> Pin 11, D4 -> pin 5, D5 -> pin 4, D6 -> pin 3, D7 -> pin 2.
- Keypad Rows: pins 40, 38, 36, 34
- Keypad Columns: pins 41, 39, 37, 35
- Servo: pin 8

## Problems I Ran Into
- RFID module I own is a clone that would not work on 3.3v like most tutorials show, and as printed on the component itself; I had to run it at 5v to get reliable reads.
- Keypad and RFID were conflicting when integrated; solved by remaking my code to use a finite state machine.

## Libraries Used
- Keypad: https://github.com/Chris--A/Keypad
- MFRC522: https://github.com/miguelbalboa/rfid
- LiquidCrystal: Built into Arduino IDE
- Servo: Built into Arduino IDE
