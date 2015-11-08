#ifndef __DRAWOUT1C_H__
#define __DRAWOUT1C_H__

#include "windows.h"
#include <GdiPlus.h>

using namespace Gdiplus;


class DrawOut1C
{
	private:

		typedef enum {
			DrawOut_BMP		= 0,
			DrawOut_PNG		= 1,
			DrawOut_GIF		= 2,
			DrawOut_JPG		= 3,
			DrawOut_TIFF	= 4,
			DrawOut_ICO		= 5,
			DrawOut_Last
		} DrawOut;

	private:

		IStream			*v_out_stream;
		void			*v_out_picture;
		CLSID			v_out_clsid;
		DrawOut			v_out;

		int				GetEncoderClsid(const WCHAR *format, CLSID *pClsid);
		void			OutToCLSID(char out, CLSID *clsid);

	public:

		DrawOut1C();
		DrawOut1C(char out);
		~DrawOut1C();

		void SetType(char out);

		void Get(Image *image, void **ptr, int *size);
		void Get(Image *image, void **ptr, int *size, int x, int y, int w, int h);
};

#endif //__DRAWOUT1C_H__