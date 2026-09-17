#ifndef BASICSERVER_H
#define BASICSERVER_H

//#include "infrastructure/SimpleFS.h"
#include "../events/DefaultEventEmitter.h"

#include "SimpleWebServer.h"

//#define USEMDNS true
/*
#if USEMDNS
#include <ESPmDNS.h>
#endif
*/
class JavascriptBase {

public:
	String makePage(){
		String ftext="";//<html><style>*{font-family: Verdana;}</style><script>var geid=document.getElementById.bind(document);function loadRemote(t,e,i){var a=new XMLHttpRequest;a.addEventListener(\"load\",e),i&&a.addEventListener(\"error\",i),a.open(\"GET\",t),a.send()}function disenableload(t,e){if(e)for(var i in e)geid(e[i]).disabled=!0;function a(){if(e)for(var t in e)geid(e[t]).disabled=!1}loadRemote(t,a,a)}function setLimit(){disenableload(\"/setLimit?limit=\"+geid(\"limit\").value,[\"limit\"])}function setCal(){disenableload(\"/calibrate?calibrationA=\"+geid(\"cala\").value+\"&offset=\"+geid(\"offset\").value,[\"cala\",\"offset\"])}function startPressed(){\"ON\"==dataobj.relay?disenableload(\"/stop\",[\"startbutton\"]):disenableload(\"/start\",[\"startbutton\"])}function erasePressed(){disenableload(\"/eraseSaved\",[\"erasebutton\"])}function switchWifi(){\"station\"==dataobj.wifi?disenableload(\"/switchToAP\",[\"ssid\",\"password\"]):disenableload(\"/switchToStation\",[\"ssid\",\"password\"])}function showPage(t){var a=document.getElementById(\"content\");function e(t,e){var i=document.createElement(t);i.innerHTML=e,a.appendChild(i)}e(\"p\",\"Volume (ml):<span id=volume style='font-size:120%;font-weight: bold;'>\"+t.volume+\"</span><br><font style='font-size:90%'> Volume Limit (ml): <input size=5 id=limit value='\"+t.limit+\"' style='font-size:120%'> <button onclick='setLimit()'>set</button></font>\"),e(\"p\",\"Average Flow (ml/min):<label id=avg>\"+t.avg+\"</label>\"),e(\"p\",\"Relay : <label id=relay style='font-size:110%;font-weight: bold;'>\"+t.relay+\"</label>\"),e(\"p\",\"<button id=startbutton onclick='startPressed()' style='font-size:110%;''>\"+t.sb+\"</button>\"),e(\"p\",\"<br><font style='font-size:90%'>Calibration factor: <input size=5 id=cala value=\"+t.cala+\"><br>Stop offset (ml):<input id=offset size=5 value=\"+t.offset+\"><button onclick='setCal()'>set</button></font>\"),e(\"p\",\"<br><font style='font-size:70%'>Wifi Switch to <span id=wifitext>\"+t.wifitext+\"</span> <br>ssid :&emsp;&emsp;<input id=ssid>, <br>password :<input id=password><br>mdns name : http://<input id=mdnsname value='\"+t.mdns+\"'>.local <button onclick='switchWifi()'>switch</button></font>\"),e(\"p\",\"<a id=ip style='font-size:80%' href='http://\"+t.ip+\"/'>http://\"+t.ip+\"/</a> / <a id=mdns style='font-size:80%' href='http://\"+t.mdns+\".local/'>http://\"+t.mdns+\".local/</a>\"),e(\"p\",\"<button style='font-size:70%' id=erasebutton onclick='erasePressed()' >erase EEPROM (if exists)</button> \")}function updatePage(t){function e(t,e){var i=geid(t);i.value?dataobj[t]&&i.value!=dataobj[t]||(i.value=e):i.innerHTML=e}e(\"volume\",t.volume),e(\"limit\",t.limit),e(\"avg\",t.avg),e(\"relay\",t.relay),e(\"startbutton\",t.sb),e(\"cala\",t.cala),e(\"offset\",t.offset),e(\"wifitext\",t.wifitext);var i=\"http://\"+t.ip+\"/\";e(\"ip\",i),geid(\"ip\").href=i,e(\"mdns\",\"http://\"+t.mdns+\".local/\")}var init=!1,dataobj={};function startScript(){function t(){var t=(new Date).getTime(),e=geid(\"volume\");e&&(e.style.color=\"lightgray\"),loadRemote(\"/data.json\",function(){console.log(\"load delay:\"+((new Date).getTime()-t)/1e3),function(){console.log(this.responseText);var t=this.responseText,e=JSON.parse(t),i=geid(\"volume\");i&&(i.style.color=\"black\"),1==e.relay?(e.relay=\"ON\",e.sb=\"Stop\"):(e.relay=\"OFF\",e.sb=\"Start\"),\"station\"==e.wifi?e.wifitext=\"AP\":e.wifitext=\"station\",init?updatePage(e):(showPage(e),init=!0),dataobj=e}.bind(this)()})}t();setInterval(t,3e3)}</script><body onload=\"startScript()\"><h2>Flow Control</h2><div id=\"content\"></div></body></html>";
		return ftext;
	}
};


class BasicServer : public DefaultEventEmitter {
	SimpleWebServer server;
	String mdns_name;
	JavascriptBase jsbase;

	char headers[10];
	unsigned long boottime=0;

public:
	BasicServer(String mdns):server(80), mdns_name(mdns){}

	void begin(){
		Serial.println("BasicServer::begin");
/*
#if USEMDNS
		bool mdnsstarted=false;
		if(mdns_name.length()==0) mdns_name="EspServer";
		mdnsstarted=MDNS.begin(mdns_name.c_str());
		Serial.println(String()+"mdns started with name : "+mdns_name);

#endif
*/
		auto that=this;

		//	server.server.onNotFound(handlenotfoundstat);
		//	server.on("/",HTTP_GET,handlerootstat);
		server.onNotFound([that]() {                          // Handle when user requests a file that does not exist
			//that->server.server.client().setNoDelay(true);
		//	that->server.server.client().soTimeout = 1000
		//	that->server.server.client().tcpNoDelay = true;
			//	Serial.println(String()+"onNotFound "+that->mdns_name);
			if (!that->handleRootPath(that->server.uri().c_str())) that->server.send(404, ("text/plain"), GenString()+that->server.uri()+" : File Not Found");
		});
		server.on("/",HTTP_GET, [that]() {
			//that->server.server.client().setNoDelay(true);
			//	Serial.println(String()+"on "+that->mdns_name);
			that->handleRootPath(that->server.uri().c_str());

		});    //Associate the handler function to the path

	//	server.server.serveStatic("/", LITTLEFS, "/");

		const char * headerkeys[] = {"x-BootTime","Accept","x-Date"} ;
		size_t headerkeyssize = sizeof(headerkeys)/sizeof(char*);
		//ask server to track these headers
		server.collectHeaders(headerkeys, headerkeyssize );

		server.begin(mdns_name.c_str());                    //Start the server
		Serial.println("Server listening");
		/*
#if USEMDNS
		if(mdnsstarted) MDNS.addService("http", "tcp", 80);
#endif
*/

	}
/*#if USEMDNS
	void renameMDNS(String newmdnsname){
		if(newmdnsname.length()>0){ MDNS.setInstanceName(newmdnsname.c_str());mdns_name=newmdnsname;}
	}
#endif
*/
	String getArgument(String argname){
		auto args=server.getArguments();
		if(args.find(argname.c_str()) != args.end()) return (args[argname.c_str()]).c_str();
		return "";
	}

	void printHeaders(){
		int s=server.headers();
		for (int i=0;i<s;i++){
			String k=server.headerName(i);
			String v=server.header(i);
			Serial.println(String()+"BasicServer::printHeaders: "+k+" "+v);
		}
	}

	void checkHeaders(){
//		printHeaders();
		if(server.hasHeader("x-Date")) {	// resync server time from client // not implemented yet
			String str=server.header("x-Date").c_str();
		//	Serial.println(String()+"found resync header: "+str);
			unsigned long nboottime=str.toInt()-millis()/1000;
		//	Serial.println(String()+"nboottime: "+nboottime+" str.toInt():"+str.toInt());

			if( (nboottime+5)<boottime || (nboottime-5)>boottime) {
				StringMapEvent nmap;
				nmap.insertValue("boottime",String(nboottime).c_str());
				//bool b=
				emit("boottime", &nmap);
				boottime=nboottime;
			};
		}
		if(server.hasHeader(("x-BootTime"))) server.sendHeader(("x-BootTime"), tostring(boottime));//	 println("IOServerv01::handleRequest: x-BootTime true");
	}

	bool handleRootPath(String path) {            //Handler for the rooth path
		Serial.println(String()+"requested path :"+path);
		checkHeaders();

		StringMapEvent nmap;
		auto args=server.getArguments();
		for(auto k : args) {nmap.insertValue(k.first.c_str(),k.second.c_str());Serial.println(String()+"BasicServer::handleRootPath argument :"+k.first.c_str()+" = "+k.second.c_str());}

#if USEMDNS
		if(nmap.exists("mdnsname")) {server.setMdnsName(nmap.values["mdnsname"].c_str());}
#endif
		bool b=emit(path.c_str(), &nmap);

		if(b && nmap.exists("response")) {
			if(nmap.exists("type")) server.send(200, nmap.values["type"], nmap.values["response"]);
			else server.send(200, "application/json", nmap.values["response"]);
			return true;
		}
	//	Serial.println(String()+"sending file :"+path);
		bool ok=handleFileRead(path);
		if(!ok) server.send(404, "text/html", "File not found");
		return true;
		if(!ok) {
			Serial.println("filenot found, sending basic page");
			String ftext=jsbase.makePage();
			server.send(200, "text/html", ftext.c_str());
		}

		return true;
	}



	void yield(){
		server.handleClient();
/* #if MDNSUSE
		MDNS.update();
#endif*/
	}

	String getContentType(String filename) {
		yield();
		if (server.hasArg("download"))      return "application/octet-stream";
		else if (filename.endsWith(".htm")) return "text/html";
		else if (filename.endsWith(".html"))return "text/html";
		else if (filename.endsWith(".css")) return "text/css";
		else if (filename.endsWith(".js"))  return "application/javascript";
		else if (filename.endsWith(".png")) return "image/png";
		else if (filename.endsWith(".gif")) return "image/gif";
		else if (filename.endsWith(".jpg")) return "image/jpeg";
		else if (filename.endsWith(".ico")) return "image/x-icon";
		else if (filename.endsWith(".xml")) return "text/xml";
		else if (filename.endsWith(".pdf")) return "application/x-pdf";
		else if (filename.endsWith(".zip")) return "application/x-zip";
		else if (filename.endsWith(".gz"))  return "application/x-gzip";
		else if (filename.endsWith(".svg")) return "image/svg+xml";
		return "text/plain";
	}


	bool handleFileRead(String path) {		//  path="/raphael-2.1.4.min.js.gz";
		Serial.println(String()+"handleFileRead: " + path);

		if (path.endsWith("/"))	path += "index.html";
		String contentType = getContentType(path);

		String pathWithGz = path + ".gz";
		//	Serial.println("PathFile: " + pathWithGz);

		if (CurFS.exists(pathWithGz.c_str()) || CurFS.exists(path.c_str()))
		{
			if (CurFS.exists(pathWithGz.c_str())) path += ".gz";
			//size_t sent =
	//		Serial.println(String()+"will stream path :"+path.c_str());
			server.streamFile(path.c_str(), contentType.c_str());
			return true;
		}

		yield();

		return false;
	}
};



#endif
