#ifndef SENSOR_H
#define SENSOR_H

//#include "HTUDriver.h"

#define DEFTICKMS 20

class SensorListener {
public:
	virtual void notify(unsigned channel)=0;
};

class Calibration {
	String channelname;
	float factor=1,linear=0;
public:
	Calibration(String channel, float factor0=1, float linear0=0):channelname(channel),factor(factor0), linear(linear0){};
	String getChannel(){return channelname;}
	float getValue(float value){ return value*factor+linear;}

	static float getValueFromList(std::vector<Calibration> &tab, String &channel, float val){
		for(Calibration cal : tab){
//			Serial.println(String()+"Calibration::getValue "+cal.getChannel()+" "+channel);
			if(cal.getChannel()==channel){return cal.getValue(val);}
		}
		return val;
	}
};

class Sensor;
void sensortickstat(Sensor *);

class Sensor {
protected:
	String name;
	std::vector<unsigned int> pins;
	SensorListener *sensorListener;

	std::vector<Calibration> calibrations;
//	bool debug=false;
	Ticker ticker;
	bool disconnected=true;

public:
	Sensor(String name0, std::vector<unsigned int> pins0):name(name0),pins(pins0){}
	virtual ~Sensor(){}
	virtual bool begin(){return false;}
	virtual bool isReady(){return false;}
	virtual bool isDisconnected(){return disconnected;}
	float getValue(String channel){return Calibration::getValueFromList(calibrations, channel, get(channel));};
	virtual float get(String channel)=0;		// get value
	virtual String getUnit(String channel)=0;
//	read(bool repeatedly=true,long atintervalms=300) {	}
	virtual void setCalibration(String channel, float factor, float linear){
		Calibration cal=Calibration(channel,factor,linear);
		calibrations.push_back(cal);
	}
	void setListener(SensorListener *listener){ sensorListener=listener;}
	virtual unsigned getChannelNumber ()=0;
	virtual String getChannelName (unsigned )=0;

	virtual bool tick(){return false;}
	virtual void selfTick(unsigned ms=DEFTICKMS){
		ticker.attach_ms(ms,sensortickstat,this );
	}
	virtual void startReading(unsigned ms=DEFTICKMS) {selfTick(ms);}
	virtual void stopSelfTick(){ticker.detach();}
	virtual void stopReading() {stopSelfTick();}



};
void sensortickstat(Sensor *sensor){sensor->tick();};


class TempRHSensor : public Sensor {
protected:
	float temp, hum;

public:
	TempRHSensor(String name, std::vector<unsigned int> pins0):Sensor(name,pins0){}
	float get(String channel){
		if(channel=="Temperature") return temp;
		if(channel=="Humidity") return hum;
		return NAN;
	}
	virtual String getUnit(String channel){
	//	Serial.println(String()+"getUnit "+channel);
		if(channel=="Temperature") return "°C";
		if(channel=="Humidity") return "%";
		return "";
	}
	virtual unsigned getChannelNumber (){return 2;};
	virtual String getChannelName (unsigned channel){
		if(channel==0) return "Temperature";
		if(channel==1) return "Humidity";
		return "";
	};

};


class TempSensor : public Sensor {
protected:
	float temp;
	bool disconnected=true;
public:
	TempSensor(String name, std::vector<unsigned int> pins0):Sensor(name,pins0){}
	float get(String channel){
		if(channel=="Temperature") return temp;
		return NAN;
	}
	virtual String getUnit(String channel){
		if(channel=="Temperature") return "°C";
		return "";
	}
	virtual unsigned getChannelNumber (){return 1;};
	virtual String getChannelName (unsigned channel){
		if(channel==0) return "Temperature";
		return "";
	};

};


#endif

