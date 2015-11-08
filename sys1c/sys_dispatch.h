#ifndef __SYS_DISPATCH_H__

#include "windows.h"
#include "timer_work.h"

extern bool Common_ExternalEvent(wchar_t *source, wchar_t *eventname, wchar_t *data);

void SysDispatch_Initialization();
void SysDispatch_Deinitialization();

wchar_t *SysDispatch_Timer_Start(int elapse);
bool SysDispatch_Timer_Stop(wchar_t *timer_id);

wchar_t *SysDispatch_Timer_GetTimerID(UINT_PTR timer);

#endif //__SYS_DISPATCH_H__
