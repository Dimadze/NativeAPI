#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"

#include "sys_dispatch.h"

bool			SysDispatch_Init		= false;
wchar_t			SysDispatch_LastError[256];

Timer1C_List	*tmr1c_list				= NULL;

VOID CALLBACK Sys_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	wchar_t *timer_id = NULL;
	if (timer_id = SysDispatch_Timer_GetTimerID(idEvent))
	{
		Common_ExternalEvent(L"NativeDraw", L"СистемныйТаймер", timer_id);
	}
}

Timer1C *GetTimer1CByTimerID(wchar_t *timer_id)
{
	if (SysDispatch_Init)
		return NULL;

	if (!wcslen(timer_id))
		return NULL;

	Timer1C *tmr1c;

	tmr1c_list->EnumStart();
	
	while (tmr1c = tmr1c_list->EnumNext())
	{
		if (!wcscmp(timer_id, tmr1c->timer_id))
			return tmr1c;
	}

	return NULL;
}

void SysDispatch_Initialization()
{
	if (SysDispatch_Init)
		return;

	tmr1c_list			= new Timer1C_List();
	SysDispatch_Init	= true;
}

void SysDispatch_Deinitialization()
{
	if (!SysDispatch_Init)
		return;

	SysDispatch_Init = false;

	delete tmr1c_list;
}


wchar_t *SysDispatch_Timer_Start(int elapse)
{
	if (!SysDispatch_Init)
		return NULL;

	try
	{
		Timer1C *tmr1c;
		if (tmr1c = tmr1c_list->Add(elapse, Sys_TimerProc))
			return tmr1c->timer_id;
	}
	catch (wchar_t *e)
	{
		wcscpy(SysDispatch_LastError, e);
	}

	return NULL;
}

bool SysDispatch_Timer_Stop(wchar_t *timer_id)
{
	if (!SysDispatch_Init)
		return false;

	Timer1C *tmr1c = GetTimer1CByTimerID(timer_id);

	if (tmr1c)
	{
		tmr1c_list->Remove(tmr1c);
		return true;
	}

	return false;
}

wchar_t *SysDispatch_Timer_GetTimerID(UINT_PTR timer)
{
	if (!SysDispatch_Init)
		return NULL;

	Timer1C *tmr1c;

	tmr1c_list->EnumStart();

	while (tmr1c = tmr1c_list->EnumNext())
	{
		if (tmr1c->timer == timer)
			return tmr1c->timer_id;
	}

	return NULL;
}