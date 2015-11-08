
#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"

#include "draw_engine.h"
#include "Shlwapi.h"


const wchar_t			Draw1C::CANVAS[]		=	L"ХОЛСТ";

Draw1C::Draw1C(unsigned short w, unsigned short h, char out)
{
	if (!(w > 0))
		throw L"Draw1C::Draw1C - Неверная ширина области рисования";

	if (!(h > 0))
		throw L"Draw1C::Draw1C - Неверная высота области рисования";

	if (!(canvas = new Bitmap(w, h, PixelFormat24bppRGB)))
		throw L"Draw1C::Draw1C - Не удалось создать области для холста";

	try
	{
		v_out	= new DrawOut1C(out);
	}
	catch (wchar_t *error)
		{
			delete canvas;
			throw error;
		}

	wsprintf(canvas_id, L"%s.%08X", CANVAS, this);
}

Draw1C::~Draw1C()
{
	delete v_out;
	delete canvas;
}

void Draw1C::DrawPoint(short x, short y, DrawColor color)
{
	((Bitmap *)canvas)->SetPixel(x, y, Color(color.a, color.r, color.g, color.b));
}

void Draw1C::DrawLine(short x1, short y1, short x2, short y2, int thickness, DrawColor color)
{
	Graphics g(canvas);
	Pen p(Color(color.a, color.r, color.g, color.b), (REAL)thickness);

	g.DrawLine(&p, x1, y1, x2, y2);
}

void Draw1C::DrawEllipse(short x, short y, short w, short h, int thickness, DrawColor color)
{
	Graphics g(canvas);
	Pen p(Color(color.a, color.r, color.g, color.b), (REAL)thickness);

	g.DrawEllipse(&p, x, y, w, h);
}


void Draw1C::DrawRectangle(short x, short y, short w, short h, int thickness, DrawColor color)
{
	Graphics g(canvas);
	Pen p(Color(color.a, color.r, color.g, color.b), (REAL)thickness);

	g.DrawRectangle(&p, x, y, w, h);
}


void Draw1C::DrawFillEllipse(short x, short y, short w, short h, DrawColor color)
{
	Graphics g(canvas);
	SolidBrush b(Color(color.a, color.r, color.g, color.b));

	g.FillEllipse(&b, x, y, w, h);
}

void Draw1C::DrawFillRectangle(short x, short y, short w, short h, DrawColor color)
{
	Graphics g(canvas);
	SolidBrush b(Color(color.a, color.r, color.g, color.b));

	g.FillRectangle(&b, x, y, w, h);
}

void Draw1C::DrawTriangle(short x1, short y1, short x2, short y2, short x3, short y3, int thickness, DrawColor color)
{
	Graphics g(canvas);
	Pen pen(Color(color.a, color.r, color.g, color.b), (REAL)thickness);
	Point *p = new Point[3];

	p[0].X = x1;
	p[0].Y = y1;
	p[1].X = x2;
	p[1].Y = y2;
	p[2].X = x3;
	p[2].Y = y3;

	g.DrawPolygon(&pen, p, 3);

	delete[] p;
}

void Draw1C::DrawFillTriangle(short x1, short y1, short x2, short y2, short x3, short y3, DrawColor color)
{
	Graphics g(canvas);
	SolidBrush b(Color(color.a, color.r, color.g, color.b));
	Point *p = new Point[3];

	p[0].X = x1;
	p[0].Y = y1;
	p[1].X = x2;
	p[1].Y = y2;
	p[2].X = x3;
	p[2].Y = y3;

	g.FillPolygon(&b, p, 3);

	delete[] p;
}

void Draw1C::DrawString(wchar_t *string, short x, short y, short w, short h, int h_align, int v_align, wchar_t *font, int font_size, int font_style, DrawColor color)
{
	Graphics g(canvas);
	SolidBrush b(Color(color.a, color.r, color.g, color.b));
	RectF r(x, y, w, h);
	Font f(font, (REAL)font_size, font_style);
	StringFormat sf;

	sf.SetAlignment((StringAlignment)h_align);
	sf.SetLineAlignment((StringAlignment)v_align);

	g.DrawString(string, -1, &f, r, &sf, &b);

}

void Draw1C::DrawImage(wchar_t *image_path, short  x, short y, short w, short h, int angle, bool flip_x, bool flip_y)
{
	if (!image_path)
		return;

	if (!wcslen(image_path))
		return;

	Image image(image_path, false);

	if (flip_x || flip_y)
	{
		RotateFlipType rft;

		if (flip_x && !flip_y)
			rft = RotateFlipType::RotateNoneFlipX;
		else if (!flip_x && flip_y)
			rft = RotateFlipType::RotateNoneFlipY;
		else if (flip_x && flip_y)
			rft = RotateFlipType::RotateNoneFlipXY;

		image.RotateFlip(rft);
	}

	if (angle)
	{
		Image *image_copy = image.Clone();
		Graphics gr(image_copy);

		int xc = image_copy->GetWidth() / 2;
		int yc = image_copy->GetHeight() / 2;

		gr.TranslateTransform((REAL)xc, (REAL)yc);
		gr.RotateTransform((REAL)angle);
		gr.TranslateTransform((REAL)-xc, (REAL)-yc);
		gr.Clear(Color(0, 0, 0, 0));
		gr.DrawImage(&image, 0, 0, image.GetWidth(), image.GetHeight());

		Graphics g(canvas);
		g.DrawImage(image_copy, x, y, w, h);

		delete image_copy;
	}
	else
	{
		Graphics g(canvas);
		g.DrawImage(&image, x, y, w, h);
	}
}

void Draw1C::DrawImage(Image *image, short  x, short y, short w, short h, int angle, bool flip_x, bool flip_y)
{
	if (!image)
		return;

	if (!angle && !flip_x && !flip_y)
	{
		Graphics g(canvas);
		g.DrawImage(image, x, y, w, h);
	} else
	{
		Image *image_copy = image->Clone();

		if (flip_x || flip_y)
		{
			RotateFlipType rft;

			if (flip_x && !flip_y)
				rft = RotateFlipType::RotateNoneFlipX;
			else if (!flip_x && flip_y)
				rft = RotateFlipType::RotateNoneFlipY;
			else if (flip_x && flip_y)
				rft = RotateFlipType::RotateNoneFlipXY;

			image_copy->RotateFlip(rft);
		}

		if (angle)
		{
			Graphics gr(image_copy);

			int xc = image_copy->GetWidth() / 2;
			int yc = image_copy->GetHeight() / 2;

			gr.TranslateTransform((REAL)xc, (REAL)yc);
			gr.RotateTransform((REAL)angle);
			gr.TranslateTransform((REAL)-xc, (REAL)-yc);
			gr.Clear(Color(0, 0, 0, 0));
			gr.DrawImage(image, 0, 0, image->GetWidth(), image->GetHeight());
		}

		Graphics g(canvas);
		g.DrawImage(image_copy, x, y, w, h);

		delete image_copy;
	}
}


void Draw1C::GetPicture(void **ptr, int *size)
{
	v_out->Get(canvas, ptr, size);
}

void Draw1C::GetPicture(void **ptr, int *size, int x, int y, int w, int h)
{
	v_out->Get(canvas, ptr, size, x, y, w, h);
}
