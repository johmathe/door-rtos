#ifndef __STRING_H__
#define __STRING_H__

#include <rtx_inc.h>

VOID StrCopy (CHAR * newString, CHAR * oldString);
UINT32 StrLength (CHAR * text);
CHAR *StrConcat (CHAR * text1, CHAR * text2);
BOOLEAN StrCompare (CHAR * text1, CHAR * text2);
CHAR *IntToHex (SINT32 number);
VOID ChopString (CHAR * data);
CHAR *IntToStr (UINT32 number);
SINT32 StrToInt (CHAR * text);
CHAR *ReverseStr (CHAR * text);
CHAR *SubString (CHAR * text, SINT32 start, SINT32 length);
BOOLEAN IsInteger (CHAR * text);
VOID Strncpy (CHAR * newString, CHAR * oldString,int N);

#endif
