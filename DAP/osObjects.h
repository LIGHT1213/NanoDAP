#ifndef __osObjects_h__
#define __osObjects_h__

#include "cmsis_os2.h"

#ifdef osObjectsExternal
extern osThreadId_t DAP_ThreadId;
#else
extern osThreadId_t DAP_ThreadId;
       osThreadId_t DAP_ThreadId;
#endif

extern void DAP_Thread (void *argument);

extern void app_main (void *argument);

#endif
/* __osObjects_h__ */
