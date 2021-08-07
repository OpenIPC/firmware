/*******************************************************************************
* Copyright @ Huawei Technologies Co., Ltd. 1998-2014. All rights reserved.  
* File name: securec.h
* Decription: 
*             the user of this secure c library should include this header file
*             in you source code. This header file declare all supported API
*             prototype of the library, such as memcpy_s, strcpy_s, wcscpy_s,
*             strcat_s, strncat_s, sprintf_s, scanf_s, and so on.
* History:   
*     1. Date: 
*         Author:  
*         Modification: 
********************************************************************************
*/

#ifndef __HI_SECUREC_H__5D13A042_DC3F_4ED9_A8D1_882811274C27
#define __HI_SECUREC_H__5D13A042_DC3F_4ED9_A8D1_882811274C27

/* If you need high performance, enable the WITH_PERFORMANCE_ADDONS macro! */
//#define WITH_PERFORMANCE_ADDONS

#include "securectype.h"    /*lint !e537*/
#include "securec.h"

#include <stdarg.h>

/*if you need export the function of this library in Win32 dll, use __declspec(dllexport) */

#ifdef __cplusplus
extern "C"
{
#endif

    /**
    * @Description:The wmemcpy_s function copies n successive wide characters from the object pointed to by src into the object pointed to by dest.
    * @param dest - destination  address
    * @param destMax -The maximum length of destination buffer
    * @param src -source  address
    * @param count -copies count wide characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_wmemcpy(wchar_t* dest, size_t destMax, const wchar_t* src, size_t count)
    {
        return wmemcpy_s(dest, destMax, src, count);
    }
    
    /**
    * @Description:The memmove_s function copies n characters from the object pointed to by src into the object pointed to by dest.
    * @param dest - destination  address
    * @param destMax -The maximum length of destination buffer
    * @param src -source  address
    * @param count -copies count wide characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_memmove(void* dest, size_t destMax, const void* src, size_t count)
    {
        return memmove_s(dest, destMax, src, count);
    }
    
    /**
    * @Description:The wmemmove_s function copies n successive wide characters from the object pointed to by src into the object pointed to by dest.
    * @param dest - destination  address
    * @param destMax -The maximum length of destination buffer
    * @param src -source  address
    * @param count -copies count wide characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_wmemmove(wchar_t* dest, size_t destMax, const wchar_t* src, size_t count)
    {
        return wmemmove_s(dest, destMax, src, count);
    }

    /**
    * @Description:The wcscpy_s function copies the wide string pointed to by strSrc (including theterminating null wide character) into the array pointed to by strDest
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer
    * @param strSrc -source  address
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_wcscpy(wchar_t* strDest, size_t destMax, const wchar_t* strSrc)
    {
        return wcscpy_s(strDest, destMax, strSrc);
    }

    /**
    * @Description:The wcsncpy_s function copies not more than n successive wide characters (not including the terminating null wide character) 
    *                       from the array pointed to by strSrc to the array pointed to by strDest
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating wide character) 
    * @param strSrc -source  address
    * @param count -copies count wide characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_wcsncpy(wchar_t* strDest, size_t destMax, const wchar_t* strSrc, size_t count)
    {
        return wcsncpy_s(strDest, destMax, strSrc, count);
    }

    /**
    * @Description:The wcscat_s function appends a copy of the wide string pointed to by strSrc (including the terminating null wide character) 
    *                     to the end of the wide string pointed to by strDest
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating wide character) 
    * @param strSrc -source  address
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_wcscat(wchar_t* strDest, size_t destMax, const wchar_t* strSrc)
    {
        return wcscat_s(strDest, destMax, strSrc);
    }

    /**
    * @Description:The wcsncat_s function appends not more than n successive wide characters (not including the terminating null wide character) 
    *                       from the array pointed to by strSrc to the end of the wide string pointed to by strDest.
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating wide character) 
    * @param strSrc -source  address
    * @param count -copies count wide characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_wcsncat(wchar_t* strDest, size_t destMax, const wchar_t* strSrc, size_t count)
    {
        return wcsncat_s(strDest, destMax, strSrc, count);
    }

    /**
    * @Description: The  strtok_s  function parses a string into a sequence of tokens,On the first call to strtok_s the string to be parsed should be specified in strToken.  
    *                       In each subsequent call that should parse the same string, strToken should be NULL
    * @param strToken - the string to be delimited
    * @param strDelimit -specifies a set of characters that delimit the tokens in the parsed string
    * @param context -is a pointer to a char * variable that is used internally by strtok_s function
    * @return:returns a pointer to the first character of a token, or a null pointer if there is no token or there is a runtime-constraint violation.
    */
    static inline char* hi_strtok(char* strToken, const char* strDelimit, char** context)
    {
        return strtok_s(strToken, strDelimit, context);
    }

     /**
    * @Description: The  wcstok_s  function  is  the  wide-character  equivalent  of the strtok_s function
    * @param strToken - the string to be delimited
    * @param strDelimit -specifies a set of characters that delimit the tokens in the parsed string
    * @param context -is a pointer to a char * variable that is used internally by strtok_s function
    * @return:returns a pointer to the first character of a token, or a null pointer if there is no token or there is a runtime-constraint violation.
    */
    #define hi_wcstok(strToken, strDelimit, context) \
        wcstok_s(strToken, strDelimit, context)

    /**
    * @Description: The sprintf_s function is equivalent to the sprintf function except for the parameter destMax and the explicit runtime-constraints violation
    * @param strDest -  produce output according to a format ,write to the character string strDest
    * @param destMax - The maximum length of destination buffer(including the terminating null byte ('\0'))
    * @param format - fromat string
    * @return:success the number of characters printed(not including the terminating null byte ('\0')), If an error occurred return -1.
    */
    #define hi_sprintf(strDest, destMax, format, ...) \
        sprintf_s(strDest, destMax, format, ##__VA_ARGS__)

    /**
    * @Description: The  swprintf_s  function  is  the  wide-character  equivalent  of the sprintf_s function
    * @param strDest -  produce output according to a format ,write to the character string strDest
    * @param destMax - The maximum length of destination buffer(including the terminating null )
    * @param format - fromat string
    * @return:success the number of characters printed(not including the terminating null wide characte), If an error occurred return -1.
    */
    #define hi_swprintf(strDest, destMax, format, ...) \
        swprintf_s(strDest, destMax, format, ##__VA_ARGS__)


    /**
    * @Description: The snprintf_s function is equivalent to the snprintf function except for the parameter destMax/count and the explicit runtime-constraints violation
    * @param strDest -  produce output according to a format ,write to the character string strDest
    * @param destMax - The maximum length of destination buffer(including the terminating null  byte ('\0'))
    * @param count - do not write more than count bytes to strDest(not including the terminating null  byte ('\0'))
    * @param format - fromat string
    * @param argptr - instead of  a variable  number of arguments
    * @return:return the number of characters printed(not including the terminating null byte ('\0')), If an error occurred return -1.
    */
    #define hi_snprintf(strDest, destMax, count, format, ...) \
        snprintf_s(strDest, destMax, count, format, ##__VA_ARGS__)

    /**
    * @Description: The scanf_s function is equivalent to fscanf_s with the argument stdin interposed before the arguments to scanf_s
    * @param format - fromat string
    * @return:returns the number of input items assigned, If an error occurred return -1.
    */
    #define hi_scanf(format, ...) scanf_s(format, ##__VA_ARGS__)

    /**
    * @Description: The  wscanf_s  function  is  the  wide-character  equivalent  of the scanf_s function
    * @param format - fromat string
    * @return:returns the number of input items assigned, If an error occurred return -1.
    */
    #define hi_wscanf(format, ...) wscanf_s(format, ##__VA_ARGS__)


    /**
    * @Description: The fscanf_s function is equivalent to fscanf except that the c, s, and [ conversion specifiers apply to a pair of arguments (unless assignment suppression is indicated by a*)
    * @param stream - stdio file stream
    * @param format - fromat string
    * @return:returns the number of input items assigned, If an error occurred return -1.
    */
    #define hi_fscanf(stream, format, ...) fscanf_s(stream, format, ##__VA_ARGS__)

    /**
    * @Description: The  fwscanf_s  function  is  the  wide-character  equivalent  of the fscanf_s function
    * @param stream - stdio file stream
    * @param format - fromat string
    * @return:returns the number of input items assigned, If an error occurred return -1.
    */
    #define hi_fwscanf(stream, format, ...) fwscanf_s(stream, format, ##__VA_ARGS__)

    /**
    * @Description: The sscanf_s function is equivalent to fscanf_s, except that input is obtained from a string (specified by the argument buffer) rather than from a stream
    * @param buffer -  read character from  buffer
    * @param format - fromat string
    * @return:returns the number of input items assigned, If an error occurred return -1.
    */
    #define hi_sscanf(buffer, format, ...) sscanf_s(buffer, format, ##__VA_ARGS__)

    /**
    * @Description: The  swscanf_s  function  is  the  wide-character  equivalent  of the sscanf_s function
    * @param buffer -  read character from  buffer
    * @param format - fromat string
    * @return:returns the number of input items assigned, If an error occurred return -1.
    */
    #define hi_swscanf(buffer, format, ...) swscanf_s(buffer, format, ##__VA_ARGS__)

    /**
    * @Description:The gets_s function reads at most one less than the number of characters specified by destMax from the stream pointed to by stdin, into the array pointed to by buffer
    * @param buffer - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating null character) 
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline char* hi_gets(char* buffer, size_t destMax)
    {
        return gets_s(buffer, destMax);
    }
    
     /**
    * @Description:The memset_s function copies the value of c (converted to an unsigned char) into each of the first count characters of the object pointed to by dest.
    * @param dest - destination  address
    * @param destMax -The maximum length of destination buffer
    * @param c - the value to be copied
    * @param count -copies fisrt count characters of  dest
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_memset(void* dest, size_t destMax, int c, size_t count)
    {
        return memset_s(dest, destMax, c, count);
    }

    /**
    * @Description:The memcpy_s function copies n characters from the object pointed to by src into the object pointed to by dest.
    * @param dest - destination  address
    * @param destMax -The maximum length of destination buffer
    * @param src -source  address
    * @param count -copies count  characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_memcpy(void* dest, size_t destMax, const void* src, size_t count)
    {
        return memcpy_s(dest, destMax, src, count);
    }

    /**
    * @Description:The strcpy_s function copies the string pointed to by strSrc (including the terminating null character) into the array pointed to by strDest
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating null character) 
    * @param strSrc -source  address
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_strcpy(char* strDest, size_t destMax, const char* strSrc)
    {
        return strcpy_s(strDest, destMax, strSrc);
    }
    
    /**
    * @Description:The strncpy_s function copies not more than n successive characters (not including the terminating null character) 
    *                     from the array pointed to by strSrc to the array pointed to by strDest
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating null character) 
    * @param strSrc -source  address
    * @param count -copies count  characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_strncpy(char* strDest, size_t destMax, const char* strSrc, size_t count)
    {
        return strncpy_s(strDest, destMax, strSrc, count);
    }

    /**
    * @Description:The strcat_s function appends a copy of the  string pointed to by strSrc (including the terminating null  character) 
    *                     to the end of the  string pointed to by strDest
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating null wide character) 
    * @param strSrc -source  address
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_strcat(char* strDest, size_t destMax, const char* strSrc)
    {
        return strcat_s(strDest, destMax, strSrc);
    }

    
    /**
    * @Description:The strncat_s function appends not more than n successive  characters (not including the terminating null  character) 
    *                       from the array pointed to by strSrc to the end of the  string pointed to by strDest.
    * @param strDest - destination  address
    * @param destMax -The maximum length of destination buffer(including the terminating null character) 
    * @param strSrc -source  address
    * @param count -copies count  characters from the  src
    * @return  EOK if there was no runtime-constraint violation
    */
    static inline int hi_strncat(char* strDest, size_t destMax, const char* strSrc, size_t count)
    {
        return strncat_s(strDest, destMax, strSrc, count);
    }


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif/* __HI_SECUREC_H__5D13A042_DC3F_4ED9_A8D1_882811274C27 */


