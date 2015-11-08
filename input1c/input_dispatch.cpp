#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"

#include "input_dispatch.h"
#include "bcbuffer.h"

#define HOOK_WINDOW		L"ОКНО"

HHOOK	hook			= NULL;

CircularBuffer	external_events;

bool	hook_keyboard_keys[1000][2];
bool	hook_mouse_messages[100][2];

bool	hook_keyboard_active	=	false;
bool	hook_mouse_active		=	false;

HANDLE	hook_thread;



DWORD WINAPI Input_Thread(CONST LPVOID lpParam)
{
	SuspendThread(hook_thread);

	while (true)
	{
		while (!cbIsEmpty(&external_events))
		{
			ElemType et;
			cbRead(&external_events, &et);

			if (et.skip)
				Common_ExternalEvent(L"NativeDraw", et.event, et.data);
			else
			{
				while (!Common_ExternalEvent(L"NativeDraw", et.event, et.data))
					Sleep(3);
			}

			Sleep(1);
		}

		SuspendThread(hook_thread);
	}

	return 0;
}

LRESULT CALLBACK Input_Hook(int code, WPARAM wParam, LPARAM lParam)
{
	if (code == HC_ACTION)
	{
		if (wParam == PM_REMOVE)
		{
			MSG *msg = (MSG *)lParam;

			if (msg->message >= WM_KEYFIRST && msg->message <= WM_KEYLAST)
				if (hook_keyboard_active)
					Input_KeyboardHook(msg);

			if (msg->message >= WM_MOUSEFIRST && msg->message <= WM_MOUSELAST)
				if (hook_mouse_active)
					Input_MouseHook(msg);
		}
	}

	return  CallNextHookEx(hook, code, wParam, lParam);
}

void Input_KeyboardHook(MSG *msg)
{
	if (!InputDispatch_KeyBoard_KeyHooked(msg->wParam))
		return;

	ElemType et;
	wsprintf(et.data, HOOK_WINDOW".%08X|0|0|%d|0", msg->hwnd, msg->wParam);

	switch (msg->message)
	{
		case WM_KEYDOWN: case WM_SYSKEYDOWN:
		{
			if (msg->lParam & HOOK_KEYBOARD_LPARAM_KEYREPEAT)
			{
				et.skip	=	true;
				wcscpy(et.event, L"ПовторКлавиши");
			} else
			{
				et.skip =	false;
				wcscpy(et.event, L"НажатиеКлавиши");
			}
			break;
		}
		case WM_KEYUP: case WM_SYSKEYUP:
		{
			et.skip	=	false;
			wcscpy(et.event, L"ОтпусканиеКлавиши");
			break;
		}
		default:
			return;
	}

	if (!cbIsFull(&external_events))
	{
		cbWrite(&external_events, &et);
		ResumeThread(hook_thread);
	}

	if (InputDispatch_KeyBoard_KeyLocked(msg->wParam))
		msg->message = NULL;
}

void Input_MouseHook(MSG *msg)
{
	if (!InputDispatch_Mouse_ActionHooked(msg->message))
		return;

	ElemType	et;
	POINT		p;

	p.x = HOOK_MOUSE_LPARAM_X(msg->lParam);
	p.y = HOOK_MOUSE_LPARAM_Y(msg->lParam);

	ClientToScreen(msg->hwnd, &p);

	switch (msg->message)
	{
		case WM_MOUSEMOVE:
			et.skip = true;
			wcscpy(et.event, L"ДвижениеМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_LBUTTONDOWN:
			et.skip = false;
			wcscpy(et.event, L"НажатиеЛевойКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_LBUTTONUP:
			et.skip = false;
			wcscpy(et.event, L"ОтпусканиеЛевойКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_LBUTTONDBLCLK:
			et.skip = false;
			wcscpy(et.event, L"ДвойнойКликЛевойКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_RBUTTONDOWN:
			et.skip = false;
			wcscpy(et.event, L"НажатиеПравойКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_RBUTTONUP:
			et.skip = false;
			wcscpy(et.event, L"ОтпусканиеПравойКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_RBUTTONDBLCLK:
			et.skip = false;
			wcscpy(et.event, L"ДвойнойКликПравойКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_MBUTTONDOWN:
			et.skip = false;
			wcscpy(et.event, L"НажатиеСреднейКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_MBUTTONUP:
			et.skip = false;
			wcscpy(et.event, L"ОтпусканиеСреднейКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_MBUTTONDBLCLK:
			et.skip = false;
			wcscpy(et.event, L"ДвойнойКликСреднейКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_XBUTTONDOWN:
			et.skip = false;
			wcscpy(et.event, L"НажатиеXКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_XBUTTONUP:
			et.skip = false;
			wcscpy(et.event, L"ОтпусканиеXКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|0", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case WM_XBUTTONDBLCLK:
			et.skip = false;
			wcscpy(et.event, L"ДвойнойКликXКнопкиМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|%d", msg->hwnd, p.x, p.y, GET_KEYSTATE_WPARAM(msg->wParam));
			break;
		case HOOK_WM_MOUSEHWHEEL_1: case HOOK_WM_MOUSEHWHEEL_2:
			et.skip = false;
			wcscpy(et.event, L"ДвижениеКолесаМыши");
			wsprintf(et.data, HOOK_WINDOW".%08X|%d|%d|%d|%d", msg->hwnd, HOOK_MOUSE_LPARAM_X(msg->lParam), HOOK_MOUSE_LPARAM_Y(msg->lParam), GET_KEYSTATE_WPARAM(msg->wParam), GET_WHEEL_DELTA_WPARAM(msg->wParam) / WHEEL_DELTA);
			break;
	}

	if (!cbIsFull(&external_events))
	{
		cbWrite(&external_events, &et);
		ResumeThread(hook_thread);
	}

	if (InputDispatch_Mouse_ActionLocked(msg->message))
		msg->message = NULL;
}

void InputDispatch_Initialization()
{
	hook					=	NULL;
	hook_thread				=	NULL;
	hook_keyboard_active	=	false;
	hook_mouse_active		=	false;

	memset((void *)hook_keyboard_keys, 0, sizeof(hook_keyboard_keys));
	memset((void *)hook_mouse_messages, 0, sizeof(hook_mouse_messages));
}

void InputDispatch_Deinitialization()
{
	InputDispatch_DeactivateKeyBoardHook();
	InputDispatch_DeactivateMouseHook();
	InputDispatch_DeinstallHook();
}

bool InputDispatch_InstallHook()
{
	if (hook)
		return false;

	if (!cbInit(&external_events, HOOK_EXTERNAL_EVENTS_POOL_SIZE))
		return false;

	if (!(hook_thread = CreateThread(NULL, 0, &Input_Thread, NULL, 0, NULL)))
		return false;

	if (!(hook = SetWindowsHookEx(WH_GETMESSAGE, Input_Hook, NULL, GetCurrentThreadId())))
	{
		TerminateThread(hook_thread, 0);
		return false;
	}

	return true;
}

bool InputDispatch_DeinstallHook()
{
	if (!hook)
		return false;

	UnhookWindowsHookEx(hook);
	SuspendThread(hook_thread);
	TerminateThread(hook_thread, 0);

	cbFree(&external_events);
	
	hook_thread				= NULL;
	hook					= NULL;

	hook_keyboard_active	= false;
	hook_mouse_active		= false;

	return true;
}


bool InputDispatch_ActivateKeyBoardHook()
{
	if (!hook)
		return false;

	if (hook_keyboard_active)
		return false;

	memset((void *)hook_keyboard_keys, 0, sizeof(hook_keyboard_keys));
	hook_keyboard_active = true;

	return true;
}

bool InputDispatch_SetKeyHook(int key, bool lock)
{
	if (!hook)
		return false;

	if (key >= 1000)
		return false;

	hook_keyboard_keys[key][0] = true;
	hook_keyboard_keys[key][1] = lock;

	return true;
}

bool InputDispatch_RemoveKeyHook(int key)
{
	if (!hook)
		return false;

	if (key >= 1000)
		return false;

	hook_keyboard_keys[key][0] = false;
	hook_keyboard_keys[key][1] = false;

	return true;
}

bool InputDispatch_DeactivateKeyBoardHook()
{
	if (!hook)
		return false;

	if (!hook_keyboard_active)
		return false;

	hook_keyboard_active = false;
	return true;
}

int InputDispatch_Mouse_ActionToMessage(wchar_t *action)
{
	if (!wcsicmp(L"ДвижениеМыши", action)						|| !wcsicmp(L"MouseMove", action))
		return WM_MOUSEMOVE;
	else if (!wcsicmp(L"НажатиеЛевойКнопкиМыши", action)		|| !wcsicmp(L"MouseLButtonDown", action))
		return WM_LBUTTONDOWN;
	else if (!wcsicmp(L"ОтпусканиеЛевойКнопкиМыши", action)		|| !wcsicmp(L"MouseLButtonUp", action))
		return WM_LBUTTONUP;
	else if (!wcsicmp(L"ДвойнойКликЛевойКнопкиМыши", action)	|| !wcsicmp(L"MouseLButtonDblClk", action))
		return WM_LBUTTONDBLCLK;
	else if (!wcsicmp(L"НажатиеПравойКнопкиМыши", action)		|| !wcsicmp(L"MouseRButtonDown", action))
		return WM_RBUTTONDOWN;
	else if (!wcsicmp(L"ОтпусканиеПравойКнопкиМыши", action)	|| !wcsicmp(L"MouseRButtonUp", action))
		return WM_RBUTTONUP;
	else if (!wcsicmp(L"ДвойнойКликПравойКнопкиМыши", action)	|| !wcsicmp(L"MouseRButtonDblClk", action))
		return WM_RBUTTONDBLCLK;
	else if (!wcsicmp(L"НажатиеСреднейКнопкиМыши", action)		|| !wcsicmp(L"MouseMButtonDown", action))
		return WM_MBUTTONDOWN;
	else if (!wcsicmp(L"ОтпусканиеСреднейКнопкиМыши", action)	|| !wcsicmp(L"MouseMButtonUp", action))
		return WM_MBUTTONUP;
	else if (!wcsicmp(L"ДвойнойКликСреднейКнопкиМыши", action)	|| !wcsicmp(L"MouseMButtonDblClk", action))
		return WM_MBUTTONDBLCLK;
	else if (!wcsicmp(L"НажатиеXКнопкиМыши", action)			|| !wcsicmp(L"MouseXButtonDown", action))
		return WM_XBUTTONDOWN;
	else if (!wcsicmp(L"ОтпусканиеXКнопкиМыши", action)			|| !wcsicmp(L"MouseXButtonUp", action))
		return WM_XBUTTONUP;
	else if (!wcsicmp(L"ДвойнойКликXКнопкиМыши", action)		|| !wcsicmp(L"MouseXButtonDblClk", action))
		return WM_XBUTTONDBLCLK;
	else if (!wcsicmp(L"ДвижениеКолесаМыши", action)		|| !wcsicmp(L"MouseWheel", action))
		return WM_MOUSEHWHEEL;
	else
		return WM_MOUSELAST + 1;
}

bool InputDispatch_ActivateMouseHook()
{
	if (!hook)
		return false;

	if (hook_mouse_active)
		return false;

	memset((void *)hook_mouse_messages, 0, sizeof(hook_mouse_messages));
	hook_mouse_active = true;

	return true;
}

bool InputDispatch_SetMouseActionHook(wchar_t *action, bool lock)
{
	if (!hook)
		return false;

	int message	=	InputDispatch_Mouse_ActionToMessage(action);

	if (message == WM_MOUSEHWHEEL)
	{
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_1 - WM_MOUSEFIRST][0] = true;
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_1 - WM_MOUSEFIRST][1] = lock;
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_2 - WM_MOUSEFIRST][0] = true;
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_2 - WM_MOUSEFIRST][1] = lock;
	}
	else
	{
		hook_mouse_messages[message	-	WM_MOUSEFIRST][0] = true;
		hook_mouse_messages[message -	WM_MOUSEFIRST][1] = lock;
	}

	return true;
}

bool InputDispatch_RemoveMouseActionHook(wchar_t *action)
{
	if (!hook)
		return false;

	int message = InputDispatch_Mouse_ActionToMessage(action);

	if (message == WM_MOUSEHWHEEL)
	{
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_1 - WM_MOUSEFIRST][0] = false;
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_1 - WM_MOUSEFIRST][1] = false;
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_2 - WM_MOUSEFIRST][0] = false;
		hook_mouse_messages[HOOK_WM_MOUSEHWHEEL_2 - WM_MOUSEFIRST][1] = false;
	}
	else
	{
		hook_mouse_messages[message - WM_MOUSEFIRST][0] = false;
		hook_mouse_messages[message - WM_MOUSEFIRST][1] = false;
	}

	return true;
}

bool InputDispatch_DeactivateMouseHook()
{
	if (!hook)
		return false;

	if (!hook_mouse_active)
		return false;

	hook_mouse_active = false;
	return true;
}

bool InputDispatch_KeyBoard_KeyHooked(int key)
{
	if (!hook)
		return false;

	if (key >= 1000)
		return false;

	return hook_keyboard_keys[key][0];
}

bool InputDispatch_KeyBoard_KeyLocked(int key)
{
	if (!hook)
		return false;

	return hook_keyboard_keys[key][1];
}


bool InputDispatch_Mouse_ActionHooked(int message)
{
	if (!hook)
		return false;

	return hook_mouse_messages[message - WM_MOUSEFIRST][0];
}

bool InputDispatch_Mouse_ActionLocked(int message)
{
	if (!hook)
		return false;

	return hook_mouse_messages[message - WM_MOUSEFIRST][1];
}



