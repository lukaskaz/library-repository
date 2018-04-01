/**
  ***************************************************************************************
  * @file    stream.h
  * @author  �ukasz Ka�mierczak
  * @version V1.1
  * @date    21/06/2013
  * @brief   This header file provides std stream operations set, multithreaded adapted
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
#ifndef STREAM_H
#define STREAM_H

#include "common.h"

#define QUIT_PROCCESS    'q'
#define OUTSTREAM        stream::get_os()
#define PRINTOUT         stream::print()

class stream {
public:
    static stream &instance() { static stream sinstance; return sinstance; }

    static std::ostream &get_os();
    static VOID print();

    static FunctionSuccesFailure get_int(INT &);
    static FunctionSuccesFailure get_char(CHAR &);
    static FunctionSuccesFailure get_text(std::string &);
    static FunctionSuccesFailure enter_input_section();
    static FunctionSuccesFailure leave_input_section();
    static FunctionSuccesFailure system_message(const std::string &module, const std::string &output);
    static FunctionSuccesFailure error_message(const std::string &module, const std::string &output);

private:
    stream() {}
    stream(const stream&);
    stream &operator=(const stream&);

    static std::ostream &os;
    static std::ostream &oerr;
    static std::istream &is;
    static const UINT_PTR istreamMutex;
    static const UINT_PTR ostreamMutex;
    static const UINT_PTR ostreamSystemMutex;

    static BOOL desynchroPending;
    static const UINT ostreamWaitMsTimeout = 100;

    static FunctionSuccesFailure output_message(const std::string &module, const std::string &output);
    static FunctionSuccesFailure error_output_message(const std::string &module, const std::string &err_output);
    static VOID desynchronized_threads_handler(UINT_PTR mutex_blocked, UINT_PTR mutex_current_owner);
    static VOID desynchronization_source_handler(UINT_PTR mutex_current_owner);

    static std::string get_input();
};


#endif
