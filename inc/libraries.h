/**
  ***************************************************************************************
  * @file    libraries.h
  * @author  £ukasz Kaümierczak
  * @version V1.2
  * @date    15/07/2013
  * @brief   This header file provides operations set for 'library' core functionalities
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
#ifndef LIBRARIES_H
#define LIBRARIES_H

#include <vector>
#include "common.h"
#include "books.h"
#include "members.h"

class Library{
public:
    static Library &instance() { static Library linstance; return linstance; }

    VOID close()                                 const;
    BOOL is_person_present(const Patron &member) const;
    BOOL is_book_present(const Book &book)       const;
    BOOL is_book_available(const Book &book)     const;
    FunctionSuccesFailure find_book()            const;
    FunctionSuccesFailure find_member()          const;
    FunctionSuccesFailure find_borrower()        const;
    FunctionSuccesFailure list_books()           const;
    FunctionSuccesFailure list_members()         const;
    FunctionSuccesFailure list_borrowers()       const;

    VOID initialize();
    VOID add_book();
    VOID add_member();
    FunctionSuccesFailure add_lending();
    FunctionSuccesFailure release_lending();
    FunctionSuccesFailure delete_book_by_data();
    FunctionSuccesFailure delete_book_by_position();
    FunctionSuccesFailure delete_member_by_data();
    FunctionSuccesFailure delete_member_by_position();

private:
    Library() { members.empty(); books_in_stock.empty(); }
    Library(const Library &);
    Library &operator=(const Library &);
    std::vector<Patron> members;
    std::vector<Book> books_in_stock;

    VOID synchro_with_db();
    VOID sort_books_by_title();
    VOID set_book_given(const Book &book);

    std::string get_members_row_content(INT, INT)   const;
    std::string get_books_row_content(INT, INT)     const;
    INT get_selection_from_books(INT)               const;
    INT get_selection_from_members()                const;
    INT get_selection_from_borrowers()              const;
    FunctionSuccesFailure get_view_from_books(INT)  const;
    FunctionSuccesFailure get_view_from_members()   const;
    FunctionSuccesFailure get_view_from_borrowers() const;

    VOID select_books_storage_file()                const;
    VOID select_members_storage_file()              const;
    FunctionSuccesFailure open_books_table()        const;
    FunctionSuccesFailure open_members_table()      const;
};


#endif
