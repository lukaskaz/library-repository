/**
  ***************************************************************************************
  * @file    time_mgmt.cpp
  * @author  £ukasz Kaümierczak
  * @version V2.1
  * @date    29/07/2013
  * @brief   This source file provides support for time flow control operations
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
#include "time_mgmt.h"
#include <Windows.h>
#include <vector>

#include "stream.h"
#include "process_mgmt.h"

#define MOD_TIME_MGMT               "time_mgmt"

using namespace std;

static DWORD WINAPI timer_loop(LPVOID lpParam);
static VOID timeoutUpdateFunction();


/******************************************************************************
*
*               delay class members
*
******************************************************************************/
class delay {
public:
    delay(): currentTimePoint(0), timeDiff(0) {}
    delay(LONGLONG referenceTimePoint) { this->delay::delay(); calculateTimeDiff(referenceTimePoint); }

    FunctionSuccesFailure create_msDelay(UINT msDelay);
    LONGLONG getCurrentTimePoint() { return currentTimePoint; }
    DOUBLE getTimeDiff() { return timeDiff; }
    DOUBLE getTimeDiff(LONGLONG referenceTimePoint) { calculateTimeDiff(referenceTimePoint); return getTimeDiff(); }

private:
    static const DOUBLE FreqMsTrans;
    static const UINT_PTR delayMutex;
    static DOUBLE Freq;
    LONGLONG currentTimePoint;
    DOUBLE timeDiff;

    static DOUBLE getFreq();
    VOID calculateTimeDiff(LONGLONG referenceTimePoint);
};

DOUBLE   const delay::FreqMsTrans = 1000.0;
UINT_PTR const delay::delayMutex  = processes::mutex_create(UNNAMED);
DOUBLE   delay::Freq              = delay::getFreq();

inline DOUBLE delay::getFreq()
{
    LARGE_INTEGER freq_base = {0};

    QueryPerformanceFrequency(&freq_base);
    return (DOUBLE(freq_base.QuadPart)/FreqMsTrans);
}

inline VOID delay::calculateTimeDiff(LONGLONG referenceTimePoint)
{
    static const DOUBLE roundLimit = 0.95;
    static const DOUBLE roundValue = 1.000001;
    LARGE_INTEGER currentTime = {0};
    DOUBLE timeDiff = 0;

    processes::mutex_take_infinite(delayMutex);
    QueryPerformanceCounter(&currentTime);
    processes::mutex_release(delayMutex);

    timeDiff = (currentTime.QuadPart - referenceTimePoint)/Freq;

    if( (timeDiff - (UINT)(timeDiff)) > roundLimit ) {
        timeDiff = (UINT)(timeDiff) + roundValue;
    }

    this->timeDiff = timeDiff;
    this->currentTimePoint = currentTime.QuadPart;
}

inline FunctionSuccesFailure delay::create_msDelay(UINT msDelay)
{
    LONGLONG startTimePoint = 0;

    calculateTimeDiff(startTimePoint);
    startTimePoint = getCurrentTimePoint();
    while((DOUBLE)msDelay > getTimeDiff(startTimePoint)){
        Sleep(1);
    }

    return FUNCTION_SUCCESS;
}

/******************************************************************************
*
*               timers singleton class members
*
******************************************************************************/
UINT_PTR const timers::timerMutex = processes::mutex_create(UNNAMED);
vector<timers::tim_info> timers::timersData(maxTimersLimit, tim_info());

VOID timeoutUpdateFunction()
{
    timers::updatePeekTimeouts();
}

DWORD WINAPI timer_loop(LPVOID lpParam)
{
    //timer_loop_param timerParam = *(timer_loop_param*)lpParam;
    timers::tim_info timerParam = *(timers::tim_info*)lpParam;

    if( timerParam.ms_interval > 0 && timerParam.fct != NULL ) {
        processes::thread_ready();

        delay new_delay;
        while(processes::event_wait(timerParam.hKillEventRaw, 0) == FUNCTION_FAILURE){
            timerParam.fct();
            new_delay.create_msDelay(timerParam.ms_interval);
        }

        processes::event_destroy(timerParam.hKillEventRaw);
        timerParam.hKillEventRaw = NULL;
        stream::instance().system_message(MOD_TIME_MGMT ,"timer killed!");
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

FunctionSuccesFailure timers::getFreeTimer(UINT &freeTimer)
{
    for(UINT tim=0; tim<maxTimersLimit; ++tim) {
        if(timersData[tim].taken == FALSE) {
            freeTimer = tim;
            return FUNCTION_SUCCESS;
        }
    }
    stream::instance().error_message(MOD_TIME_MGMT, "All free timers depleted!");

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure
timers::setTaken(UINT timer, tim_info &timData)
{
    if( timer < maxTimersLimit && timersData[timer].taken == FALSE ) {
        timersData[timer] = timData;
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure timers::releaseTaken(UINT timer)
{
    if( timer < maxTimersLimit && timersData[timer].taken == TRUE ) {
        timersData[timer] = tim_info();
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

INT timers::timer_create(const UINT ms_interval, VOID (*ext_fct)(VOID) )
{
    processes::mutex_take_infinite(timerMutex);
    INT fresult   = FUNCTION_FAILURE;
    UINT newTimer = 0;

    if(getFreeTimer(newTimer) != FUNCTION_FAILURE) {
        tim_info timerData(IDT_KTIMER, 0, 0, 0, ext_fct, ms_interval);
        timerData.hKillEventRaw = processes::event_create(FALSE, UNNAMED);
        if(timerData.hKillEventRaw != NULL) {
            timerData.hThreadRaw = processes::thread_create(timer_loop, (LPVOID*)&timerData, 0);
            if(timerData.hThreadRaw != NULL) {
                setTaken(newTimer, timerData);
            }
        }
    }
    processes::mutex_release(timerMutex);

    return fresult;
}

INT timers::timeout_peek_create(const UINT timeout)
{
    static const UINT tUpdateRateMs    = 1;
    static VOID (*const ext_fct)(VOID) = timeoutUpdateFunction;

    processes::mutex_take_infinite(timerMutex);
    INT fresult     = FUNCTION_FAILURE;
    UINT newTimeout = 0;

    if(getFreeTimer(newTimeout) != FUNCTION_FAILURE) {
        tim_info timeoutData(IDT_KTIMEOUT, 0, timeout, 0, ext_fct, tUpdateRateMs);

        if(isTimeoutDeployed() == FALSE) {
            timeoutData.hKillEventRaw = processes::event_create(TRUE, UNNAMED);
            if(timeoutData.hKillEventRaw != NULL) {
                timeoutData.hThreadRaw = processes::thread_create(timer_loop, (LPVOID*)&timeoutData, 0);
                if(timeoutData.hThreadRaw != NULL) {
                    setTaken(newTimeout, timeoutData);
                    fresult = newTimeout;
                }
            }
        }
        else {
            getDeployedTimeoutData(timeoutData);
            setTaken(newTimeout, timeoutData);
            fresult = newTimeout;
        }
    }
    processes::mutex_release(timerMutex);

    return fresult;
}

FunctionSuccesFailure timers::timeout_create(const UINT ms_interval)
{
    delay new_delay;
    return new_delay.create_msDelay(ms_interval);
};

FunctionSuccesFailure timers::timer_kill(UINT timer)
{
    if( timer < maxTimersLimit && timersData[timer].taken == TRUE ) {
        if( timersData[timer].Type == IDT_KTIMER || (timersData[timer].Type == IDT_KTIMEOUT && isTimeoutLast() == TRUE) ) {
            destroyTimerThread(timer);
        }
        releaseTaken(timer);
        return FUNCTION_SUCCESS;
    }

    return FUNCTION_FAILURE;
}

FunctionSuccesFailure timers::timeout_kill(UINT timeoutRaw)
{
    return timer_kill(timeoutRaw);
}

VOID timers::updatePeekTimeouts()
{
    static LONGLONG startTimePoint = 0;
    static BOOL updateComplete = FALSE;
    UINT decrTimeMsInterv = 0;
    UINT timeouts_amount  = 0;
    UINT timeout_number   = 0;
    UINT timeDiff         = 0;

    delay newTimePoint(startTimePoint);
    if(startTimePoint != 0) {
        timeDiff = (UINT)newTimePoint.getTimeDiff();
    }
    startTimePoint = newTimePoint.getCurrentTimePoint();

    processes::mutex_take_infinite(timerMutex);
    for(UINT tim=0; tim<maxTimersLimit; ++tim) {
        if( timersData[tim].taken == TRUE && timersData[tim].Type == IDT_KTIMEOUT ) {
            ++timeouts_amount;
            timeout_number = tim;
            if(timersData[tim].TimeoutDelayMs > timeDiff) {
                timersData[tim].TimeoutDelayMs -= timeDiff;
            }
            else {
                timersData[tim].TimeoutDelayMs = 0;
            }
        }
    }

    if( timeouts_amount == 1 && timersData[timeout_number].TimeoutDelayMs == 0 ) {
        destroyTimerThread(timeout_number);
    }
    processes::mutex_release(timerMutex);

}

INT timers::timeout_peek_left_ms(UINT timeout)
{
    processes::mutex_take_infinite(timerMutex);
    INT fresult = FUNCTION_FAILURE;

    if( timeout < maxTimersLimit && timersData[timeout].taken == TRUE ) {
        UINT TimeoutDelayMs = timersData[timeout].TimeoutDelayMs;
        if(TimeoutDelayMs == 0) {
            releaseTaken(timeout);
        }

        fresult = (INT)TimeoutDelayMs;
    }
    processes::mutex_release(timerMutex);

    return fresult;
};

BOOL timers::isTimeoutDeployed()
{
    for(UINT tim=0; tim<maxTimersLimit; ++tim) {
        if( timersData[tim].taken == TRUE && timersData[tim].Type == IDT_KTIMEOUT &&
            timersData[tim].hKillEventRaw != NULL                                 &&
            processes::event_wait(timersData[tim].hKillEventRaw, 0) == FUNCTION_FAILURE) {
            return TRUE;
        }
    }

    return FALSE;
}

FunctionSuccesFailure timers::getDeployedTimeoutData(tim_info &timeoutData)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    for(UINT tim=0; tim<maxTimersLimit; ++tim) {
        if( timersData[tim].Type == IDT_KTIMEOUT && timersData[tim].taken == TRUE) {
            timeoutData.hThreadRaw    = timersData[tim].hThreadRaw;
            timeoutData.hKillEventRaw = timersData[tim].hKillEventRaw;
            fresult = FUNCTION_SUCCESS;
            break;
        }
    }

    return fresult;
}

BOOL timers::isTimeoutLast()
{
    UINT timeouts = 0;
    for(UINT tim=0; tim<maxTimersLimit; ++tim) {
        if( timersData[tim].taken == TRUE && timersData[tim].Type == IDT_KTIMEOUT ) {
            ++timeouts;
            if(timeouts > 1) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

FunctionSuccesFailure timers::destroyTimerThread(UINT timer)
{
    if( timer < maxTimersLimit && timersData[timer].taken == TRUE ) {
        return processes::event_signal(timersData[timer].hKillEventRaw);
    }

    return FUNCTION_FAILURE;
}

