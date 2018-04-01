/**
  ***************************************************************************************
  * @file    books.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.3
  * @date    12/07/2013
  * @brief   This source file provides support for books objects
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
#include "books.h"
#include "time_id.h"

#define MOD_BOOKS               "books"

using namespace std;

static BOOL is_ISBN_correct(const ISBN &tst);


ISBN::ISBN(INT ft, INT tr, INT sn, CHAR fs): fst_symbol(fs), snd_series(sn), trd_series(tr), fth_series(ft), full(number_to_string())
{
    if( !is_ISBN_correct(*this) ) throw("ISBN(ftsf): unexpected ISBN format received!");
}

string const ISBN::number_to_string() const
{
    string output = "";

    if( fst_symbol=='0' && !snd_series && !trd_series && !fth_series) {
        output = UNAVAILABLE_TEXT;
    }
    else {
        output += to_string(static_cast<long long>(fth_series));
        output += '-';  
        output += to_string(static_cast<long long>(trd_series));
        output += '-'; 
        output += to_string(static_cast<long long>(snd_series));
        output += '-'; 
        output += string(1,fst_symbol);
    }

    return output;
}

VOID Book::set_as_lent(INT userID)
{
    lending_date = get_today_date();
    borrower_id = userID;
}

BOOL Book::operator==(const Book &comp) const
{
    if( ISBN_number.is_blank() || comp.ISBN_number.is_blank() )
        throw invalid_argument("Book::operator==(): At least one of compared books has blank ISBN number!");

    return (ISBN_number.full == comp.ISBN_number.full);
}

static BOOL is_ISBN_correct(const ISBN &tst)
{
static int snd_series_limit = 1000;
static int trd_series_limit = 100;
static int fth_series_limit = 1000;

    if( isdigit(tst.fst_symbol) || isalpha(tst.fst_symbol) )
        if( tst.snd_series >= 0 && tst.snd_series < snd_series_limit)
            if( tst.trd_series >= 0 && tst.trd_series < trd_series_limit)
                if( tst.fth_series >= 0 && tst.fth_series < fth_series_limit)
                    return TRUE;
    return FALSE;
}

ostream &operator<<(ostream &os, const Book &output)
{
    os<<endl
      <<"--------------------------------------------------"<<endl
      <<"Book's title: "<<output.title<<endl
      <<"Author's first name: "<<output.author_fname<<endl
      <<"Author's last name:  "<<output.author_sname<<endl
      <<"ISBN number: "<<output.ISBN_number.full<<endl;

    return os;
}

