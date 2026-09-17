#ifndef EVENTEMITTER_H
#define EVENTEMITTER_H

#include "Event.H"

class EventListener {
public:
	virtual bool notify(GenString ename,Event*event=0)=0;//{return false;};
};


class EventEmitter {	// interface for eventemitter independent from implementation
public:
	EventEmitter(void){};
	virtual ~EventEmitter(){};
	virtual void on(GenString ename, EventListener*)=0;	// subscribe
	virtual void once(GenString ename, EventListener*)=0;	// subscribe to next event only
	virtual void removeListener(GenString ename, EventListener*)=0;	// subscribe

	virtual bool emit(GenString ename, Event*event=0)=0;// Returns true if the event had listeners, false otherwise.
			// if we move to async we should use event instead *event
	virtual void on(EventListener *er){on("",er);};
	virtual bool emit(Event*e){return emit("",e);};
};

class StringListener : public EventListener {
public:
	bool notify(GenString ename, Event *e){
		if(ename[ename.length()-1]=='/') ename=ename.substr(0,ename.length()-1);
		StringEvent *se=0;
		if(e->isClassType("StringEvent")) se=(StringEvent *)e;
		if(se==0) return false;
		return notify(String(ename.c_str()),se);
	}
	virtual bool notify(String ename, StringEvent *e)=0;
};

class StringMapListener : public EventListener {
public:
	bool notify(GenString ename, Event *e){
		if(ename[ename.length()-1]=='/') ename=ename.substr(0,ename.length()-1);
		StringMapEvent *se=0;
		if(e->isClassType("StringMapEvent")) se=(StringMapEvent *)e;
		if(se==0) return false;
		return notify(String(ename.c_str()),se);
	}
	virtual bool notify(String ename, StringMapEvent *e)=0;
};
#endif
