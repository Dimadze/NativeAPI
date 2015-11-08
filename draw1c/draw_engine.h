#ifndef __DRAW1C_H__

#include "windows.h"
#include "draw_out.h"
#include <GdiPlus.h>

using namespace Gdiplus;

typedef struct {
	unsigned char	a;
	unsigned char	r;
	unsigned char	g;
	unsigned char	b;
} DrawColor;


class Draw1C
{
	private:

		DrawOut1C					*v_out;

	public:

		Image						*canvas;
		wchar_t						canvas_id[64];
		UINT_PTR					timer_id;

		static const wchar_t		CANVAS[64];

		Draw1C(unsigned short w, unsigned short h, char out);
		~Draw1C();

		void		DrawPoint(short x, short y, DrawColor color);
		void		DrawLine(short x1, short y1, short x2, short y2, int thickness, DrawColor color);

		void		DrawEllipse(short x, short y, short w, short h, int thickness, DrawColor color);
		void		DrawFillEllipse(short x, short y, short w, short h, DrawColor color);

		void		DrawRectangle(short x, short y, short w, short h, int thickness, DrawColor color);
		void		DrawFillRectangle(short x, short y, short w, short h, DrawColor color);

		void		DrawTriangle(short x1, short y1, short x2, short y2, short x3, short y3, int thickness, DrawColor color);
		void		DrawFillTriangle(short x1, short y1, short x2, short y2, short x3, short y3, DrawColor color);

		void		DrawString(wchar_t *string, short x, short y, short w, short h, int h_align, int v_align, wchar_t *font, int font_size, int font_style, DrawColor color);
		
		void		DrawImage(wchar_t *image_path, short  x, short y, short w, short h, int angle, bool flip_x, bool flip_y);
		void		DrawImage(Image *image, short  x, short y, short w, short h, int angle, bool flip_x, bool flip_y);

		void		GetPicture(void **ptr, int *size);
		void		GetPicture(void **ptr, int *size, int x, int y, int w, int h);
};


class Draw1C_List
{
	typedef struct Draw1C_ListStruct {
		Draw1C				*draw1c;
		Draw1C_ListStruct	*next;
	} Draw1C_ListStruct;

	private:

		Draw1C_ListStruct	*list;
		Draw1C_ListStruct	*list_enum_start;

	public:

		int					count;

		Draw1C_List()
		{
			list	= NULL;
			count	= 0;
		}

		~Draw1C_List()
		{
			RemoveAll();
			count = 0;
		}

		Draw1C *Add(unsigned short w, unsigned short h, char out)
		{

			Draw1C *drw1c;

			try { drw1c = new Draw1C(w, h, out); }
				catch (wchar_t *e) { throw e; }
				


			if (!list)
			{
				list			= new Draw1C_ListStruct;
				list->draw1c	= drw1c;
				list->next		= NULL;

				count++;

				return list->draw1c;
			}
			else
			{
				Draw1C_ListStruct *list_last = list;
				
				while (list_last->next)
					list_last = list_last->next;

				list_last->next			=	new Draw1C_ListStruct;
				list_last->next->draw1c	=	drw1c;
				list_last->next->next	=	NULL;

				count++;

				return list_last->next->draw1c;
			}
		}

		void Remove(Draw1C *draw1c)
		{

			Draw1C_ListStruct *list_cur		= list;
			Draw1C_ListStruct *list_prev	= NULL;

			if (list_cur)
			{
				while (list_cur)
				{
					if (list_cur->draw1c == draw1c)
					{
						if (list_cur == list && !list_cur->next)
						{
							delete list_cur->draw1c;
							delete list_cur;
							list = NULL;
							count--;
							break;
						}
						else
						{
							if (list_prev)
							{
								list_prev->next = list_cur->next;
								delete list_cur->draw1c;
								delete list_cur;
								list_cur = list_prev;
								count--;
							}
							else
							{
								list = list_cur->next;
								delete list_cur->draw1c;
								delete list_cur;

								count--;

								if (!(list && list->next))
									break;
									
								list_cur = list;
							}

						}
					}

					list_prev	= list_cur;
					list_cur	= list_cur->next;
				}
			}

		}

		void RemoveAll()
		{

			Draw1C_ListStruct *list_cur		= list;
			Draw1C_ListStruct *list_next	= NULL;

			if (list_cur)
			{
				while (list_cur)
				{
					list_next = list_cur->next;

					delete list_cur->draw1c;
					delete list_cur;

					list_cur = list_next;
				}

				list = NULL;
			}

		}

		void EnumStart()
		{
			list_enum_start = list;
		}

		Draw1C *EnumNext()
		{
			if (!list_enum_start)
				return NULL;

			Draw1C *ret		= list_enum_start->draw1c;
			list_enum_start = list_enum_start->next;

			return ret;
		}
};

#endif //__DRAW1C_H__
