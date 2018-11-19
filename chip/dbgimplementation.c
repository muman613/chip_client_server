/**
        @file dbgimplementation.c
        @brief User mode test application, debugging print implementation.
*/

#define ALLOW_OS_CODE

#include <stdio.h>
#include <stdarg.h>

int verbose_stdout = 1;
int verbose_stderr = 1;

int console_output(const char *format, ...);
int file_output(FILE *fptr, const char *format, ...);
int DebugLevel = 1;

int console_output(const char *format, ...)
{
        va_list ap;
        int res;
                                                                                
        if (verbose_stdout != 0) {
                va_start(ap, format);
                res = vprintf(format, ap);
                va_end(ap);
                return(res);
        }
        return(0);
}
                                                                                
int file_output(FILE *fptr, const char *format, ...)
{
        va_list ap;
        int res;
                                                                                
	if (((fptr != stderr) && (verbose_stdout != 0)) || 
 		((fptr == stderr) && (verbose_stderr != 0))) {
                va_start(ap, format);
                res = vfprintf(fptr, format, ap);
                va_end(ap);
                return(res);
        }
        return(0);
}

#include "rmdef/rmdef.h"

#define RM_MAX_STRING 1024

static char str[RM_MAX_STRING];

#if ((RMCOMPILERID==RMCOMPILERID_GCC) || (RMCOMPILERID==RMCOMPILERID_ARMELF_GCC))

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifndef RMDBGLOG_implementation
void RMDBGLOG_implementation(RMbool active,const RMascii *filename,RMint32 line,const RMascii *text,...)
{  
        if (active && (verbose_stderr != 0)) {
		va_list ap;
		
                snprintf((char *)str,RM_MAX_STRING,"[%s:%ld] ",(char *)filename,line);
		
                va_start(ap, text);
                vsnprintf((char *)(str+strlen(str)), RM_MAX_STRING-strlen(str), text, ap); 
                va_end(ap);
                
		fprintf(stderr,str);
		
        }
}
#endif // RMDBGLOG_implementation

#ifndef RMDBGPRINT_implementation
void RMDBGPRINT_implementation(RMbool active,const RMascii *filename,RMint32 line,const RMascii *text,...)
{
        if (active && (verbose_stderr != 0)) {
		va_list ap;
		
                va_start(ap, text);
                vsnprintf((char *)str,RM_MAX_STRING,text,ap); 
                va_end(ap);
                
               	fprintf(stderr,str);
        }
}
#endif // RMDBGPRINT_implementation

#elif (RMCOMPILERID==RMCOMPILERID_VISUALC)

#include <windows.h>
#include <stdio.h>

#ifndef RMDBGLOG_implementation
void RMDBGLOG_implementation(RMbool active,const RMascii *filename,RMint32 line,const RMascii *text,...)
{  
        if (active && (verbose_stdout != 0)) {
		va_list ap;
		
#ifdef UNICODE
		sprintf((char *)str, (char *)"[%ls:%ld] ", filename, line);
#else
		sprintf((char *)str, (char *)"[%s:%ld] ", filename, line);
#endif
		
		va_start(ap, text);
		vsprintf((char *)(str+strlen(str)), (const char *)text, ap); 
		va_end(ap);
		
		OutputDebugString(str);
	}
}
#endif // RMDBGLOG_implementation

#ifndef RMDBGPRINT_implementation
void RMDBGPRINT_implementation(RMbool active,const RMascii *filename,RMint32 line,const RMascii *text,...)
{
        if (active && (verbose_stdout != 0)) {
		va_list ap;
		
		va_start(ap, text);
		vsprintf((char *)str, (const char *)text, ap); 
		va_end(ap);
		
		OutputDebugString(str);
	}
}
#endif // RMDBGPRINT_implementation

#else

NOTCOMPILABLE

#endif 
