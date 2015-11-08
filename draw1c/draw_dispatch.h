#ifndef __DRAW_DISPATCH_H__

#include "windows.h"
#include "draw_engine.h"
#include "image_work.h"

void DrawDispatch_Initialization();
void DrawDispatch_Deinitialization();

wchar_t *DrawDispatch_Image_Create(wchar_t *image);
bool DrawDispatch_Image_Resize(wchar_t *image_id, int w, int h);
bool DrawDispatch_Image_Rotate(wchar_t *image_id, int angle, bool changedimensions);
bool DrawDispatch_Image_Flip(wchar_t *image_id, bool x, bool y);
bool DrawDispatch_Image_Crop(wchar_t *image_id, int x, int y, int w, int h);
bool DrawDispatch_Image_ToClipBoard(wchar_t *image_id);
bool DrawDispatch_Image_Delete(wchar_t *image_id);

int DrawDispatch_Image_GetWidth(wchar_t *image_id);
int DrawDispatch_Image_GetHeight(wchar_t *image_id);

void DrawDispatch_Image_GetPicture(wchar_t *image_id, char out, void **picture_file_body, int *picture_file_size);
void DrawDispatch_Image_GetPictureClip(wchar_t *image_id, char out, void **picture_file_body, int *picture_file_size, short x, short y, short w, short h);

wchar_t *DrawDispatch_Canvas_Create(unsigned short w, unsigned short h, char out);
bool DrawDispatch_Canvas_Delete(wchar_t *canvas_id);
void DrawDispatch_Canvas_GetPicture(wchar_t *canvas_id, void **picture_file_body, int *picture_file_size);
void DrawDispatch_Canvas_GetPictureClip(wchar_t *canvas_id, void **picture_file_body, int *picture_file_size, short x, short y, short w, short h);

void DrawDispatch_Draw_RGBToString(wchar_t *color, unsigned char a, unsigned char r, unsigned char g, unsigned char b);
DrawColor DrawDispatch_Draw_StringToColor(wchar_t *color);

void DrawDispatch_Draw_Point(wchar_t *canvas_id, short x, short y, wchar_t *color);
void DrawDispatch_Draw_Line(wchar_t *canvas_id, short x1, short y1, short x2, short y2, int thickness, wchar_t *color);
void DrawDispatch_Draw_Ellipse(wchar_t *canvas_id, short x, short y, short w, short h, int thickness, wchar_t *color);
void DrawDispatch_Draw_FillEllipse(wchar_t *canvas_id, short x, short y, short w, short h, wchar_t *color);
void DrawDispatch_Draw_Rectangle(wchar_t *canvas_id, short x, short y, short w, short h, int thickness, wchar_t *color);
void DrawDispatch_Draw_FillRectangle(wchar_t *canvas_id, short x, short y, short w, short h, wchar_t *color);
void DrawDispatch_Draw_Triangle(wchar_t *canvas_id, short x1, short y1, short x2, short y2, short x3, short y3, int thickness, wchar_t *color);
void DrawDispatch_Draw_FillTriangle(wchar_t *canvas_id, short x1, short y1, short x2, short y2, short x3, short y3, wchar_t *color);
void DrawDispatch_Draw_String(wchar_t *canvas_id, wchar_t *string, short  x, short y, short w, short h, int h_align, int v_align, wchar_t *font, int font_size, int font_style, wchar_t *color);
void DrawDispatch_Draw_Image(wchar_t *canvas_id, wchar_t *image_id, short  x, short y, short w, short h, int angle, bool flip_x, bool flip_y);


#endif //__DRAW_DISPATCH_H__
