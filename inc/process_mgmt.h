/**
  ***************************************************************************************
  * @file    process_mgmt.h
  * @author  £ukasz Kaümierczak
  * @version V2.0
  * @date    25/06/2013
  * @brief   This header file provides operations set for processes, threads and IPC
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
#ifndef PROCESS_MGMT_H
#define PROCESS_MGMT_H

#include "common.h"


class processes {
public:
    //static processes &instance() { static processes pinstance; return pinstance; }
    static const enum console_mode{ PROC_CONSOLE_UNDEF, PROC_CONSOLE_DISABLED, PROC_CONSOLE_NEW, PROC_CONSOLE_CURRENT, };
    static const enum priority_set{ PROC_PRIORITY_UNDEF, PROC_PRIORITY_LOW, PROC_PRIORITY_NORMAL, PROC_PRIORITY_HIGH, };

    static UINT_PTR process_create(std::string processNameRaw, std::string processPathRaw, std::string processParamsRaw, console_mode console,
                                   priority_set priority, UINT_PTR hStdInputRaw, UINT_PTR hStdOutputRaw, UINT_PTR hStdErrorRaw);
    static FunctionSuccesFailure process_wait(UINT_PTR hProcessRaw, UINT miliseconds);
    static FunctionSuccesFailure process_wait_infinite(UINT_PTR hProcessRaw);
    static FunctionSuccesFailure process_destroy(UINT_PTR hProcessRaw);

    static UINT_PTR thread_create(DWORD WINAPI proc(LPVOID), LPVOID param, SIZE_T stackSize);
    static UINT_PTR thread_get_current();
    static FunctionSuccesFailure thread_wait(UINT_PTR hThreadRaw, UINT miliseconds);
    static FunctionSuccesFailure thread_wait_infinite(UINT_PTR hThreadRaw);
    static FunctionSuccesFailure thread_ready();

    static UINT_PTR event_create(BOOL bManualReset, const std::string &eventNameRaw);
    static FunctionSuccesFailure event_signal(UINT_PTR hEventRaw);
    static FunctionSuccesFailure event_pulse(UINT_PTR hEventRaw);
    static FunctionSuccesFailure event_reset(UINT_PTR hEventRaw);
    static FunctionSuccesFailure event_wait(UINT_PTR hEventRaw, UINT miliseconds);
    static FunctionSuccesFailure event_wait_infinite(UINT_PTR hEventRaw);
    static FunctionSuccesFailure event_destroy(UINT_PTR hEventRaw);

    static UINT_PTR mutex_create(const std::string &mutexNameRaw);
    static FunctionSuccesFailure mutex_take(UINT_PTR hMutexRaw, UINT miliseconds);
    static FunctionSuccesFailure mutex_take_infinite(UINT_PTR hMutexRaw);
    static FunctionSuccesFailure mutex_release(UINT_PTR hMutexRaw);
    static FunctionSuccesFailure mutex_destroy(UINT_PTR hMutexRaw);

    static FunctionSuccesFailure pipe_unnamed_create(UINT_PTR &hReadPipeRaw, UINT_PTR &hWritePipeRaw);
    static FunctionSuccesFailure pipe_unnamed_read(UINT_PTR hReadPipeRaw, std::string &inBuffer);
    static FunctionSuccesFailure pipe_unnamed_write(UINT_PTR hWritePipeRaw, const std::string &outBuffer);
    static FunctionSuccesFailure pipe_unnamed_destroy(UINT_PTR hReadPipeRaw, UINT_PTR hWritePipeRaw);
    static FunctionSuccesFailure pipe_named_peek_data(UINT_PTR hReadPipeRaw, std::string &peekBuffer);

    static UINT_PTR pipe_named_create(BOOL bMessageType, BOOL bAsyncMode, UINT bufferSize, const std::string &namedPipeNameRaw);
    static UINT_PTR pipe_named_open(BOOL bAsyncMode, const std::string &namedPipeNameRaw);
    static FunctionSuccesFailure pipe_named_client_wait(UINT_PTR hNamedPipeRaw);
    static FunctionSuccesFailure pipe_named_wait(std::string &namedPipeNameRaw, UINT miliseconds);
    static FunctionSuccesFailure pipe_named_wait_infinite(std::string &namedPipeNameRaw);
    static BOOL pipe_named_operation_is_pending(UINT_PTR hNamedPipeRaw);
    static FunctionSuccesFailure pipe_named_read(UINT_PTR hNamedPipeRaw, std::string &inBuffer);
    static FunctionSuccesFailure pipe_named_write(UINT_PTR hNamedPipeRaw, const std::string &outBuffer);
    static FunctionSuccesFailure pipe_named_destroy(UINT_PTR hNamedPipeRaw);

private:
    processes() {}
    processes(const processes&);
    processes &operator=(const processes&);
};


#endif
