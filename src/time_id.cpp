/**
  ***************************************************************************************
  * @file    time_id.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.0
  * @date    5/07/2013
  * @brief   This source file provides support for time/date operations
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
#include "time_id.h"
#include <ctime>
#include <sstream>

#define MOD_TIME_ID               "time_id"

using namespace std;

static Date const &default_date();
static tm const get_nowadays();
static BOOL is_leap_year(INT year);
static BOOL is_date_correct(const Date &tst);

string const Date::Month_name[] = { "dummy", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


string const Date::date_to_string() const
{
    string output = "";

    if( (*this)==default_date() ) {
        output = UNAVAILABLE_TEXT;
    }
    else {
        if(day<10) output += '0';
        output += to_string(static_cast<long long>(day));
        output += '-';
        //output += Month_name[month];
        output += to_string(static_cast<long long>(month));
        output += '-';
        output += to_string(static_cast<long long>(year));
    }

    return output;
}

Date::Date(INT d, Month m, INT y): day(d), month(m), year(y), full(date_to_string())
{
    if( !is_date_correct(*this) ) throw invalid_argument("Date(dmy): unexpected date format received!");
}

Date::Date(const std::string &dateRaw)
{
    this->Date::Date();

    if(dateRaw != EMPTY_STRING)
    {
        istringstream dateFieldStream, dateStream(dateRaw);
        for(INT i=member_first, k = member_last; i<k; i++)
        {
            string dateField;
            getline(dateStream, dateField, '-');
            dateFieldStream = istringstream(dateField);

            if(i == member_day)
            {
                dateFieldStream>>day;
            }
            else if(i == member_month)
            {
                INT monthRaw = 0;
                dateFieldStream>>monthRaw;

                if(monthRaw != 0)
                {
                    month = (Month)monthRaw;
                }
            }
            else
            {
                getline(dateStream, dateField);
                dateFieldStream = istringstream(dateField);
                dateFieldStream>>year;
            }
        }
    }

    if( !is_date_correct(*this) ) throw invalid_argument("Date(stringRaw): unexpected date format received!");
}

Date::Date(): day(default_date().day), month(default_date().month), year(default_date().year)
{
}

string const Time::time_to_string() const
{
    string output = "";

    if(hour<10) output += ' ';
    output += to_string(static_cast<long long>(hour));
    output += ':';
    if(minute<10) output += '0';
    output += to_string(static_cast<long long>(minute));
    output += ':';
    if(second<10) output += '0';
    output += to_string(static_cast<long long>(second));

    return output; 
}

static Date const &default_date()
{
    static Date ddate(1, Date::jan, 1900);
    return ddate;
}

Date const get_today_date()
{
    tm now = get_nowadays();
    return Date(static_cast<SHORT>(now.tm_mday), Date::Month(now.tm_mon), now.tm_year);
}

Time const get_current_time()
{
    tm now = get_nowadays();
    return Time(now.tm_hour, now.tm_min, now.tm_sec);
}

static tm const get_nowadays()
{
    const INT month_offset = 1;
    const INT year_offset  = 1900;
    time_t raw_time =  0;
    tm curr_time    = {0};

    time(&raw_time);
    localtime_s(&curr_time, &raw_time);

    curr_time.tm_mon  += month_offset;
    curr_time.tm_year += year_offset;

    return curr_time;
}

static BOOL is_leap_year(INT year)
{
    if( ( !(year%4) && year%100 ) || !(year%400) ) {
        return TRUE;
    }

    return FALSE;
}

static BOOL is_date_correct(const Date &tst)
{
    static const INT day_lowlimit    = 1;
    static INT day_highlimit         = 0;

    static const INT month_lowlimit  = Date::jan;
    static const INT month_highlimit = Date::dec;

    static const INT year_lowlimit   = 1900;
    static const INT year_highlimit  = 2200;

    switch(tst.month)
    {
        case Date::feb:
            if( is_leap_year(tst.year) ) day_highlimit = 29;
            else day_highlimit = 28;
            break;
        case Date::apr:
        case Date::jun:
        case Date::sep:
        case Date::nov:
            day_highlimit = 30;
            break;
        default:
            day_highlimit = 31;
    }

    if( year_lowlimit <= tst.year && tst.year <= year_highlimit ) 
        if( month_lowlimit <= tst.month && tst.month <= month_highlimit )
            if( day_lowlimit <= tst.day && tst.day <= day_highlimit )
                return TRUE;
    return FALSE;
}

//VOID time_delay(INT sec)
//{
//    for(clock_t current=clock()/CLOCKS_PER_SEC, delay=current+sec; delay>current; current=clock()/CLOCKS_PER_SEC);
//}
//
//VOID time_delayms(INT msec)
//{
//    const INT msec_per_sec = 1000;
//    for(clock_t current=clock()/(CLOCKS_PER_SEC/msec_per_sec), delay=current+msec; delay>current; current=clock()/(CLOCKS_PER_SEC/msec_per_sec));
//}

