#ifndef __IMAGE1C_H__

#include "windows.h"
#include <GdiPlus.h>
#include "draw_out.h"

#define _USE_MATH_DEFINES

#include "math.h"

using namespace Gdiplus;

class Image1C
{
	private:

		DrawOut1C	*v_out;
		double		DegreeToRadian(int angle);

	public:

		static const wchar_t		IMAGE[64];
		wchar_t						image_id[64];

		Image		*image;

		Image1C();
		Image1C(wchar_t *img_path);
		Image1C(Image *img);
		~Image1C();

		short GetHeight();
		short GetWidth();
		void Resize(short w, short h);
		void Rotate(int angle, bool changedimensions);
		void Flip(bool x, bool y);
		void Crop(int x, int y, int w, int h);

		bool ToClipBoard();

		void GetPicture(char out, void **ptr, int *size);
		void GetPicture(char out, void **ptr, int *size, int x, int y, int w, int h);
};


class Image1C_List
{
	typedef struct Image1C_ListStruct {
		Image1C				*image1c;
		Image1C_ListStruct	*next;
	} Image1C_ListStruct;

	private:

		Image1C_ListStruct	*list;
		Image1C_ListStruct	*list_enum_start;

	public:

		int					count;

		Image1C_List()
		{
			list	= NULL;
			count	= 0;
		}

		~Image1C_List()
		{
			RemoveAll();
			count = 0;
		}

		Image1C *Add()
		{

			Image1C *img1c;

			try { img1c = new Image1C(); }
				catch (wchar_t *e) { throw e; }

			if (!list)
			{
				list			= new Image1C_ListStruct;
				list->image1c	= img1c;
				list->next		= NULL;

				count++;

				return list->image1c;
			}
			else
			{
				Image1C_ListStruct *list_last = list;

				while (list_last->next)
					list_last = list_last->next;

				list_last->next				= new Image1C_ListStruct;
				list_last->next->image1c	= img1c;
				list_last->next->next		= NULL;

				count++;

				return list_last->next->image1c;
			}
		}

		Image1C *Add(Image *img)
		{

			Image1C *img1c;

			try { img1c = new Image1C(img); }
				catch (wchar_t *e) { throw e; }

			if (!list)
			{
				list			= new Image1C_ListStruct;
				list->image1c	= img1c;
				list->next		= NULL;

				count++;

				return list->image1c;
			}
			else
			{
				Image1C_ListStruct *list_last = list;
				
				while (list_last->next)
					list_last = list_last->next;

				list_last->next				=	new Image1C_ListStruct;
				list_last->next->image1c	=	img1c;
				list_last->next->next		=	NULL;

				count++;

				return list_last->next->image1c;
			}
		}

		Image1C *Add(wchar_t *img_path)
		{
			Image1C *img1c;

			try { img1c = new Image1C(img_path); }
				catch (wchar_t *e) { throw e; }

			if (!list)
			{
				list			= new Image1C_ListStruct;
				list->image1c	= img1c;
				list->next		= NULL;

				count++;

				return list->image1c;
			}
			else
			{
				Image1C_ListStruct *list_last = list;

				while (list_last->next)
					list_last = list_last->next;

				list_last->next				= new Image1C_ListStruct;
				list_last->next->image1c	= img1c;
				list_last->next->next		= NULL;

				count++;

				return list_last->next->image1c;
			}
		}

		void Remove(Image1C *image1c)
		{

			Image1C_ListStruct *list_cur	= list;
			Image1C_ListStruct *list_prev	= NULL;

			if (list_cur)
			{
				while (list_cur)
				{
					if (list_cur->image1c == image1c)
					{
						if (list_cur == list && !list_cur->next)
						{
							delete list_cur->image1c;
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
								delete list_cur->image1c;
								delete list_cur;
								list_cur = list_prev;
								count--;
							}
							else
							{
								list = list_cur->next;
								delete list_cur->image1c;
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

			Image1C_ListStruct *list_cur		= list;
			Image1C_ListStruct *list_next	= NULL;

			if (list_cur)
			{
				while (list_cur)
				{
					list_next = list_cur->next;

					delete list_cur->image1c;
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

		Image1C *EnumNext()
		{
			if (!list_enum_start)
				return NULL;

			Image1C *ret		= list_enum_start->image1c;
			list_enum_start		= list_enum_start->next;

			return ret;
		}
};

#endif //__IMAGE1C_H__
