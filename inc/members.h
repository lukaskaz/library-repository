/**
  ***************************************************************************************
  * @file    members.h
  * @author  £ukasz Kaümierczak
  * @version V1.0
  * @date    5/07/2013
  * @brief   This header file provides members operations set
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
#ifndef MEMBERS_H
#define MEMBERS_H

#include "common.h"
#include "time_id.h"

class Patron{
public:
    Patron(std::string nam, std::string snam): uid(0), fname(nam), sname(snam), birthday(Date()), borrowedBooksAmount(0), penalty(0) {}
    //Patron(): fname(""), sname(""), user_id(0), penalty(0) {}
    //Patron(std::string nam, std::string snam, int id): user_id(id), fname(nam), sname(snam), birthday(Date()), penalty(0) {}
    Patron(INT id, std::string patFname, std::string patSname, Date birth, Date joinDate, INT borrowedBooks): 
        uid(id), fname(patFname), sname(patSname), birthday(birth), affiliationDate(joinDate), borrowedBooksAmount(borrowedBooks), penalty(0) {}
    Patron(std::string patFname, std::string patSname, Date birth): 
        uid(0), fname(patFname), sname(patSname), birthday(birth), affiliationDate(Date()), borrowedBooksAmount(0), penalty(0) {}

    VOID increment_borrowed_books() { borrowedBooksAmount += 1; }
    VOID decrement_borrowed_books() { if(borrowedBooksAmount > 0) borrowedBooksAmount -= 1; }
    VOID clear_penalty() { penalty = 0; }
    VOID set_penalty(DOUBLE val) { penalty = val; }

    std::string const &get_sname() const { return sname; }
    std::string const &get_fname() const { return fname; }
    INT const get_id()             const { return uid;   }
    Date get_affiliation_date()    const { return affiliationDate;     }
    Date const &get_birth()        const { return birthday;            }
    BOOL   is_penalty()            const { return (penalty != 0);      }
    INT get_borrowed_books_amt()   const { return borrowedBooksAmount; }
    DOUBLE get_penalty()           const { return penalty;             }

    friend std::ostream &operator<<(std::ostream &os, const Patron &person);

private:
    INT uid;
    std::string fname;
    std::string sname;
    Date birthday;
    Date affiliationDate;
    INT borrowedBooksAmount;
    DOUBLE penalty;

};

std::ostream &operator<<(std::ostream &os, const Patron &person);


#endif
