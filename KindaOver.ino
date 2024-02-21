
#include <HCSR04.h>


#include <Servo.h>

byte triggerPin = 3;
byte echoPin = 2;

const int buzzer = 5;

Servo myservo;

int pos = 0;

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // set the LCD address to 0x27 for a 16 chars and 2 line display

// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#include <SPI.h>
#include <MFRC522.h>
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
#define SS_PIN 10
#define RST_PIN 9
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
byte readCard[4];
String MasterTag = "9A907019";
String tagID = "";
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
//  Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
// MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM

/**********************************************************************************************
 * setup() function
 **********************************************************************************************/
void setup()
{

    Serial.begin(9600);
    HCSR04.begin(triggerPin, echoPin);

    pinMode(buzzer, OUTPUT);

    lcd.init();
    lcd.backlight();
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    Serial.begin(9600);
    SPI.begin();
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    mfrc522.PCD_Init();
    delay(4);
    // Show details of PCD - MFRC522 Card Reader
    mfrc522.PCD_DumpVersionToSerial();
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    Serial.println("--------------------------");
    Serial.println(" Access Control ");
    Serial.println("Scan Your Card>>");
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    myservo.attach(6);
    myservo.write(pos);

    lcd.setCursor(0, 0);
    lcd.print("Use card!");
}

/**********************************************************************************************
 * loop() function
 **********************************************************************************************/
void loop()
{

    double *distances = HCSR04.measureDistanceCm();

    bool open = 0;

    if (!open)
    {
        if (distances[0] > 10)
            tone(buzzer, 1000);
        else
            noTone(buzzer);
    }
    //----------------------------------------------------------------------
    // Wait until new tag is available
    while (getID())
    {

        // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
        if (tagID == MasterTag)
        {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Access Granted!");
            open = 1;
            tone(buzzer, 1000); // Send 1KHz sound signal...
            delay(500);
            // ...for 1 sec
            noTone(buzzer);
            for (pos = 0; pos <= 120; pos += 1)
            { // goes from 0 degrees to 180 degrees
                // in steps of 1 degree
                myservo.write(pos); // tell servo to go to position in variable 'pos'
                delay(15);          // waits 15 ms for the servo to reach the position
            }
            delay(5000);

            for (pos = 120; pos >= 0; pos -= 1)
            {                       // goes from 180 degrees to 0 degrees
                myservo.write(pos); // tell servo to go to position in variable 'pos'
                delay(15);          // waits 15 ms for the servo to reach the position
            }
            open = 0;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Use card!");
            Serial.println(" Access Granted!");
            Serial.println("--------------------------");
            // You can write any code here like, opening doors,
            // switching ON a relay, lighting up an LED etc...
        }
        // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
        else
        {
            tone(buzzer, 1000); // Send 1KHz sound signal...
            delay(200);
            // ...for 1 sec
            noTone(buzzer);
            tone(buzzer, 1000); // Send 1KHz sound signal...
            delay(200);
            // ...for 1 sec
            noTone(buzzer);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Access Denied!");
            delay(1500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Use card!");
            Serial.println(" Access Denied!");
            Serial.println("--------------------------");
        }
        // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
        delay(2000);
        Serial.println(" Access Control ");
        Serial.println("Scan Your Card>>");
    }
    //----------------------------------------------------------------------
}

/**********************************************************************************************
 * getID() function
 * Read new tag if available
 **********************************************************************************************/
boolean getID()
{
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    //  Getting ready for Reading PICCs
    // If a new PICC placed to RFID reader continue
    if (!mfrc522.PICC_IsNewCardPresent())
    {
        return false;
    }
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    // Since a PICC placed get Serial and continue
    if (!mfrc522.PICC_ReadCardSerial())
    {
        return false;
    }
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    tagID = "";
    // The MIFARE PICCs that we use have 4 byte UID
    for (uint8_t i = 0; i < 4; i++)
    {
        // readCard[i] = mfrc522.uid.uidByte[i];
        //  Adds the 4 bytes in a single String variable
        tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
    tagID.toUpperCase();
    mfrc522.PICC_HaltA(); // Stop reading
    return true;
    // MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM
}