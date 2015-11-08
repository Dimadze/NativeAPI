
#include "stdafx.h"

#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <iconv.h>
#endif

#include <stdio.h>
#include <wchar.h>
#include "AddInNative.h"
#include <string>

#include "draw1c/draw_dispatch.h"
#include "input1c/input_dispatch.h"
#include "sys1c/sys_dispatch.h"
#include "ww1c/ww_dispatch.h"

static const wchar_t *g_PropNames[] =
{
	L"Version"
};

static const wchar_t *g_MethodNames[] =
{
	L"Canvas_Create",
	L"Canvas_Binary",
	L"Canvas_ClipBinary",
	L"Canvas_Delete",

	L"Image_Create",
	L"Image_Resize",
	L"Image_Rotate",
	L"Image_Flip",
	L"Image_Crop",
	L"Image_ToClipBoard",
	L"Image_GetWidth",
	L"Image_GetHeight",
	L"Image_Binary",
	L"Image_ClipBinary",
	L"Image_Delete",

	L"Draw_GetColor",
	L"Draw_Point",
	L"Draw_Line",
	L"Draw_Ellipse",
	L"Draw_FillEllipse",
	L"Draw_Rectangle",
	L"Draw_FillRectangle",
	L"Draw_Triangle",
	L"Draw_FillTriangle",
	L"Draw_String",
	L"Draw_Image",

	L"Timer_Start",
	L"Timer_Stop",

	L"Input_InstallHook",
	L"Input_ActivateKeyBoardHook",
	L"Input_SetKeyHook",
	L"Input_RemoveKeyHook",
	L"Input_DeactivateKeyBoardHook",
	L"Input_ActivateMouseHook",
	L"Input_SetMouseActionHook",
	L"Input_RemoveMouseActionHook",
	L"Input_DeactivateMouseHook",
	L"Input_DeinstallHook",

	L"Window_Main",
	L"Window_GetByTitle",
	L"Window_IsChild",
	L"Window_GetCoordinates",
	L"Window_ToClientCoordinates",
	L"Window_Move",
	L"Window_Action",

	L"Test_ExternalEvent"
};

static const wchar_t *g_PropNamesRu[] =
{
	L"Версия"
};

static const wchar_t *g_MethodNamesRu[] =
{
	L"Холст_Создать",
	L"Холст_ДвоичныеДанные",
	L"Холст_УчастокДвоичныеДанные",
	L"Холст_Удалить",

	L"Изображение_Создать",
	L"Изображение_ИзменитьРазмер",
	L"Изображение_Повернуть",
	L"Изображение_Отразить",
	L"Изображение_Обрезать",
	L"Изображение_ВБуферОбмена",
	L"Изображение_ПолучитьШирину",
	L"Изображение_ПолучитьВысоту",
	L"Изображение_ДвоичныеДанные",
	L"Изображение_УчастокДвоичныеДанные",
	L"Изображение_Удалить",

	L"Рисование_ПолучитьЦвет",
	L"Рисование_Точка",
	L"Рисование_Линия",
	L"Рисование_Эллипс",
	L"Рисование_ЗаполненныйЭллипс",
	L"Рисование_Прямоугольник",
	L"Рисование_ЗаполненныйПрямоугольник",
	L"Рисование_Треугольник",
	L"Рисование_ЗаполненныйТреугольник",
	L"Рисование_Текст",
	L"Рисование_Изображение",

	L"Таймер_Запустить",
	L"Таймер_Остановить2",

	L"Ввод_УстановитьПерехват",
	L"Ввод_АктивироватьПерехватКлавиатуры",
	L"Ввод_ЗадатьПерехватКлавиши",
	L"Ввод_УбратьПерехватКлавиши",
	L"Ввод_ДеактивироватьПерехватКлавиатуры",
	L"Ввод_АктивироватьПерехватМыши",
	L"Ввод_ЗадатьПерехватДействияМыши",
	L"Ввод_УбратьПерехватДействияМыши",
	L"Ввод_ДеактивироватьПерехватМыши",
	L"Ввод_УбратьПерехват",

	L"Окно_Главное",
	L"Окно_ПолучитьПоЗаголовку",
	L"Окно_Дочернее",
	L"Окно_ПолучитьКоординаты",
	L"Окно_ВОтносительныеКоординаты",
	L"Окно_Переместить",
	L"Окно_Действие",

	L"Тест_ВнешнееСобытие"
};


static const wchar_t g_kClassNames[] = L"CAddInNative"; //"|OtherClass1|OtherClass2";
static IAddInDefBase *pAsyncEvent = NULL;

uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len = 0);
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len = 0);
uint32_t getLenShortWcharStr(const WCHAR_T* Source);
static AppCapabilities g_capabilities = eAppCapabilitiesInvalid;
static WcharWrapper s_names(g_kClassNames);

//---------------------------------------------------------------------------//
long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        *pInterface= new CAddInNative;
        return (long)*pInterface;
    }
    return 0;
}
//---------------------------------------------------------------------------//
AppCapabilities SetPlatformCapabilities(const AppCapabilities capabilities)
{
    g_capabilities = capabilities;
    return eAppCapabilitiesLast;
}
//---------------------------------------------------------------------------//
long DestroyObject(IComponentBase** pIntf)
{
    if(!*pIntf)
        return -1;

    delete *pIntf;
    *pIntf = 0;
    return 0;
}
//---------------------------------------------------------------------------//
bool Common_ExternalEvent(wchar_t *source, wchar_t *eventname, wchar_t *data)
{
	if (pAsyncEvent)
		return pAsyncEvent->ExternalEvent(source, eventname, data);
	else
		return false;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    return s_names;
}


// CAddInNative
//---------------------------------------------------------------------------//
CAddInNative::CAddInNative()
{
    m_iMemory = 0;
    m_iConnect = 0;
}
//---------------------------------------------------------------------------//
CAddInNative::~CAddInNative()
{
}

//---------------------------------------------------------------------------//
bool CAddInNative::Init(void* pConnection)
{ 
    m_iConnect		= (IAddInDefBase*)pConnection;
	pAsyncEvent		= m_iConnect;

	wcscpy(m_wstrVersion, L"2.5.1");

	DrawDispatch_Initialization();
	SysDispatch_Initialization();
	InputDispatch_Initialization();
	WindowsWorkDispatch_Initialization();

    return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetInfo()
{ 
    // Component should put supported component technology version 
    // This component supports 2.0 version
    return 2000; 
}
//---------------------------------------------------------------------------//
void CAddInNative::Done()
{
	DrawDispatch_Deinitialization();
	SysDispatch_Deinitialization();
	InputDispatch_Deinitialization();
	WindowsWorkDispatch_Deinitialization();
}
/////////////////////////////////////////////////////////////////////////////
// ILanguageExtenderBase
//---------------------------------------------------------------------------//
bool CAddInNative::RegisterExtensionAs(WCHAR_T** wsExtensionName)
{ 
    const wchar_t *wsExtension = L"NativeDraw";
    int iActualSize = ::wcslen(wsExtension) + 1;
    WCHAR_T* dest = 0;

    if (m_iMemory)
    {
        if(m_iMemory->AllocMemory((void**)wsExtensionName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(wsExtensionName, wsExtension, iActualSize);
        return true;
    }

    return false; 
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNProps()
{ 
    // You may delete next lines and add your own implementation code here
    return ePropLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindProp(const WCHAR_T* wsPropName)
{ 
    long plPropNum = -1;
    wchar_t* propName = 0;

    ::convFromShortWchar(&propName, wsPropName);
    plPropNum = findName(g_PropNames, propName, ePropLast);

    if (plPropNum == -1)
        plPropNum = findName(g_PropNamesRu, propName, ePropLast);

    delete[] propName;

    return plPropNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetPropName(long lPropNum, long lPropAlias)
{ 
    if (lPropNum >= ePropLast)
        return NULL;

    wchar_t *wsCurrentName = NULL;
    WCHAR_T *wsPropName = NULL;
    int iActualSize = 0;

    switch(lPropAlias)
    {
		case 0: // First language
			wsCurrentName = (wchar_t*)g_PropNames[lPropNum];
			break;
		case 1: // Second language
			wsCurrentName = (wchar_t*)g_PropNamesRu[lPropNum];
			break;
		default:
			return 0;
    }
    
    iActualSize = wcslen(wsCurrentName) + 1;

    if (m_iMemory && wsCurrentName)
    {
        if (m_iMemory->AllocMemory((void**)&wsPropName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsPropName, wsCurrentName, iActualSize);
    }

    return wsPropName;
}

//---------------------------------------------------------------------------//
bool CAddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
	switch (lPropNum)
	{
		case ePropVersion:
		{
			allocString(pvarPropVal, m_wstrVersion);
			break;
		}

		default:
			return false;
	}

    return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::SetPropVal(const long lPropNum, tVariant *varPropVal)
{ 
	switch (lPropNum)
	{
		default:
			return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropReadable(const long lPropNum)
{ 
	switch (lPropNum)
	{
		case ePropVersion:
			return true;
		default:
			return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
bool CAddInNative::IsPropWritable(const long lPropNum)
{
	switch (lPropNum)
	{
		default:
			return false;
	}

	return false;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNMethods()
{ 
    return eMethLast;
}
//---------------------------------------------------------------------------//
long CAddInNative::FindMethod(const WCHAR_T* wsMethodName)
{ 
    long plMethodNum = -1;
    wchar_t* name = 0;

    ::convFromShortWchar(&name, wsMethodName);

    plMethodNum = findName(g_MethodNames, name, eMethLast);

    if (plMethodNum == -1)
        plMethodNum = findName(g_MethodNamesRu, name, eMethLast);

    delete[] name;

    return plMethodNum;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInNative::GetMethodName(const long lMethodNum, const long lMethodAlias)
{ 
    if (lMethodNum >= eMethLast)
        return NULL;

    wchar_t *wsCurrentName = NULL;
    WCHAR_T *wsMethodName = NULL;
    int iActualSize = 0;

    switch(lMethodAlias)
    {
		case 0: // First language
			wsCurrentName = (wchar_t*)g_MethodNames[lMethodNum];
			break;
		case 1: // Second language
			wsCurrentName = (wchar_t*)g_MethodNamesRu[lMethodNum];
			break;
		default: 
			return 0;
    }

    iActualSize = wcslen(wsCurrentName) + 1;

    if (m_iMemory && wsCurrentName)
    {
        if(m_iMemory->AllocMemory((void**)&wsMethodName, iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(&wsMethodName, wsCurrentName, iActualSize);
    }

    return wsMethodName;
}
//---------------------------------------------------------------------------//
long CAddInNative::GetNParams(const long lMethodNum)
{ 
	switch (lMethodNum)
	{
		case eMethCanvas_Create:
			return 3;
		case eMethCanvas_Binary:
			return 1;
		case eMethCanvas_ClipBinary:
			return 5;
		case eMethCanvas_Delete:
			return 1;

		case eMethImage_Create:
			return 1;
		case eMethImage_Resize:
			return 3;
		case eMethImage_Rotate:
			return 3;
		case eMethImage_Flip:
			return 3;
		case eMethImage_Crop:
			return 5;
		case eMethImage_ToClipBoard:
			return 1;
		case eMethImage_GetWidth:
			return 1;
		case eMethImage_GetHeight:
			return 1;
		case eMethImage_Binary:
			return 2;
		case eMethImage_ClipBinary:
			return 6;
		case eMethImage_Delete:
			return 1;

		case eMethDraw_GetColor:
			return 4;
		case eMethDraw_Point:
			return 4;
		case eMethDraw_Line:
			return 7;
		case eMethDraw_Ellipse:
			return 7;
		case eMethDraw_FillEllipse:
			return 6;
		case eMethDraw_Rectangle:
			return 7;
		case eMethDraw_FillRectangle:
			return 6;
		case eMethDraw_Triangle:
			return 9;
		case eMethDraw_FillTriangle:
			return 8;
		case eMethDraw_String:
			return 12;
		case eMethDraw_Image:
			return 9;

		case eMethTimer_Start:
			return 1;
		case eMethTimer_Stop:
			return 1;

		case eMethInput_InstallHook:
			return 0;
		case eMethInput_ActivateKeyBoardHook:
			return 0;
		case eMethInput_SetKeyHook:
			return 2;
		case eMethInput_RemoveKeyHook:
			return 1;
		case eMethInput_DeactivateKeyBoardHook:
			return 0;
		case eMethInput_ActivateMouseHook:
			return 0;
		case eMethInput_SetMouseActionHook:
			return 2;
		case eMethInput_RemoveMouseActionHook:
			return 1;
		case eMethInput_DeactivateMouseHook:
			return 0;
		case eMethInput_DeinstallHook:
			return 0;

		case eMethWindow_Main:
			return 0;
		case eMethWindow_GetByTitle:
			return 1;
		case eMethWindow_IsChild:
			return 2;
		case eMethWindow_GetCoordinates:
			return 5;
		case eMethWindow_ToClientCoordinates:
			return 3;
		case eMethWindow_Move:
			return 5;
		case eMethWindow_Action:
			return 2;

		case eMethTest_ExternalEvent:
			return 3;

		default:
			return 0;
	}

	return 0;
}
//---------------------------------------------------------------------------//
bool CAddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum,
                        tVariant *pvarParamDefValue)
{ 
	TV_VT(pvarParamDefValue) = VTYPE_EMPTY;

	switch (lMethodNum)
	{
		default:
			return false;
	}

	return false;
} 
//---------------------------------------------------------------------------//
bool CAddInNative::HasRetVal(const long lMethodNum)
{ 
	switch (lMethodNum)
	{
		case eMethCanvas_Create:
		case eMethCanvas_Binary:
		case eMethCanvas_ClipBinary:
		case eMethCanvas_Delete:

		case eMethImage_Create:
		case eMethImage_Resize:
		case eMethImage_Rotate:
		case eMethImage_Flip:
		case eMethImage_Crop:
		case eMethImage_ToClipBoard:
		case eMethImage_GetWidth:
		case eMethImage_GetHeight:
		case eMethImage_Binary:
		case eMethImage_ClipBinary:
		case eMethImage_Delete:

		case eMethDraw_GetColor:

		case eMethTimer_Start:
		case eMethTimer_Stop:

		case eMethInput_InstallHook:
		case eMethInput_ActivateKeyBoardHook:
		case eMethInput_SetKeyHook:
		case eMethInput_RemoveKeyHook:
		case eMethInput_DeactivateKeyBoardHook:
		case eMethInput_ActivateMouseHook:
		case eMethInput_SetMouseActionHook:
		case eMethInput_RemoveMouseActionHook:
		case eMethInput_DeactivateMouseHook:
		case eMethInput_DeinstallHook:
			
		case eMethWindow_Main:
		case eMethWindow_GetByTitle:
		case eMethWindow_IsChild:
		case eMethWindow_GetCoordinates:
		case eMethWindow_ToClientCoordinates:
		case eMethWindow_Move:
		case eMethWindow_Action:

		case eMethTest_ExternalEvent:
			return true;

		default:
			return false;
	}

	return false;
}

//---------------------------------------------------------------------------//
bool CAddInNative::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
	switch (lMethodNum)
	{
		case eMethDraw_Point:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[3]);

			int x				= digital(&paParams[1]);
			int y				= digital(&paParams[2]);

			DrawDispatch_Draw_Point(canvas_id, x, y, color);
			return true;
		}

		case eMethDraw_Line:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[6]);

			int x1				= digital(&paParams[1]);
			int y1				= digital(&paParams[2]);
			int x2				= digital(&paParams[3]);
			int y2				= digital(&paParams[4]);
			int t				= digital(&paParams[5]);

			DrawDispatch_Draw_Line(canvas_id, x1, y1, x2, y2, t, color);
			return true;
		}

		case eMethDraw_Ellipse:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[6]);

			int x				= digital(&paParams[1]);
			int y				= digital(&paParams[2]);
			int w				= digital(&paParams[3]);
			int h				= digital(&paParams[4]);
			int t				= digital(&paParams[5]);

			DrawDispatch_Draw_Ellipse(canvas_id, x, y, w, h, t, color);
			return true;
		}

		case eMethDraw_FillEllipse:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[5]);

			int x				= digital(&paParams[1]);
			int y				= digital(&paParams[2]);
			int w				= digital(&paParams[3]);
			int h				= digital(&paParams[4]);

			DrawDispatch_Draw_FillEllipse(canvas_id, x, y, w, h, color);
			return true;
		}

		case eMethDraw_Rectangle:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[6]);

			int x				= digital(&paParams[1]);
			int y				= digital(&paParams[2]);
			int w				= digital(&paParams[3]);
			int h				= digital(&paParams[4]);
			int t				= digital(&paParams[5]);

			DrawDispatch_Draw_Rectangle(canvas_id, x, y, w, h, t, color);
			return true;
		}

		case eMethDraw_FillRectangle:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[5]);

			int x				= digital(&paParams[1]);
			int y				= digital(&paParams[2]);
			int w				= digital(&paParams[3]);
			int h				= digital(&paParams[4]);

			DrawDispatch_Draw_FillRectangle(canvas_id, x, y, w, h, color);
			return true;
		}

		case eMethDraw_Triangle:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[8]);

			int x1				= digital(&paParams[1]);
			int y1				= digital(&paParams[2]);
			int x2				= digital(&paParams[3]);
			int y2				= digital(&paParams[4]);
			int x3				= digital(&paParams[5]);
			int y3				= digital(&paParams[6]);
			int t				= digital(&paParams[7]);

			DrawDispatch_Draw_Triangle(canvas_id, x1, y1, x2, y2, x3, y3, t, color);
			return true;
		}

		case eMethDraw_FillTriangle:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*color		= TV_WSTR(&paParams[7]);

			int x1				= digital(&paParams[1]);
			int y1				= digital(&paParams[2]);
			int x2				= digital(&paParams[3]);
			int y2				= digital(&paParams[4]);
			int x3				= digital(&paParams[5]);
			int y3				= digital(&paParams[6]);

			DrawDispatch_Draw_FillTriangle(canvas_id, x1, y1, x2, y2, x3, y3, color);
			return true;
		}

		case eMethDraw_String:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*string		= TV_WSTR(&paParams[1]);
			wchar_t	*font		= TV_WSTR(&paParams[8]);
			wchar_t	*color		= TV_WSTR(&paParams[11]);

			int x				= digital(&paParams[2]);
			int y				= digital(&paParams[3]);
			int w				= digital(&paParams[4]);
			int h				= digital(&paParams[5]);

			int h_align			= digital(&paParams[6]);
			int v_align			= digital(&paParams[7]);
			int font_size		= digital(&paParams[9]);
			int font_style		= digital(&paParams[10]);

			DrawDispatch_Draw_String(canvas_id, string, x, y, w, h, h_align, v_align, font, font_size, font_style, color);
			return true;
		}


		case eMethDraw_Image:
		{
			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);
			wchar_t	*image		= TV_WSTR(&paParams[1]);

			int x				= digital(&paParams[2]);
			int y				= digital(&paParams[3]);
			int w				= digital(&paParams[4]);
			int h				= digital(&paParams[5]);
			int a				= digital(&paParams[6]);
			bool fx				= TV_BOOL(&paParams[7]);
			bool fy				= TV_BOOL(&paParams[8]);

			DrawDispatch_Draw_Image(canvas_id, image, x, y, w, h, a, fx, fy);
			return true;
		}

		default:
			return false;
	}

	return true;
}
//---------------------------------------------------------------------------//
bool CAddInNative::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
	switch (lMethodNum)
	{
		case eMethDraw_GetColor:
		{
			int a = digital(&paParams[0]);
			int r = digital(&paParams[1]);
			int g = digital(&paParams[2]);
			int b = digital(&paParams[3]);

			wchar_t tmp[64];
			DrawDispatch_Draw_RGBToString(tmp, a, r, g, b);
			
			allocString(pvarRetValue, tmp);
			return true;
		}

		case eMethCanvas_Create:
		{
			int w = digital(&paParams[0]);
			int h = digital(&paParams[1]);
			int o = digital(&paParams[2]);

			allocString(pvarRetValue, DrawDispatch_Canvas_Create(w, h, o));
			return true;

		}

		case eMethCanvas_Binary:
		{
			TV_VT(pvarRetValue) = VTYPE_BLOB;

			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);

			void	*body		= NULL;
			int		size		= NULL;

			DrawDispatch_Canvas_GetPicture(canvas_id, &body, &size);

			if (body && size)
			{
				void *b = NULL;
				m_iMemory->AllocMemory((void **)&b, size);

				if (b)
					memcpy(b, body, size);

				if (b)
				{
					
					pvarRetValue->pwstrVal	= (wchar_t *)b;
					pvarRetValue->wstrLen	= size;
					return true;
				}

			}

			pvarRetValue->wstrLen = 0;
			return true;
		}


		case eMethCanvas_ClipBinary:
		{
			TV_VT(pvarRetValue) = VTYPE_BLOB;

			wchar_t	*canvas_id	= TV_WSTR(&paParams[0]);

			int x				= digital(&paParams[1]);
			int y				= digital(&paParams[2]);
			int w				= digital(&paParams[3]);
			int h				= digital(&paParams[4]);

			void	*body = NULL;
			int		size = NULL;

			DrawDispatch_Canvas_GetPictureClip(canvas_id, &body, &size, x, y, w, h);

			if (body && size)
			{
				void *b = NULL;
				m_iMemory->AllocMemory((void **)&b, size);

				if (b)
					memcpy(b, body, size);

				if (b)
				{

					pvarRetValue->pwstrVal = (wchar_t *)b;
					pvarRetValue->wstrLen = size;
					return true;
				}

			}

			pvarRetValue->wstrLen = 0;
			return true;
		}

		case eMethCanvas_Delete:
		{
			wchar_t	*canvas_id		= TV_WSTR(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= DrawDispatch_Canvas_Delete(canvas_id);
			return true;
		}

		case eMethTimer_Start:
		{
			int		elapse			= TV_I4(&paParams[0]);

			allocString(pvarRetValue, SysDispatch_Timer_Start(elapse));
			return true;
		}

		case eMethTimer_Stop:
		{
			wchar_t	*timer_id		= TV_WSTR(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= SysDispatch_Timer_Stop(timer_id);
			return true;
		}

		case eMethImage_Create:
		{
			wchar_t	*image = TV_WSTR(&paParams[0]);

			allocString(pvarRetValue, DrawDispatch_Image_Create(image));
			return true;
		}

		case eMethImage_Resize:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);
			int w					= digital(&paParams[1]);
			int h					= digital(&paParams[2]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= DrawDispatch_Image_Resize(image_id, w, h);
			return true;
		}

		case eMethImage_Rotate:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);
			int angle				= TV_I4(&paParams[1]);
			bool changedimension	= TV_BOOL(&paParams[2]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= DrawDispatch_Image_Rotate(image_id, angle, changedimension);
			return true;
		}

		case eMethImage_Flip:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);
			bool x					= TV_BOOL(&paParams[1]);
			bool y					= TV_BOOL(&paParams[2]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= DrawDispatch_Image_Flip(image_id, x, y);
			return true;
		}

		case eMethImage_Crop:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);
			int x					= digital(&paParams[1]);
			int y					= digital(&paParams[2]);
			int w					= digital(&paParams[3]);
			int h					= digital(&paParams[4]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= DrawDispatch_Image_Crop(image_id, x, y, w, h);
			return true;
		}


		case eMethImage_ToClipBoard:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= DrawDispatch_Image_ToClipBoard(image_id);
			return true;
		}

		case eMethImage_GetWidth:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_I4;
			TV_I4(pvarRetValue)		= DrawDispatch_Image_GetWidth(image_id);
			return true;
		}

		case eMethImage_GetHeight:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_I4;
			TV_I4(pvarRetValue)		= DrawDispatch_Image_GetHeight(image_id);
			return true;
		}

		case eMethImage_Binary:
		{
			TV_VT(pvarRetValue) = VTYPE_BLOB;

			wchar_t	*image_id	= TV_WSTR(&paParams[0]);
			int		out			= digital(&paParams[1]);

			void	*body		= NULL;
			int		size		= NULL;

			DrawDispatch_Image_GetPicture(image_id, out, &body, &size);

			if (body && size)
			{
				void *b = NULL;
				m_iMemory->AllocMemory((void **)&b, size);

				if (b)
					memcpy(b, body, size);

				if (b)
				{
					pvarRetValue->pwstrVal = (wchar_t *)b;
					pvarRetValue->wstrLen = size;
					return true;
				}

			}

			pvarRetValue->wstrLen = 0;
			return true;
		}


		case eMethImage_ClipBinary:
		{
			TV_VT(pvarRetValue) = VTYPE_BLOB;

			wchar_t	*image_id	= TV_WSTR(&paParams[0]);
			int		out			= digital(&paParams[1]);

			int x				= digital(&paParams[2]);
			int y				= digital(&paParams[3]);
			int w				= digital(&paParams[4]);
			int h				= digital(&paParams[5]);

			void	*body		= NULL;
			int		size		= NULL;

			DrawDispatch_Image_GetPictureClip(image_id, out, &body, &size, x, y, w, h);

			if (body && size)
			{
				void *b = NULL;
				m_iMemory->AllocMemory((void **)&b, size);

				if (b)
					memcpy(b, body, size);

				if (b)
				{
					pvarRetValue->pwstrVal = (wchar_t *)b;
					pvarRetValue->wstrLen = size;
					return true;
				}

			}

			pvarRetValue->wstrLen = 0;
			return true;
		}


		case eMethImage_Delete:
		{
			wchar_t	*image_id		= TV_WSTR(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= DrawDispatch_Image_Delete(image_id);
			return true;
		}

		case eMethInput_InstallHook:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_InstallHook();
			return true;
		}

		case eMethInput_ActivateKeyBoardHook:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_ActivateKeyBoardHook();
			return true;
		}

		case eMethInput_SetKeyHook:
		{
			int key					= TV_I4(&paParams[0]);
			bool lock				= TV_BOOL(&paParams[1]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_SetKeyHook(key, lock);
			return true;
		}

		case eMethInput_RemoveKeyHook:
		{
			int		key				= TV_I4(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_RemoveKeyHook(key);
			return true;
		}

		case eMethInput_DeactivateKeyBoardHook:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_DeactivateKeyBoardHook();
			return true;
		}

		case eMethInput_ActivateMouseHook:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_ActivateMouseHook();
			return true;
		}

		case eMethInput_SetMouseActionHook:
		{
			wchar_t	*action			= TV_WSTR(&paParams[0]);
			bool lock				= TV_BOOL(&paParams[1]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_SetMouseActionHook(action, lock);
			return true;
		}

		case eMethInput_RemoveMouseActionHook:
		{
			wchar_t	*action			= TV_WSTR(&paParams[0]);

			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_RemoveMouseActionHook(action);
			return true;
		}

		case eMethInput_DeactivateMouseHook:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_DeactivateMouseHook();
			return true;
		}

		case eMethInput_DeinstallHook:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;
			TV_BOOL(pvarRetValue)	= InputDispatch_DeinstallHook();
			return true;
		}

		case eMethWindow_Main:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;

			wchar_t	tmp[64];

			WindowsWorkDispatch_GetMainWindowID(tmp);

			allocString(pvarRetValue, tmp);
			return true;
		}

		case eMethWindow_GetByTitle:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;

			wchar_t	*title = TV_WSTR(&paParams[0]);
			wchar_t	tmp[64];

			WindowsWorkDispatch_GetWindowIDByTitle(tmp, title);

			allocString(pvarRetValue, tmp);
			return true;
		}

		case eMethWindow_IsChild:
		{
			TV_VT(pvarRetValue) = VTYPE_BOOL;

			wchar_t	*window_id_owner	= TV_WSTR(&paParams[0]);
			wchar_t	*window_id_child	= TV_WSTR(&paParams[1]);

			TV_BOOL(pvarRetValue)		= WindowsWorkDispatch_IsChildWindow(window_id_owner, window_id_child);
			return true;
		}

		case eMethWindow_GetCoordinates:
		{
			TV_VT(pvarRetValue)		= VTYPE_BOOL;

			wchar_t	*window_id		= TV_WSTR(&paParams[0]);

			int x1, x2, y1, y2;

			bool	res				=	WindowsWorkDispatch_GetCoordinates(window_id, &x1, &y1, &x2, &y2);

			if (res)
			{
				TV_I4(&paParams[1]) = x1;
				TV_I4(&paParams[2]) = y1;
				TV_I4(&paParams[3]) = x2;
				TV_I4(&paParams[4]) = y2;
			}

			TV_BOOL(pvarRetValue) = res;
			return true;
		}

		case eMethWindow_ToClientCoordinates:
		{
			TV_VT(pvarRetValue) = VTYPE_BOOL;

			wchar_t	*window_id	= TV_WSTR(&paParams[0]);

			int x				= TV_I4(&paParams[1]);
			int y				= TV_I4(&paParams[2]);

			bool res			= WindowsWorkDispatch_ToClientCoordinates(window_id, &x, &y);

			if (res)
			{
				TV_I4(&paParams[1]) = x;
				TV_I4(&paParams[2]) = y;
			}

			TV_BOOL(pvarRetValue) = res;
			return true;
		}

		case eMethWindow_Move:
		{
			TV_VT(pvarRetValue) = VTYPE_BOOL;

			wchar_t	*window_id	= TV_WSTR(&paParams[0]);

			int x				= TV_I4(&paParams[1]);
			int y				= TV_I4(&paParams[2]);
			int w				= TV_I4(&paParams[3]);
			int h				= TV_I4(&paParams[4]);

			TV_BOOL(pvarRetValue) = WindowsWorkDispatch_Move(window_id, x, y, w, h);
			return true;
		}

		case eMethWindow_Action:
		{
			TV_VT(pvarRetValue) = VTYPE_BOOL;

			wchar_t	*window_id	= TV_WSTR(&paParams[0]);
			wchar_t	*action		= TV_WSTR(&paParams[1]);

			TV_BOOL(pvarRetValue) = WindowsWorkDispatch_Action(window_id, action);
			return true;
		}

		case eMethTest_ExternalEvent:
		{
			TV_VT(pvarRetValue)	= VTYPE_BOOL;

			wchar_t	*source			= TV_WSTR(&paParams[0]);
			wchar_t	*event			= TV_WSTR(&paParams[1]);
			wchar_t	*data			= TV_WSTR(&paParams[2]);

			TV_BOOL(pvarRetValue)	= Common_ExternalEvent(source, event, data);
			return true;
		}

		default:
			return false;
	}
}

//---------------------------------------------------------------------------//
void CAddInNative::SetLocale(const WCHAR_T* loc)
{
#ifndef __linux__
    _wsetlocale(LC_ALL, loc);
#else
    //We convert in char* char_locale
    //also we establish locale
    //setlocale(LC_ALL, char_locale);
#endif
}
/////////////////////////////////////////////////////////////////////////////
// LocaleBase
//---------------------------------------------------------------------------//
bool CAddInNative::setMemManager(void* mem)
{
    m_iMemory = (IMemoryManager*)mem;
    return m_iMemory != 0;
}


//---------------------------------------------------------------------------//
int CAddInNative::digital(tVariant *digital)
{
	if (digital)
	{
		switch (TV_VT(digital))
		{
			case VTYPE_I4:
				return TV_I4(digital);
			case VTYPE_R8:
				return (int)ceil(digital->dblVal);
			default:
				return TV_I4(digital);
		}
	}

	return 0;
}

//---------------------------------------------------------------------------//
void CAddInNative::allocString(tVariant *wstring, wchar_t *string)
{
	if (wstring)
	{
		if (string)
		{
			TV_VT(wstring)		= VTYPE_PWSTR;
			m_iMemory->AllocMemory((void**)&wstring->pwstrVal, wcslen(string) * sizeof(wchar_t) + sizeof(wchar_t));
			wcscpy(wstring->pwstrVal, string);
			wstring->wstrLen = wcslen(string);
		}
		else
		{
			TV_VT(wstring)		= VTYPE_PWSTR;
			wstring->wstrLen	= 0;
		}
	}
}

//---------------------------------------------------------------------------//
void CAddInNative::addError(uint32_t wcode, const wchar_t* source, 
                        const wchar_t* descriptor, long code)
{
    if (m_iConnect)
    {
        WCHAR_T *err = 0;
        WCHAR_T *descr = 0;
        
        ::convToShortWchar(&err, source);
        ::convToShortWchar(&descr, descriptor);

        m_iConnect->AddError(wcode, err, descr, code);
        delete[] err;
        delete[] descr;
    }
}
//---------------------------------------------------------------------------//
long CAddInNative::findName(const wchar_t* names[], const wchar_t* name, 
                        const uint32_t size) const
{
    long ret = -1;
    for (uint32_t i = 0; i < size; i++)
    {
        if (!wcscmp(names[i], name))
        {
            ret = i;
            break;
        }
    }
    return ret;
}
//---------------------------------------------------------------------------//
uint32_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, uint32_t len)
{
    if (!len)
        len = ::wcslen(Source) + 1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(WCHAR_T));
#ifdef __linux__
    size_t succeed = (size_t)-1;
    size_t f = len * sizeof(wchar_t), t = len * sizeof(WCHAR_T);
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    iconv_t cd = iconv_open("UTF-16", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpWChar, &f, (char**)&tmpShort, &t);
        iconv_close(cd);
        if(succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif //__linux__
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpShort = (WCHAR_T)*tmpWChar;
    }

    return res;
}
//---------------------------------------------------------------------------//
uint32_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, uint32_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source) + 1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    WCHAR_T* tmpShort = (WCHAR_T*)Source;
    uint32_t res = 0;

    ::memset(*Dest, 0, len * sizeof(wchar_t));
#ifdef __linux__
    size_t succeed = (size_t)-1;
    const char* fromCode = sizeof(wchar_t) == 2 ? "UTF-16" : "UTF-32";
    size_t f = len * sizeof(WCHAR_T), t = len * sizeof(wchar_t);
    iconv_t cd = iconv_open("UTF-32", fromCode);
    if (cd != (iconv_t)-1)
    {
        succeed = iconv(cd, (char**)&tmpShort, &f, (char**)&tmpWChar, &t);
        iconv_close(cd);
        if(succeed != (size_t)-1)
            return (uint32_t)succeed;
    }
#endif //__linux__
    for (; len; --len, ++res, ++tmpWChar, ++tmpShort)
    {
        *tmpWChar = (wchar_t)*tmpShort;
    }

    return res;
}
//---------------------------------------------------------------------------//
uint32_t getLenShortWcharStr(const WCHAR_T* Source)
{
    uint32_t res = 0;
    WCHAR_T *tmpShort = (WCHAR_T*)Source;

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//

#ifdef LINUX_OR_MACOS
WcharWrapper::WcharWrapper(const WCHAR_T* str) : m_str_WCHAR(NULL),
                           m_str_wchar(NULL)
{
    if (str)
    {
        int len = getLenShortWcharStr(str);
        m_str_WCHAR = new WCHAR_T[len + 1];
        memset(m_str_WCHAR,   0, sizeof(WCHAR_T) * (len + 1));
        memcpy(m_str_WCHAR, str, sizeof(WCHAR_T) * len);
        ::convFromShortWchar(&m_str_wchar, m_str_WCHAR);
    }
}
#endif
//---------------------------------------------------------------------------//
WcharWrapper::WcharWrapper(const wchar_t* str) :
#ifdef LINUX_OR_MACOS
    m_str_WCHAR(NULL),
#endif 
    m_str_wchar(NULL)
{
    if (str)
    {
        int len = wcslen(str);
        m_str_wchar = new wchar_t[len + 1];
        memset(m_str_wchar, 0, sizeof(wchar_t) * (len + 1));
        memcpy(m_str_wchar, str, sizeof(wchar_t) * len);
#ifdef LINUX_OR_MACOS
        ::convToShortWchar(&m_str_WCHAR, m_str_wchar);
#endif
    }

}
//---------------------------------------------------------------------------//
WcharWrapper::~WcharWrapper()
{
#ifdef LINUX_OR_MACOS
    if (m_str_WCHAR)
    {
        delete [] m_str_WCHAR;
        m_str_WCHAR = NULL;
    }
#endif
    if (m_str_wchar)
    {
        delete [] m_str_wchar;
        m_str_wchar = NULL;
    }
}
//---------------------------------------------------------------------------//
