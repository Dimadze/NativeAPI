

/* Opaque buffer element type.  This would be defined by the application. */
typedef struct
{
	bool			skip;
	wchar_t			event[512];
	wchar_t			data[512];
}ElemType;

/* Circular buffer object */
typedef struct
{
    int         size;   /* maximum number of elements           */
    int         start;  /* index of oldest element              */
    int         end;    /* index at which to write new element  */
    ElemType   *elems;  /* vector of elements                   */
} CircularBuffer;

int		cbInit(CircularBuffer *cb, int size);
void	cbFree(CircularBuffer *cb);
int		cbIsFull(CircularBuffer *cb);
int		cbIsEmpty(CircularBuffer *cb);
int		cbOccupancy(CircularBuffer *cb);
void	cbWrite(CircularBuffer *cb, ElemType *elem);
void	cbRead(CircularBuffer *cb, ElemType *elem);

