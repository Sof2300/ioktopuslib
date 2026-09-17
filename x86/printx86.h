#ifndef PRINTx86_H
#define PRINTx86_H

#include<iostream>

#include<math.h>


//#define to_string std::to_string

template <typename T>
inline void print(T str) {	std::cout << str ;}
template <typename T>
inline void println(T str){std::cout << str << std::endl;}
template <typename T>
inline void renprintln(T str){std::cout << str << std::endl;}
template <typename T>
inline void println(T str, int i){println(str);}

inline void println() {std::cout << std::endl;}





#define DEC 1
//static inline void initPrint(){}

static std::string tostringl(uint64_t num){
	uint8_t i = 0;  uint64_t n = num;
	char str[24];//should be 21
	do i++;
	while ( n /= 10 );
	str[i] = '\0';
	n = num;
	do str[--i] = ( n % 10 ) + '0';
	while ( n /= 10 );

	return std::string(str);
};


static std::string replaceAll2(std::string src, std::string pattern,std::string newval){
	std::string::size_type n = 0;
	while ( ( n = src.find( pattern, n ) ) != std::string::npos ){
		src.replace( n, pattern.size(), newval );
		n += pattern.size();
	}
	return src;
}

#include <math.h>
static std::string doubleToString(double d,int decimals=2){
	bool neg=false;
	if(d<0) {neg=true;d=-d;}
	long int l=1;
	for(int i=decimals;i>0;i--) l=l*10;
	l=round(l*d);
	std::string str=std::to_string(l);
	int sl=str.size();
	while(sl<=decimals) {str="0"+str;sl=str.size();}
	str=str.substr(0,sl-decimals)+"."+str.substr(sl-decimals,sl);
	for(int i=decimals;i>0;i--)
		if(str.substr(str.size()-1,str.size())=="0")
			str=str.substr(0,str.size()-1);
	if(str.substr(str.size()-1,str.size())==".") str=str.substr(0,str.size()-1);
	if(neg && str!="0") str="-"+str;
	return str;
}

inline int stdStringtoInt(std::string str) {return std::stoi(str);}

class String {
	std::string str;
public:

	String(const char *strptr):str(strptr) {}
	String(char c){str=c;}
	String(double d){str=doubleToString(d);}
	String(unsigned int strint) {str=tostringl(strint);}
	String(unsigned long int strint) {str=tostringl(strint);}
	String(uint64_t strint) {str=tostringl(strint);}
	String(int strint) {str=tostringl(strint);}
	String(long int strint) {str=tostringl(strint);}

	String(std::string str0):str(str0) {}
	String(){}
	const char *c_str(){return str.c_str();}
	char operator[](unsigned int i) const {return str[i];}
	String operator+(String nstr) const {return String(str+nstr.getStr());}
	String operator+(const char *strptr) const {return String(str+std::string(strptr));}
	String operator+(const char *strptr) {return String(str+std::string(strptr));}
	String operator+(char c) {return String(str+c);}
	String operator+(int i)  {return String(str+std::to_string(i));}
	String operator+(unsigned int i) {return String(str+std::to_string(i));}
	String operator+(unsigned long i) {return String(str+std::to_string(i));}
	String operator+(float i) {return String(str+doubleToString(i));}
	String operator+(double d) {return String(str+doubleToString(d));}
	String operator+(uint64_t strint) {return String(str+tostringl(strint));}

	//	String operator+(String &str2,const char *strptr) {return String(std::string(strptr)+str2.getStr());}
	//	const char* operator+(String nstr)  {return String(str+nstr.getStr()).c_str();}
	String &operator+=(String nstr) {str=str+nstr.getStr();return *this;}
	String &operator+=(const char *strptr) {str=str+std::string(strptr);return *this;}
	bool operator<(String str2) {return str<str2.getStr();}
	bool operator<(const String &str1) const {
		String str2=str1;
		return str<str2.getStr();}
	//	bool operator==(String str2) {return str==str2.getStr();}
	bool operator==(String str2) const {
		String str1=str2;
		return str==str1.getStr();}

	bool operator!=(const String str2) {
		String str1=str2;
		return str!=str1.getStr();}
	float toFloat(){return atof(str.c_str());}
	int toInt(){ return stdStringtoInt(str);}
	unsigned char charAt(unsigned int i){return str[i];}
	void remove(int start, int length) {str.erase(start,length);}
	std::string getStr(){return str;};
	int indexOf(String tofind, int startindex=0) {
		std::string::size_type loc =str.find(tofind.getStr(),startindex);
		if( loc == std::string::npos ) return -1;
		else return loc;
	}
	int lastIndexOf(String tofind, int startindex=-1) {
		if(startindex==-1) startindex=length();
		std::string::size_type loc =str.find_last_of(tofind.getStr(),startindex);
//		string::size_type loc2 =str.find_last_of("(",length());
		if( loc == std::string::npos ) return -1;
		else return loc;
	}
	void trim(){
		std::string str2=str;
		int f=str.find_first_not_of(' ');
		if(f>0) str2.erase(0, f);       //prefixing spaces
		size_t l=str.find_last_not_of(' ');
		if(l!=std::string::npos && l<str2.size()) str2.erase(l+1);         //surfixing spaces
		str=str2;
	}
	//const char *c_str(){return str.c_str();}
	//std::string getStdString(){return str};
	void replace(String pattern, String newval){replaceAll2(str,pattern.c_str(),newval.c_str());}

	//	int indexOf(const char *strptr, int startindex=0) {return indexOf(String(strptr),startindex);}

	String substring(int start, int end=-1){
		if(end<0) end=str.size();
		return str.substr(start,end-start);}
	unsigned length(){return str.size();}

	bool endsWith(String ending){
			if (str.length() >= ending.length()) {
				return (0 == str.compare (str.length() - ending.length(), ending.length(), ending.c_str()));
			} else return false;

	}
};

static class SerialPrinter {
public:
	void begin(unsigned){};
	void println (const char *c){std::cout << c<<std::endl;}
	void println (String c){println(c.c_str());}
	void println (){std::cout << std::endl;}

	void print (const char *c){std::cout << c;}
	void print (String c){print(c.c_str());}

} Serial;

/*
class String {
protected:
	std::string str;
public:
	std::string getStr(){return str;}
	const char *c_str(){return str.c_str();}

	String(){}
	String(float str0):str(to_string(str0)) {}
	template <typename T> String(T str0):str(str0) {}

	unsigned char charAt(unsigned int i){return str[i];}
	unsigned char operator[](unsigned int i){return charAt(i);}

	String operator+=(String other){
		str+=other.str;
		return *this;
	}
	String operator+(String other){return String(str+other.str);}
	bool operator!=(String other){return str!=other.str;}
	bool operator==(String other){return str==other.str;}
	const bool operator==(String other) const{return str==other.str;}

	void remove(unsigned int from, unsigned int to){str.erase(from,to-from);}

	unsigned int length(){return str.size();}

	float toFloat(){return atof(str.c_str());}

	int indexOf(String s,int start=0){
		std::size_t found=str.find(s.str,start);
		if (found!=std::string::npos) return found;
		else return -1;
	}
	int lastIndexOf(String s,int start=0){
		std::size_t found=str.find_last_of(s.str,start);
		if (found!=std::string::npos) return found;
		else return -1;
	}

	String substring(int start=0,int end=-1){
		if(end==-1) end=str.size();
		return String(str.substr(start,end-start));
	}

	void trim(){
		// trim trailing spaces
		size_t endpos = str.find_last_not_of(" \t");
		size_t startpos = str.find_first_not_of(" \t");

		if( std::string::npos != endpos) str = str.substr( 0, endpos+1 );
		str = str.substr( startpos );
	}

	bool replace(const String& from, const String& to) {
	    size_t start_pos = str.find(from.str);
	    if(start_pos == std::string::npos)
	        return false;
	    str.replace(start_pos, from.str.size(), to.str);
	    return true;
	}


};



class SerialEmulator {
public:
	void begin(int) {}
	void println(const char *s) {renprintln(s);}
	void println(std::string s) {renprintln(s);}
	void println(String s) {renprintln(s.getStr());}
} Serial;
*/

#endif
