/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 *
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 *
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * This sample shows how to read and write data blocks on a MIFARE Classic PICC
 * (= card/tag).
 *
 * BEWARE: Data will be written to the PICC, in sector #1 (blocks #4 to #7).
 *
 *
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS 1    SDA(SS)      10            53        D10        10               10
 * SPI SS 2    SDA(SS)      2             53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 */

#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN        9           // Configurable, see typical pin layout above
#define SS_1_PIN        4          // Configurable, see typical pin layout above
#define SS_2_PIN        7         // Configurable, see typical pin layout above

#define NR_OF_READERS   2


byte ssPins[] = {SS_1_PIN, SS_2_PIN};
//boolean to check if the tag is inside the room
bool inside = false;
//variables for the timestamp
unsigned long timezero = 0;
unsigned long timeone = 0;
//interval to avoid rollover
int interval = 1000;
//unsinged long to be used to avoid rollover
unsigned long previousMillis = 0;
MFRC522 mfrc522[NR_OF_READERS];   // Create MFRC522 instance.
unsigned long time;
int speakerPin = 3;
/**
 * Initialize.
 */
void setup() {

  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial);    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    mfrc522[reader].PCD_Init(ssPins[reader], RST_PIN); // Init each MFRC522 card
  }
}

/**
 * Main loop.
 */
void loop() {

    unsigned long currentMillis = millis();
    
    for (uint8_t reader = 0; reader < NR_OF_READERS; reader++) {
    // Look for new cards

    if (mfrc522[reader].PICC_IsNewCardPresent() && mfrc522[reader].PICC_ReadCardSerial()) {
      //print some data to check if the card is being red and what data it gives
      Serial.print(F("Reader: "));
      Serial.print(reader);
      // Show some details of the PICC (that is: the tag/card)
      Serial.print(F(" Card UID:"));
      dump_byte_array(mfrc522[reader].uid.uidByte, mfrc522[reader].uid.size);
      Serial.println();
      Serial.print("Time sinds program is running: ");
       time= millis();
      Serial.print(time);
      Serial.println();
        Serial.print(F("PICC type: "));
      MFRC522::PICC_Type piccType = mfrc522[reader].PICC_GetType(mfrc522[reader].uid.sak);
      Serial.println(mfrc522[reader].PICC_GetTypeName(piccType));
      

// if the timer reset, the timestamps wil be mesured, and the biggest timestamp wil get the value 1 and the smallest zero. 
    if ((unsigned long)(currentMillis - previousMillis) >= interval){
      Serial.print("old time for reader 0: ");
      Serial.println(timezero);
      Serial.print("old time for reader 1: ");
      Serial.println(timeone);
        if (timezero > timeone){
          timezero = 1;
          timeone = 0;
        }
        else if (timeone > timezero){
          timeone = 1;
          timezero = 0;
        }
        Serial.print("new time for reader 0: ");
        Serial.println(timezero);
        Serial.print("new time for reader 1: ");
        Serial.println(timeone);
        previousMillis = currentMillis;
    }
// if the reader is the ousize one, the timestamp wil be set and logged
    if (reader == 0){
      timezero = millis();
      Serial.print("new time for reader 0: ");
      Serial.println(timezero);
    }
// if the reader is the inside one, the timestamp wil be set and logged
    if (reader == 1){
      timeone = millis();
      Serial.print("new time for reader 1: ");
      Serial.println(timeone);  
    }
// if the timestamp of the inside reader is bigger than the outside one, inside wil be set to true
    if (timeone > timezero){
        Serial.print("the cat is  inside");
        inside == true;
        startspeaker();
    
      }
  // if the timestamp of the inside reader is smaller than the outside one, inside wil be set to false
    if (timeone < timezero){
        Serial.print("the cat is outside");
        inside == false;
        stopspeaker();
    }

      

    
    
      // Halt PICC
      mfrc522[reader].PICC_HaltA();
      // Stop encryption on PCD
      mfrc522[reader].PCD_StopCrypto1();
    } //if (mfrc522[reader].PICC_IsNewC
  } //for(uint8_t reader
}
void startspeaker(){
   unsigned int freq= 60000;
   tone(speakerPin, freq);
   Serial.println("the speaker is now playing at ");
   Serial.print(freq);
   Serial.print(" Hz");
}
void stopspeaker(){
  noTone(speakerPin);
   Serial.println("The speaker has stoped playing");
   
}
/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
