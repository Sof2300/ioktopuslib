#ifndef SIMPLEFS_H
#define SIMPLEFS_H

#ifdef ESP32BUILD
#define SIMPLEFS SIMPLEFSESP32
#include "SimpleFSEsp32.h"
#endif

#ifdef ESP8266BUILD
#define SIMPLEFS SIMPLEFSESP8266
#include "SimpleFSEsp8266.h"
//#define SIMPLEFS SIMPLEFSESP8266
//#include "SimpleFSEsp8266.h"
#endif

#define SimpleFS SIMPLEFS

#endif
