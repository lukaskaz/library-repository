/**
  ***************************************************************************************
  * @file    time_mgmt.h
  * @author  �ukasz Ka�mierczak
  * @version V2.1
  * @date    29/07/2013
  * @brief   This header file provides time flow control operations set(timers, timeouts)
  ***************************************************************************************
  * @copy
  *
  * Autor:  �ukasz Ka�mierczak, 2013 POLSKA
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Prawa autorskie � 2013 �ukasz Ka�mierczak. Wszystkie prawa zastrze�one.
  * 
  * To Oprogramowanie zamieszczone w �r�d�ach internetowych lub innych zasobach podlega
  * ochronie praw autorskich. Wszelkie dzia�ania zwi�zane z rozpowszechnianiem, modyfikowaniem,
  * publikowaniem rozwi�za� zamieszczonych w Kodzie �r�d�owym wymaga uzgodnienia z autorem.
  *
  * AUTOR PREZENTOWANYCH ROZWI�ZA� NIE PONOSI ODPOWIEDZIALNO�CI ZA WSZELKIEGO RODZAJU
  * SZKODY(W TYM TAK�E OBOWI�ZKI ZWI�ZANE Z ICH LIKWIDACJ�) SPOWODOWANE U�YWANIEM LUB
  * MAJ�CE ZWI�ZEK Z U�YWANIEM OPROGRAMOWANIA. DOTYCZY TO R�WNIE� PRZYPADKU GDY OSTRZE�ENIE
  * LUB UWAGA O MO�LIWO�CI POJAWIENIA SI� SZKODY LUB PROBLEMU ZOSTA�O PRZEKAZANE.
  *
  * Author: �ukasz Ka�mierczak, 2013 POLAND
  * E-mail: lukasz.kazmierczak.biz@o2.pl
  * Copyright � 2013 �ukasz Ka�mierczak. All rights reserved.
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
  * <h2><center>&copy; COPYRIGHT 2013 �ukasz Ka�mierczak </center></h2>
*/
#ifndef TIME_MGMT_H
#define TIME_MGMT_H

#include <vector>
#include "common.h"


class timers {
public:
    static timers &instance() { static timers tinstance; return tinstance; }
    static const INT  invalidTimer = -1;

    static FunctionSuccesFailure timeout_create(UINT ms_interval);
    static INT timer_create(UINT ms_interval, VOID (*ext_funct)(VOID));
    static INT timeout_peek_create(UINT ms_delay);
    static INT timeout_peek_left_ms(UINT timeout);
    static FunctionSuccesFailure timer_kill(UINT timer);
    static FunctionSuccesFailure timeout_kill(UINT timeout);

private:
    static const enum timerKind{ IDT_KUNDEF=0, IDT_KTIMER, IDT_KTIMEOUT };

    timers() {}
    timers(const timers&);
    timers &operator=(const timers&);

    struct tim_info{
        tim_info():taken(FALSE), hKillEventRaw(NULL), Type(IDT_KUNDEF), TimeoutDelayMs(0), hThreadRaw(0), fct(NULL), ms_interval(0) {}
        tim_info(timerKind tKind, UINT_PTR hEvtRaw, UINT Timeout, UINT_PTR hThdRaw, VOID (*f)(VOID), UINT ms_ival):
                taken(TRUE), hKillEventRaw(hEvtRaw), Type(tKind), TimeoutDelayMs(Timeout), hThreadRaw(hThdRaw), fct(f), ms_interval(ms_ival) {}

        BOOL      taken;
        timerKind Type;
        UINT_PTR  hKillEventRaw;
        UINT      TimeoutDelayMs;
        UINT_PTR  hThreadRaw;
        VOID      (*fct)(VOID);
        UINT      ms_interval;
    };

    static std::vector<tim_info> timersData;
    static const UINT_PTR timerMutex;
    static const UINT maxTimersLimit = 20;

    static BOOL isTimeoutDeployed();
    static BOOL isTimeoutLast();
    static FunctionSuccesFailure getDeployedTimeoutData(tim_info &);
    static FunctionSuccesFailure getFreeTimer(UINT &);
    static FunctionSuccesFailure destroyTimerThread(UINT);

    static VOID updatePeekTimeouts();
    static FunctionSuccesFailure setTaken(UINT, tim_info &);
    static FunctionSuccesFailure releaseTaken(UINT);

    friend VOID timeoutUpdateFunction();
    friend DWORD WINAPI timer_loop(LPVOID lpParam);
};


#endif
