#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

#pragma pack(push, 4)

///////////////////////////////////////////////////////////////////////////////
// class CAddInNative
class CAddInNative : public IComponentBase
{
public:
	
	enum Props
	{
		ePropVersion = 0,
		ePropLast      // Always last
	};

	enum Methods
	{
		eMethCanvas_Create = 0,
		eMethCanvas_Binary,
		eMethCanvas_ClipBinary,
		eMethCanvas_Delete,

		eMethImage_Create,
		eMethImage_Resize,
		eMethImage_Rotate,
		eMethImage_Flip,
		eMethImage_Crop,
		eMethImage_ToClipBoard,
		eMethImage_GetWidth,
		eMethImage_GetHeight,
		eMethImage_Binary,
		eMethImage_ClipBinary,
		eMethImage_Delete,

		eMethDraw_GetColor,
		eMethDraw_Point,
		eMethDraw_Line,
		eMethDraw_Ellipse,
		eMethDraw_FillEllipse,
		eMethDraw_Rectangle,
		eMethDraw_FillRectangle,
		eMethDraw_Triangle,
		eMethDraw_FillTriangle,
		eMethDraw_String,
		eMethDraw_Image,

		eMethTimer_Start,
		eMethTimer_Stop,

		eMethInput_InstallHook,
		eMethInput_ActivateKeyBoardHook,
		eMethInput_SetKeyHook,
		eMethInput_RemoveKeyHook,
		eMethInput_DeactivateKeyBoardHook,
		eMethInput_ActivateMouseHook,
		eMethInput_SetMouseActionHook,
		eMethInput_RemoveMouseActionHook,
		eMethInput_DeactivateMouseHook,
		eMethInput_DeinstallHook,

		eMethWindow_Main,
		eMethWindow_GetByTitle,
		eMethWindow_IsChild,
		eMethWindow_GetCoordinates,
		eMethWindow_ToClientCoordinates,
		eMethWindow_Move,
		eMethWindow_Action,

		eMethTest_ExternalEvent,

		eMethLast      // Always last
	};

    CAddInNative(void);
    virtual ~CAddInNative();
    // IInitDoneBase
    virtual bool ADDIN_API Init(void*);
    virtual bool ADDIN_API setMemManager(void* mem);
    virtual long ADDIN_API GetInfo();
    virtual void ADDIN_API Done();
    // ILanguageExtenderBase
    virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T**);
    virtual long ADDIN_API GetNProps();
    virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
    virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
    virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
    virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* varPropVal);
    virtual bool ADDIN_API IsPropReadable(const long lPropNum);
    virtual bool ADDIN_API IsPropWritable(const long lPropNum);
    virtual long ADDIN_API GetNMethods();
    virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
    virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, 
                            const long lMethodAlias);
    virtual long ADDIN_API GetNParams(const long lMethodNum);
    virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum,
                            tVariant *pvarParamDefValue);   
    virtual bool ADDIN_API HasRetVal(const long lMethodNum);
    virtual bool ADDIN_API CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray);
    virtual bool ADDIN_API CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
    // LocaleBase
    virtual void ADDIN_API SetLocale(const WCHAR_T* loc);
    
private:
	int			digital(tVariant *digital);
	void		allocString(tVariant *pvarPropVal, wchar_t *string);
    long		findName(const wchar_t* names[], const wchar_t* name, const uint32_t size) const;
    void		addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code);

    // Attributes
    IAddInDefBase      *m_iConnect;
    IMemoryManager     *m_iMemory;


public:
	wchar_t				m_wstrVersion[32];
	wchar_t				m_wstrTemp[128];
};

class WcharWrapper
{
public:
#ifdef LINUX_OR_MACOS
    WcharWrapper(const WCHAR_T* str);
#endif
    WcharWrapper(const wchar_t* str);
    ~WcharWrapper();
#ifdef LINUX_OR_MACOS
    operator const WCHAR_T*(){ return m_str_WCHAR; }
    operator WCHAR_T*(){ return m_str_WCHAR; }
#endif
    operator const wchar_t*(){ return m_str_wchar; }
    operator wchar_t*(){ return m_str_wchar; }
private:
    WcharWrapper& operator = (const WcharWrapper& other) {}; 
    WcharWrapper(const WcharWrapper& other) {};
private:
#ifdef LINUX_OR_MACOS
    WCHAR_T* m_str_WCHAR;
#endif
    wchar_t* m_str_wchar;
};
#endif //__ADDINNATIVE_H__

#pragma pack(pop)