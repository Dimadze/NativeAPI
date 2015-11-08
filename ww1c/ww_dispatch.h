#ifndef __WINDOWSWORK_DISPATCH_H__

#include "windows.h"

typedef struct
{
	bool	result;
	HWND	hwnd;
	HWND	hwnd_main;
	DWORD	tid;
	bool	enum_child;
	wchar_t title[1024];
	wchar_t class_name[1024];
	wchar_t window_name[1024];
}Window1C;

void WindowsWorkDispatch_Initialization();
void WindowsWorkDispatch_Deinitialization();

bool WindowsWorkDispatch_GetMainWindowID(wchar_t *ret_window_id);
bool WindowsWorkDispatch_GetWindowIDByTitle(wchar_t *ret_window_id, wchar_t *title);

bool WindowsWorkDispatch_IsChildWindow(wchar_t *window_id_owner, wchar_t *window_id_child);
bool WindowsWorkDispatch_GetCoordinates(wchar_t *window_id, int *x1, int *y1, int *x2, int *y2);
bool WindowsWorkDispatch_ToClientCoordinates(wchar_t *window_id, int *x, int *y);
bool WindowsWorkDispatch_Move(wchar_t *window_id, int x, int y, int w, int h);
bool WindowsWorkDispatch_Action(wchar_t *window_id, wchar_t *action);

#endif //__WINDOWSWORK_DISPATCH_H__
