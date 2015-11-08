#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"
#include "Shlwapi.h"

#include "image_work.h"

const wchar_t	Image1C::IMAGE[] = L"ИЗОБРАЖЕНИЕ";

Image1C::Image1C()
{
	HBITMAP tv_hbitmap;

	try
	{
		v_out = new DrawOut1C();
	}
	catch (wchar_t *error)
	{
		throw error;
	}

	if (!OpenClipboard(NULL))
		throw L"Image1C::Image1C - Ошибка открытия буфера обмена";

	if (!(tv_hbitmap = (HBITMAP)GetClipboardData(CF_BITMAP)))
	{
		throw L"Image1C::Image1C - Ошибка открытия изображения буфера обмена";
		CloseClipboard();
	}

	image = new Bitmap(tv_hbitmap, NULL);


	if (!image)
	{
		throw L"Image1C::Image1C - Не удалось создать изображение";
		CloseClipboard();
	}


	wsprintf(image_id, L"%s.%08X", IMAGE, this);
	CloseClipboard();
}

Image1C::Image1C(wchar_t *img_path)
{
	try
	{
		v_out = new DrawOut1C();
	}
	catch (wchar_t *error)
	{
		throw error;
	}

	image = new Image(img_path, 0);

	if (!image)
		throw L"Image1C::Image1C - Не удалось создать изображение";

	wsprintf(image_id, L"%s.%08X", IMAGE, image);
}

Image1C::Image1C(Image *img)
{
	try
	{
		v_out = new DrawOut1C();
	}
	catch (wchar_t *error)
	{
		throw error;
	}

	image = img->Clone();

	if (!image)
		throw L"Image1C::Image1C - Не удалось создать изображение";

	wsprintf(image_id, L"%s.%08X", IMAGE, this);
}

short Image1C::GetHeight()
{
	return image->GetHeight();
}

short Image1C::GetWidth()
{
	return image->GetWidth();
}

double Image1C::DegreeToRadian(int angle)
{
	return angle * M_PI / 180;
}

void Image1C::Resize(short w, short h)
{
	Bitmap		*image_rsz = new Bitmap(w, h);
	Graphics	g(image_rsz);

	g.Clear(Color(0, 0, 0, 0));
	g.DrawImage(image, 0, 0, w, h);

	delete image; image = image_rsz;
}

void Image1C::Rotate(int angle, bool changedimensions)
{
	if (!changedimensions)
	{
		int xc = image->GetWidth() / 2;
		int yc = image->GetHeight() / 2;
		
		Graphics g(image);
		Image *image_copy = image->Clone();

		g.TranslateTransform((REAL)xc, (REAL)yc);
		g.RotateTransform((REAL)angle);
		g.TranslateTransform((REAL)-xc, (REAL)-yc);
		g.Clear(Color(0, 0, 0, 0));
		g.DrawImage(image_copy, 0, 0, image_copy->GetWidth(), image_copy->GetHeight());

		delete image_copy;
	}
	else
	{
		//Преоброзуем угол в удобный вид
		int a = (360 + (angle % 360)) % 360;

		switch (a)
		{
			case 0:
				image->RotateFlip(RotateFlipType::RotateNoneFlipNone);
				break;
			case 90:
				image->RotateFlip(RotateFlipType::Rotate90FlipNone);
				break;
			case 180:
				image->RotateFlip(RotateFlipType::Rotate180FlipNone);
				break;
			case 270:
				image->RotateFlip(RotateFlipType::Rotate270FlipNone);
				break;
			default:
				{
					double	x, y, ar, ow, oh, nw_1, nh_1, cos_a, sin_a;
					int		nw, nh;

					ow		= image->GetWidth();
					oh		= image->GetHeight();


					if ((a > 90 && a < 180) || (a > 270 && a < 360))
					{
						ar		= DegreeToRadian(a - 90);

						cos_a	= cos(ar);
						sin_a	= sin(ar);
						 
						nh		= abs((int)ceil(ow * cos_a + oh * sin_a));
						nw		= abs((int)ceil(ow * sin_a + oh * cos_a));
					}
					else
					{
						ar		= DegreeToRadian(a);

						cos_a	= cos(ar);
						sin_a	= sin(ar);

						nw		= abs((int)ceil(ow * cos_a + oh * sin_a));
						nh		= abs((int)ceil(ow * sin_a + oh * cos_a));
					}

					x			= nw / 2;
					y			= nh / 2;

					nw_1		= 1 / (nw / ow);
					nh_1		= 1 / (nh / oh);

					Bitmap		*image_rot = new Bitmap((int)nw, (int)nh);
					Graphics	g(image_rot);
					
					g.TranslateTransform((REAL)x, (REAL)y);
					g.RotateTransform((REAL)a);
					g.ScaleTransform((REAL)nw_1, (REAL)nh_1);
					g.TranslateTransform((REAL)-x, (REAL)-y);
					
					g.Clear(Color(0, 0, 0, 0));
					g.DrawImage(image, 0, 0, (int)nw, (int)nh);

					delete image; image = image_rot;
					break;
				}
		}
	}
}

void Image1C::Flip(bool x, bool y)
{
	RotateFlipType rft;

	if (!x && !y)
		rft = RotateFlipType::RotateNoneFlipNone;
	else if (x && !y)
		rft = RotateFlipType::RotateNoneFlipX;
	else if (!x && y)
		rft = RotateFlipType::RotateNoneFlipY;
	else if (x && y)
		rft = RotateFlipType::RotateNoneFlipXY;

	image->RotateFlip(rft);
}

void Image1C::Crop(int x, int y, int w, int h)
{
	if (!w || !h)
		return;

	Bitmap		*image_crop = new Bitmap(w, h);
	Graphics	g(image_crop);
	g.DrawImage(image, RectF(0, 0, (REAL)w, (REAL)h), (REAL)x, (REAL)y, (REAL)w, (REAL)h, Gdiplus::Unit::UnitPixel);
	
	delete image; image = image_crop;
}

bool Image1C::ToClipBoard()
{
	HDC		tv_hdc;
	HBITMAP	tv_hbitmap = NULL, tv_hdbb = NULL;

	((Bitmap *)image)->GetHBITMAP(Color(0, 0, 0, 0), &tv_hbitmap);

	if (!tv_hbitmap)
		return false;

	DIBSECTION ds;
	GetObject(tv_hbitmap, sizeof(ds), &ds);
	ds.dsBmih.biCompression = BI_RGB;

	tv_hdbb = CreateDIBitmap(tv_hdc = GetDC(NULL), &ds.dsBmih, CBM_INIT, ds.dsBm.bmBits, (BITMAPINFO *)&ds.dsBmih, DIB_RGB_COLORS);
	ReleaseDC(NULL, tv_hdc);

	if (!tv_hdbb)
	{
		DeleteObject(tv_hbitmap);
		return false;
	}

	if (!OpenClipboard(NULL))
	{
		DeleteObject(tv_hbitmap);
		return false;
	}

	EmptyClipboard(); 
	SetClipboardData(CF_BITMAP, tv_hdbb);
	CloseClipboard(); 

	DeleteObject(tv_hbitmap);

	return true;
}

void Image1C::GetPicture(char out, void **ptr, int *size)
{
	v_out->SetType(out);
	v_out->Get(image, ptr, size);
}

void Image1C::GetPicture(char out, void **ptr, int *size, int x, int y, int w, int h)
{
	v_out->SetType(out);
	v_out->Get(image, ptr, size, x, y, w, h);
}

Image1C::~Image1C()
{
	delete v_out;
	delete image;
}
