/**
  ***************************************************************************************
  * @file    common.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.4
  * @date    15/07/2013
  * @brief   This source file provides support for common use operations
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
#include "common.h"
#include <Windows.h>
#include <sstream>

#define MOD_COMMON               "common"

using namespace std;


string decToString(const UINT dInput)
{
    ostringstream os;
    os<<dInput;

    return os.str();
}

string decToHexString(const UINT dInput)
{
    ostringstream os;
    os<<hex<<"0x"<<uppercase<<dInput<<nouppercase<<dec;

    return os.str();
}

INT stringToDec(const string &input)
{
    INT output = 0;
    istringstream is(input);
    is>>output;

    return output;
}

wstring stringToWc(const string &sInput)
{
    wstring output = L"";

    if(sInput != EMPTY_STRING) {
        UINT maxWcSize = sInput.size()+SPACE_FOR_NULL;
        WCHAR *wOutput = new WCHAR[maxWcSize];
        if( wOutput != NULL && mbstowcs_s(NULL, wOutput, maxWcSize, sInput.c_str(), maxWcSize) == 0 ) {
            if( *wOutput != NULL ) {
                output = wstring(wOutput);
            }

            delete[] wOutput;
        }
    }

    return output;
}

string wcToString(const WCHAR *const wInput)
{
    string output = EMPTY_STRING;

    if( wInput != NULL && *wInput != NULL ) {
        UINT maxCSize = wcslen(wInput)+SPACE_FOR_NULL;
        CHAR *cOutput = new CHAR[maxCSize];

        if( cOutput != NULL && wcstombs_s(NULL, cOutput, maxCSize, wInput, maxCSize) == 0 ) {
            if( *cOutput != NULL ) {
                output = string(cOutput);
            }

            delete[] cOutput;
        }
    }

    return output;
}

BOOL isWcSame(const WCHAR *const wcComp_1, const WCHAR *const wcComp_2)
{
    UINT wcCompSize_1 = wcslen(wcComp_1);
    UINT wcCompSize_2 = wcslen(wcComp_2);

    if( wcCompSize_1 == wcCompSize_2 && wcsncmp(wcComp_1, wcComp_2, wcCompSize_1) == 0 ) {
        return TRUE;
    }

    return FALSE;
}

FunctionSuccesFailure copyWCharString(WCHAR *wcDest, UINT const wcDestSize, const WCHAR *const wcSrc)
{
    static const WCHAR c_string_delimiter_wc = L'\0';
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    const UINT wcSrcSize = wcslen(wcSrc)+SPACE_FOR_NULL;

    if( wcDest != NULL && wcSrc != NULL && wcDestSize >= wcSrcSize ) {
        *wcDest = c_string_delimiter_wc;
        if(*wcSrc == c_string_delimiter_wc) {
            fresult = FUNCTION_SUCCESS;
        }
        else if(wcsncpy_s(wcDest, wcDestSize, wcSrc, wcSrcSize) == 0) {
            fresult = FUNCTION_SUCCESS;
        }
        else {
            /* unsupported error case */
        }
    }

    return fresult;
}

BOOL isKeyDown(UINT key)
{
    static const SHORT filter = (SHORT)(1<<(CHAR_BIT*sizeof(SHORT)-1));

    if(GetAsyncKeyState(key) & filter) {
        return TRUE;
    }

    return FALSE;
}

UINT getPressedKeys()
{
    static const UINT first_key    = 0x01;
    static const UINT last_key     = 0x9F;
    static const UINT keys_to_scan = 2;
    UINT pressed_key = 0;

    for(UINT nb_key_pressed=0, i=first_key, k=last_key; i<k; ++i) {
        if(isKeyDown(i)) {
            pressed_key |= i;
            if(++nb_key_pressed == keys_to_scan) {
                break;
            }
        }
    }

    return pressed_key;
}

