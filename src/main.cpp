/**
  ***************************************************************************************
  * @file    main.cpp
  * @author  �ukasz Ka�mierczak
  * @version V2.1
  * @date    29/07/2013
  * @brief   This source file provides startup and exception support for program flow
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
#include "stream.h"
#include "interface.h"
#include "libraries.h"
#include "console.h"
#include "time_mgmt.h"

#define MOD_MAIN               "main"

using namespace std;

#define LOGOUT_TEXT_DELAY       2000


int main()
{
    console::settings().initialize();
    Library::instance().initialize();

    while(1) {
        try {
            library_interface();
        }
        catch(exception &ex) {
            stream::instance().error_message(MOD_MAIN, string(ex.what())+" has occured!");
        }
        catch( decltype(QUIT_PROCCESS) input ) {
            if(input == QUIT_PROCCESS) {
                OUTSTREAM<<"Logging out!"<<endl<<endl;
                PRINTOUT;
            }
        }
        catch(...) {
            stream::instance().error_message(MOD_MAIN, "Undetermined error has occured!");
        }

        timers::instance().timeout_create(LOGOUT_TEXT_DELAY);
    }

    Library::instance().close();
    return EXIT_SUCCESS;
}

