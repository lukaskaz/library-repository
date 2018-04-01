/**
  ***************************************************************************************
  * @file    main.cpp
  * @author  £ukasz Kaümierczak
  * @version V2.1
  * @date    29/07/2013
  * @brief   This source file provides startup and exception support for program flow
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

