#ifndef _TYPE_H_
#define _TYPE_H_

/* exact-width signed integer types */
typedef   signed          char int8_t;
typedef   signed short     int int16_t;
typedef   signed           int int32_t;
typedef   signed       __int64 int64_t;

typedef   char                 char8_t;

    /* exact-width unsigned integer types */
typedef unsigned          char uint8_t;
typedef unsigned          char bool_t;
typedef unsigned short     int uint16_t;
typedef unsigned           int uint32_t;
typedef unsigned       __int64 uint64_t;
typedef unsigned           int*	PVOID;

typedef void (*PTIMERFUNC)(PVOID pvParams);

typedef enum {OS_FALSE   = 0, OS_TRUE    = !OS_FALSE   } OS_BOOL;

#ifndef NULL
#define NULL    ((void *)0)
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#ifndef true
#define true    (1)
#endif

#ifndef false
#define false   (0)
#endif

#ifndef BOOLEAN
#define BOOLEAN  unsigned char
#endif

#ifndef tBoolean
#define tBoolean  BOOLEAN   
#endif

#define MSB(x)	((x >> 8) & 0xFF)
#define LSB(x)	((x) & 0xFF)

#define MAX(x,y) ((x) > (y) ? (x) : (y)) 
#define MIN(x,y) ((x) > (y) ? (y) : (x)) 

#define BIGEDIAN_U16(x)	(((x >> 8) & 0xFF) | ((x & 0xFF) << 8))

#endif

