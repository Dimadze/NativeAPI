#include "stdlib.h"
#include "stdio.h"
#include "wchar.h"
#include "windows.h"

#include "draw_dispatch.h"

bool			DrawDispatch_Init		= false;

wchar_t			DrawDispatch_LastError[256];

Draw1C_List		*drw1c_list				= NULL;
Image1C_List	*img1c_list				= NULL;

Draw1C *GetDraw1CByCanvasID(wchar_t *canvas_id)
{
	if (!DrawDispatch_Init)
		return NULL;

	Draw1C *drw1c;

	if (!wcslen(canvas_id))
		return NULL;

	drw1c_list->EnumStart();
	while (drw1c = drw1c_list->EnumNext())
	{
		if (!wcscmp(canvas_id, drw1c->canvas_id))
			return drw1c;
	}

	return NULL;
}

Image1C *GetImage1CByImageID(wchar_t *image_id)
{
	if (!DrawDispatch_Init)
		return NULL;

	if (!wcslen(image_id))
		return NULL;

	Image1C *img1c;

	img1c_list->EnumStart();
	while (img1c = img1c_list->EnumNext())
	{
		if (!wcscmp(image_id, img1c->image_id))
			return img1c;
	}

	return NULL;
}


void DrawDispatch_Initialization()
{
	if (DrawDispatch_Init)
		return;

	drw1c_list			= new Draw1C_List();
	img1c_list			= new Image1C_List();
	DrawDispatch_Init	= true;
}

void DrawDispatch_Deinitialization()
{
	if (!DrawDispatch_Init)
		return;

	DrawDispatch_Init	= false;
		
	delete drw1c_list;
	delete img1c_list;

}

wchar_t *DrawDispatch_Image_Create(wchar_t *image)
{
	if (!DrawDispatch_Init)
		return NULL;

	if (!wcslen(image))
		return NULL;

	Draw1C *drw1c;
	Image1C *img1c;

	if (!wcsicmp(L"БуферОбмена", image) || !wcsicmp(L"ClipBoard", image))
	{
		try
		{
			Image1C *img1c = img1c_list->Add();
			return img1c->image_id;
		}
		catch (wchar_t *e)
		{
			wcscpy(DrawDispatch_LastError, e);
			return NULL;
		}
	}

	drw1c = GetDraw1CByCanvasID(image);

	if (drw1c)
	{
		try
		{
			Image1C *img1c_copy = img1c_list->Add(drw1c->canvas);
			return img1c_copy->image_id;
		}
		catch (wchar_t *e)
		{
			wcscpy(DrawDispatch_LastError, e);
			return NULL;
		}
	}

	img1c = GetImage1CByImageID(image);

	if (img1c)
	{
		try
		{
			Image1C *img1c_copy = img1c_list->Add(img1c->image);
			return img1c_copy->image_id;
		}
		catch (wchar_t *e)
		{
			wcscpy(DrawDispatch_LastError, e);
			return NULL;
		}
	}

	try
	{
		img1c = img1c_list->Add(image);
		return img1c->image_id;
	}
	catch (wchar_t *e)
	{
		wcscpy(DrawDispatch_LastError, e);
		return NULL;
	}
}

bool DrawDispatch_Image_Resize(wchar_t *image_id, int w, int h)
{
	if (!DrawDispatch_Init)
		return false;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
	{
		img1c->Resize(w, h);
		return true;
	}

	return false;
}

bool DrawDispatch_Image_Rotate(wchar_t *image_id, int angle, bool changedimensions)
{
	if (!DrawDispatch_Init)
		return false;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
	{
		img1c->Rotate(angle, changedimensions);
		return true;
	}

	return false;
}

bool DrawDispatch_Image_Flip(wchar_t *image_id, bool x, bool y)
{
	if (!DrawDispatch_Init)
		return false;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
	{
		img1c->Flip(x, y);
		return true;
	}

	return false;
}

bool DrawDispatch_Image_Crop(wchar_t *image_id, int x, int y, int w, int h)
{
	if (!DrawDispatch_Init)
		return false;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
	{
		img1c->Crop(x, y, w, h);
		return true;
	}

	return false;
}

bool DrawDispatch_Image_ToClipBoard(wchar_t *image_id)
{
	if (!DrawDispatch_Init)
		return false;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
		return img1c->ToClipBoard();

	return false;
}

bool DrawDispatch_Image_Delete(wchar_t *image_id)
{
	if (!DrawDispatch_Init)
		return false;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
	{
		img1c_list->Remove(img1c);
		return true;
	}

	return false;
}

int DrawDispatch_Image_GetWidth(wchar_t *image_id)
{
	if (!DrawDispatch_Init)
		return 0;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
		return img1c->GetWidth();

	return 0;
}

int DrawDispatch_Image_GetHeight(wchar_t *image_id)
{
	if (!DrawDispatch_Init)
		return 0;
		
	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
		return img1c->GetHeight();

	return 0;
}

void DrawDispatch_Image_GetPicture(wchar_t *image_id, char out, void **picture_file_body, int *picture_file_size)
{
	if (!DrawDispatch_Init)
		return;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
		img1c->GetPicture(out, picture_file_body, picture_file_size);
}

void DrawDispatch_Image_GetPictureClip(wchar_t *image_id, char out, void **picture_file_body, int *picture_file_size, short x, short y, short w, short h)
{
	if (!DrawDispatch_Init)
		return;

	Image1C *img1c = GetImage1CByImageID(image_id);

	if (img1c)
		img1c->GetPicture(out, picture_file_body, picture_file_size, x, y, w, h);
}

wchar_t *DrawDispatch_Canvas_Create(unsigned short w, unsigned short h, char out)
{
	if (!DrawDispatch_Init)
		return NULL;

	try 
	{
		Draw1C *drw1c;
		if (drw1c = drw1c_list->Add(w, h, out))
			return drw1c->canvas_id;
	} catch(wchar_t *e)
	{
		wcscpy(DrawDispatch_LastError, e);
	}

	return NULL;
}

bool DrawDispatch_Canvas_Delete(wchar_t *canvas_id)
{
	if (!DrawDispatch_Init)
		return false;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
	{
		drw1c_list->Remove(drw1c);
		return true;
	}

	return false;
}

void DrawDispatch_Canvas_GetPicture(wchar_t *canvas_id, void **picture_file_body, int *picture_file_size)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->GetPicture(picture_file_body, picture_file_size);
}

void DrawDispatch_Canvas_GetPictureClip(wchar_t *canvas_id, void **picture_file_body, int *picture_file_size, short x, short y, short w, short h)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->GetPicture(picture_file_body, picture_file_size, x, y, w, h);
}

void DrawDispatch_Draw_RGBToString(wchar_t *color, unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{
	if (color)
		wsprintf(color, L"%02X%02X%02X%02X", b, g, r, a);
}

DrawColor DrawDispatch_Draw_StringToColor(wchar_t *color)
{
	if (!color)
		return { 0, 0, 0, 0 };

	if (!wcsicmp(color, L"Красный") ||
		!wcsicmp(color, L"Red"))
	{
		return { 255, 255, 0, 0 };
	}
	else
	if (!wcsicmp(color, L"Оранжевый") ||
		!wcsicmp(color, L"Orange"))
	{
		return{ 255, 255, 128, 0 };
	}
	else
	if (!wcsicmp(color, L"Жёлтый") ||
		!wcsicmp(color, L"Yellow"))
	{
		return{ 255, 255, 255, 0 };
	}
	else
	if (!wcsicmp(color, L"Зелёный") ||
		!wcsicmp(color, L"Green"))
	{
		return { 255, 0, 255, 0 };
	}
	else
	if (!wcsicmp(color, L"Синий") ||
		!wcsicmp(color, L"Blue"))
	{
		return { 255, 0, 0, 255 };
	}
	else
	if (!wcsicmp(color, L"Феолетовый") ||
		!wcsicmp(color, L"Purple"))
	{
		return{ 255, 90, 0, 157 };
	}
	else
	if (!wcsicmp(color, L"Серый") ||
		!wcsicmp(color, L"Gray"))
	{
		return{ 255, 133, 133, 133 };
	}
	else
	if (!wcsicmp(color, L"Белый") ||
		!wcsicmp(color, L"White"))
	{
		return { 255, 255, 255, 255 };
	}
	else
	if (!wcsicmp(color, L"Чёрный") ||
		!wcsicmp(color, L"Black"))
	{
		return { 255, 0, 0, 0 };
	}
	else
	{
		int a; int r; int g; int b;
		swscanf(color, L"%02X%02X%02X%02X", &b, &g, &r, &a);
		return { (unsigned char)a, (unsigned char)r, (unsigned char)g, (unsigned char)b };
	}
}

void DrawDispatch_Draw_Point(wchar_t *canvas_id, short x, short y, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawPoint(x, y, DrawDispatch_Draw_StringToColor(color));
}

void DrawDispatch_Draw_Line(wchar_t *canvas_id, short x1, short y1, short x2, short y2, int thickness, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawLine(x1, y1, x2, y2, thickness, DrawDispatch_Draw_StringToColor(color));
}


void DrawDispatch_Draw_Ellipse(wchar_t *canvas_id, short x, short y, short w, short h, int thickness, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawEllipse(x, y, w, h, thickness, DrawDispatch_Draw_StringToColor(color));
}

void DrawDispatch_Draw_FillEllipse(wchar_t *canvas_id, short x, short y, short w, short h, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawFillEllipse(x, y, w, h, DrawDispatch_Draw_StringToColor(color));
}

void DrawDispatch_Draw_Rectangle(wchar_t *canvas_id, short x, short y, short w, short h, int thickness, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawRectangle(x, y, w, h, thickness, DrawDispatch_Draw_StringToColor(color));
}

void DrawDispatch_Draw_FillRectangle(wchar_t *canvas_id, short x, short y, short w, short h, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawFillRectangle(x, y, w, h, DrawDispatch_Draw_StringToColor(color));
}


void DrawDispatch_Draw_Triangle(wchar_t *canvas_id, short x1, short y1, short x2, short y2, short x3, short y3, int thickness, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawTriangle(x1, y1, x2, y2, x3, y3, thickness, DrawDispatch_Draw_StringToColor(color));
}

void DrawDispatch_Draw_FillTriangle(wchar_t *canvas_id, short x1, short y1, short x2, short y2, short x3, short y3, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawFillTriangle(x1, y1, x2, y2, x3, y3, DrawDispatch_Draw_StringToColor(color));
}

void DrawDispatch_Draw_String(wchar_t *canvas_id, wchar_t *string, short  x, short y, short w, short h, int h_align, int v_align, wchar_t *font, int font_size, int font_style, wchar_t *color)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
		drw1c->DrawString(string, x, y, w, h, h_align, v_align, font, font_size, font_style, DrawDispatch_Draw_StringToColor(color));
}

void DrawDispatch_Draw_Image(wchar_t *canvas_id, wchar_t *image_id, short  x, short y, short w, short h, int angle, bool flip_x, bool flip_y)
{
	if (!DrawDispatch_Init)
		return;

	Draw1C *drw1c = GetDraw1CByCanvasID(canvas_id);

	if (drw1c)
	{
		Image1C *img1c = GetImage1CByImageID(image_id);

		if (img1c)
			drw1c->DrawImage(img1c->image, x, y, w, h, angle, flip_x, flip_y);
		else
			drw1c->DrawImage(image_id, x, y, w, h, angle, flip_x, flip_y);
	}
}






