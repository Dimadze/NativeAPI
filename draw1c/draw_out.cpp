
#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"

#include "draw_out.h"
#include "Shlwapi.h"

DrawOut1C::DrawOut1C()
{
	SetType(0);

	if (!(v_out_stream = SHCreateMemStream(NULL, NULL)))
		throw L"DrawOut1C::DrawOut1C - Не удалось создать поток для вывода";

	v_out_picture = NULL;
}

DrawOut1C::DrawOut1C(char out)
{
	SetType(out);

	if (!(v_out_stream = SHCreateMemStream(NULL, NULL)))
		throw L"DrawOut1C::DrawOut1C - Не удалось создать поток для вывода";

	v_out_picture	=	NULL;
}

DrawOut1C::~DrawOut1C()
{
	if (v_out_stream)
		v_out_stream->Release();

	if (v_out_picture)
		delete v_out_picture;
}

int DrawOut1C::GetEncoderClsid(const WCHAR *format, CLSID *pClsid)
{
	UINT  num = 0;
	UINT  size = 0;

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return -1;

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return -1;

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}

	free(pImageCodecInfo);
	return -1;
}

void DrawOut1C::OutToCLSID(char out, CLSID *clsid)
{
	switch ((DrawOut)out)
	{
		case DrawOut::DrawOut_PNG:
		{
			GetEncoderClsid(L"image/png", clsid);
			break;
		}

		case DrawOut::DrawOut_GIF:
		{
			GetEncoderClsid(L"image/gif", clsid);
			break;
		}

		case DrawOut::DrawOut_JPG:
		{
			GetEncoderClsid(L"image/jpeg", clsid);
			break;
		}

		case DrawOut::DrawOut_TIFF:
		{
			GetEncoderClsid(L"image/tiff", clsid);
			break;
		}

		case DrawOut::DrawOut_ICO:
		{
			GetEncoderClsid(L"image/vnd.microsoft.icon", clsid);
			break;
		}

		case DrawOut::DrawOut_BMP: default:
		{
			GetEncoderClsid(L"image/bmp", clsid);
			break;
		}
	}
}

void DrawOut1C::SetType(char out)
{
	v_out = (DrawOut)out;
	OutToCLSID(v_out, &v_out_clsid);
}

void DrawOut1C::Get(Image *image, void **ptr, int *size)
{
	unsigned long tv_file_size;

	ULARGE_INTEGER	tv_ui;
	LARGE_INTEGER	tv_li;

	if (!ptr && !size)
		return;

	*ptr	= NULL;
	*size	= NULL;

	memset((void *)&tv_li, 0, sizeof(tv_li));

	//Запишем картинку в поток
	if (!v_out_stream->Seek(tv_li, STREAM_SEEK_SET, &tv_ui))
	{
		if (!image->Save(v_out_stream, &v_out_clsid, NULL))
		{
			//Посчитаем размер
			if (!v_out_stream->Seek(tv_li, STREAM_SEEK_CUR, &tv_ui))
			{
				tv_file_size = tv_ui.LowPart;

				//Выделим буфер и считаем в него файл
				if (!v_out_stream->Seek(tv_li, STREAM_SEEK_SET, &tv_ui))
				{
					if (v_out_picture = realloc(v_out_picture, tv_file_size))
					{
						if (!v_out_stream->Read(v_out_picture, tv_file_size, &tv_file_size))
						{
							*ptr	= v_out_picture;
							*size	= tv_file_size;
						}
					}
				}
			}
		}
	}
}

void DrawOut1C::Get(Image *image, void **ptr, int *size, int x, int y, int w, int h)
{
	unsigned long tv_file_size;

	ULARGE_INTEGER	tv_ui;
	LARGE_INTEGER	tv_li;

	if (!ptr && !size)
		return;

	*ptr	= NULL;
	*size	= NULL;


	Image		*is			= image;
	Bitmap		*image_crop	= NULL;

	if (!(x == 0 && y == 0 && w == is->GetWidth() && h == is->GetHeight()))
	{
		image_crop	=	new Bitmap(w, h);
		Graphics	g(image_crop);
		g.DrawImage(image, 0, 0, x, y, w, h, Gdiplus::Unit::UnitPixel);

		is			= image_crop;
	}

	memset((void *)&tv_li, 0, sizeof(tv_li));

	//Запишем картинку в поток
	if (!v_out_stream->Seek(tv_li, STREAM_SEEK_SET, &tv_ui))
	{
		if (!is->Save(v_out_stream, &v_out_clsid, NULL))
		{
			//Посчитаем размер
			if (!v_out_stream->Seek(tv_li, STREAM_SEEK_CUR, &tv_ui))
			{
				tv_file_size = tv_ui.LowPart;

				//Выделим буфер и считаем в него файл
				if (!v_out_stream->Seek(tv_li, STREAM_SEEK_SET, &tv_ui))
				{
					if (v_out_picture = realloc(v_out_picture, tv_file_size))
					{
						if (!v_out_stream->Read(v_out_picture, tv_file_size, &tv_file_size))
						{
							*ptr = v_out_picture;
							*size = tv_file_size;
						}
					}
				}
			}
		}
	}

	if (image_crop)
		delete image_crop;
}
