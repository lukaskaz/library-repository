/**
  ***************************************************************************************
  * @file    books.h
  * @author  £ukasz Kaümierczak
  * @version V1.3
  * @date    12/07/2013
  * @brief   This header file provides books operations set
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
#ifndef BOOKS_H
#define BOOKS_H

#include "common.h"
#include "time_id.h"

struct ISBN{
    ISBN(): fst_symbol('0'), snd_series(0), trd_series(0), fth_series(0), full(number_to_string()) {}
    ISBN(INT ft, INT tr, INT sn, CHAR fs);

    CHAR fst_symbol;
    INT snd_series;
    INT trd_series;
    INT fth_series;
    std::string full;

    BOOL is_blank() const { if(fst_symbol != '0' || snd_series || trd_series || fth_series) return FALSE; return TRUE; }
    std::string const number_to_string() const;
};

class Book{
public:
    static const enum Genre { undef=0, sci_fi, facts, magaz, biogrph, child };
    //Book(): borrowed(false), title(""), ISBN_number(ISBN()), author_fname(""), author_sname(""), type(undef), copyright_date(Date()) {}
    //Book(string tit): lent(false), title(tit), ISBN_number(ISBN()), author_fname(""), author_sname(""), copyright_date(Date()) {}
    Book(INT id, std::string tit, std::string fname, std::string sname, Date lentDate, INT userId):
        uid(id), lent(0), title(tit), ISBN_number(ISBN()), author_fname(fname), author_sname(sname),
        lending_date(lentDate), borrower_id(userId), quantity(1), type(undef), copyright_date(Date()) {}

    Book(std::string tit, std::string fname, std::string sname):
        uid(0), lent(0), title(tit), ISBN_number(ISBN()), author_fname(fname), author_sname(sname),
        lending_date(Date()), borrower_id(0), quantity(1), type(undef), copyright_date(Date()) {}

    VOID add() { ++quantity; }
    VOID set_lent() { if( is_available()==FALSE ) throw std::runtime_error("Book::set_lent(): this book is unavailable!"); lent++;}
    VOID set_returned() { if(!lent) throw std::runtime_error("Book::set_returned(): all books are already returned!"); lent--; }

    BOOL is_available()                   const { return (lent<quantity);     }
    INT const &get_id()                   const { return uid;                 }
    std::string const &get_title()        const { return title;               }
    std::string const &get_isbn()         const { return ISBN_number.full;    }
    Date const &get_lending_date()        const { return lending_date;        }
    INT  const  get_borrower_id()         const { return borrower_id;         }
    std::string const &get_author_fname() const { return author_fname;        }
    std::string const &get_author_sname() const { return author_sname;        }
    std::string const &get_copyrgt_date() const { return copyright_date.full; }

    VOID set_as_lent(INT);
    VOID release_lent() { lending_date = Date(); };

    BOOL operator==(const Book &comp) const;
    BOOL operator!=(const Book &comp) const { if( (*this)==comp ) return FALSE; return TRUE; }
    friend std::ostream &operator<<(std::ostream &os, const Book &output);

private:
    INT   uid;
    SHORT lent;
    SHORT quantity;

    std::string title;
    ISBN ISBN_number;
    std::string author_fname;
    std::string author_sname;
    Genre type;
    Date copyright_date;
    Date lending_date;
    INT borrower_id;
};

std::ostream &operator<<(std::ostream &os, const Book &output);


#endif
