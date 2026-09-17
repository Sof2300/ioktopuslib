#ifndef TIMEUTIL_H
#define TIMEUTIL_H

#include <time.h>


#ifdef x86BUILD
#include <iomanip>
#include <sstream>

extern "C" char* strptime(const char* s,
		const char* f,
		struct tm* tm) {
	// Isn't the C++ standard lib nice? std::get_time is defined such that its
	// format parameters are the exact same as strptime. Of course, we have to
	// create a string stream first, and imbue it with the current C locale, and
	// we also have to make sure we return the right things if it fails, or
	// if it succeeds, but this is still far simpler an implementation than any
	// of the versions in any of the C standard libraries.
	std::istringstream input(s);
	input.imbue(std::locale(setlocale(LC_ALL, nullptr)));
	input >> std::get_time(tm, f);
	if (input.fail()) {
		return nullptr;
	}
	return (char*)(s + input.tellg());
}
#endif

#define SECONDSPERDAY 60*60*24
#define SECONDSPERHOUR 60*60
#define MILLICYCLE 0x100000000/1000	//in seconds

// this is a unix kind time keeping system using uint32 for second since epoch
// adapted to sync the esp to external world time (by redefining the zerotime)


uint32_t now32_zerotime=0;
uint32_t now32_boottime=millis()/1000;	// name looks like a local variable should be changed to Now32BootTime & Now32TimeZone
int now32_ltimezone=+3;			// is it worth to make a Now32 class ? like I did before ? why did I turn to making functions ?
uint32_t now32_lastcyclezero=0;	// maybe because I end up with a global object anyway and only now32 function uses the global variables.
uint32_t now32_lastmillis=0;	// maybe that's the mistake since I update boottime in DataManager and in Server has a var with same name
									// other time functions are unrelated
#define TICKERUPDATE
#ifdef TICKERUPDATE
Ticker keepcycle;
void updateDecycledmillis();
#endif

uint32_t now32decycledmillis() {	// we count that this is called at least twice per cycle or else it looses sync
	uint32_t now=millis();
	if(now<now32_lastmillis) {		// clock cycled
		now32_lastcyclezero+=MILLICYCLE;
#ifdef TICKERUPDATE
		keepcycle.once_ms((uint32_t)MILLICYCLE*1000-now,updateDecycledmillis);	//call again at the next cycle
#endif
	}
	now32_lastmillis=now;
	return now32_lastcyclezero+now/1000;
}
#ifdef TICKERUPDATE
void updateDecycledmillis(){now32decycledmillis();};
#endif

unsigned long now32(){
	return now32_zerotime+now32decycledmillis();//millis()/1000;  //+now32_ltimezone*SECONDSPERHOUR;
}

unsigned long now32raw(){	// without using zero time
	return now32decycledmillis();//millis()/1000;  //+now32_ltimezone*SECONDSPERHOUR;
}

unsigned long now32ZeroTime(){return now32_zerotime;}
unsigned long now32BootTime(){return now32_boottime+now32_zerotime;}


unsigned long now32Local(){return now32() +now32_ltimezone*SECONDSPERHOUR;}
unsigned long now32BootTimeLocal(){return now32BootTime() +now32_ltimezone*SECONDSPERHOUR;}

unsigned long now32TimeZone(){return now32_ltimezone;}
void setTimeZoneNow32(int timezone){ now32_ltimezone=timezone;}

void setNowZerotimeNow32(uint32_t nowsec){
	uint32_t now=now32();
	now32_zerotime=nowsec-now;}	// we suppose its utc we don't need the timezone
void setBoottimeNow32(uint64_t nboottime){now32_boottime=nboottime;}	// in seconds here !! we suppose its utc we don't need the timezone






bool durationFormat(std::string duration){	//format is dd:hh:mm:ss
	std::vector<std::string> v{explode(duration, ':')};
	if(v.size()>=0 && v.size()<=3) return true;
	else return false;
}

uint64_t durationToMS(std::string duration){	//format is dd:hh:mm:ss
	std::vector<std::string> v{explode(duration, ':')};
	std::vector<int> factor={1000,60,60,24};
	uint64_t fduration=0,f=1;
	for(int i=v.size();i>0;i--){
		f*=factor[v.size()-i];
		fduration+=lstoull(v[i-1])*f;
	}
	return fduration;
};



bool isdigit(String str){
	char *ptr;
	//double srcval=
	std::strtod(str.c_str(),&ptr);
	if(*ptr || str.length()==0) return false;
	return true;
};

bool isTimeStamp(String s){
	char sep='/';
	Serial.println(String()+"isTimeStamp "+s);
	int i=s.indexOf(String(sep));
	if(i<1 || i>2) return false;
	Serial.println(String()+"isTimeStamp s1:"+s.substring(0,i));
	if(!isdigit(s.substring(0,i))) return false;
	if(s.substring(0,i).toInt()>31) return false;
	int j=s.indexOf(sep,i+1);
	if(j<i+2 || j>i+2) return false;
	Serial.println(String()+"isTimeStamp s2:"+s.substring(i+1,j));
	if(!isdigit(s.substring(i+1,j))) return false;
	if(s.substring(i+1,j).toInt()>12) return false;

	Serial.println(String()+"isTimeStamp s3:"+s.substring(j+1,s.length()));
	if(!isdigit(s.substring(j+1,s.length()))) return false;
	return true;
}





//int debugcount=0;


String getTimeString(uint32_t sec, bool includedate=true, bool includetime=true, bool includeseconds=true){	// unix timestamp format (seconds since 1980)


	time_t rawtime;
	time (&rawtime);
//	Serial.println(String()+"The current local time is: "+ ctime (&rawtime));

	if(!includedate && !includetime) return "";
	tm * timeinfo;
	timeinfo = localtime ((time_t*)(&sec));		//how does he know what is local time on esp ? does it work or should we rely on client ?
												// an alternative would be to use now32()_timezone
//	auto ct=ctime((time_t*)(&sec));

	time_t tm = sec;
	String date=String(ctime((time_t*)(&tm)));
//	Serial.println(String()+"The current local time 0 is "+ date);
	String year=date.substring(date.length()-5, date.length()-1);
	year=year.substring(2, 4);

//	Serial.println(String()+"The current local time is year :"+ year);
	char buffer [50];
	if(includedate && includetime) {
		if(includeseconds) sprintf(buffer,"%d/%d/%s %02d:%02d:%02d", timeinfo->tm_mday,(timeinfo->tm_mon+1),year.c_str(), timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		else sprintf(buffer,"%d/%d/%s %02d:%02d", timeinfo->tm_mday,(timeinfo->tm_mon+1),year.c_str(), timeinfo->tm_hour, timeinfo->tm_min);
	}
	if(includedate && !includetime) sprintf(buffer,"%d/%d/%s", timeinfo->tm_mday,(timeinfo->tm_mon+1),year.c_str());
	if(!includedate && includetime) {
		if(includeseconds) sprintf(buffer,"%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		else sprintf(buffer,"%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min);
	}
	String timestring=String(buffer);

	return timestring;
}

void altstrptime(const char *in,const char *format, struct tm *ptr){
	if(!ptr) return;
	int day, month, year,hour=0,min=0,sec=0;
	sscanf(in,format, &day, &month, &year, &hour,&min,&sec);
	if(year<50) year+=2000;
	else if(year<100) year+=1900;
	if(year<1971) year=1971;	// limitation due to unix timestamp (should be 1970 though)
	ptr->tm_year=year-1900;
	ptr->tm_mon=month-1;
	ptr->tm_mday=day;
	ptr->tm_hour=hour;
	ptr->tm_min=min;
	ptr->tm_sec=sec;

}
void inittm(struct tm *tmptr){
	tmptr->tm_hour=0;
	tmptr->tm_min=0;
	tmptr->tm_sec=0;
	tmptr->tm_mday=1;
	tmptr->tm_mon=0;
	tmptr->tm_year=71;
	tmptr->tm_isdst=0;
	tmptr->tm_wday=0;
	tmptr->tm_yday=0;
}
long tsFromString(String str, bool dateonly=true){

//	time_t rawtime;	time (&rawtime);
//	Serial.println(String()+"The current local time is: "+ ctime (&rawtime));

/*
	tm * timeinfo;
	timeinfo = localtime ((time_t*)(&rawtime));
	auto ct=ctime((time_t*)(&rawtime));
*//*
	struct tm my_tm;
	char in_buffer[ 80 ] ="26/10/20",	out_buffer[ 80 ];
	time_t t;

	inittm(&my_tm);//	memset (&my_tm, 0, sizeof(struct tm))

	altstrptime(in_buffer, "%d/%d/%d", &my_tm );// Convert the string to a struct tm.

	t = mktime (&my_tm);// Convert the struct tm to a time_t (to fill in the missing fields).

	//	strftime( out_buffer, 80, "That's %D (a %A), at %T",			localtime (&t) );
	Serial.println(String()+" t:"+String((long)t));
	String text=getTimeString((long)t);
	Serial.println(String()+"back:"+text);
*/

	long ret=0;
	if(dateonly) str+=" 00:00:00";	//
	struct tm result;
	inittm(&result);
	altstrptime(str.c_str(), "%d/%d/%d %d:%d:%d",&result);
	bool b=result.tm_isdst;
	ret=mktime(&result);
	if(result.tm_isdst && !b) ret-=3600;	// mktime will add an hour in that case
	if(!result.tm_isdst && b) ret+=3600;	// will mktime substract an hour in that case ?
//	altstrptime(str.c_str(), "%d/%d/%d %d:%d:%d",&result);
//	ret=mktime(&result);// we do it twice because first time isdst is adjusted (and hour eventually modified)

	String text=getTimeString((long)ret);
//		Serial.println(String()+"back:"+text);
	//	Serial.println(String()+"tsFromString:: ret "+ret);
	return ret;
}


String getTimeString(bool includedate=true, bool includetime=true, bool includeseconds=true){
	uint32_t sec = now32();
	return getTimeString(sec,includedate,includetime,includeseconds);
}

#endif
