/**
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * Minimal example how to use the interrupts to read the UID of a MIFARE Classic PICC
 * (= card/tag).
 * 
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        3                10
 * IRQ         ?            ?             ?         ?          2                10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * 
 */

/* Modified by me
 * */

#include <SPI.h>
#include <MFRC522.h>


void dump_byte_array(byte *buffer, byte bufferSize);
String byte_arrayToHex(byte *buffer, byte bufferSize);

volatile bool bNewInt = false; // this var is global, because interrupt require global function
/**
 * MFRC522 interrupt serving routine
 */
void statreadCard() {
	bNewInt = true;
}
class RFIDReader {
	byte regVal = 0x7F;
	//	MFRC522::MIFARE_Key key;
	MFRC522 mfrc522;
	byte sspin,rspin,irqpin;
	String tag;

public:
	void init(byte sspin0, byte rspin0, byte irqpin0){//:mfrc522(sspin0, rspin0){
		sspin=sspin0;rspin=rspin0;irqpin=irqpin0;
		SPI.begin();          // Init SPI bus
		mfrc522.PCD_Init(sspin, rspin); // Init MFRC522 card//		mfrc522.PCD_Init(); // Init MFRC522 card

		/* read and printout the MFRC522 version (valid values 0x91 & 0x92)*/
		Serial.print(F("MFRC522 Ver: 0x"));
		byte readReg = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
		Serial.println(readReg, HEX);

		/* setup the IRQ pin*/
		pinMode(irqpin, INPUT_PULLUP);

		/*
		 * Allow the ... irq to be propagated to the IRQ pin
		 * For test purposes propagate the IdleIrq and loAlert
		 */
		regVal = 0xA0; //rx irq
		mfrc522.PCD_WriteRegister(mfrc522.ComIEnReg, regVal);

		bNewInt = false; //interrupt flag

		/*Activate the interrupt*/
		attachInterrupt(digitalPinToInterrupt(irqpin), statreadCard, FALLING);	//interrupt man does a better job to convert static call to object call
		//		  Serial.println(F("RFIDReader end setup"));
	}

	String readCard2(){
		//	  long ts=millis();
		if (bNewInt) { //new read interrupt
			//		    Serial.print(F("Interrupt. "));
			mfrc522.PICC_ReadCardSerial(); //read the tag data
			// Show some details of the PICC (that is: the tag/card)
					    Serial.print(F("Card UID:"));
					    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
			tag=byte_arrayToHex(mfrc522.uid.uidByte, mfrc522.uid.size);
			//		    Serial.println();

			clearInt(mfrc522);
			mfrc522.PICC_HaltA();
			bNewInt = false;
			return tag;
		}

		// The receiving block needs regular retriggering (tell the tag it should transmit??)
		// (mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg,mfrc522.PICC_CMD_REQA);)
		activateRec(mfrc522);
		//Serial.println(String()+"time loop :"+(millis()-ts));
		return "";
	}

	String readCard() {
	    // 1. Look for new cards
	    if ( ! mfrc522.PICC_IsNewCardPresent()) {
	        return "";
	    }

	    // 2. Select one of the cards (THIS MUST RETURN TRUE)
	    if ( ! mfrc522.PICC_ReadCardSerial()) {
	        return ""; // Read failed, do NOT process the UID buffer!
	    }

	    // 3. Read the UID (Now it is guaranteed to be the new tag)
//	    tag=byte_arrayToHex(mfrc522.uid.uidByte, mfrc522.uid.size);
	    tag = "";
	    for (byte i = 0; i < mfrc522.uid.size; i++) {
	    	tag += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
	    	tag += String(mfrc522.uid.uidByte[i], HEX);
	    }
	    tag.toUpperCase();
	    Serial.println(String()+"Card UID:"+tag);
	    // 4. Halt PICC and Stop encryption on PCD (CRITICAL to prevent ghost reads!)
	    mfrc522.PICC_HaltA();
	    mfrc522.PCD_StopCrypto1();

	    return tag;
	}

	void sleep(byte mosfetPin) {
		  mfrc522.PICC_HaltA();
		  mfrc522.PCD_StopCrypto1();
		  digitalWrite(mosfetPin, LOW);
	}
	void wakeup(byte mosfetPin) {
		  digitalWrite(mosfetPin, HIGH);
	}

	/*
	 * The function sending to the MFRC522 the needed commands to activate the reception
	 */
	void activateRec(MFRC522 mfrc522) {
		mfrc522.PCD_WriteRegister(mfrc522.FIFODataReg, mfrc522.PICC_CMD_REQA);
		mfrc522.PCD_WriteRegister(mfrc522.CommandReg, mfrc522.PCD_Transceive);
		mfrc522.PCD_WriteRegister(mfrc522.BitFramingReg, 0x87);
	}

	/*
	 * The function to clear the pending interrupt bits after interrupt serving routine
	 */
	void clearInt(MFRC522 mfrc522) {
		mfrc522.PCD_WriteRegister(mfrc522.ComIrqReg, 0x7F);
	}
	///////////////////////////////////////////////////// INIT
public:
	void mfrc522_fast_Reset()
	{
	  digitalWrite(rspin, HIGH);
	  mfrc522.PCD_Reset();
	  mfrc522.PCD_WriteRegister(mfrc522.TModeReg, 0x80);      // TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
	  mfrc522.PCD_WriteRegister(mfrc522.TPrescalerReg, 0x43);   // 10μs.
	  //  mfrc522.PCD_WriteRegister(mfrc522.TPrescalerReg, 0x20);   // test

	  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegH, 0x00);   // Reload timer with 0x064 = 30, ie 0.3ms before timeout.
	  mfrc522.PCD_WriteRegister(mfrc522.TReloadRegL, 0x1E);
	  //mfrc522.PCD_WriteRegister(mfrc522.TReloadRegL, 0x1E);

	  mfrc522.PCD_WriteRegister(mfrc522.TxASKReg, 0x40);    // Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	  mfrc522.PCD_WriteRegister(mfrc522.ModeReg, 0x3D);   // Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)

	  mfrc522.PCD_AntennaOn();            // Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
	}
};


/**
 * Helper routine to dump a byte array as hex values to String.
 */
String byte_arrayToHex(byte *buffer, byte bufferSize) {
	String val;
	for (byte i = 0; i < bufferSize; i++) {
		char hexStr[10];  // String to hold the hexadecimal value
		sprintf(hexStr, "%X", buffer[i]);  // Convert integer to hexadecimal string
		val+=buffer[i] < 0x10 ? " 0" : " ";
		val+=String(hexStr);
	}
	return val;
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

