/* EXAMPLES -> DUMP INFO
 * --------------------------------------------------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC to serial.
 * --------------------------------------------------------------------------------------------------------------------
 * This is a MFRC522 library example; for further details and other examples see: https://github.com/miguelbalboa/rfid
 * 
 * Example sketch/program showing how to read data from a PICC (that is: a RFID Tag or Card) using a MFRC522 based RFID
 * Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout below), load this sketch into Arduino IDE
 * then verify/compile and upload it. To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M). When
 * you present a PICC (that is: a RFID Tag or Card) at reading distance of the MFRC522 Reader/PCD, the serial output
 * will show the ID/UID, type and any data blocks it can read. Note: you may see "Timeout in communication" messages
 * when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs presented (that is: multiple tag reading).
 * So if you stack two or more PICCs on top of each other and present them to the reader, it will first output all
 * details of the first and then the next PICC. Note that this may take some time as all data blocks are dumped, so
 * keep the PICCs at reading distance until complete.
 * 
 * @license Released into the public domain.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 *
 * More pin layouts for other boards can be found here: https://github.com/miguelbalboa/rfid#pin-layout
 */

#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>

#define RST_PIN         9          // Configurable, see typical pin layout above
#define SS_PIN          10         // Configurable, see typical pin layout above

#define RX_PIN_S2C1     2
#define TX_PIN_S2C1     3
#define RX_PIN_S2C2     4
#define TX_PIN_S2C2     5

#define REPETITIONS     5

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance
SoftwareSerial BTserial1(RX_PIN_S2C1, TX_PIN_S2C1); // RX | TX
SoftwareSerial BTserial2(RX_PIN_S2C2, TX_PIN_S2C2); // RX | TX

enum {
  CAR1,
  CAR2,
};

uint8_t cid[2][4] = {{3, 117, 46, 47}, {227, 57, 161, 30}};
uint8_t rules[9] = {'A', 'S', 120, 'L', 0, 'B', 0, 'T', 10}; 


void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)

  // Scanner initialization
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
	delay(4);				// Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  // Initialize BT link S2C1
  BTserial1.begin(9600);  
  Serial.println("BTserial1 started at 9600");

  // Initialize BT link S2C2
  BTserial2.begin(9600);  
  Serial.println("BTserial2 started at 9600");
}

bool is_car(uint8_t n) {
  bool match = true;
  for (int i=0; i<mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] != cid[n][i]) {
      match = false;
      break;
    }
  }

  return match;  
}

void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if ( ! mfrc522.PICC_ReadCardSerial()) {
		return;
	}

#if 0
	// Dump debug info about the card; PICC_HaltA() is automatically called
	mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
#endif

  if (is_car(CAR1)) {
    Serial.println("Car 1");
    //BTserial1.println("hello car1");
    for (int i=0; i<REPETITIONS; i++) {
      BTserial1.write(rules, sizeof(rules));
    }
  } else if (is_car(CAR2)) {
    Serial.println("Car 2");
    //BTserial2.println("hello car2");
    for (int i=0; i<REPETITIONS; i++) {
      BTserial2.write(rules, sizeof(rules));
    }
  }
}
