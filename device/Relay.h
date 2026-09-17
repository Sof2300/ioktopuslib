#ifndef RELAY_H
#define RELAY_H


//////////////////
#define RELAY_ON LOW
#define RELAY_OFF HIGH

class Relay{
	bool state;
	uint8_t pin;
	void applyState(){
		if(state) digitalWrite(pin,RELAY_ON);else digitalWrite(pin,RELAY_OFF);}
public:
	Relay(uint8_t pin0, bool startstate=false){
		pin=pin0;
		pinMode(pin,OUTPUT);
		state=startstate;
		applyState();
	}
	bool getState(){return state;};
	void setState(bool val){if(val!=state){state=val;applyState();}};
	void switchOff(){setState(false);};
	void switchOn(){setState(true);};
};








#endif
