#ifndef WIFIMAN_H
#define WIFIMAN_H



class WifiMan  {	//dummy class, do nothing, suppose wifi is connected by default
public:
	static String getMode(){return "ST";}
	static void reconnectIfNeeded(){}
	static bool connectStation(String ssid,String password){return true;}
	static bool connectAP(String ssid,String password){return true;}
	static String getIP(){return "192.168.2.6";}
/*	bool notify(std::string, Event*){return false;};
	static void initFromConfig(GenMap *config){
		CLOCK32.resyncSec(millis64()/1000);	//cause up to 1second lag with real time, but simulate NTP resync on esp
		//CLOCK32.resyncMS(millis64());
	};
	static void reconnect(){};
	static void startAP(){};
	static void stopAP(){};
	static void reconnectIfRequired(){};
	static void yield(){};
	*/
};

#endif
