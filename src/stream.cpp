/**
  ***************************************************************************************
  * @file    stream.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.1
  * @date    21/06/2013
  * @brief   This source file provides support for multithreaded std stream operations
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
#include "stream.h"
#include <iostream>

#include "process_mgmt.h"

#define MOD_STREAM               "stream"

using namespace std;

#define EMPTY_SPACE             ' '
#define DIGIT_SEPARATOR_1       '-'
#define DIGIT_SEPARATOR_2       '/'
#define DIGIT_SEPARATOR_3       '\\'
#define IS_DIGITS_SEPARATOR(x)  ( (x)==DIGIT_SEPARATOR_1 || (x)==DIGIT_SEPARATOR_2 || (x)==DIGIT_SEPARATOR_3 )

istream &stream::is   = cin;
ostream &stream::os   = cout;
ostream &stream::oerr = cerr;
UINT_PTR const stream::istreamMutex          = processes::mutex_create(UNNAMED);
UINT_PTR const stream::ostreamMutex          = processes::mutex_create(UNNAMED);
UINT_PTR const stream::ostreamSystemMutex    = processes::mutex_create(UNNAMED);
BOOL stream::desynchroPending  = FALSE;

static string create_output_string(const string &module, const string &output);


FunctionSuccesFailure stream::error_message(const string &module, const string &output)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

#if _ERROR_NOTIFICATION_
    static const string error_signature = "[(!!) error] ";
    fresult = error_output_message(error_signature + module, output);
#endif

    return fresult;
}

FunctionSuccesFailure stream::system_message(const string &module, const string &output)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

#if _SYSTEM_NOTIFICATION_
    static const string system_signature = "[system] ";
    fresult = output_message(system_signature + module, output);
#endif

    return fresult;
}

FunctionSuccesFailure stream::output_message(const string &module, const string &output)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if( desynchroPending == FALSE && processes::mutex_take(ostreamMutex, ostreamWaitMsTimeout) == FUNCTION_SUCCESS ) {
        os<<create_output_string(module, output);
        processes::mutex_release(ostreamMutex);
        fresult = FUNCTION_SUCCESS;
    }
    else if(processes::mutex_take(ostreamSystemMutex, ostreamWaitMsTimeout) == FUNCTION_SUCCESS) {
        os<<create_output_string(module, output);

        processes::mutex_release(ostreamSystemMutex);
        fresult = FUNCTION_SUCCESS;
    }
    else {
        //no action defined
    }

    return fresult;
}

FunctionSuccesFailure stream::error_output_message(const string &module, const string &err_output)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if( desynchroPending == FALSE && processes::mutex_take(ostreamMutex, ostreamWaitMsTimeout) == FUNCTION_SUCCESS ) {
        oerr<<create_output_string(module, err_output);

        processes::mutex_release(ostreamMutex);
        fresult = FUNCTION_SUCCESS;
    }
    else if(processes::mutex_take(ostreamSystemMutex, ostreamWaitMsTimeout) == FUNCTION_SUCCESS) {
        oerr<<create_output_string(module, err_output);

        processes::mutex_release(ostreamSystemMutex);
        fresult = FUNCTION_SUCCESS;
    }
    else {
        //no action defined
    }

    return fresult;
}

ostream &stream::get_os()
{
    static UINT_PTR mutex_current_owner = NULL;

    if(processes::mutex_take(ostreamMutex, ostreamWaitMsTimeout) == FUNCTION_FAILURE) {
        desynchronized_threads_handler(ostreamMutex, mutex_current_owner);
    }
    else if(desynchroPending == TRUE) {
        desynchronization_source_handler(mutex_current_owner);
    }

    mutex_current_owner = processes::thread_get_current();
    os.flush();

    return os;
}

VOID stream::print()
{
    processes::mutex_release(ostreamMutex);
}

VOID stream::desynchronized_threads_handler(UINT_PTR mutex_blocked, UINT_PTR mutex_current_owner)
{
    static const UINT_PTR ostreamDesynchroMutex = processes::mutex_create(UNNAMED);

    if(processes::mutex_take(ostreamDesynchroMutex, 0) == FUNCTION_FAILURE) {
        processes::mutex_take_infinite(mutex_blocked);
    }
    else {
        if(processes::mutex_take(mutex_blocked, ostreamWaitMsTimeout) == FUNCTION_FAILURE) {
            desynchroPending = TRUE;
            error_message(MOD_STREAM, "ostream desynchronization occured! Possibility of PRINTOUT() being neglected in thread: "
                                    +decToHexString(mutex_current_owner)+"\nall threads using ostream are scheduled to be SUSPENDED!");

            processes::mutex_take_infinite(mutex_blocked);
            error_message(MOD_STREAM, "ostream synchronization RESTORED!");
            desynchroPending = FALSE;
        }

        processes::mutex_release(ostreamDesynchroMutex);
    }
}

VOID stream::desynchronization_source_handler(UINT_PTR mutex_current_owner)
{
    error_message(MOD_STREAM, "ostream invoked by corrupted thread: ["+decToHexString(mutex_current_owner)+"] again!\n"+
                            "functionality in deadlock state - emergency program shutdown!");
    exit(EXIT_FAILURE);
}

FunctionSuccesFailure stream::enter_input_section()
{
    static const UINT lockInputImeout = 100;
    return processes::mutex_take(istreamMutex, lockInputImeout);
}

FunctionSuccesFailure stream::leave_input_section()
{
    return processes::mutex_release(istreamMutex);
}

string stream::get_input()
{
    processes::mutex_take_infinite(istreamMutex);

    string data;
    CHAR input[255+SPACE_FOR_NULL] = {0};
    is.getline(input, 255);
    data = string(input);

    processes::mutex_release(istreamMutex);

    if(data.size() == 1 && data[0] == QUIT_PROCCESS) {
        throw QUIT_PROCCESS;
    }

    return string(input);
}

FunctionSuccesFailure stream::get_int(INT &data)
{
    string input = get_input();

    data = 0;
    for(INT i=0, k=input.size(); i<k; ++i)
    {
        if(isdigit(input[i]) == 0)
        {
            error_message(MOD_STREAM, "Not a number!");
            return FUNCTION_FAILURE;
        }
    }
    data = stringToDec(input);

    return FUNCTION_SUCCESS;
}

FunctionSuccesFailure stream::get_char(CHAR &data)
{
    string input = get_input();

    data = 0;
    if(input.size() != 1 || isalnum(input[0]) == 0)
    {
        error_message(MOD_STREAM, "Not a single character!");
        return FUNCTION_FAILURE;
    }
    data = input[0];

    return FUNCTION_SUCCESS;
}

FunctionSuccesFailure stream::get_text(string &data)
{
    BOOL isNumText = FALSE;
    string input = get_input();

    data = EMPTY_STRING;
    for(INT i=0, k=input.size(); i<k; ++i)
    {
        if(isalnum(input[i]) == 0 && input[i] != EMPTY_SPACE && IS_DIGITS_SEPARATOR(input[i]) != 0)
        {
            error_message(MOD_STREAM, "Not a text or numerical text format!");
            return FUNCTION_FAILURE;
        }
    }
    data = input;

    return FUNCTION_SUCCESS;
}

string create_output_string(const string &module, const string &output)
{
    static const string separator = ": ";
    string ostring = NEW_LINE+module+separator;

    for(UINT i=0, k=output.size(); i<k; ++i) {
        if(output[i] == NEW_LINE) {
            ostring += NEW_LINE+module+separator;
        }
        else {
            ostring += output[i];
        }
    }
    ostring += NEW_LINE;

    return ostring;
}

