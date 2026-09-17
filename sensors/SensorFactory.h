#ifndef SENSORFACTORY_H
#define SENSORFACTORY_H

#include "HTUDriver.h"
#include "Ds18b20Driver.h"

class SensorFactoryPrototype {
public:
	Sensor* createSensor(String type, String name, std::vector<unsigned int> pins){

		if(type=="HTU21D") {
			HTUDriver *s=new HTUDriver(name,pins);
			s->begin();
			s->setHeater(false);
			return s;}

		if(type=="DS18B20") {
			DS18B20Driver *s=new DS18B20Driver(name,pins);
			s->begin();
			return s;}
		return 0;

	};
} SensorFactory;


#endif

