// include all the libraries
#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Servo.h>

// sets the pins for the RFID module
#define SS_PIN 10
#define RST_PIN 7

MFRC522 myRFID(SS_PIN, RST_PIN);   // Create MFRC522 instance labeled "myRFID"

byte allowedUID[4] = {0x59, 0xA3, 0xCC, 0x05};  // the UID that this code compares to (different for other cards)


LiquidCrystal lcd(12,11,5,4,3,2); // sets the pins for the LCD screen

Servo myServo; // creates an object "myServo" to refer to in the code

void setup() // initializes all of the variables and components
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  myRFID.PCD_Init();   // Initiate MFRC522
  Serial.println("Please scan your RFID card...");
  Serial.println();

  lcd.begin(16,2);   // confirms how many rows and columns are on the LCD screen (column, row)
  lcd.print("Please scan ...");

  myServo.attach(37);  // tells the arduino that the servo is connected to pin 37
}
void loop() // repeatedly runs code and checks conditions
{
  // Wait for RFID cards to be scanned
  if ( ! myRFID.PICC_IsNewCardPresent()) 
  {

    return;
  }
  // an RFID card has been scanned but no UID is present
  if ( ! myRFID.PICC_ReadCardSerial()) 
  {
    
    return;
  }
  //Show UID on serial monitor
  Serial.print("USER ID tag :");
  String content= "";
 
  for (byte i = 0; i < myRFID.uid.size; i++) // prints the UID on the serial monitor
  {
     Serial.print(myRFID.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(myRFID.uid.uidByte[i], HEX);
     content.concat(String(myRFID.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(myRFID.uid.uidByte[i], HEX));
  }

  lcd.clear();
  lcd.print("Scanning...");
  delay(1000);
  Serial.println();

  bool accessGranted = true; // variable representing whether user has access or not -> set to true

if (myRFID.uid.size != 4) { // access denied when the UID is not 4 bytes long
  accessGranted = false;
} else {  // if the UID is 4 bytes, then it chekcs the already preset UID does not match the reading so that access is denied
  for (byte i = 0; i < 4; i++) {
    if (myRFID.uid.uidByte[i] != allowedUID[i]) {
      accessGranted = false;
      break;
    }
  }  // if the UID is 4 bytes long and it matches the preset UID, then the variable remains true meaning access granted
}
if (accessGranted) {  // if (true), then it will run the code and grant access
  Serial.println("ACCESS GRANTED");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS GRANTED");
  lcd.setCursor(0, 1);
  lcd.print("Welcome.");    // when access is granted, the serial monitor prints "ACCESS GRANTED" and so does the LCD screen

  myServo.write(0);
  delay(10);    // when access is granted, the servo moves to "unlock" the door
  }

else {   // if (false), then it will run this code and deny access
  Serial.println("ACCESS DENIED");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESS DENIED");
  lcd.setCursor(0, 1);
  lcd.print("Try again."); // when access is denied, the serial monitor prints "ACCESS DENIED" and so does the LCD screen

  myServo.write(180);
  delay(10);  // when access is denied, the servo moves to "lock" the door
}

} 