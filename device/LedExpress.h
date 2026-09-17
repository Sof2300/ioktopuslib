#ifndef LEDEXPRESS_H
#define LEDEXPRESS_H

#include<Ticker.h>
#include<vector>

#include"../easing.h"

#define TICKER_INTERVAL_MS 10

//#define USELEDC

//#define USEHWTIMER

float easeValue(float v, String easingmode){
	return v;
	if(easingmode=="" || easingmode=="linear") return v;
	auto easingFunction = getEasingFunction( EaseInOutExpo );
	if(easingmode=="Sine") easingFunction= getEasingFunction( EaseInOutSine );
	if(easingmode=="Expo") easingFunction= getEasingFunction( EaseInOutExpo );
	if(easingmode=="Quad") easingFunction= getEasingFunction( EaseInOutQuad );
	if(easingmode=="Cubic") easingFunction= getEasingFunction( EaseInOutCubic );
	if(easingmode=="Elastic") easingFunction= getEasingFunction( EaseInOutElastic );
	double result = easingFunction(v);
	return result;
}

struct LedMotion {
	float startintensity=0, endintensity=0;
	unsigned duration=0;
	String easingmode;
	bool reversed=false;
	float outdiff;
	LedMotion(float startintensity0, float endintensity0, unsigned duration0, String easingmode0="", bool reversed0=false) {
		startintensity=startintensity0;endintensity=endintensity0;duration=duration0;easingmode=easingmode0;reversed=reversed0;outdiff=endintensity-startintensity;
	}
	LedMotion() {
		startintensity=0;endintensity=0;duration=1;easingmode="";reversed=0;outdiff=endintensity-startintensity;
	}
	float /*IRAM_ATTR*/ getEasedValue(unsigned startts){
		//short version
		float t=(float)(millis()-startts)/duration;
		if(t>1) return endintensity;
		float te=t;//easeValue(t,easingmode);
		return outdiff*te+startintensity;
		/*
		unsigned now=millis();
		unsigned diff=now-startts;
		float t=(float)diff/duration;
		if(t>1) return endintensity;
		float te=easeValue(t,easingmode);
		float outdiff=endintensity-startintensity;
		float v=outdiff*te+startintensity;
		return v;*/
	}
};

#define LEDC_TIMER_12_BIT 12// use 12 bit precision for LEDC timer
#define LEDC_BASE_FREQ 5000// use 5000 Hz as a LEDC base frequency

#define MAX_LED_SEQ 1024


#ifdef USELEDC
// Arduino like analogWrite value has to be between 0 and valueMax
void ledcAnalogWrite(uint8_t pin, uint32_t value, uint32_t valueMax = 255) {
	// calculate duty, 4095 from 2 ^ 12 - 1
	uint32_t duty = (4095 / valueMax) * min(value, valueMax);

	// write duty to LEDC
	ledcWrite(pin, duty);
// 	Serial.println(String()+"ledcAnalogWrite: duty:"+duty+", value:"+value);
}
void ledcAnalogWrite12bit(uint8_t pin, uint32_t duty) {
	// write duty to LEDC
	ledcWrite(pin, duty);
 	Serial.println(String()+"ledcAnalogWrite12bit: duty:"+duty);
}
#endif


String toprint;
//class LedExpress;
void IRAM_ATTR tickstat(void *);

void IRAM_ATTR donothing(){};

#ifdef USEHWTIMER
hw_timer_t * timer = NULL;      //H/W timer defining (Pointer to the Structure)
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class LedExpress {
	byte pin;

	std::vector<LedMotion> playlist;
	unsigned current=0;
	//LedMotion *currentmotion;
	bool play;
	Ticker ledticker;
	unsigned playts=0;
	unsigned endts=0;
	bool init=false;

public:
	LedExpress(byte ledpin){pin=ledpin;}

	void stopLed(){
#ifdef USELEDC
		ledcAnalogWrite(pin, 0);
#else
		analogWrite(pin, 0);// analogWrite(ledPin, brightness);
#endif
		Serial.println(String()+"analogWrite: duty:"+0);
	}
// should precalculate and then make a track
	unsigned fadecounter=0;
	void /*IRAM_ATTR*/ tick(){//Serial.println("LedExpress tick");
	  	unsigned now=millis();
	 	if(!play) return;
	 	if(now>endts) {
	 		if(current+1>=playlist.size()) {
	 			play=false;
	 			stopTimer();
	 		//	Serial.println("LedExpress playlist ended");
	 			writeOutLed(playlist[current].endintensity);
	 			return;
	 		}	//finished playlist
	 		current++;
			playts=millis();
			endts=playts+playlist[current].duration;
	 	}
	 	float v=playlist[current].getEasedValue(playts);
	 	writeOutLed(v);
	 	//	 	float v2=playlist[current].getEasedValue(playts-TICKER_INTERVAL_MS);

//		float v=(float)(fadecounter%256)/255;
//		fadecounter++;
	}

void writeOutLed(float v){
#ifdef USELEDC
	//	ledcAnalogWrite(pin, 255*v);
 		ledcAnalogWrite12bit(pin, 4095*v);
//		ledcFadeWithInterrupt(pin, 4095*v, 4095*v2, TICKER_INTERVAL_MS, donothing);
//		Serial.println(String()+"analogWrite: ledc duty:"+(4095*v)+", value:"+v+/*", v2:"+v2+*/", millis:"+millis());
#else
		analogWrite(pin, 255*v);// analogWrite(ledPin, brightness);
//		Serial.println(String()+"analogWrite: duty:"+(255*v)+", value:"+v+", fadecounter:"+fadecounter+", millis:"+millis());
//		toprint+=String()+"analogWrite: duty:"+(255*v)+", value:"+v+", millis:"+millis()+"\n";
#endif
	}





	void startTimer(){
#ifdef USEHWTIMER
 		timer = timerBegin(1000000);           	// timer 0, prescalar: 80, UP counting
		timerAttachInterruptArg(timer, &tickstat, this); 	// Attach interrupt
//		timerAttachInterrupt(timer, &tickstat);//, this); 	// Attach interrupt
		timerAlarm(timer, 10000, true, 0);  		// Match value= 1000000 for 1 sec. delay.
	//	timerAlarmEnable(timer);
#else
		ledticker.attach_ms(TICKER_INTERVAL_MS, tickstat,(void *)this);
		Serial.println(String()+"LedExpress started timer");
#endif
	}

	void stopTimer(){
#ifdef USEHWTIMER

#else
		ledticker.detach();
#endif
	}
	void update(){
		Serial.println(String()+"LedExpress update");
		if(!init) {
			pinMode(pin, OUTPUT);
#ifdef USELEDC
			ledcAttach(pin, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT);
#endif
			init=true;

		}
		if(playlist.size()>0 && !play) {
			startTimer();
			play=true;
			playts=millis();
			endts=playts+playlist[current].duration;
		}
	}

	void flash(float intensity, unsigned duration1){
		playlist.push_back(LedMotion(intensity,intensity, duration1));
		update();
	}

	void flashMotion(float startIntensity, float endIntensity, unsigned duration1, String easingmode1="", bool reversed1=false){
		playlist.push_back(LedMotion(startIntensity,endIntensity, duration1, easingmode1, reversed1));
		update();
	}

	void flashPattern(String patternname, unsigned duration1=100, String easingmode="") {
		if(patternname=="wakeup") {
		//	flashMotion(1,0,duration1/8,easingmode);
			flashMotion(0,1,duration1*3/4,easingmode);
			flashMotion(1,1,duration1/8,easingmode);
			flashMotion(1,0.3,duration1/8,easingmode);
		}
		if(patternname=="pulse") {
			flashMotion(0,1,duration1/2,easingmode);
			flashMotion(1,0,duration1/2,easingmode);
		}
		if(patternname=="accelerating pulse") {
			flashMotion(0,1,duration1/2,easingmode);
			flashMotion(1,0,duration1/2,easingmode);
			flashMotion(0,1,duration1/2,easingmode);
			flashMotion(1,0,duration1/2,easingmode);
			flashMotion(0,1,duration1/2,easingmode);
			flashMotion(1,0,duration1/2,easingmode);
			flashMotion(0,1,duration1/2,easingmode);
			flashMotion(1,0,duration1/2,easingmode);
		}
	}
};
unsigned lastms=0;
unsigned callcount=0;
unsigned lastdiff=0;

#ifdef USEHWTIMER
//volatile SemaphoreHandle_t timerSemaphore = xSemaphoreCreateBinary();
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#endif



void tickstat(void *obj){
//	toprint+=String()+"LedExpress tickstat "+callcount+" millis:"+millis()+"\n";
// 	Serial.println(String()+"LedExpress tickstat millis:"+millis());
/*	if(obj) ((LedExpress*)obj)->tick();/*portENTER_CRITICAL_ISR(&timerMux);
 interruptCounter++;
 portEXIT_CRITICAL_ISR(&timerMux);*/
#ifdef USEHWTIMER
 	portENTER_CRITICAL(&timerMux);
#endif
 	callcount++;
	unsigned now=millis();
	lastdiff=(now-lastms);
	lastms=now;
	if(obj) ((LedExpress*)obj)->tick();
#ifdef USEHWTIMER
	portEXIT_CRITICAL_ISR(&timerMux);
#endif
};


#endif
