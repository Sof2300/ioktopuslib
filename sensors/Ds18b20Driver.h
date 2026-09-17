#ifndef DS18BDRIVER_H
#define DS18BDRIVER_H

#include <OneWire.h>
#include <DallasTemperature.h>

#include "Sensor.h"


class DS18B20Driver : public TempSensor                        
{

	OneWire *oneWire;//(SENSOR_PIN);
	DallasTemperature *DS18B20;//(&oneWire);
	DeviceAddress deviceAddress;

	const unsigned int timeout=2000;
	uint32_t reqts=0;
	bool valueready=false;

public:
	DS18B20Driver(String name, std::vector<unsigned int> pins0):TempSensor(name,pins0){}
	//~DS18B20Driver(){}

	bool begin(){return init();}
	virtual bool isReady(){return valueready;}

	bool init() {
		oneWire=new OneWire(pins[0]);
		DS18B20=new DallasTemperature(oneWire);
		DS18B20->begin();    // initialize the DS18B20 sensor
		uint8_t deviceIndex=0;
		bool connected=DS18B20->getAddress(deviceAddress, deviceIndex);
		DS18B20->setWaitForConversion(false);
		if (!connected) {Serial.println(("DS18B20 not connected!")); disconnected=true; return false;}
		else Serial.println(("DS18B20Driver init with success!"));
		disconnected=false;
		return true;
	};


	bool tick(){	// read temp, then hum
		uint32_t now=millis();
		if(reqts==0) {
			bool connected=DS18B20->requestTemperaturesByAddress(deviceAddress);
			disconnected=!connected;
			valueready=false;
			reqts=now;
			if(disconnected) Serial.println(String()+"Device DS18B20 disconnected !");
			//Serial.println(String()+"Device DS18B20 requested value !");
			return false;
		}
		if(DS18B20->isConversionComplete()){
			temp = DS18B20->getTempC((uint8_t*) deviceAddress);
			if(temp==DEVICE_DISCONNECTED_C) {
				Serial.println(String()+"Device DS18B20 disconnected !");
				disconnected=true;
				return false;
			}
			if(temp==85) {
				Serial.println(String()+"Device DS18B20 gave strange value :"+temp+", will try again");
				reqts=0;
				return false;
			}
			//Serial.println(String()+"Device DS18B20 got value: "+temp);
			valueready=true;
			reqts=0;
			if(sensorListener) sensorListener->notify(0); // should we notify with -1 when disconnected ?
			return true;
		}
		if(reqts!=0 && (now-reqts)>timeout){
			Serial.println("DS18B20 timed out");
			disconnected=true;
			reqts=0;
			return false;
		}
		return false;
	}
};

#endif

