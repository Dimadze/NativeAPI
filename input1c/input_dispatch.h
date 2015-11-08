#ifndef __INPUT_DISPATCH_H__

#include "windows.h"

#define	HOOK_KEYBOARD_LPARAM_KEYDOWN	0x00000000 
#define	HOOK_KEYBOARD_LPARAM_KEYREPEAT	0x40000000 
#define	HOOK_KEYBOARD_LPARAM_KEYUP		0xC0000000

#define HOOK_MOUSE_LPARAM_X(lp)			((int)(short)LOWORD(lp))
#define HOOK_MOUSE_LPARAM_Y(lp)			((int)(short)HIWORD(lp))

#define HOOK_EXTERNAL_EVENTS_POOL_SIZE 5000

#define HOOK_WM_MOUSEHWHEEL_1			0x20A
#define HOOK_WM_MOUSEHWHEEL_2			0x20E

extern bool Common_ExternalEvent(wchar_t *source, wchar_t *eventname, wchar_t *data);

void Input_KeyboardHook(MSG *msg);
void Input_MouseHook(MSG *msg);

void InputDispatch_Initialization();
void InputDispatch_Deinitialization();

bool InputDispatch_InstallHook();
bool InputDispatch_DeinstallHook();

bool InputDispatch_ActivateKeyBoardHook();
bool InputDispatch_DeactivateKeyBoardHook();

int InputDispatch_Mouse_ActionToMessage(wchar_t *action);

bool InputDispatch_ActivateMouseHook();
bool InputDispatch_DeactivateMouseHook();

bool InputDispatch_SetMouseActionHook(wchar_t *action, bool lock);
bool InputDispatch_RemoveMouseActionHook(wchar_t *action);

bool InputDispatch_SetKeyHook(int key, bool lock);
bool InputDispatch_RemoveKeyHook(int key);

bool InputDispatch_KeyBoard_KeyHooked(int key);
bool InputDispatch_KeyBoard_KeyLocked(int key);

bool InputDispatch_Mouse_ActionHooked(int message);
bool InputDispatch_Mouse_ActionLocked(int message);




#endif //__INPUT_DISPATCH_H__
