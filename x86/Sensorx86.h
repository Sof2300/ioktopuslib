//#ifndef SENSOR_H
//#define SENSOR_H
#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

//namespace espsensor {
#include <sensors\Sensor.h>
//}
#include <sensors/FakeDriver.h>

#define TEMPCHANNEL "Temperature"
#define HUMCHANNEL "Humidity"
/*
class Sensor {
public:
	virtual bool begin(){return true;};
	virtual bool isReady(){return false;}
	virtual float get(String channel)=0;
	virtual String getUnit(String channel)=0;
	virtual bool tick()=0;
	//	read(bool repeatedly=true,long atintervalms=300) {	}
};
*/
class FakeRHSensor : public TempRHSensor{
	FakeSensor fsensor;
	bool tempChanged=false, humChanged=false;
 	bool readingtemp=true,readinghum=false;
	float temp, hum;
public:
	FakeRHSensor(String name0, std::vector<unsigned int> pins0):TempRHSensor(name0,pins0){}
	bool tick(){
	//	if(debug) println("FakeRHSensor::sensorTick");
		if(readingtemp){
			bool b=readTemperatureAsync(temp,100);
			//	println("HTUSensorReader::sensorTick temp:");
			//	if(b) println("true"); else println("false");
			if(b && tempChanged) {readingtemp=false;readinghum=true;}
			return false;
		}
		if(readinghum){
			bool b=readHumidityAsync(hum,100);
			//	println("HTUSensorReader::sensorTick hum:");
			//	if(b) println("true"); else println("false");
			if(b && humChanged) {
				readinghum=false;readingtemp=true;
				return true;
			}
		}
		return false;
	}

	virtual bool isReady(){return tick();}
	virtual float get(String channel){
		FakeValue *fv=fsensor.findByName(channel.c_str());
		if(fv!=0) return fv->val;
		return -1;
	};
	virtual String getUnit(String channel){
		if(channel==TEMPCHANNEL){return "°C";}
		if(channel==HUMCHANNEL){return "%";}
		return "";
	}
	//	read(bool repeatedly=true,long atintervalms=300) {	}

	bool begin(){
		if(fsensor.empty()){
			fsensor.addFakeValue(new FakeValue((TEMPCHANNEL),11,48,5));
			fsensor.addFakeValue(new FakeValue((HUMCHANNEL),30,70));
			fsensor.readtimeout=75;//75ms to read both temp 25ms and humidity 50ms
		}
		return true;};

private:
	bool readTemperatureAsync(float &f,unsigned int ms=0){
		bool b= fsensor.readAsync((TEMPCHANNEL),f,ms);
		tempChanged=fsensor.findByName((TEMPCHANNEL))->hasChanged();
		return b;
	};
	bool readHumidityAsync(float &f,unsigned int ms=0){
		bool b= fsensor.readAsync((HUMCHANNEL),f,ms);
		humChanged=fsensor.findByName((HUMCHANNEL))->hasChanged();
		return b;
	};
};
class SensorFactoryPrototypex86 {
public:
	Sensor* createSensor(String type, String name, std::vector<unsigned int> pins){
		if(type=="HTU21D" || type=="SHT15") {
			Sensor *s=new FakeRHSensor(name,pins);
			s->begin();
			return s;}
		return 0;

	};
} SensorFactory;

#endif

