/**
  ***************************************************************************************
  * @file    common.h
  * @author  £ukasz Kaümierczak
  * @version V1.4
  * @date    15/07/2013
  * @brief   This header file provides common use operations and consts set
  ***************************************************************************************
  * @copy
  *
  * Autor:  £ukasz Kaümierczak, 2013 POLSKA
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Prawa autorskie © 2013 £ukasz Kaümierczak. Wszystkie prawa zastrzeøone.
  * 
  * To Oprogramowanie zamieszczone w ürÛd≥ach internetowych lub innych zasobach podlega
  * ochronie praw autorskich. Wszelkie dzia≥ania zwiπzane z rozpowszechnianiem, modyfikowaniem,
  * publikowaniem rozwiπzaÒ zamieszczonych w Kodzie èrÛd≥owym wymaga uzgodnienia z autorem.
  *
  * AUTOR PREZENTOWANYCH ROZWI•ZA— NIE PONOSI ODPOWIEDZIALNOåCI ZA WSZELKIEGO RODZAJU
  * SZKODY(W TYM TAKØE OBOWI•ZKI ZWI•ZANE Z ICH LIKWIDACJ•) SPOWODOWANE UØYWANIEM LUB
  * MAJ•CE ZWI•ZEK Z UØYWANIEM OPROGRAMOWANIA. DOTYCZY TO R”WNIEØ PRZYPADKU GDY OSTRZEØENIE
  * LUB UWAGA O MOØLIWOåCI POJAWIENIA SI  SZKODY LUB PROBLEMU ZOSTA£O PRZEKAZANE.
  *
  * Author: £ukasz Kaümierczak, 2013 POLAND
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Copyright © 2013 £ukasz Kaümierczak. All rights reserved.
  * 
  * This Software available in Internet sources or other resources is protected by copyrights.
  * All actions related to redistribution, modifying or publishing solutions included in 
  * the Source Code require permission of the Author.
  *
  * AUTHOR OF PRESENTED SOLUTIONS SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR ANY
  * TYPE OF DAMAGES(INCLUDING SERVICES OF LIQUIDATION OF THESE DAMAGES) ARISING IN
  * ANY WAY OF USE OF THIS SOFTWARE. THIS SHALL ALSO BE EFFECTIVE IF WARNING OR NOTICE
  * ABOUT POSSIBLE DAMAGE OR PROBLEM WAS COMMUNICATED.
  *
  * <h2><center>&copy; COPYRIGHT 2013 £ukasz Kaümierczak </center></h2>
*/
#ifndef COMMON_H
#define COMMON_H

// !! definition below is to be included only when header <WinDef.h> is in use, otherwise it can be omitted
// define base platform for application: _68K_ || _MPPC_ || _X86_ || _IA64_ || _AMD64_ || _M_IX86
#define     _X86_

/******************************************************************************
*                       included common librarys
******************************************************************************/
#include <string>
#include <WinDef.h>

/******************************************************************************
*                       constant macros without parameters
******************************************************************************/
#define WIN_OS                    1
#define UNIX                      0

#define _ERROR_NOTIFICATION_      1
#define _SYSTEM_NOTIFICATION_     1

#define C_STRING_DELIMITER        '\0'
#define NEW_LINE                  '\n'
#define SPACE_FOR_NULL            1U
#define EMPTY_STRING              ""
#define UNNAMED                   EMPTY_STRING
#define UNAVAILABLE_TEXT          "<N/A>"
#define PROMPT_PHRASE             "/> "

/******************************************************************************
*                       macros with parameter(s)
******************************************************************************/
#define ARRAY_SIZE(x)       ( sizeof( (x) )/sizeof( *(x ) ) )

/******************************************************************************
*                       common typedefs
******************************************************************************/
//typedef char                 char_t;
//typedef char                 *pchar_t;
//typedef wchar_t              Wchar_t;
//typedef wchar_t              *pWchar_t;
//typedef signed   char        sint8_t;
//typedef unsigned char        uint8_t;
//                             
//typedef signed   short       sint16_t;
//typedef unsigned short       uint16_t;
//                             
//typedef signed   int         sint32_t;
//typedef unsigned int         uint32_t;
//
//typedef signed   long long   sint64_t;
//typedef unsigned long long   uint64_t;
//
//typedef float                real32_t;
//typedef double               real64_t;
//
//typedef sint32_t             bool_t;
//typedef uint64_t             addr_t;
//typedef void                 void_t;
typedef double DOUBLE;

/******************************************************************************
*                       common enums
******************************************************************************/
const enum FunctionSuccesFailure {
    FUNCTION_UNDEF    =  0,
    FUNCTION_FAILURE  = -1,
    FUNCTION_SUCCESS  =  1,
    FUNCTION_PENDING
};


/******************************************************************************
*                       common function's prototypes
******************************************************************************/
std::string  decToString(const UINT dInput);
std::string  decToHexString(const UINT dInput);
INT stringToDec(const std::string &input);
std::wstring stringToWc(const std::string &sInput);
std::string  wcToString(const WCHAR *const wInput);
BOOL isWcSame(const WCHAR *const wcComp_1, const WCHAR *const wcComp_2);
FunctionSuccesFailure copyWCharString(WCHAR *wcDest, UINT const wcDestSize, const WCHAR *const wcSrc);
UINT getPressedKeys();
BOOL isKeyDown(UINT key);


#endif
