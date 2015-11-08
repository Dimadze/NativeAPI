#ifndef __TIMER1C_H__

#include "windows.h"

;

class Timer1C
{
	public:

		static const wchar_t		TIMER[64];
		wchar_t						timer_id[64];

		UINT_PTR					timer;

		Timer1C(int elapse, TIMERPROC timer_proc);
		~Timer1C();
};

class Timer1C_List
{
	typedef struct Timer1C_ListStruct {
		Timer1C				*timer1c;
		Timer1C_ListStruct	*next;
	} Timer1C_ListStruct;

	private:

		Timer1C_ListStruct	*list;
		Timer1C_ListStruct	*list_enum_start;

	public:

		int					count;

		Timer1C_List()
		{
			list	= NULL;
			count	= 0;
		}

		~Timer1C_List()
		{
			RemoveAll();
			count = 0;
		}

		Timer1C *Add(int elapse, TIMERPROC timer_proc)
		{

			Timer1C *tmr1c;

			try { tmr1c = new Timer1C(elapse, timer_proc); }
				catch (wchar_t *e) { throw e; }

			if (!list)
			{
				list			= new Timer1C_ListStruct;
				list->timer1c	= tmr1c;
				list->next		= NULL;

				count++;

				return list->timer1c;
			}
			else
			{
				Timer1C_ListStruct *list_last = list;

				while (list_last->next)
					list_last = list_last->next;

				list_last->next				= new Timer1C_ListStruct;
				list_last->next->timer1c	= tmr1c;
				list_last->next->next		= NULL;

				count++;

				return list_last->next->timer1c;
			}
		}

		void Remove(Timer1C *timer1c)
		{

			Timer1C_ListStruct *list_cur	= list;
			Timer1C_ListStruct *list_prev	= NULL;

			if (list_cur)
			{
				while (list_cur)
				{
					if (list_cur->timer1c == timer1c)
					{
						if (list_cur == list && !list_cur->next)
						{
							delete list_cur->timer1c;
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
								delete list_cur->timer1c;
								delete list_cur;
								list_cur = list_prev;
								count--;
							}
							else
							{
								list = list_cur->next;
								delete list_cur->timer1c;
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

			Timer1C_ListStruct *list_cur		= list;
			Timer1C_ListStruct *list_next	= NULL;

			if (list_cur)
			{
				while (list_cur)
				{
					list_next = list_cur->next;

					delete list_cur->timer1c;
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

		Timer1C *EnumNext()
		{
			if (!list_enum_start)
				return NULL;

			Timer1C *ret		= list_enum_start->timer1c;
			list_enum_start		= list_enum_start->next;

			return ret;
		}
};

#endif //__TIMER1C_H__
