#ifndef WIFIMAN_H
#define WIFIMAN_H

#ifdef ESP32BUILD
#include <WiFi.h>
#include "esp_wifi.h"	// improve performance
#endif

#ifdef ESP8266BUILD
#include <ESP8266WiFi.h>

#endif

//////////////////////////////////////

class WifiMan {
public:
	static String mode;
	static unsigned limitwaitms;
	static String apssid,appassword,stssid,stpassword;
	static unsigned long previousMillis, interval;

	static String getMode(){return mode;};

	static String getIP(){
		if(mode=="station") return WiFi.localIP().toString();
		if(mode=="AP") return WiFi.softAPIP().toString();
		return "";
	}
	static bool connectAP(const char *nssid,const char *npassword){
		WiFi.mode(WIFI_AP);           //Only Access point
#ifdef ESP32BUILD
		esp_wifi_set_ps (WIFI_PS_NONE);
#endif
#ifdef ESP8266BUILD
		WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
		WiFi.softAP(nssid, npassword);  //Start HOTspot removing password will disable security

		IPAddress myIP = WiFi.softAPIP(); //Get IP address
		Serial.println(String()+"created AP : "+nssid);
		Serial.print("IP address:");
		Serial.println(myIP);
		mode="AP";
		apssid=nssid;
		appassword=npassword;
		return true;
	}

	static bool connectStation(const char *nssid,const char *npassword){
		Serial.printf("Connecting to %s ", nssid);
		Serial.println();
		WiFi.mode(WIFI_STA);
		WiFi.disconnect(true);
#ifdef ESP32BUILD
		esp_wifi_set_ps (WIFI_PS_NONE);
		Serial.println("Wifi set to WIFI_PS_NONE");
		WiFi.waitForConnectResult();
#endif
#ifdef ESP8266BUILD
		WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
		unsigned timestarted=millis();
		WiFi.begin(nssid, npassword);
		while (WiFi.status() != WL_CONNECTED){
			if((millis()-timestarted)>limitwaitms) return false;//connectAP(apssid,"");
			delay(500);
			Serial.print("-");
		}
		mode="station";
		Serial.println(String()+ " connected in "+String((millis()-timestarted)/1000)+" sec");
		Serial.print("IP address: ");
		Serial.println(WiFi.localIP());  //Print the local IP
		stssid=nssid;
		stpassword=npassword;
		return true;
	};

	static bool reconnectIfNeeded(){
		unsigned long currentMillis = millis();
		// if WiFi is down, try reconnecting
		if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >=interval)) {
		  Serial.print(millis());
		  Serial.println("WifiMan::Reconnecting to WiFi...");
		  WiFi.disconnect();
		  WiFi.reconnect();
		  previousMillis = currentMillis;
		  return true;
		}
		return false;
	}
};
String WifiMan::mode, WifiMan::apssid, WifiMan::appassword, WifiMan::stssid, WifiMan::stpassword;
unsigned WifiMan::limitwaitms=30000;
unsigned long WifiMan::interval=30000, WifiMan::previousMillis=0;





#endif
