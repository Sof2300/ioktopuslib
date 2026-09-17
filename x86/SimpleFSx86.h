#ifndef SIMPLEFS_H
#define SIMPLEFS_H

#ifdef x86BUILD

#include "CurFSx86.h"
#define SIMPLEFS CurFSx86
#define CurFS SIMPLEFS
#endif


#define SimpleFS SIMPLEFS

#endif
