#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"

#include "ww_dispatch.h"

//const wchar_t	WINDOWSWORK_WINDOW[] = L"ОКНО";

#define WINDOWSWORK_WINDOW	L"ОКНО"

BOOL CALLBACK WindowsWorkDispatch_EnumChildWindowsProc(HWND hwnd, LPARAM lParam)
{
	Window1C *win1c = (Window1C *)lParam;

	wchar_t title[1024];
	GetWindowText(hwnd, title, sizeof(title));

	if (!wcscmp(title, win1c->title))
	{
		win1c->hwnd = hwnd;
		return false;
	}
	else
		return true;

}

BOOL CALLBACK WindowsWorkDispatch_EnumChildWindowsProc2(HWND hwnd, LPARAM lParam)
{
	Window1C *win1c = (Window1C *)lParam;

	if (!win1c->result)
	{
		EnumChildWindows(hwnd, WindowsWorkDispatch_EnumChildWindowsProc2, lParam);

		if (win1c->hwnd == hwnd)
		{
			win1c->result = true;
			return false;
		}
	}

	return true;
}

BOOL CALLBACK WindowsWorkDispatch_EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	wchar_t class_name[512];
	Window1C *win1c = (Window1C *)lParam;

	GetClassName(hwnd, class_name, sizeof(class_name));

	if (!wcscmp(class_name, win1c->class_name) && win1c->tid ==	GetWindowThreadProcessId(hwnd, NULL))
	{
		win1c->hwnd		= hwnd;

		if (win1c->enum_child)
		{
			win1c->hwnd_main	= hwnd;

			if (!EnumChildWindows(win1c->hwnd_main, WindowsWorkDispatch_EnumChildWindowsProc, lParam))
				return false;
		}
		else
			return false;
	} 

	return true;	
}

HWND WindowsWorkDispatch_GetHWNDByWindowID(wchar_t *window_id)
{
	HWND res;

	if (!window_id)
		return NULL;

	swscanf(window_id, WINDOWSWORK_WINDOW".%08X", (unsigned int *)&res);
	return res;
}

bool WindowsWorkDispatch_GetWindowIDByHWND(wchar_t *window_id, HWND hwnd)
{
	if (!window_id)
		return false;

	wsprintf(window_id, WINDOWSWORK_WINDOW".%08X", hwnd);
	return true;
}

void WindowsWorkDispatch_Initialization()
{

}

void WindowsWorkDispatch_Deinitialization()
{

}

bool WindowsWorkDispatch_GetMainWindowID(wchar_t *ret_window_id)
{
	Window1C win1c;

	if (!ret_window_id)
		return false;

	wcscpy(ret_window_id, L"");
	wcscpy(win1c.class_name, L"V8TopLevelFrame");
	win1c.enum_child	= false;
	win1c.tid			= GetCurrentThreadId();

	if (!EnumWindows(WindowsWorkDispatch_EnumWindowsProc, (LPARAM)&win1c))
		return WindowsWorkDispatch_GetWindowIDByHWND(ret_window_id, win1c.hwnd);
	else
		return false;
}

bool WindowsWorkDispatch_GetWindowIDByTitle(wchar_t *ret_window_id, wchar_t *title)
{
	Window1C win1c;

	if (!title)
		return false;

	if (!ret_window_id)
		return false;

	wcscpy(ret_window_id, L"");
	wcscpy(win1c.title, title);
	wcscpy(win1c.class_name, L"V8TopLevelFrame");
	win1c.tid			= GetCurrentThreadId();

	if (!EnumWindows(WindowsWorkDispatch_EnumWindowsProc, (LPARAM)&win1c))
		return WindowsWorkDispatch_GetWindowIDByHWND(ret_window_id, win1c.hwnd);
	else
		return false;
}

bool WindowsWorkDispatch_IsChildWindow(wchar_t *window_id_owner, wchar_t *window_id_child)
{
	HWND hwnd_owner, hwnd_child;
	Window1C win1c;

	if (!(hwnd_owner = WindowsWorkDispatch_GetHWNDByWindowID(window_id_owner)))
		return false;

	if (!(hwnd_child = WindowsWorkDispatch_GetHWNDByWindowID(window_id_child)))
		return false;

	win1c.hwnd		= hwnd_child;
	win1c.hwnd_main	= hwnd_owner;
	win1c.result	= false;

	EnumChildWindows(win1c.hwnd_main, WindowsWorkDispatch_EnumChildWindowsProc2, (LPARAM)&win1c);
	return win1c.result;
}

bool WindowsWorkDispatch_GetCoordinates(wchar_t *window_id, int *x1, int *y1, int *x2, int *y2)
{
	if (!window_id)
		return false;

	HWND hwnd;

	if (hwnd = WindowsWorkDispatch_GetHWNDByWindowID(window_id))
	{
		RECT rc;
		GetWindowRect(hwnd, &rc);

		if (x1)
			*x1 = rc.left;
		if (y1)
			*y1 = rc.top;
		if (x2)
			*x2 = rc.right;
		if (y2)
			*y2 = rc.bottom;

		return true;
	}
	else return false;
}

bool WindowsWorkDispatch_ToClientCoordinates(wchar_t *window_id, int *x, int *y)
{
	if (!window_id)
		return false;

	HWND hwnd;

	if (hwnd = WindowsWorkDispatch_GetHWNDByWindowID(window_id))
	{

		if (x && y)
		{
			POINT p;

			p.x = *x;
			p.y = *y;

			ScreenToClient(hwnd, &p);

			*x = p.x;
			*y = p.y;

			return true;
		}
	}

	return false;
}

bool WindowsWorkDispatch_Move(wchar_t *window_id, int x, int y, int w, int h)
{
	if (!window_id)
		return false;

	HWND hwnd;

	if (hwnd = WindowsWorkDispatch_GetHWNDByWindowID(window_id))
		if (MoveWindow(hwnd, x, y, w, h, true))
			return true;

	return false;
}

bool WindowsWorkDispatch_Action(wchar_t *window_id, wchar_t *action)
{
	if (!window_id)
		return false;

	if (!action)
		return false;

	HWND hwnd;

	if (hwnd = WindowsWorkDispatch_GetHWNDByWindowID(window_id))
	{
		if (!wcsicmp(action, L"Свернуть")				|| !wcsicmp(action, L"Minimize"))
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
		else if(!wcsicmp(action, L"Развернуть")			|| !wcsicmp(action, L"Maximize"))
			SendMessage(hwnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		else if (!wcsicmp(action, L"Восстановить")		|| !wcsicmp(action, L"Restore"))
			SendMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);
		else
			return false;

		return true;
	}

	return false;
}

