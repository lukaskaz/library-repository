/**
  ***************************************************************************************
  * @file    time_id.h
  * @author  £ukasz Kaümierczak
  * @version V1.0
  * @date    5/07/2013
  * @brief   This header file provides time/date operations set
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
#ifndef TIME_ID_H
#define TIME_ID_H

#include "common.h"

struct Date{
    static const enum members { member_invalid=0, member_first, member_day=member_first, member_month, member_year, member_last };
    static const enum Month { jan=1, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec, };
    static const std::string Month_name[];
    Date();
    Date(INT d, Month m, INT y);
    Date(const std::string &);

    SHORT day;
    Month month;
    INT year;
    std::string full;

    std::string const date_to_string() const;
    BOOL operator==(const Date &comp)  const { if(day==comp.day && month==comp.month && year==comp.year) return TRUE; return FALSE; }
    BOOL operator!=(const Date &comp)  const { if( (*this)==comp ) return FALSE; return TRUE; }
};

struct Time{
    Time(): hour(0), minute(0), second(0), full(time_to_string()){}
    Time(INT hr, INT min, INT sec): hour(hr), minute(min), second(sec), full(time_to_string()) {}

    INT hour;
    INT minute;
    INT second;
    std::string full;

    std::string const time_to_string() const;
};


Date const get_today_date();
Time const get_current_time();
//VOID time_delay(INT sec);
//VOID time_delayms(INT msec);


#endif
