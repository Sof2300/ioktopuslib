#ifndef HTUDRIVER_H
#define HTUDRIVER_H

#include "AsyncHTU21Dmod.h"	// custom driver from sparkfunHTU21D modified to get async and the error back

#include "Sensor.h"

#define HTU21D_TEMP_COEFFICIENT      -0.15     //temperature coefficient for RH compensation at range 0�C..80�C, for HTU21D & SHT21 only
#define COMPENSATEHUMIDITY

/*
class FakeHTU {
public:
	FakeSensor fsensor;
	bool tempChanged=false, humChanged=false;
	bool begin(){
		if(fsensor.empty()){
			fsensor.addFakeValue(new FakeValue((TEMPERATURE_CHANNEL_TYPE),11,18));
			fsensor.addFakeValue(new FakeValue((HUMIDITY_CHANNEL_TYPE),30,70));
			fsensor.readtimeout=75;//75ms to read both temp 25ms and humidity 50ms
		}
		return true;};
	bool readTemperatureAsync(float &f,unsigned int ms=0){
		bool b= fsensor.readAsync((TEMPERATURE_CHANNEL_TYPE),f,ms);
		tempChanged=fsensor.findByName((TEMPERATURE_CHANNEL_TYPE))->hasChanged();
		return b;
	};
	bool readHumidityAsync(float &f,unsigned int ms=0){
		bool b= fsensor.readAsync((HUMIDITY_CHANNEL_TYPE),f,ms);
		humChanged=fsensor.findByName((HUMIDITY_CHANNEL_TYPE))->hasChanged();
		return b;
	};
};
*/



class HTUDriver : public TempRHSensor                        // HTU21D fully hot pluggable
{
	HTU21D *htu=0;//= Sensirion(dataPin, clockPin); 	//FakeHTU htu = FakeHTU();	// could use a real htu too
	bool readingtemp=true,readinghum=false;
	bool firstread=true;
	const unsigned int timeout=2000;
	uint32_t reqts=0;
	bool valueready=false;

public:
 	HTUDriver(String name, std::vector<unsigned int> pins0):TempRHSensor(name,pins0){}
	~HTUDriver(){if(htu) delete htu;}

	bool begin(){return init();}
	virtual bool isReady(){if(valueready) {valueready=false;/*selfTick();*/return true;} else return false;}

	void setHeater(bool on){if(htu) htu->setHeater(on);}

	bool init() {
		if(pins.size()==2) htu=new HTU21D(pins[1], pins[0]); //dataPin, clockPin
		//if (!htu->begin()) {println(("Couldn't find sensor HTU!"));return false;}
		if (!htu->begin()) {Serial.println(("Couldn't find sensor HTU!"));}
		else Serial.println(("HTUDriver init with success!"));

		return true;
	};

	// HTU allow hold and no hold, take 50ms to read
	// for ESP8266 one process only, hold is no good, we use hold and come back every tick to check if the result changed
	bool tick(){	// read temp, then hum
//			Serial.println("HTU sensorTick");
		uint32_t now=millis();
		if (firstread) {//Serial.println("HTU first read");
			reqts=now;firstread=false;
		} else if((now-reqts)>timeout){
			//Serial.println("HTU timed out");
			disconnected=true;
			readinghum=false;readingtemp=true;
			firstread=true;
			return true;
		}
		if(readingtemp){
			bool b=htu->readTemperatureAsync(temp,100);

			if(htu->readTimeout){
				//Serial.println("HTU temp timed out");
				disconnected=true;
				firstread=true;
				return true;
			}
			if (b) disconnected=false;
			if(b && htu->tempChanged) {
				//Serial.println("HTU temp changed");
				readingtemp=false;readinghum=true;
				sensorListener->notify(0); // value too high ?
			}
			return false;
		}
		if(readinghum){
			bool b=htu->readHumidityAsync(hum,100);
 			if(htu->readTimeout){
 				//Serial.println("HTU hum timed out");
 				disconnected=true;
 				readinghum=false;readingtemp=true;
				firstread=true;
				return true;
			}
 			if (b) {
 			//	Serial.println("HTU hum connected");
 				disconnected=false;
 			}
 		//	else Serial.println("HTU hum disconnected");
			if(b && htu->humChanged) {
#ifdef COMPENSATEHUMIDITY
				if(temp>0 && temp <80) hum=hum + (25.0 - temp) * HTU21D_TEMP_COEFFICIENT;
#endif
				//Serial.println("HTU hum changed");
				readinghum=false;readingtemp=true;
				firstread=true;
				valueready=true;
				if(sensorListener) {
				//	sensorListener->notify(0);
					sensorListener->notify(1);
				}
				return true;
			}
		}
		return false;
	}
};

#endif

