#include <rtx.h>
#include <rtx_inc.h>
#include <string.h>

volatile CHAR string_holder[20];
volatile CHAR string_holder_temp[20];

/******************************************************************************
* VOID StrCopy(CHAR* ,CHAR* ): Copies the contents of one string into another
******************************************************************************/
VOID
Strncpy (CHAR * newString, CHAR * oldString,int N)
{
  volatile CHAR *backupOld=NULL;
  volatile CHAR *backupNew;

  int count;
  count = 0;
  //backup the pointers to the strings
  backupOld = oldString;
  backupNew = newString;

  //Copy the values over to the new string location
  while (*oldString != '\0' && count < N)
    {
      count++;
      *newString++ = *oldString++;
    }

  //Put the null character at the end of the new string
  *newString = '\0';

  //restore the backed up pointers
  oldString = (CHAR*) backupOld;
  newString = (CHAR*) backupNew;
}

/******************************************************************************
* UINT32 StrLength(CHAR* ): Finds the length of a string
******************************************************************************/
UINT32
StrLength (CHAR * text)
{
  UINT32 length;		//length of the string
  CHAR *backupString;		//backup pointer to start of string

  backupString = text;		//backup the string
  length = 0;			//initialize length to 0

  //Loop through the string, calculating the length
  while (*text++ != '\0')
    {
      length++;
    }
  length++;			//take into account the null terminator

  text = backupString;		//restore the string pointer

  return length;		//return the length of the string
}

/******************************************************************************
* CHAR* StrConcat(CHAR* , CHAR* ): Concatenates two strings and returns pointer
* to new string location
******************************************************************************/
CHAR *
StrNConcat (CHAR * text1, CHAR * text2, int N)
{
  CHAR *backupText1 = text1;	//backup pointer to start of first string
  CHAR *backupText2 = text2;	//backup pointer to start of second string
  CHAR *newText = "\0";		//pointer to start of new string
  CHAR *backupNewText = newText;	//backup pointer to start of new string
  int count=0;
  //Copy the contents of the first string the new string
  while (*text1 != '\0' && count < N)
    {
      *newText++ = *text1++;
    }
  //Append the contents of the second string
  while (*text2 != '\0' && count < N)
    {
      *newText++ = *text2++;
    }
  //Add the null terminator to the new string
  *newText = '\0';

  //restore the backed up pointers to start of passed in strings
  text1 = backupText1;
  text2 = backupText2;

  //return pointer to start of new string
  return backupNewText;
}

/******************************************************************************
* BOOLEAN StrCompare(CHAR* , CHAR* ): Compares two strings for equality
******************************************************************************/
BOOLEAN
StrCompare (CHAR * text1, CHAR * text2)
{
  BOOLEAN equal = 0;

  //check to see if the two strings are the same length
  if (StrLength (text1) == StrLength (text2))
    {
      equal = 1;
    }

  //Compare the two strings
  while (equal)
    {
      if (*text1 == '\0' && *text2 == '\0')
	{
	  break;
	}
      if (*text1++ != *text2++)
	{
	  equal = 0;
	}
    }

  return equal;			//return result
}

/******************************************************************************
* CHAR* IntToHex( SINT32 )
* Convert integer to hex
******************************************************************************/
CHAR *
IntToHex (SINT32 number)
{
  CHAR *text2 = "\0";		//converted string
  CHAR *headPtr2 = text2;	//pointer to start of converted string
  SINT32 digit;			//individual digits of conversion
  BOOLEAN negative = 0;		//flag for negative number
  CHAR *tempPtr2 = "\0";


  //if the number is negative, flag negative and make number positive
  if (number < 0)
    {
      negative = 1;
      number = number * -1;
    }

  //if number is zero, simply copy it over
  if (number == 0)
    {
      *text2++ = 0x30;
    }
  else
    {
      //divide by 10 and store remainders in string
      //string is in reverse order of correct value
      while (number > 0)
	{
	  digit = number % 16;
	  if (digit < 10)
	    {
	      *text2++ = 0x30 + digit;
	    }
	  else
	    {
	      *text2++ = 0x37 + digit;
	    }
	  number = (SINT32) (number / 16);
	}
    }

  //if number is negative, add - at end of string
  if (negative == 1)
    {
      *text2++ = '-';
    }
  //null terminate string
  *text2 = '\0';

  //return the reverse of obtained string
  tempPtr2 = ReverseStr (headPtr2);
  return tempPtr2;
}

/******************************************************************************
* VOID ChopString(CHAR* data)
* Chop the line break and carriage returns
******************************************************************************/
VOID
ChopString (CHAR * data)
{
  volatile CHAR *bkpData = data;

  while (data[0] != '\0')
    {
      if (data[0] == LF || *data == CR)
	{
	  data[0] = '\0';
	}

      data[0] = *data;
      data++;
    }

  data = (CHAR*) bkpData;
}

CHAR *
IntToStr (UINT32 number)
{
  CHAR *temp = (CHAR*) string_holder_temp;
  CHAR *reversedString = (CHAR*) string_holder;
  CHAR digit;			//individual digits of conversion

  temp[0] = '\0';			//converted string

  //if number is zero, simply copy it over
  if (number == 0)
    {
      temp[0] = 0x30;
      temp++;
    }
  else
    {
      //divide by 10 and store remainders in string
      //string is in reverse order of correct value
      while (number > 0)
	{
	  digit = number % 10;
	  temp[0] = (CHAR) 0x30 + digit;
      temp++;
	  number = (SINT32) (number / 10);
	}
    }
  temp--;

  //reverse the string
  while (temp != string_holder_temp)
    {
      reversedString[0] = temp[0];
      reversedString++;
      temp--;
    }

  *reversedString++ = *temp;
  reversedString[0] = '\0';

  return (CHAR*) string_holder;
}


/******************************************************************************
* CHAR* ReverseStr( CHAR* )
* Reverses a string
******************************************************************************/
CHAR *
ReverseStr (CHAR * text)
{
  CHAR *newString = "\0";	//new string pointer
  UINT32 counter = 0;		//counter
  UINT32 size = StrLength (text);
  CHAR stringArray[StrLength (text)];
  CHAR *headPtr = newString;	//initialize head pointer to start of new string

  //go to end of input string
  while (*text != '\0')
    {
      text++;
      counter++;
    }

  //skip the null terminator
  text--;

  stringArray[counter] = '\0';

  //copy input string in reverse order
  while (counter > 0)
    {
      stringArray[size - counter - 1] = *text;
      text--;
      counter--;
    }

  counter = 0;

  //copy array into string
  while (stringArray[counter] != '\0')
    {
      *newString++ = stringArray[counter++];
    }

  //null terminate the new string
  *newString = '\0';

  //return pointer to start of new string
  return headPtr;
}


/******************************************************************************
* BOOLEAN IsInteger( CHAR* )
* 
* Checks if input is an integer
******************************************************************************/
BOOLEAN
IsInteger (CHAR * text)
{
//      CHAR* backup = text;
  SINT32 i = 0;

  while (text[i] != '\0')
    {
      if (text[i] < '0' || text[i] > '9')
	{
	  //rtxprint(text);
	  //rtxprint("\n\r");
	  //text = backup;
	  return RTX_ERROR;
	}
      i++;
    }

  //text = backup;

  return RTX_SUCCESS;
}
