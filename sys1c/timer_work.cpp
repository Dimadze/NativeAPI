#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"

#include "timer_work.h"


const wchar_t	Timer1C::TIMER[] = L"ТАЙМЕР";

Timer1C::Timer1C(int elapse, TIMERPROC timer_proc)
{
	if (!(elapse > 0))
		throw L"Timer1C::Timer1C - Неверное значение таймера";

	if (!timer_proc)
		throw L"Timer1C::Timer1C - Не указана процедура для таймера";

	if (!(timer = SetTimer(NULL, NULL, elapse, timer_proc)))
		throw L"Timer1C::Timer1C - Не удалось создать таймер перерисовки";
	else
		wsprintf(timer_id, L"%s.%08X", TIMER, this);
}

Timer1C::~Timer1C()
{
	if (KillTimer(NULL, timer))
		timer = NULL;
}