#ifndef BEEPER_H
#define BEEPER_H


#define BUZZER_SEQ_MAX_LENGTH 64

#ifdef ESP32BUILD

//#include <analogWrite.h>

// for esp32
int playing = 0;
void playTone(byte pin, int freq) {
	/*  ledcSetup(0, 2000, 8); // setup beeper
  ledcAttachPin(pin, 0); // attach beeper
  ledcWriteTone(0, freq); // play tone
	 */
	tone(pin, freq);
	playing = pin;
}
void noTone() {
	noTone(playing);
	playing=0;
}
#endif













Ticker vBeep;
bool updir=false, vbeepplaying=false;
int frequency0=0, frequency1=0, vbeepstep=0, current=0;
unsigned vbeepstartts=0;
byte vbeeppin=0;

void playVBeep() {
	if(!vbeepplaying) {vBeep.detach();return;}
	if(current==0) current=frequency0;
	else current+=vbeepstep;
	if(current>frequency1) {vBeep.detach();return;}
	tone(vbeeppin, current);
}

void variableBeep(byte pin, bool goUp, unsigned durationms,int freq0, int freq1) {
	vbeeppin=pin;
	int diff=freq1-freq0;
	vbeepstep=diff/durationms;
	frequency0=freq0;
	frequency1=freq1;
	vbeepplaying=true;
	vBeep.attach_ms(1, playVBeep);
}









////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Buzzer {
	int buzzerpin;
	unsigned long beepts=0, sequencets=0;
	bool active=false;
	float seq[BUZZER_SEQ_MAX_LENGTH], steps=0, tempo=250;
	int currentstep=0, silentsteps=0;
	bool repeat=false;
	//  bool seqplaying=false;
public:
	Buzzer(int buzzerpin0, bool active1=true):buzzerpin(buzzerpin0), active(active1){
		pinMode(buzzerpin, OUTPUT);
	};

	bool isPlaying(){return sequencets || beepts;};

	void beepPassive(float freq=1000, long durationms=50){
		//	Serial.println(String()+"beepPassive freq:"+freq+ " durationms:"+durationms);
		playTone(buzzerpin, freq);
		beepts=millis()+durationms;
	};
	void beepActive(float pwm=1024, long durationms=50){
		if(pwm>=1024) digitalWrite(buzzerpin, HIGH);
		else if(pwm==0) digitalWrite(buzzerpin, LOW);
		else analogWrite(buzzerpin,pwm);
		beepts=millis()+durationms;
	};

	void beep(float param=1024, long durationms=50) {
		if(active) beepActive(param, durationms);
		else beepPassive(param, durationms);
	}

	void playsequencefixedtempo(float *seq1, int steps1, bool repeat1=false, int silentsteps1=0, int durationms=-1){
		for(int i=0;i<steps1;i++) seq[i]=seq1[i];
		steps=steps1;
		repeat=repeat1;
		silentsteps=silentsteps1;
		if(durationms>=0) tempo=durationms;
		Serial.println(String()+"playsequencefixedtempo 0 :"+seq[0]+ " tempo:"+tempo);
		beep(seq[0],tempo);
		//    if(seq[0]>=1024) digitalWrite(buzzerpin, HIGH); else digitalWrite(seq[0], HIGH);
		currentstep=1;
		sequencets=millis()+tempo;
		//    Serial.println(String()+"sequencets :"+sequencets);
	}

	void stopSequence() {sequencets=0;if(beepts>0 && beepts<millis()) {beepts=0; digitalWrite(buzzerpin, LOW);/*Serial.println(String()+"stopSequence");*/}}

	long testts=0;
	void run(){
		/*	if(testts){
		long diff=millis()-testts;
		if(diff>1) Serial.println(String()+"Buzzer::beeper run testts:"+testts+" beepts:"+beepts+" milis():"+millis()+" diff:"+(millis()-beepts));
	}*/
		if(beepts>0 && beepts<millis()) {/*Serial.println(String()+"beeper stopped beepts:"+beepts+" milis():"+millis()+" diff:"+(millis()-beepts));*/
			beepts=0; if(active) digitalWrite(buzzerpin, LOW); else noTone();}
		//if(sequencets>0)
		//   Serial.println(String()+"- sequencets:"+sequencets+", millis()"+millis());
		if(sequencets>0 && sequencets<millis()) {
			//     Serial.println(String()+"current step:"+currentstep);
			if(currentstep>=steps+silentsteps) { // stop or restart
				if(repeat) currentstep=0; else sequencets=0;
			}
			//      Serial.println(String()+"sequencets:"+sequencets+ " millis:"+millis());
			if(sequencets>0) {
				//      Serial.println(String()+"current step:"+currentstep+ " steps:"+steps);
				if(currentstep<steps) {
					//       	  Serial.println(String()+"playsequencefixedtempo "+currentstep+" :"+seq[0]+ " tempo:"+tempo);
					beep(seq[currentstep],tempo);
				}
				currentstep++;
				sequencets=millis()+tempo;
			}
		}
	};
};













////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define BEEPLEVEL 8

#define LOUDLEVEL 1024
#define STARTBEEP 512
#define ENDBEEP 256
#define SEQUENCELENGHT 64
#define BEEPLENGTH 8
float seq[SEQUENCELENGHT];
float beep1[]={LOUDLEVEL,LOUDLEVEL,0,LOUDLEVEL ,0,0,0,0}, beep3[]={256,0,128,0, 0,64,0,32}, beep2[]={32,0,0,128, 0,0,256,0}; //shutdown, no rise, fallback
float beepstart[]={STARTBEEP/128,STARTBEEP/64,STARTBEEP/32,STARTBEEP/16, STARTBEEP/8,STARTBEEP/4,STARTBEEP/2,STARTBEEP};
float beepend[]={ENDBEEP,0,ENDBEEP/2,0, ENDBEEP/4,0,ENDBEEP/8,ENDBEEP/16};
int beepduration1=50, beepduration2=200;

float seq1[]={3,0,3,0, 0,0,0,0,  3,3,3,0};

#define BEEPERPILESIZE 64
#define MINPAUSELENGTH 100

class Beeper{
	int pin;
	Buzzer buzz;
	bool started=false, interrupt=false;
	unsigned long pausets=0;

	float ppile[BEEPERPILESIZE];
	long dpile[BEEPERPILESIZE];
	unsigned int pileend=0, pilestart=0;

	int getPileLength(){
		int pilelength=pileend-pilestart;
		if(pilelength<0) pilelength=-pilelength;
		//		Serial.println(String()+"Beeper:: pileLength :"+pilelength);
		return pilelength;
	}

public:
	Beeper(int pin0, bool active=true):buzz(pin0,active){}

	bool isPlaying(){return buzz.isPlaying();}

	void stop(){
		if(started) buzz.stopSequence();
		started=false;
	}

	void run(){
		buzz.run();

		if(!buzz.isPlaying() && getPileLength()>0) {
			auto now=millis();
			if(!pausets) pausets=now;
			else if((now-pausets)>MINPAUSELENGTH) {
				pausets=0;
				buzz.beep(ppile[pilestart],dpile[pilestart]);
				//		Serial.println(String()+"Beeper:: piling down pilestart:"+pilestart+" param:"+ppile[pilestart]+" "+dpile[pilestart]);
				pilestart++;if(pilestart>=BEEPERPILESIZE) pilestart=0;
			}
		}
	}
	void simpleBeep(float param=1024, long durationms=50){
		if(buzz.isPlaying()){
			if(getPileLength()<BEEPERPILESIZE) {
				ppile[pileend]=param, dpile[pileend]=durationms;
				//			Serial.println(String()+"Beeper:: piling up pileend:"+pileend+" param:"+param+" "+durationms);
				pileend++;if(pileend>=BEEPERPILESIZE) pileend=0;
				return;
			} else {Serial.println("Buzzer still busy playing something else");if(!interrupt) return;}
		}
		// 		Serial.println(String()+"Beeper::simpleBeep Buzzer will play "+param+" "+durationms);
		buzz.beep(param,durationms);
	}

	void repeatBeep(int type, int repeat=1, bool repeatseq=true){
		// 		Serial.println(String()+"repeatBeep:: isPlaying() : "+isPlaying()+" started:"+started);
		if(buzz.isPlaying()) {
			//	Serial.println("Buzzer busy already playing something else");
			return;	//should be postponed instead ?
		}
		//		Serial.println(String()+"repeat : "+repeat);
		//type=1;
		int rep=repeat*BEEPLENGTH;
		for(int i=0;i<SEQUENCELENGHT;i++){
			if(rep>0) {
				//	Serial.println(String()+"repeat2 : "+rep);
				if (type==2) seq[i]=beep2[i%BEEPLENGTH];
				else if (type==3) seq[i]=beep3[i%BEEPLENGTH];
				else if (type==4) seq[i]=beepstart[i%BEEPLENGTH];
				else if (type==5) seq[i]=beepend[i%BEEPLENGTH];
				else seq[i]=beep1[i%BEEPLENGTH];
			} else seq[i]=0;
			rep--;
		}
		//	Serial.print("Beeper : ");
		for(int j=0;j<SEQUENCELENGHT;j++){
			if(j>0) Serial.print(", ");
			Serial.print(String()+seq[j]);
		}
		Serial.println();
		int beepduration=200;
		if(type==2) beepduration=beepduration2;
		else beepduration=beepduration1;
		//		Serial.println(String()+"repeat*BEEPLENGTH : "+repeat*BEEPLENGTH);
		buzz.playsequencefixedtempo(&seq[0], repeat*BEEPLENGTH, repeatseq, repeat*BEEPLENGTH, beepduration);
		// buzz.playsequencefixedtempo(seq1,12,true, 20);
		started=true;
	}
};




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define VBEEPDEFTIME 5
class VBeeper;
void vbeepcb(VBeeper *);

class VBeeper {
	Ticker vbticker;
	bool playing=false;
	//unsigned vbeepstartts=0;
	byte pin=0;
	unsigned timestep;

	struct VBeep {
		float start_freq=0, end_freq=0, freq_step=0, current_freq=0;
		unsigned step_number=0, current_step=0;
		VBeep(){};
		VBeep(float start_freq1, float end_freq1, unsigned step_number0) {
			set(start_freq1, end_freq1, start_freq1,step_number0);
		}
		void set(float start_freq1,float end_freq1, float current_freq1, unsigned step_number1) {
			step_number=step_number1;
			freq_step=((float)end_freq1-(float)start_freq1)/step_number;
			start_freq=start_freq1, end_freq=end_freq1, current_freq=current_freq1;
			if(start_freq==current_freq) current_step=0;
		}
		bool step(){
			if(current_step==0) {current_freq=start_freq;}
			else {current_freq+=freq_step;}
			current_step++;
			//      Serial.println(String()+"step current_step :"+current_step+" step_number:"+step_number);
			if(current_step>step_number) return true; else return false;
		}
	} currentbeep;

	std::vector<VBeep> pile;
public:
	void init(byte pin0, unsigned timestep0=VBEEPDEFTIME) {pin=pin0;timestep=timestep0;}

	void playVBeep() {
		//        Serial.println(String()+"playVBeep current:"+currentbeep.current_freq+" playing:"+playing);
		if(!playing) {
			//       Serial.println(String()+"playVBeep not playing current:"+currentbeep.current_freq+" playing:"+playing);
			vbticker.detach();return;}
		bool endofbeep=currentbeep.step();
		//     Serial.println(String()+"playVBeep end of beep :"+endofbeep);
		if(endofbeep) {
			noTone(pin);
			if(pile.size()>0) {
				VBeep vb=pile[0];
				pile.erase(pile.begin());
				currentbeep=vb;
				//            Serial.println(String()+"playVBeep pop pile, size:"+pile.size());
			} else {
				//              Serial.println(String()+"playVBeep stop playing at all, size:"+pile.size()+" millis:"+millis());
				vbticker.detach();
				playing=false;
				return;
			}
		}
		//       Serial.println(String()+"playVBeep gonna play current_freq:"+currentbeep.current_freq+" on pin:"+pin);
		tone(pin, currentbeep.current_freq);
	}


	void beep(unsigned durationms,int freqstart, int freqend) {
		unsigned stepnumber=durationms /timestep;
		//       Serial.println(String()+"beep stepnumber:"+stepnumber+" millis:"+millis());
		if(playing) {
			pile.push_back(VBeep(freqstart,freqend, stepnumber));
			//          Serial.println(String()+"piling size:"+pile.size()+", beep already playing:"+playing);
			return;
		}
		currentbeep.set(freqstart,freqend, freqstart, stepnumber);
		playing=true;
		vbticker.attach_ms(timestep, vbeepcb, this);
		playVBeep();
	}

	void playSpecial(String beepname){
		//        Serial.println(String()+"playSpecial beepname:"+beepname);
		if(beepname=="allla") {
			beep(100,110,110);
			beep(10,110,220);
			beep(100,220,220);
			beep(10,220,440);
			beep(100,440,440);
			beep(10,440,880);
			beep(100,880,880);
			beep(10,880,1760);
			beep(100,1760 ,1760 );
			beep(10,1760,3520);
			beep(100,3520,3520);
			beep(10, 3520 ,7040 );
			beep(100,7040 ,7040 );
		}
		if(beepname=="beep") {
			beep(500,1760 ,1760 );
		}
		if(beepname=="superhighbeep") {
			beep(200,3520 ,3520 );
		}
		if(beepname=="highbeep") {
			beep(200,3520 ,3520 );
		}
		if(beepname=="funkybeep") {
			beep(20,3520 ,3550 );
			beep(20,3550 ,3520 );
			beep(20,3520 ,3550 );
			beep(20,3550 ,3520 );
			beep(20,3520 ,3550 );
			beep(20,3550 ,3520 );
			beep(20,3520 ,3550 );
			beep(20,3550 ,3520 );
			beep(20,3520 ,3550 );
			beep(20,3550 ,3520 );
		}

		if(beepname=="ready") {
			beep(50,440 ,440 );
		}
		if(beepname=="shortup") {
			beep(200,1760,3520);
		}

		if(beepname=="angryerror") {
			beep(100,3520,3520);
		}

		if(beepname=="shortdown") {
			beep(200,2000,1500);
		}
		if(beepname=="open") {
			beep(300,200,3000);
		}
		if(beepname=="ok") {
			beep(100,3000,3200);
			beep(100,3200,2000);
		}
		if(beepname=="on") {
			beep(1000,1000,5000);
			beep(1000,5000,5000);
		}
		if(beepname=="shortfail") {
			beep(100,2000,600);
			beep(100,600,600);
		}
		if(beepname=="fail") {
			beep(1000,2000,600);
			beep(1000,600,600);
		}
		if(beepname=="doublefail") {
			beep(300,2000,600);
			beep(300,600,600);
			beep(300,2000,600);
			beep(300,600,600);
		}
		if(beepname=="reconnu2") {
			beep(100, 1760, 1568);   // A6  (880 → 1760, 784 → 1568)
			beep(100, 1760, 1568);   // A6  (880 → 1760, 784 → 1568)
			beep(200, 1040, 1040);   // G6  (520 → 1040)
			beep(400, 1040, 2092);   // C7  (1046 → 2092)
			beep(200, 2092, 2092);   // C7  (1046 → 2092)
		}

		if(beepname=="reconnu") {
			// 	beep(100, 1760, 1568);   // A6  (880 → 1760, 784 → 1568)
			beep(200, 1040, 1040);   // G6  (520 → 1040)
			beep(400, 1040, 2092);   // C7  (1046 → 2092)
			beep(200, 2092, 2092);   // C7  (1046 → 2092)

			/*  	beep(100, 880, 784);   // A5  (was A4: 440 → 880, 392 → 784)
        	beep(100, 520, 520);   // G5  (was G4: 260 → 520)
        	beep(200,  1046, 1046); // C6  (was C4: 523 → 1046)
        	beep(100,  1046, 1046); // C6  (was C5: 523 → 1046)*/
		}
		if(beepname=="reconnu3") {
			beep(100, 2080, 1760);
			beep(200, 2080, 2080);   // G6  (520 → 1040)
			beep(400, 2080, 4160);   // C7  (1046 → 2092)
			beep(200, 4160, 4160);   // C7  (1046 → 2092)
		}
		if(beepname=="reconnu4") {
			beep(100, 3080, 2760);
			beep(200, 3080, 3080);   // G6  (520 → 1040)
			beep(400, 3080, 5160);   // C7  (1046 → 2092)
			beep(200, 5160, 5160);   // C7  (1046 → 2092)
		}
		if(beepname=="close") {
			beep(300,3000,200);
		}
		if(beepname=="slow close") {
			beep(1000,3000,200);
		}
		if(beepname=="very slow close") {
			beep(4000,3000,200);
		}

		if(beepname=="full sweep") {
			beep(80,200,1000);
			beep(80,1000,2000);
			beep(80,2000,4000);
			beep(80,4000,8000);
			beep(80,8000,16000);
		}
		if(beepname=="wow") {
			beep(80,200,1000);
			beep(80,1000,2000);
			beep(80,2000,4000);
			beep(80,4000,8000);
			beep(80,8000,16000);
			beep(80,16000,8000);
			beep(80,8000,4000);
			beep(80,4000,2000);
			beep(80,2000,1000);
			beep(80,1000,200);
		}
		if(beepname=="siren") {
			beep(100,1000,2000);
			beep(100,2000,1000);
			beep(100,1000,2000);
			beep(100,2000,1000);
			beep(100,1000,2000);
			beep(100,2000,1000);
			beep(100,1000,2000);
			beep(100,2000,1000);
		}
		if(beepname=="bungee") {
			beep(100,2000,1000);
			beep(100,1000,10000);
			beep(100,10000,1000);
			beep(100,1000,2000);
		}
		if(beepname=="whisper") {
			beep(100,8000,10000);
			beep(100,10000,200);
			beep(100,200,10000);
			beep(100,10000,8000);
		}
		if(beepname=="spring") {
			beep(200,2000,2100);
			beep(100,2100,8000);
			beep(200,8000,9000);
		}
		if(beepname=="agree") {
			beep(200,1000,1100);
			beep(100,1500,1000);
			beep(200,1000,900);
		}
		if(beepname=="takeoff") {
			beep(50,400,4000);
			beep(50,400,6000);
			beep(50,500,7000);
			beep(50,500,8000);
			beep(50,500,9000);
			beep(50,500,10000);
		}
		if(beepname=="slow takeoff") {
			beep(200,400,2000);
			beep(200,400,3000);
			beep(200,400,4500);
			beep(200,400,6750);
			beep(200,400,10125);
		}
		if(beepname=="exclamation") {
			beep(200,400,2000);
			beep(200,7000,150);
			beep(200,3300,3400);
			beep(200,5000,9750);
			beep(200,3000,1125);
		}
	}
	void testbeep(){
		beep(200,400,2000);
		beep(200,7000,150);
		beep(200,3300,3300);
		beep(200,5000,9750);
		beep(200,3000,1125);
	}
};
void vbeepcb(VBeeper *obj) {
	if(!obj) {Serial.println("vbeepcb called with no obj");return;}
	obj->playVBeep();
};



#endif
