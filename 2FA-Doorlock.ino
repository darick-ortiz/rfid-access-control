// RFID + Keypad Access Control System
// Two-factor authentication: RFID card + keypad code
// Servo physically locks/unlocks on access granted

// ----------- Libraries & Defining -------------
#include <LiquidCrystal.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Keypad.h>

#define RST_PIN 49
#define SDA_PIN 53
#define SERVO_PIN 8

// ---------- Libraries Setup --------
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
Servo myServo;
MFRC522 mfrc522(SDA_PIN, RST_PIN);

// ---------- KeyPad Setup ---------
const byte ROWS = 4; 
const byte COLS = 4;
char hexKeys[ROWS][COLS] = {                          // Matrix defines buttons on keypad
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {40, 38, 36, 34};                // Connect to the row pinouts of the keypad
byte colPins[COLS] = {41, 39, 37, 35};                // Connect to the column pinouts of the keypad
Keypad myKeypad = Keypad( makeKeymap(hexKeys), rowPins, colPins, ROWS, COLS);      // Makes new instance of the class Keypad
char key;

// ---------   Set Authorized Uid & Code ------------
byte authUid[] = {0x35, 0x3E, 0xA6, 0x4};  // In hex
String authCode = "A1B2";

// ---------- FSM Setup --------------
enum State{cardRequest, codeRequest, accGranted, accDenied};
State currentState = cardRequest;
String enteredCode = "";                              // Start w/ empty code
bool stateChanged = true;



void setup() {
  // One time run
  Serial.begin(9600);
  lcd.begin(16, 2);                                 // Start the LCD at 16 col 2 row
  SPI.begin(); 
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max);  // Set antena range to max for easier use
  myServo.attach(SERVO_PIN);
}

void loop() {
  // Always running

  // --------------------- FSM for Authorization --------------------
  switch(currentState){
    case cardRequest:{
      if(stateChanged){              //  Only update LCD and servo on state entry, not every cycle
      myServo.write(0);
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("Scan Card.");                         
      stateChanged = false;
      }
      // ------ Updates Card Data -----
      bool cardPresent = mfrc522.PICC_IsNewCardPresent();         // Checks if card is nearby
      bool cardSerial = mfrc522.PICC_ReadCardSerial();           // Reads the card UID
      byte *uid = mfrc522.uid.uidByte;                          // Pointer to shorten call
      byte uidSize = mfrc522.uid.size;                         // Variable to shorten call

      // --- State Changer ---
      if(cardPresent && cardSerial){                         // Only change if the card is near and serial is read
        int uidCheck = memcmp(uid, authUid, uidSize);       // compares keycard uid and authorized uid
        if(uidCheck == 0){
          stateChanged = true;
          currentState = codeRequest;
        }
        else{
          stateChanged = true;
          currentState = accDenied;
        }
      }
      break;
    }

    case codeRequest:
      if(stateChanged){
        enteredCode = "";
        lcd.clear();
        lcd.print("Enter Passkey.");
        lcd.setCursor(0, 1);
        stateChanged = false;
      }

      key = myKeypad.getKey();
      if(key && key != '#' && key != '*'){       // Allows digit entering besides * and #
        enteredCode += key;
        lcd.print(key);

      }else if(key == '*'){                    // Erases entered digits
        lcd.clear();
        lcd.print("Enter Passkey.");
        lcd.setCursor(0, 1);
        enteredCode = "";

      }else if(key =='#'){                  // Submits the code
        if(enteredCode == authCode){
          stateChanged = true;
          currentState = accGranted;

        }else {
          stateChanged = true;
          currentState = accDenied;
        }
      }
      break;

    case accDenied:
      if(stateChanged){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Denied.");
        delay(1500);
        currentState = cardRequest;
      }
      break;

    case accGranted:
      if(stateChanged){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Access Granted.");
        myServo.write(90);             // Open lock
        delay(2000);
        currentState = cardRequest;
      }
      break;

  }
}

