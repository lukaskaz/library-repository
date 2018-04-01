/**
  ***************************************************************************************
  * @file    process_mgmt.cpp
  * @author  £ukasz Kaümierczak
  * @version V2.0
  * @date    25/06/2013
  * @brief   This source file provides support for processes, threads and IPC operations
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
#include "process_mgmt.h"
#include <Windows.h>
#include <vector>

#include "stream.h"

#define MOD_PROCESS_MGMT               "process_mgmt"

using namespace std;

static FunctionSuccesFailure single_object_wait(HANDLE hHandle, DWORD dwMiliseconds);
static FunctionSuccesFailure single_object_wait_abandoned(HANDLE hHandle, DWORD dwMiliseconds);
static FunctionSuccesFailure single_object_handle_close(HANDLE hHandle);
static FunctionSuccesFailure single_object_read_data(UINT_PTR hHandleRaw, string &inBuffer, LPOVERLAPPED pipeAsyncStruct);
static FunctionSuccesFailure single_object_write_data(UINT_PTR hHandleRaw, const string &outBuffer, LPOVERLAPPED pipeAsyncStruct);


/******************************************************************************
*
*               threads singleton class
*
******************************************************************************/
class threads {
public:
    static threads &instance() { static threads tinstance; return tinstance; }

    static UINT_PTR thread_create_ext(DWORD WINAPI proc(LPVOID), LPVOID param, SIZE_T stackSize);
    static UINT_PTR thread_get_current_ext();
    static FunctionSuccesFailure thread_ready_ext();
    static FunctionSuccesFailure thread_wait_ext(UINT_PTR hThreadRaw, UINT miliseconds);
    static FunctionSuccesFailure thread_wait_infinite_ext(UINT_PTR hThreadRaw);
    static FunctionSuccesFailure thread_destroy_ext(UINT_PTR hThreadRaw);  /* READ PRECAUTION'S ANNOTATION WITHIN THE FUNCTION BODY */

private:
    threads() {}
    threads(const threads&);
    threads &operator=(const threads&);

    static const UINT_PTR threadReadyEvent;
};

UINT_PTR const threads::threadReadyEvent = processes::event_create(FALSE, UNNAMED);


/******************************************************************************
*
*               processes function prototypes
*
******************************************************************************/
static UINT_PTR process_create_ext(LPWSTR commandPath, DWORD initialCreationStataus, UINT_PTR hStdInRaw, UINT_PTR hStdOutRaw, UINT_PTR hStdErrRaw);
static FunctionSuccesFailure process_wait_ext(UINT_PTR hProcessRaw, UINT miliseconds);
static FunctionSuccesFailure process_wait_infinite_ext(UINT_PTR hProcessRaw);
static FunctionSuccesFailure process_destroy_ext(UINT_PTR hProcessRaw);


/******************************************************************************
*
*               events function prototypes
*
******************************************************************************/
static UINT_PTR event_create_ext(BOOL bManualReset, LPCTSTR eventName);
static FunctionSuccesFailure event_signal_ext(UINT_PTR hEventRaw);
static FunctionSuccesFailure event_pulse_ext(UINT_PTR hEventRaw);
static FunctionSuccesFailure event_reset_ext(UINT_PTR hEventRaw);
static FunctionSuccesFailure event_wait_ext(UINT_PTR hEventRaw, UINT miliseconds);
static FunctionSuccesFailure event_wait_infinite_ext(UINT_PTR hEventRaw);
static FunctionSuccesFailure event_destroy_ext(UINT_PTR hEventRaw);


/******************************************************************************
*
*               mutexes function prototypes
*
******************************************************************************/
static UINT_PTR mutex_create_ext(LPCTSTR mutexName);
static FunctionSuccesFailure mutex_take_ext(UINT_PTR hMutexRaw, UINT miliseconds);
static FunctionSuccesFailure mutex_take_infinite_ext(UINT_PTR hMutexRaw);
static FunctionSuccesFailure mutex_release_ext(UINT_PTR hMutexRaw);
static FunctionSuccesFailure mutex_destroy_ext(UINT_PTR hMutexRaw);


/******************************************************************************
*
*               pipes function prototypes
*
******************************************************************************/
class pipesNamedAsyncData{
public:
    struct pipeNamedInfo{
        UINT_PTR   hPipeNamedRaw;
        OVERLAPPED pipeNamedAsyncStruct;
    };

    static pipesNamedAsyncData &instance() { static pipesNamedAsyncData pinstance; return pinstance; }
    static FunctionSuccesFailure pipeNamedAsyncStructCreate(UINT_PTR hPipeNamedRaw);
    static pipeNamedInfo &pipeNamedAsyncStructGet(UINT_PTR hPipeNamedRaw);
    static FunctionSuccesFailure pipeNamedAsyncStructDelete(UINT_PTR hPipeNamedRaw);
    static BOOL pipeNamedAsyncStructIsPending(UINT_PTR hPipeNamedRaw);

private:
    pipesNamedAsyncData() {}
    pipesNamedAsyncData(const pipesNamedAsyncData&);
    pipesNamedAsyncData &operator=(const pipesNamedAsyncData&);
    static pipeNamedInfo &pipeNamedGetEmptyStruct() { static pipeNamedInfo pipeNamedInfoEmpty = {0}; return pipeNamedInfoEmpty; }

    static vector<pipeNamedInfo> pipeNamedAllInfo;
};

static FunctionSuccesFailure pipe_unnamed_create_ext(UINT_PTR &hReadPipeRaw, UINT_PTR &hWritePipe);
static FunctionSuccesFailure pipe_unnamed_read_ext(UINT_PTR hReadPipeRaw, string &inBuffer);
static FunctionSuccesFailure pipe_unnamed_write_ext(UINT_PTR hWritePipeRaw, const string &outBuffer);
static FunctionSuccesFailure pipe_unnamed_destroy_ext(UINT_PTR hReadPipeRaw, UINT_PTR hWritePipeRaw);
static UINT pipe_named_peek_data_size_ext(UINT_PTR hReadPipeRaw);
static FunctionSuccesFailure pipe_named_peek_data_ext(UINT_PTR hReadPipeRaw, string &outBuffer);

static UINT_PTR pipe_named_create_ext(BOOL bMessageType, BOOL bAsyncMode, UINT bufferSize, LPCTSTR pipeName);
static UINT_PTR pipe_named_open_ext(BOOL bAsyncMode, LPCTSTR pipeName);
static FunctionSuccesFailure pipe_named_client_wait_ext(UINT_PTR hNamedPipeRaw);
static FunctionSuccesFailure pipe_named_wait_ext(LPCTSTR namedPipeName, UINT miliseconds);
static FunctionSuccesFailure pipe_named_wait_infinite_ext(LPCTSTR namedPipeName);
static BOOL pipe_named_operation_is_pending_ext(UINT_PTR hNamedPipeRaw);
static FunctionSuccesFailure pipe_named_read_ext(UINT_PTR hNamedPipeRaw, string &inBuffer);
static FunctionSuccesFailure pipe_named_write_ext(UINT_PTR hNamedPipeRaw, const string &outBuffer);
static FunctionSuccesFailure pipe_named_destroy_ext(UINT_PTR hNamedPipeRaw);


/******************************************************************************
*
*               proccesses singleton class members
*
******************************************************************************/
UINT_PTR processes::process_create(string processNameRaw, string processPathRaw, string processParamsRaw, console_mode console,
                                    priority_set priority, UINT_PTR hStdInputRaw, UINT_PTR hStdOutputRaw, UINT_PTR hStdErrorRaw)
{
    static const string parameters_separator = " ";
    static const string name_prefix = "\\";
    UINT_PTR fresult = 0;

    DWORD initialCreationStataus = 0;
    wstring commandPathString    = stringToWc(processPathRaw + name_prefix + processNameRaw + parameters_separator + processParamsRaw);
    UINT    commandPathSize      = commandPathString.size() + SPACE_FOR_NULL;
    WCHAR   *pCommandPath        = new WCHAR[commandPathSize];
    copyWCharString(pCommandPath, commandPathSize, commandPathString.c_str());

    switch(priority) {
        case processes::PROC_PRIORITY_LOW:
            initialCreationStataus |= BELOW_NORMAL_PRIORITY_CLASS;
            break;
        case processes::PROC_PRIORITY_NORMAL:
            initialCreationStataus |= NORMAL_PRIORITY_CLASS;
            break;
        case processes::PROC_PRIORITY_HIGH:
            initialCreationStataus |= ABOVE_NORMAL_PRIORITY_CLASS;
            break;
        default: { /* no action determined */}
    }

    switch(console) {
        case processes::PROC_CONSOLE_DISABLED:
            initialCreationStataus |= CREATE_NO_WINDOW;
            break;
        case processes::PROC_CONSOLE_CURRENT:
            /* no action needed*/
            break;
        case processes::PROC_CONSOLE_NEW:
            initialCreationStataus |= CREATE_NEW_CONSOLE;
            break;
        default: { /* no action determined */}
    }

    fresult = process_create_ext(pCommandPath, initialCreationStataus, hStdInputRaw, hStdOutputRaw, hStdErrorRaw);
    delete[] pCommandPath;

    return fresult;
}

FunctionSuccesFailure processes::process_wait(UINT_PTR hProcessRaw, UINT miliseconds)
{
    return process_wait_ext(hProcessRaw, miliseconds);
}

FunctionSuccesFailure processes::process_wait_infinite(UINT_PTR hProcessRaw)
{
    return process_wait_infinite_ext(hProcessRaw);
}

FunctionSuccesFailure processes::process_destroy(UINT_PTR hProcessRaw)
{
    return process_destroy_ext(hProcessRaw);
}

UINT_PTR processes::thread_create(DWORD WINAPI proc(LPVOID), LPVOID param, SIZE_T stackSize)
{
    return threads::instance().thread_create_ext(proc, param, stackSize);
}

UINT_PTR processes::thread_get_current()
{
    return threads::instance().thread_get_current_ext();
}

FunctionSuccesFailure processes::thread_wait(UINT_PTR hThreadRaw, UINT miliseconds)
{
    return threads::instance().thread_wait_ext(hThreadRaw, miliseconds);
}

FunctionSuccesFailure processes::thread_wait_infinite(UINT_PTR hThreadRaw)
{
    return threads::instance().thread_wait_infinite_ext(hThreadRaw);
}

FunctionSuccesFailure processes::thread_ready()
{
    return threads::instance().thread_ready_ext();
}

UINT_PTR processes::event_create(BOOL bManualReset, const string &eventNameRaw)
{
    LPCTSTR eventName = NULL;
    if(eventNameRaw != UNNAMED) {
        wstring eventNameWstr = stringToWc(eventNameRaw);
        eventName = eventNameWstr.c_str();
    }

    return event_create_ext(bManualReset, eventName);
}

FunctionSuccesFailure processes::event_signal(UINT_PTR hEventRaw)
{
    return event_signal_ext(hEventRaw);
}

FunctionSuccesFailure processes::event_pulse(UINT_PTR hEventRaw)
{
    return event_pulse_ext(hEventRaw);
}

FunctionSuccesFailure processes::event_reset(UINT_PTR hEventRaw)
{
    return event_reset_ext(hEventRaw);
}

FunctionSuccesFailure processes::event_wait(UINT_PTR hEventRaw, UINT miliseconds)
{
    return event_wait_ext(hEventRaw, miliseconds);
}

FunctionSuccesFailure processes::event_wait_infinite(UINT_PTR hEventRaw)
{
    return event_wait_infinite_ext(hEventRaw);
}

FunctionSuccesFailure processes::event_destroy(UINT_PTR hEventRaw)
{
    return event_destroy_ext(hEventRaw);
}

UINT_PTR processes::mutex_create(const string &mutexNameRaw)
{
    LPCTSTR mutexName = NULL;
    if(mutexNameRaw != UNNAMED) {
        wstring mutexNameWstr = stringToWc(mutexNameRaw);
        mutexName = mutexNameWstr.c_str();
    }

    return mutex_create_ext(mutexName);
}

FunctionSuccesFailure processes::mutex_take(UINT_PTR hMutexRaw, UINT miliseconds)
{
    return mutex_take_ext(hMutexRaw, miliseconds);
}

FunctionSuccesFailure processes::mutex_take_infinite(UINT_PTR hMutexRaw)
{
    return mutex_take_infinite_ext(hMutexRaw);
}

FunctionSuccesFailure processes::mutex_release(UINT_PTR hMutexRaw)
{
    return mutex_release_ext(hMutexRaw);
}

FunctionSuccesFailure processes::mutex_destroy(UINT_PTR hMutexRaw)
{
    return mutex_destroy_ext(hMutexRaw);
}

FunctionSuccesFailure processes::pipe_unnamed_create(UINT_PTR &hReadPipeRaw, UINT_PTR &hWritePipeRaw)
{
    return pipe_unnamed_create_ext(hReadPipeRaw, hWritePipeRaw);
}

FunctionSuccesFailure processes::pipe_unnamed_read(UINT_PTR hReadPipeRaw, string &inBuffer)
{
    return pipe_unnamed_read_ext(hReadPipeRaw, inBuffer);
}

FunctionSuccesFailure processes::pipe_unnamed_write(UINT_PTR hReadPipeRaw, const string &outBuffer)
{
    return pipe_unnamed_write_ext(hReadPipeRaw, outBuffer);
}

FunctionSuccesFailure processes::pipe_unnamed_destroy(UINT_PTR hReadPipeRaw, UINT_PTR hWritePipeRaw)
{
    return pipe_unnamed_destroy_ext(hReadPipeRaw, hWritePipeRaw);
}

FunctionSuccesFailure processes::pipe_named_peek_data(UINT_PTR hReadPipeRaw, string &peekBuffer)
{
    return pipe_named_peek_data_ext(hReadPipeRaw, peekBuffer);
}

UINT_PTR processes::pipe_named_create(BOOL bMessageType, BOOL bAsyncMode, UINT bufferSize, const std::string &namedPipeNameRaw)
{
    UINT_PTR fresult = 0;

    if(namedPipeNameRaw == UNNAMED) {
        stream::instance().error_message(MOD_PROCESS_MGMT, "create named pipe without name parameter invoked");
    }
    else {
        wstring namedPipeNameWstr = stringToWc(namedPipeNameRaw);
        LPCTSTR namedPipeName = namedPipeNameWstr.c_str();
        fresult = pipe_named_create_ext(bMessageType, bAsyncMode, bufferSize, namedPipeName);
    }

    return fresult;
}

UINT_PTR processes::pipe_named_open(BOOL bAsyncMode, const string &namedPipeNameRaw)
{
    UINT_PTR fresult = 0;

    if(namedPipeNameRaw == UNNAMED) {
        stream::instance().error_message(MOD_PROCESS_MGMT, "open named pipe without name parameter invoked");
    }
    else {
        wstring namedPipeNameWstr = stringToWc(namedPipeNameRaw);
        LPCTSTR namedPipeName = namedPipeNameWstr.c_str();
        fresult = pipe_named_open_ext(bAsyncMode, namedPipeName);
    }

    return fresult;
}

FunctionSuccesFailure processes::pipe_named_client_wait(UINT_PTR hNamedPipeRaw)
{
    return pipe_named_client_wait_ext(hNamedPipeRaw);
}

FunctionSuccesFailure processes::pipe_named_wait(string &namedPipeNameRaw, UINT miliseconds)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(namedPipeNameRaw == UNNAMED) {
        stream::instance().error_message(MOD_PROCESS_MGMT, "wait named pipe without name parameter invoked");
    }
    else {
        wstring namedPipeNameWstr = stringToWc(namedPipeNameRaw);
        LPCTSTR namedPipeName = namedPipeNameWstr.c_str();
        fresult = pipe_named_wait_ext(namedPipeName, miliseconds);
    }

    return fresult;
}

FunctionSuccesFailure processes::pipe_named_wait_infinite(std::string &namedPipeNameRaw)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(namedPipeNameRaw == UNNAMED) {
        stream::instance().error_message(MOD_PROCESS_MGMT, "wait infinite named pipe without name parameter invoked");
    }
    else {
        wstring namedPipeNameWstr = stringToWc(namedPipeNameRaw);
        LPCTSTR namedPipeName = namedPipeNameWstr.c_str();
        fresult = pipe_named_wait_infinite_ext(namedPipeName);
    }

    return fresult;
}

BOOL processes::pipe_named_operation_is_pending(UINT_PTR hNamedPipeRaw)
{
    return pipe_named_operation_is_pending_ext(hNamedPipeRaw);
}

FunctionSuccesFailure
processes::pipe_named_read(UINT_PTR hNamedPipeRaw, string &inBuffer)
{
    return pipe_named_read_ext(hNamedPipeRaw, inBuffer);
}

FunctionSuccesFailure
processes::pipe_named_write(UINT_PTR hNamedPipeRaw, const string &outBuffer)
{
    return pipe_named_write_ext(hNamedPipeRaw, outBuffer);
}

FunctionSuccesFailure processes::pipe_named_destroy(UINT_PTR hNamedPipeRaw)
{
    return pipe_named_destroy_ext(hNamedPipeRaw);
}

/******************************************************************************
*
*                          processes functions
*
******************************************************************************/
UINT_PTR process_create_ext(LPWSTR commandPath, DWORD initialCreationStataus, UINT_PTR hStdInRaw, UINT_PTR hStdOutRaw, UINT_PTR hStdErrRaw)
{
    UINT_PTR fresult = NULL;

    SECURITY_ATTRIBUTES default_procAttributes = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    SECURITY_ATTRIBUTES default_thrdAttributes = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    STARTUPINFO         default_startup     = {0};
    BOOL                default_hInherit    = TRUE;
    PROCESS_INFORMATION extracted_pinfo = {0};

    default_startup.cb         = sizeof(STARTUPINFO);
    default_startup.hStdInput  = (HANDLE)hStdInRaw;
    default_startup.hStdOutput = (HANDLE)hStdOutRaw;
    default_startup.hStdError  = (HANDLE)hStdErrRaw;

    if( CreateProcess(NULL, commandPath, &default_procAttributes, &default_thrdAttributes, default_hInherit,
                      initialCreationStataus, NULL, NULL, &default_startup, &extracted_pinfo)  != NULL) {

        HANDLE hThread = extracted_pinfo.hThread;
        single_object_handle_close(hThread);

        HANDLE hProcess      = extracted_pinfo.hProcess;
        UINT_PTR hProcessRaw = (UINT_PTR)hProcess;

        stream::instance().system_message(MOD_PROCESS_MGMT, "process created[HANDLE]: "+decToHexString(hProcessRaw));
        fresult = (UINT_PTR)hProcessRaw;
    }
    else {
        stream::instance().error_message(MOD_PROCESS_MGMT, "process launching FAILED");
    }

    return fresult;
}

FunctionSuccesFailure process_destroy_ext(UINT_PTR hProcessRaw)
{
    static const UINT processTermMsTimeout = 100;
    HANDLE hProcess = (HANDLE)hProcessRaw;

    stream::instance().system_message(MOD_PROCESS_MGMT, "child process forced termination");

    if( TerminateProcess(hProcess, EXIT_FAILURE) != 0 && process_wait_ext(hProcessRaw, processTermMsTimeout) == FUNCTION_SUCCESS ) {
        return single_object_handle_close(hProcess);
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure process_wait_ext(UINT_PTR hProcessRaw, UINT miliseconds)
{
    HANDLE hProcess      = (HANDLE)hProcessRaw;
    DWORD  dwMiliseconds = (DWORD)miliseconds;

    return single_object_wait(hProcess, dwMiliseconds);
}

FunctionSuccesFailure process_wait_infinite_ext(UINT_PTR hProcessRaw)
{
    HANDLE hProcess      = (HANDLE)hProcessRaw;
    DWORD  dwMiliseconds = INFINITE;

    return single_object_wait(hProcess, dwMiliseconds);
}


/******************************************************************************
*
*               threads singleton class members
*
******************************************************************************/
inline UINT_PTR threads::thread_create_ext(DWORD WINAPI proc(LPVOID), LPVOID param, SIZE_T stackSize)
{
    static const UINT_PTR threadMutex = processes::mutex_create(UNNAMED); 
    static UINT_PTR fresult           = NULL;

    processes::mutex_take_infinite(threadMutex);
    SECURITY_ATTRIBUTES default_attributes  = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };
    DWORD default_initialCreationStataus    = 0;
    LPDWORD default_threadIdPointer         = NULL;

    HANDLE hThread = CreateThread(&default_attributes, stackSize, proc, param, default_initialCreationStataus, default_threadIdPointer);
    if( hThread != NULL) {
        UINT_PTR hThreadRaw = (UINT_PTR)hThread;
        static const UINT threadReadyMsTimeout = 50;

        if(processes::event_wait(threadReadyEvent, threadReadyMsTimeout) == FUNCTION_SUCCESS) {
            stream::instance().system_message(MOD_PROCESS_MGMT, "thread created[HANDLE]: "+decToHexString(hThreadRaw));
            fresult = hThreadRaw;
        }
        else {
            static const UINT selfThreadTermMsTimeout = 0;
            stream::instance().error_message(MOD_PROCESS_MGMT, "emergency thread closing[HANDLE]: "+decToHexString(hThreadRaw));

            if( thread_wait_ext(hThreadRaw, selfThreadTermMsTimeout) == FUNCTION_FAILURE && thread_destroy_ext(hThreadRaw) == FUNCTION_FAILURE ) {
                stream::instance().error_message(MOD_PROCESS_MGMT, "<!!> emergency thread closing process FAILED");
            }

            fresult = NULL;
        }
    }
    else {
        fresult = NULL;
    }
    processes::mutex_release(threadMutex); 

    return fresult;
}

inline UINT_PTR threads::thread_get_current_ext()
{
    HANDLE hThread      = OpenThread(THREAD_ALL_ACCESS, FALSE, GetCurrentThreadId());
    UINT_PTR hThreadRaw = (UINT_PTR)hThread;

    return hThreadRaw;
}

inline FunctionSuccesFailure threads::thread_ready_ext()
{
    return( processes::event_signal(threadReadyEvent) );
}

inline FunctionSuccesFailure threads::thread_wait_ext(UINT_PTR hThreadRaw, UINT miliseconds)
{
    HANDLE hThread       = (HANDLE)hThreadRaw;
    DWORD  dwMiliseconds = (DWORD)miliseconds;

    return single_object_wait(hThread, dwMiliseconds);
}

inline FunctionSuccesFailure threads::thread_wait_infinite_ext(UINT_PTR hThreadRaw)
{
    HANDLE hThread       = (HANDLE)hThreadRaw;
    DWORD  dwMiliseconds = INFINITE;

    return single_object_wait(hThread, dwMiliseconds);
}

inline FunctionSuccesFailure threads::thread_destroy_ext(UINT_PTR hThreadRaw)
{
/*
        USE THIS FUNCTION DELIBERATELY AS THE LAST METHOD OF TERMINATING THREAD
        IT MAY BE A CAUSE OF DEADLOCK STALL OR RESOURCES LEAKAGE!!
*/

    static const UINT threadTermMsTimeout = 100;
    HANDLE hThread = (HANDLE)hThreadRaw;

    stream::instance().error_message(MOD_PROCESS_MGMT, "<!> unsafe TerminateThread() function invoked");

    if( TerminateThread(hThread, EXIT_FAILURE) != 0 && thread_wait_ext(hThreadRaw, threadTermMsTimeout) == FUNCTION_SUCCESS ) {
        return single_object_handle_close(hThread);
    }

    return FUNCTION_FAILURE;
}


/******************************************************************************
*
*                          events functions
*
******************************************************************************/
UINT_PTR event_create_ext(BOOL bManualReset, LPCTSTR eventName)
{
    UINT_PTR fresult = NULL;
    SECURITY_ATTRIBUTES default_attributes = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };
    BOOL default_initialSignalledState     = FALSE;

    HANDLE hEvent = CreateEvent(&default_attributes, bManualReset, default_initialSignalledState, eventName);
    fresult = (UINT_PTR)hEvent;

    return fresult;
}

FunctionSuccesFailure event_signal_ext(UINT_PTR hEventRaw)
{
    HANDLE hEvent = (HANDLE)hEventRaw;
    if(SetEvent(hEvent) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure event_pulse_ext(UINT_PTR hEventRaw)
{
    HANDLE hEvent = (HANDLE)hEventRaw;
    if(PulseEvent(hEvent) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure event_reset_ext(UINT_PTR hEventRaw)
{
    HANDLE hEvent = (HANDLE)hEventRaw;
    if(ResetEvent(hEvent) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure event_wait_ext(UINT_PTR hEventdRaw, UINT miliseconds)
{
    HANDLE hEvent        = (HANDLE)hEventdRaw;
    DWORD  dwMiliseconds = (DWORD)miliseconds;

    return single_object_wait(hEvent, dwMiliseconds);
}

FunctionSuccesFailure event_wait_infinite_ext(UINT_PTR hEventRaw)
{
    HANDLE hEvent        = (HANDLE)hEventRaw;
    DWORD  dwMiliseconds = INFINITE;

    return single_object_wait(hEvent, dwMiliseconds);
}

FunctionSuccesFailure event_destroy_ext(UINT_PTR hEventRaw)
{
    HANDLE hEvent = (HANDLE)hEventRaw;
    return single_object_handle_close(hEvent);
}


/******************************************************************************
*
*                          mutexes functions
*
******************************************************************************/
UINT_PTR mutex_create_ext(LPCTSTR mutexName)
{
    UINT_PTR fresult = NULL;
    SECURITY_ATTRIBUTES default_attributes = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };
    BOOL default_initialOwnerState         = FALSE;

    HANDLE hMutex = CreateMutex(&default_attributes, default_initialOwnerState, mutexName);
    fresult = (UINT_PTR)hMutex;

    return fresult;
}

FunctionSuccesFailure mutex_take_ext(UINT_PTR hMutexRaw, UINT miliseconds)
{
    HANDLE hMutex        = (HANDLE)hMutexRaw;
    DWORD  dwMiliseconds = (DWORD)miliseconds;

    return single_object_wait(hMutex, dwMiliseconds);
}

FunctionSuccesFailure mutex_take_infinite_ext(UINT_PTR hMutexRaw)
{
    HANDLE hMutex        = (HANDLE)hMutexRaw;
    DWORD  dwMiliseconds = INFINITE;

    return single_object_wait(hMutex, dwMiliseconds);
}

FunctionSuccesFailure mutex_release_ext(UINT_PTR hMutexRaw)
{
    HANDLE hMutex        = (HANDLE)hMutexRaw;

    if(ReleaseMutex(hMutex) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure mutex_destroy_ext(UINT_PTR hMutexRaw)
{
    HANDLE hMutex = (HANDLE)hMutexRaw;
    return single_object_handle_close(hMutex);
}


/******************************************************************************
*
*                          pipes functions
*
******************************************************************************/
vector<pipesNamedAsyncData::pipeNamedInfo> pipesNamedAsyncData::pipeNamedAllInfo(0);

inline FunctionSuccesFailure pipesNamedAsyncData::pipeNamedAsyncStructCreate(UINT_PTR hPipeNamedRaw)
{
    pipeNamedInfo pipeNamedNewStruct = {0};

    UINT_PTR hEventRaw = processes::event_create(TRUE, UNNAMED);
    HANDLE hEvent = (HANDLE)hEventRaw;

    pipeNamedNewStruct.pipeNamedAsyncStruct.hEvent = hEvent;
    pipeNamedNewStruct.hPipeNamedRaw = hPipeNamedRaw;

    pipeNamedAllInfo.push_back(pipeNamedNewStruct);

    return FUNCTION_SUCCESS;
}

inline pipesNamedAsyncData::pipeNamedInfo
&pipesNamedAsyncData::pipeNamedAsyncStructGet(UINT_PTR hPipeNamedRaw)
{
    for(UINT i=0, k=pipeNamedAllInfo.size(); i<k; ++i) {
        if(pipeNamedAllInfo[i].hPipeNamedRaw == hPipeNamedRaw) {
            return pipeNamedAllInfo[i];
        }
    }

    return pipeNamedGetEmptyStruct();
}

inline FunctionSuccesFailure pipesNamedAsyncData::pipeNamedAsyncStructDelete(UINT_PTR hPipeNamedRaw)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    for(UINT i=0, k=pipeNamedAllInfo.size(); i<k; ++i) {
        if(pipeNamedAllInfo[i].hPipeNamedRaw == hPipeNamedRaw) {
            UINT_PTR hEventRaw = (UINT_PTR)pipeNamedAllInfo[i].pipeNamedAsyncStruct.hEvent;
            pipeNamedAllInfo.erase(pipeNamedAllInfo.begin()+i);
            fresult = processes::event_destroy(hEventRaw);
        }
    }

    return fresult;
}

inline BOOL pipesNamedAsyncData::pipeNamedAsyncStructIsPending(UINT_PTR hPipeNamedRaw)
{
    pipeNamedInfo pipeNamedStruct = pipesNamedAsyncData::instance().pipeNamedAsyncStructGet(hPipeNamedRaw);
    UINT_PTR hEventRaw = (UINT_PTR)pipeNamedStruct.pipeNamedAsyncStruct.hEvent;

    if(processes::event_wait(hEventRaw, 0) == FUNCTION_SUCCESS) {
        return FALSE;
    }

    return TRUE;
}

FunctionSuccesFailure pipe_unnamed_create_ext(UINT_PTR &hReadPipeRaw, UINT_PTR &hWritePipeRaw)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    SECURITY_ATTRIBUTES default_attributes = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    HANDLE hReadPipe  = NULL;
    HANDLE hWritePipe = NULL;
    DWORD  default_pipeBufferSize = 0;

    hReadPipeRaw  = NULL;
    hWritePipeRaw = NULL;
    if(CreatePipe(&hReadPipe, &hWritePipe, &default_attributes, default_pipeBufferSize) != 0) {
        hReadPipeRaw  = (UINT_PTR)hReadPipe;
        hWritePipeRaw = (UINT_PTR)hWritePipe;
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure pipe_unnamed_read_ext(UINT_PTR hReadPipeRaw, string &inBuffer)
{
    return single_object_read_data(hReadPipeRaw, inBuffer, NULL);
}

FunctionSuccesFailure pipe_unnamed_write_ext(UINT_PTR hWritePipeRaw, const string &outBuffer)
{
    return single_object_write_data(hWritePipeRaw, outBuffer, NULL);
}

FunctionSuccesFailure pipe_unnamed_destroy_ext(UINT_PTR hReadPipeRaw, UINT_PTR hWritePipeRaw)
{
    HANDLE hReadPipe  = (HANDLE)hReadPipeRaw;
    HANDLE hWritePipe = (HANDLE)hWritePipeRaw;

    if( (single_object_handle_close(hReadPipe) == FUNCTION_SUCCESS)   &&
        (single_object_handle_close(hWritePipe) == FUNCTION_SUCCESS)   ) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

UINT pipe_named_peek_data_size_ext(UINT_PTR hReadPipeRaw)
{
    UINT   fresult     = 0;
    HANDLE hReadPipe   = (HANDLE)hReadPipeRaw;
    DWORD  totalBytes  = 0;

    if(PeekNamedPipe(hReadPipe, NULL, NULL, NULL, &totalBytes, NULL) != 0) {
        fresult = (UINT)totalBytes;
    }

    return fresult;
}

FunctionSuccesFailure pipe_named_peek_data_ext(UINT_PTR hReadPipeRaw, string &peekBuffer)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    HANDLE hReadPipe = (HANDLE)hReadPipeRaw;
    UINT   dataSize  = pipe_named_peek_data_size_ext(hReadPipeRaw);

    peekBuffer = EMPTY_STRING;
    if(dataSize > 0) {
        DWORD  nbPeekedBytes = 0;
        CHAR  *peekInput = new CHAR[dataSize+SPACE_FOR_NULL];

        if( PeekNamedPipe(hReadPipe, peekInput, dataSize, NULL, &nbPeekedBytes, NULL) != 0 && nbPeekedBytes == dataSize ) {
            if(peekInput[dataSize-1] != C_STRING_DELIMITER) {
                peekInput[dataSize] = C_STRING_DELIMITER;
            }

            peekBuffer = string(peekInput);
            fresult    = FUNCTION_SUCCESS;
        }

        delete[] peekInput;
    }

    return fresult;
}

UINT_PTR pipe_named_create_ext(BOOL bMessageType, BOOL bAsyncMode, UINT bufferSize, LPCTSTR pipeName)
{
    UINT_PTR hPipeNamedRaw = NULL;
    static SECURITY_ATTRIBUTES default_attributes = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };
    static const DWORD default_maxInstances = 10;
    static const DWORD default_timeOut = 0;
    DWORD default_openMode      = PIPE_ACCESS_DUPLEX;
    DWORD default_pipeMode      = PIPE_WAIT;
    DWORD default_outBufferSize = bufferSize;
    DWORD default_inBufferSize  = bufferSize;

    if(bAsyncMode == TRUE) {
        default_openMode |= FILE_FLAG_OVERLAPPED;
    }
    if(bMessageType == TRUE) {
        default_pipeMode |= PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE;
    }

    HANDLE hNamedPipe = CreateNamedPipe(pipeName, default_openMode, default_pipeMode, default_maxInstances,
                             default_outBufferSize, default_inBufferSize, default_timeOut, &default_attributes);

    if(hNamedPipe != INVALID_HANDLE_VALUE) {
        hPipeNamedRaw = (UINT_PTR)hNamedPipe;
        if( (default_pipeMode & FILE_FLAG_OVERLAPPED) != 0 ) {
            pipesNamedAsyncData::instance().pipeNamedAsyncStructCreate(hPipeNamedRaw);
        }
    }
    else {
        hPipeNamedRaw = 0;
    }

    return hPipeNamedRaw;
}

UINT_PTR pipe_named_open_ext(BOOL bAsyncMode, LPCTSTR pipeName)
{
    UINT_PTR hPipeNamedRaw = NULL;
    SECURITY_ATTRIBUTES default_attributes = { sizeof(SECURITY_ATTRIBUTES), NULL, FALSE };
    DWORD  default_accessMode   = GENERIC_READ|GENERIC_WRITE;
    DWORD  default_shareMode    = 0;
    DWORD  default_creationMode = OPEN_EXISTING;
    DWORD  default_FlagsAndAttributes = FILE_ATTRIBUTE_NORMAL|FILE_FLAG_DELETE_ON_CLOSE;
    HANDLE default_hTemplateFile = NULL;

    if(bAsyncMode == TRUE) {
        default_FlagsAndAttributes |= FILE_FLAG_OVERLAPPED;
    }

    HANDLE hNamedPipe = CreateFile(pipeName, default_accessMode, default_shareMode, &default_attributes,
                            default_creationMode, default_FlagsAndAttributes, default_hTemplateFile);

    if(hNamedPipe != INVALID_HANDLE_VALUE) {
        hPipeNamedRaw = (UINT_PTR)hNamedPipe;
        if( (default_FlagsAndAttributes & FILE_FLAG_OVERLAPPED) != 0 ) {
            pipesNamedAsyncData::instance().pipeNamedAsyncStructCreate(hPipeNamedRaw);
        }
    }
    else {
        hPipeNamedRaw = 0;
    }

    return hPipeNamedRaw;
}

FunctionSuccesFailure pipe_named_client_wait_ext(UINT_PTR hNamedPipeRaw)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    pipesNamedAsyncData::pipeNamedInfo &pipeNamedStruct = pipesNamedAsyncData::instance().pipeNamedAsyncStructGet(hNamedPipeRaw);
    HANDLE hNamedPipe = (HANDLE)hNamedPipeRaw;

    if( ((pipeNamedStruct.hPipeNamedRaw == 0) && (ConnectNamedPipe(hNamedPipe, NULL) != 0))                                   ||
        ((pipeNamedStruct.hPipeNamedRaw != 0) && (ConnectNamedPipe(hNamedPipe, &pipeNamedStruct.pipeNamedAsyncStruct) != 0))   ) {
            fresult = FUNCTION_SUCCESS;
    }
    else {
        switch(GetLastError()) {
            case ERROR_PIPE_CONNECTED:
                fresult = FUNCTION_SUCCESS;
                break;
            case ERROR_IO_PENDING:
                fresult = FUNCTION_PENDING;
                break;
            default: { /* no action defined */ }
        }
    }

    return fresult;
}

FunctionSuccesFailure pipe_named_wait_ext(LPCTSTR namedPipeName, UINT miliseconds)
{
    DWORD nTimeout = (DWORD)miliseconds;

    if(WaitNamedPipe(namedPipeName, nTimeout) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure pipe_named_wait_infinite_ext(LPCTSTR namedPipeName)
{
    DWORD nTimeout = NMPWAIT_WAIT_FOREVER;

    if(WaitNamedPipe(namedPipeName, nTimeout) != 0) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

BOOL pipe_named_operation_is_pending_ext(UINT_PTR hNamedPipeRaw)
{
    return pipesNamedAsyncData::instance().pipeNamedAsyncStructIsPending(hNamedPipeRaw);
}

FunctionSuccesFailure pipe_named_read_ext(UINT_PTR hNamedPipeRaw, string &inBuffer)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    pipesNamedAsyncData::pipeNamedInfo &pipeNamedStruct = pipesNamedAsyncData::instance().pipeNamedAsyncStructGet(hNamedPipeRaw);

    if( ((pipeNamedStruct.hPipeNamedRaw == 0) && (single_object_read_data(hNamedPipeRaw, inBuffer, NULL) != FUNCTION_FAILURE))                                  ||
        ((pipeNamedStruct.hPipeNamedRaw != 0) && (single_object_read_data(hNamedPipeRaw, inBuffer, &pipeNamedStruct.pipeNamedAsyncStruct) != FUNCTION_FAILURE))  )
    {
        fresult = FUNCTION_SUCCESS;
    }
    else {
        switch(GetLastError()) {
            case ERROR_IO_PENDING:
                fresult = FUNCTION_PENDING;
                break;
            default: { /* no action defined */ }
        }
    }

    return fresult;
}

FunctionSuccesFailure pipe_named_write_ext(UINT_PTR hNamedPipeRaw, const string &outBuffer)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    pipesNamedAsyncData::pipeNamedInfo &pipeNamedStruct = pipesNamedAsyncData::instance().pipeNamedAsyncStructGet(hNamedPipeRaw);

    if(pipe_named_peek_data_size_ext(hNamedPipeRaw) == 0) {
        if( ((pipeNamedStruct.hPipeNamedRaw == 0) && (single_object_write_data(hNamedPipeRaw, outBuffer, NULL) != FUNCTION_FAILURE))                                  ||
            ((pipeNamedStruct.hPipeNamedRaw != 0) && (single_object_write_data(hNamedPipeRaw, outBuffer, &pipeNamedStruct.pipeNamedAsyncStruct) != FUNCTION_FAILURE))  ) {
                    fresult = FUNCTION_SUCCESS;
        }
        else {
            switch(GetLastError()) {
                case ERROR_IO_PENDING:
                    fresult = FUNCTION_PENDING;
                    break;
                default: { /* no action defined */ }
            }
        }
    }

    return fresult;
}

FunctionSuccesFailure pipe_named_destroy_ext(UINT_PTR hNamedPipeRaw)
{
    HANDLE hNamedPipe = (HANDLE)hNamedPipeRaw;
    return single_object_handle_close(hNamedPipe);
}

/******************************************************************************
*
*                          auxiliary functions
*
******************************************************************************/
FunctionSuccesFailure single_object_wait(HANDLE hHandle, DWORD dwMiliseconds)
{
    if( WaitForSingleObject(hHandle, dwMiliseconds) == WAIT_OBJECT_0 ) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure single_object_wait_abandoned(HANDLE hHandle, DWORD dwMiliseconds)
{
    if( WaitForSingleObject(hHandle, dwMiliseconds) == WAIT_ABANDONED ) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure
single_object_read_data(UINT_PTR hHandleRaw, string &inBuffer, LPOVERLAPPED pipeAsyncStruct)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    HANDLE hHandle = (HANDLE)hHandleRaw;
    const UINT presentDataSize = pipe_named_peek_data_size_ext(hHandleRaw);

    if(presentDataSize > 0) {
        DWORD  nbOfBytesCopied = 0;
        CHAR  *input = new CHAR[presentDataSize+SPACE_FOR_NULL];

        if( ReadFile(hHandle, input, presentDataSize, &nbOfBytesCopied, pipeAsyncStruct) != 0 && nbOfBytesCopied == presentDataSize ) {
            if(input[presentDataSize-1] != C_STRING_DELIMITER) {
                input[presentDataSize] = C_STRING_DELIMITER;
            }

            inBuffer = string(input);
            fresult  = FUNCTION_SUCCESS;
        }

        delete[] input;
    }

    return fresult;
}

FunctionSuccesFailure
single_object_write_data(UINT_PTR hHandleRaw, const string &outBuffer, LPOVERLAPPED pipeAsyncStruct)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    DWORD  nbOfBytesCopied = 0;
    HANDLE hHandle = (HANDLE)hHandleRaw;

    if(WriteFile(hHandle, outBuffer.c_str(), outBuffer.size()+SPACE_FOR_NULL, &nbOfBytesCopied, pipeAsyncStruct) != 0) {
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure single_object_handle_close(HANDLE hHandle)
{
    if( CloseHandle(hHandle) != 0 ) {
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

