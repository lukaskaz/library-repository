/**
  ***************************************************************************************
  * @file    libraries.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.2
  * @date    15/07/2013
  * @brief   This source file provides support for 'library' core functionalities
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
#include "libraries.h"
#include <sstream>

#include "storage_mgmt.h"
#include "stream.h"
#include "console.h"
#include "time_mgmt.h"
#include "time_id.h"

#define MOD_LIBRARIES               "libraries"

using namespace std;

#define OPER_NOTIF_MSDELAY          2000U
#define OPER_NOTIF_SHORT_MSDELAY    1000U
static string get_user_text_inserted_by_guidance(const string &guide);
static INT get_user_sel_from_menu(const string &menu);

enum library_categories { LIB_CATEGORY_INVALID=0, LIB_CATEGORY_FIRST, LIB_CATEGORY_BOOKS=LIB_CATEGORY_FIRST,
                          LIB_CATEGORY_MEMBERS, LIB_CATEGORY_LAST };

const CHAR *booksTableName    = "books";
const CHAR *membersTableName  = "members";

const CHAR *booksFileName     = booksTableName;
const CHAR *membersFileName   = membersTableName;

const CHAR *booksColumnName[]   = {"Id", "Title", "Author name", "Author last name", "Date of lending", "Borrower id"};
const CHAR *membersColumnName[] = {"Id", "Member name", "Member last name", "Member bithday", "Date of join", "<Unused>"};


static string get_books_columns_name()
{
    string output;
    INT columnToNeglect = 5;
    for(INT i=0, k=ARRAY_SIZE(booksColumnName); i<k; ++i)
    {
        if(i != columnToNeglect)
        {
            output += string(booksColumnName[i]) + LINE_FIELDS_DELIMITER;
        }
    }
    output += NEW_LINE_TOKEN;

    return output;
}

static string get_members_columns_name()
{
    string output;
    INT columnToNeglect = 5;
    for(INT i=0, k=ARRAY_SIZE(membersColumnName); i<k; ++i)
    {
        if(i != columnToNeglect)
        {
            output += string(membersColumnName[i]) + LINE_FIELDS_DELIMITER;
        }
    }
    output += NEW_LINE_TOKEN;

    return output;
}

BOOL Library::is_person_present(const Patron &person) const
{
    for(INT i=0, k=members.size(); i<k; ++i)
        if( members[i].get_sname()==person.get_sname() && members[i].get_fname()==person.get_fname() )
            return TRUE;

    return FALSE;
}

BOOL Library::is_book_present(const Book &book) const
{
    for(INT i=0, k=books_in_stock.size(); i<k; ++i)
    {
        if( books_in_stock[i].get_title()==book.get_title()               &&
            books_in_stock[i].get_author_sname()==book.get_author_sname() &&
            books_in_stock[i].get_author_fname()==book.get_author_fname() &&
            books_in_stock[i].get_isbn()==book.get_isbn()                  )
            return TRUE;
    }

    return FALSE;
}

BOOL Library::is_book_available(const Book &book) const
{
    for(INT i=0, k=books_in_stock.size(); i<k; ++i)
        if( books_in_stock[i].get_title()==book.get_title()               &&
            books_in_stock[i].get_author_sname()==book.get_author_sname() &&
            books_in_stock[i].get_author_fname()==book.get_author_fname() &&
            books_in_stock[i].get_isbn()==book.get_isbn()                  )
            return books_in_stock[i].is_available();

    return FALSE;
}

void Library::sort_books_by_title()
{
    for(INT i=0, k=books_in_stock.size(); i<k; i++) {
        for(INT j=i; j<k; j++) {
            if( books_in_stock[i].get_title() > books_in_stock[j].get_title() )
            {
                Book bookToRelocate = books_in_stock[j];
                books_in_stock.erase(books_in_stock.begin()+j);
                books_in_stock.insert(books_in_stock.begin()+i, bookToRelocate);
            }
        }
    }
}

VOID Library::set_book_given(const Book &book)
{
    for(INT i=0, k=books_in_stock.size(); i<k; ++i)
        if( books_in_stock[i].get_title()==book.get_title()               &&
            books_in_stock[i].get_author_sname()==book.get_author_sname() &&
            books_in_stock[i].get_author_fname()==book.get_author_fname() &&
            books_in_stock[i].get_isbn()==book.get_isbn()                  )
            books_in_stock[i].set_lent();
}

FunctionSuccesFailure Library::open_books_table() const
{
    return database_carrier::instance().select_table(booksTableName);
}

FunctionSuccesFailure Library::open_members_table() const
{
    return database_carrier::instance().select_table(membersTableName);
}

VOID Library::select_books_storage_file() const
{
    file_carrier::instance().set_storage_file_name(booksFileName);
}

VOID Library::select_members_storage_file() const
{
    file_carrier::instance().set_storage_file_name(membersFileName);
}

VOID Library::close() const
{
    database_carrier::instance().close();
}

VOID Library::initialize()
{
    synchro_with_db();
}

VOID Library::add_member()
{
    Date actual_date = get_today_date();
    string fname(UNAVAILABLE_TEXT), sname(UNAVAILABLE_TEXT), birth(UNAVAILABLE_TEXT);

    fname = get_user_text_inserted_by_guidance("Enter member's name");
    sname = get_user_text_inserted_by_guidance("Enter member's last name");
    birth = get_user_text_inserted_by_guidance("Enter member's birthsday[DD-MM-YYYY]");

    open_members_table();
    database_carrier::instance().add_entry(
        store_carrier_member(store_carrier_member::MEMBER_ALL, fname, sname,
                              Date(birth).date_to_string(), actual_date.date_to_string(), 0)
    );

    INT newIdValue = database_carrier::instance().get_last_insert_id();
    select_members_storage_file();
    file_carrier::instance().add_entry( store_carrier_member(store_carrier_member::MEMBER_ALL,
        newIdValue, fname, sname, Date(birth).date_to_string(), actual_date.date_to_string(), 0 ) );

    members.push_back( Patron(newIdValue, fname, sname, Date(birth).date_to_string(), actual_date, 0) );
}

VOID Library::add_book()
{
    string title(UNAVAILABLE_TEXT), fauthor(UNAVAILABLE_TEXT), sauthor(UNAVAILABLE_TEXT);

    title   = get_user_text_inserted_by_guidance("Enter book's title");
    fauthor = get_user_text_inserted_by_guidance("Enter book's author name");
    sauthor = get_user_text_inserted_by_guidance("Enter book's author last name");

    open_books_table();
    database_carrier::instance().add_entry(
        store_carrier_member(store_carrier_member::MEMBER_ALL, title, fauthor, sauthor, Date().date_to_string(), 0)
    );

    INT newIdValue = database_carrier::instance().get_last_insert_id();

    select_books_storage_file();
    file_carrier::instance().add_entry(
        store_carrier_member(store_carrier_member::MEMBER_ALL, newIdValue, title, fauthor, sauthor, Date().date_to_string(), 0)
    );
    books_in_stock.push_back(Book(newIdValue, title, fauthor, sauthor, Date(), 0));
    sort_books_by_title();
}

FunctionSuccesFailure Library::delete_book_by_data()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string title(UNAVAILABLE_TEXT), fauthor(UNAVAILABLE_TEXT), sauthor(UNAVAILABLE_TEXT);

    title   = get_user_text_inserted_by_guidance("Enter book's title");
    fauthor = get_user_text_inserted_by_guidance("Enter book's author name");
    sauthor = get_user_text_inserted_by_guidance("Enter book's author last name");

    if(title != EMPTY_STRING && sauthor != EMPTY_STRING)
    {
        for(INT i=0; i<books_in_stock.size(); ++i)
        {
            if(books_in_stock[i].get_title() == title &&
                books_in_stock[i].get_author_sname() == sauthor &&
                books_in_stock[i].get_author_fname() == fauthor )
            {
                open_books_table();
                database_carrier::instance().delete_entry( store_carrier_member
                        (store_carrier_member::MEMBER_ID, books_in_stock[i].get_id()) );

                select_books_storage_file();
                file_carrier::instance().delete_entry( store_carrier_member
                        (store_carrier_member::MEMBER_ID, books_in_stock[i].get_id()) );

                books_in_stock.erase(books_in_stock.begin()+i);
                OUTSTREAM<<endl<<"Record deleted!"<<endl;
                PRINTOUT;
                fresult = FUNCTION_SUCCESS;
                break;
            }
        }

        if(fresult == FUNCTION_FAILURE)
        {
            OUTSTREAM<<endl<<"Record not found!"<<endl;
            PRINTOUT;
        }
    }
    else
    {
        OUTSTREAM<<endl<<"Inserted data is insufficient!"<<endl;
        PRINTOUT;
    }

    timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
    return fresult;
}

FunctionSuccesFailure Library::delete_book_by_position()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    INT entryToDeleteIdx = get_selection_from_books(0);
    if(entryToDeleteIdx >= 0)
    {
        open_books_table();
        database_carrier::instance().delete_entry( store_carrier_member( store_carrier_member::MEMBER_ID,
            books_in_stock[entryToDeleteIdx].get_id()) );

        select_books_storage_file();
        file_carrier::instance().delete_entry( store_carrier_member( store_carrier_member::MEMBER_ID,
            books_in_stock[entryToDeleteIdx].get_id()) );

        books_in_stock.erase(books_in_stock.begin()+entryToDeleteIdx);
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure Library::delete_member_by_data()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string fname(UNAVAILABLE_TEXT), sname(UNAVAILABLE_TEXT), birth(UNAVAILABLE_TEXT);

    fname = get_user_text_inserted_by_guidance("Enter member's name");
    sname = get_user_text_inserted_by_guidance("Enter member's last name");
    birth = get_user_text_inserted_by_guidance("Enter member's birthsday[DD-MM-YYYY]");

    if(fname != EMPTY_STRING && sname != EMPTY_STRING)
    {
        for(INT i=0; i<members.size(); ++i)
        {
             if(members[i].get_fname() == fname &&
                members[i].get_sname() == sname &&
                members[i].get_birth() == birth  )
            {
                open_members_table();
                database_carrier::instance().delete_entry(
                    store_carrier_member( store_carrier_member::MEMBER_ID, members[i].get_id()) );

                select_members_storage_file();
                file_carrier::instance().delete_entry( store_carrier_member
                        (store_carrier_member::MEMBER_ID, members[i].get_id()) );

                members.erase(members.begin()+i);
                OUTSTREAM<<endl<<"Record deleted!"<<endl;
                PRINTOUT;
                fresult = FUNCTION_SUCCESS;
                break;
            }
        }

        if(fresult == FUNCTION_FAILURE)
        {
            OUTSTREAM<<endl<<"Record not found!"<<endl;
            PRINTOUT;
        }
    }
    else
    {
        OUTSTREAM<<endl<<"Inserted data is insufficient!"<<endl;
        PRINTOUT;
    }

    timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
    return fresult;
}

FunctionSuccesFailure Library::delete_member_by_position()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    INT entryToDeleteIdx = get_selection_from_members();
    if(entryToDeleteIdx >= 0)
    {
        open_members_table();
        database_carrier::instance().delete_entry( store_carrier_member( store_carrier_member::MEMBER_ID,
            members[entryToDeleteIdx].get_id()) );

        select_members_storage_file();
        file_carrier::instance().delete_entry( store_carrier_member
                (store_carrier_member::MEMBER_ID, members[entryToDeleteIdx].get_id()) );

        members.erase(members.begin()+entryToDeleteIdx);
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure Library::find_book() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string title(UNAVAILABLE_TEXT), fauthor(UNAVAILABLE_TEXT), sauthor(UNAVAILABLE_TEXT);

    title   = get_user_text_inserted_by_guidance("Enter book's title");
    fauthor = get_user_text_inserted_by_guidance("Enter book's author name");
    sauthor = get_user_text_inserted_by_guidance("Enter book's author last name");

    string output = get_books_columns_name();
    if(title != EMPTY_STRING)
    {
        for(INT i=0, id=0; i<books_in_stock.size(); ++i)
        {
            BOOL isBookFound = FALSE;
            if(sauthor != EMPTY_STRING)
            {
                if(fauthor != EMPTY_STRING                          &&
                    books_in_stock[i].get_title() == title          &&
                    books_in_stock[i].get_author_sname() == sauthor &&
                    books_in_stock[i].get_author_fname() == fauthor  )
                {
                    isBookFound = TRUE;
                }
                else if(books_in_stock[i].get_title() == title          &&
                        books_in_stock[i].get_author_sname() == sauthor  )
                {
                    isBookFound = TRUE;
                }
                else
                {
                    //item not match to delete pattern, continue
                }
            }
            else if(fauthor != EMPTY_STRING)
            {
                if(books_in_stock[i].get_title().find(title) != string::npos &&
                    books_in_stock[i].get_author_fname() == fauthor )
                {
                    isBookFound = TRUE;
                }
            }
            else if(books_in_stock[i].get_title().find(title) != string::npos)
            {
                isBookFound = TRUE;
            }
            else
            {
                //item not match to delete pattern, continue
            }

            if(isBookFound == TRUE)
            {
                id++;
                output += get_books_row_content(i, id);
                fresult = FUNCTION_SUCCESS;
            }
        }
    }

    if(fresult == FUNCTION_SUCCESS) {
        console::settings().clear();
        OUTSTREAM<<endl<<"Listing of found books matches";
        PRINTOUT;
        print_data_table(output);
        get_user_text_inserted_by_guidance("Press enter to continue");
    }
    else
    {
        OUTSTREAM<<endl<<"No records found!"<<endl;
        PRINTOUT;
        timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
    }

    return fresult;
}

FunctionSuccesFailure Library::find_member() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string fname, sname, birth;

    fname = get_user_text_inserted_by_guidance("Enter member's name");
    sname = get_user_text_inserted_by_guidance("Enter member's last name");
    birth = get_user_text_inserted_by_guidance("Enter member's birthsday[DD-MM-YYYY]");

    string output = get_members_columns_name();
    if(sname != EMPTY_STRING)
    {
        for(INT i=0, id=0; i<members.size(); ++i)
        {
            BOOL isMemberFound = FALSE;
            if(fname != EMPTY_STRING)
            {
                if( members[i].get_fname() == fname &&
                    members[i].get_sname() == sname &&
                    members[i].get_birth() == birth  )
                {
                    isMemberFound = TRUE;
                }
            }
            else if(members[i].get_sname().find(sname) != string::npos)
            {
                isMemberFound = TRUE;
            }
            else
            {
                //item not match to delete pattern, continue
            }

            if(isMemberFound == TRUE)
            {
                id++;
                output += get_members_row_content(i, id);
                fresult = FUNCTION_SUCCESS;
            }
        }
    }

    if(fresult == FUNCTION_SUCCESS) {
        console::settings().clear();
        OUTSTREAM<<endl<<"Listing of found members matches";
        PRINTOUT;
        print_data_table(output);
        get_user_text_inserted_by_guidance("Press enter to continue");
    }
    else
    {
        OUTSTREAM<<endl<<"No records found!"<<endl;
        PRINTOUT;
        timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
    }

    return fresult;
}

FunctionSuccesFailure Library::find_borrower() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    vector<INT> shownLinePosition;
    string sname = get_user_text_inserted_by_guidance("Enter member's last name");

    string output = get_members_columns_name();
    if(sname != EMPTY_STRING)
    {
        for(INT i=0, id=0; i<members.size(); ++i)
        {
            if(members[i].get_sname().find(sname) != string::npos &&
               members[i].get_borrowed_books_amt() > 0             )
            {
                id++;
                shownLinePosition.push_back(i);
                output += get_members_row_content(i, id);
                fresult = FUNCTION_SUCCESS;
            }
        }
    }

    if(fresult == FUNCTION_SUCCESS) {
        console::settings().clear();
        OUTSTREAM<<endl<<"Listing of found members matches";
        PRINTOUT;
        print_data_table(output);

        INT shownLinePosIdx = get_user_sel_from_menu("Select the user. ");
        INT selectedUserIdx = shownLinePosition[shownLinePosIdx-1];
        OUTSTREAM<<"Books borrowed by: "<<members[selectedUserIdx].get_sname();
        PRINTOUT;
        get_view_from_books(members[selectedUserIdx].get_id());
    }
    else
    {
        OUTSTREAM<<endl<<"No records found!"<<endl;
        PRINTOUT;
        timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
    }

    return fresult;
}

FunctionSuccesFailure Library::add_lending()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    INT selectedUserIdx = get_selection_from_members();
    if(selectedUserIdx >= 0)
    {
        INT entryToBeLentIdx = get_selection_from_books(0);
        if(entryToBeLentIdx >= 0)
        {
            if(books_in_stock[entryToBeLentIdx].get_borrower_id() == 0)
            {
                books_in_stock[entryToBeLentIdx].set_as_lent(members[selectedUserIdx].get_id());
                members[selectedUserIdx].increment_borrowed_books();

                open_members_table();
                database_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEM_5,
                    members[selectedUserIdx].get_id(), members[selectedUserIdx].get_borrowed_books_amt()));

                select_members_storage_file();
                file_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEMS,
                    members[selectedUserIdx].get_id(), members[selectedUserIdx].get_fname(),
                    members[selectedUserIdx].get_sname(), members[selectedUserIdx].get_birth().date_to_string(),
                    members[selectedUserIdx].get_affiliation_date().date_to_string(),
                    members[selectedUserIdx].get_borrowed_books_amt()));

                open_books_table();
                database_carrier::instance().update_entry( store_carrier_member( store_carrier_member::MEMBER_ITEM_4,
                    books_in_stock[entryToBeLentIdx].get_id(),
                    books_in_stock[entryToBeLentIdx].get_lending_date().date_to_string()) );

                database_carrier::instance().update_entry( store_carrier_member( store_carrier_member::MEMBER_ITEM_5,
                    books_in_stock[entryToBeLentIdx].get_id(),
                    books_in_stock[entryToBeLentIdx].get_borrower_id()) );

                select_books_storage_file();
                file_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEMS,
                    books_in_stock[entryToBeLentIdx].get_id(), books_in_stock[entryToBeLentIdx].get_title(),
                    books_in_stock[entryToBeLentIdx].get_author_fname(), books_in_stock[entryToBeLentIdx].get_author_sname(),
                    books_in_stock[entryToBeLentIdx].get_lending_date().date_to_string(),
                    books_in_stock[entryToBeLentIdx].get_borrower_id()) );

                fresult = FUNCTION_SUCCESS;
            }
            else
            {
                OUTSTREAM<<endl<<"Book is already lent!";
                PRINTOUT;
                timers::instance().timeout_create(OPER_NOTIF_SHORT_MSDELAY);
            }
        }
    }

    return fresult;
}

FunctionSuccesFailure Library::release_lending()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    INT selectedUserIdx = get_selection_from_borrowers();
    if(selectedUserIdx >= 0)
    {
        INT entryToBeLentIdx = get_selection_from_books(members[selectedUserIdx].get_id());
        if(entryToBeLentIdx >= 0)
        {
            books_in_stock[entryToBeLentIdx].release_lent();
            members[selectedUserIdx].decrement_borrowed_books();

            open_members_table();
            database_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEM_5,
                members[selectedUserIdx].get_id(), members[selectedUserIdx].get_borrowed_books_amt()));

            select_members_storage_file();
            file_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEMS,
                members[selectedUserIdx].get_id(), members[selectedUserIdx].get_fname(), 
                members[selectedUserIdx].get_sname(), members[selectedUserIdx].get_birth().date_to_string(),
                members[selectedUserIdx].get_affiliation_date().date_to_string(),
                members[selectedUserIdx].get_borrowed_books_amt()));

            open_books_table();
            database_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEM_4,
                books_in_stock[entryToBeLentIdx].get_id(), Date().date_to_string()) );

            database_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEM_5,
                books_in_stock[entryToBeLentIdx].get_id(), 0) );

            select_books_storage_file();
            file_carrier::instance().update_entry( store_carrier_member(store_carrier_member::MEMBER_ITEMS,
                books_in_stock[entryToBeLentIdx].get_id(), books_in_stock[entryToBeLentIdx].get_title(),
                books_in_stock[entryToBeLentIdx].get_author_fname(), books_in_stock[entryToBeLentIdx].get_author_sname(),
                books_in_stock[entryToBeLentIdx].get_lending_date().date_to_string(), 0) );

            fresult = FUNCTION_SUCCESS;
        }
    }

    return fresult;
}

INT Library::get_selection_from_books(INT userID) const
{
    INT fresult = -1;
    INT line=0, linesShown=0, lineNb=0;
    static const INT linesPerView = 10;

    while(line < books_in_stock.size())
    {
        console::settings().clear();
        OUTSTREAM<<endl<<"Listing of all books";
        PRINTOUT;

        linesShown = 0;
        vector<INT> shownLinePosition;
        string whole_stock = get_books_columns_name();
        for(INT k=books_in_stock.size(), maxLines=line+linesPerView; line<k && line<maxLines; line++)
        {
            if( userID == 0 || userID == books_in_stock[line].get_borrower_id() )
            {
                lineNb++;
                linesShown++;
                shownLinePosition.push_back(line);
                whole_stock += get_books_row_content(line, lineNb);
            }
        }

        print_data_table(whole_stock);

        string viewCommand = get_user_text_inserted_by_guidance("Insert entry ID to be selected, "
                 "press ENTER to show next page or 'b'+ENTER to show previous page, 'c'+ENTER aborts listing");

        if(viewCommand == "c") {
            break;
        }
        else if(viewCommand == "b")
        {
            line -= linesShown;
            lineNb -= linesShown;

            ((line-linesPerView) >= 0) ? line-=linesPerView : line=0;
            ((lineNb-linesPerView) >= 0) ? lineNb-=linesPerView : lineNb=0;
        }
        else if(stringToDec(viewCommand) > 0)
        {
            INT entryToSelectIdx = shownLinePosition[(stringToDec(viewCommand)-1)%linesPerView];
            //TODO: insert assertion in here to verify if user inserted value is in range of books vector size
            string recordCommand = get_user_text_inserted_by_guidance("Do you want to select book: '"
                    +get_books_row_content(entryToSelectIdx, stringToDec(viewCommand))+"'? [Y/N/Any other to continue browsing]");

            if(recordCommand == "Y" || recordCommand == "y")
            {
                fresult = entryToSelectIdx;
                break;
            }
            else if(recordCommand == "N" || recordCommand == "n")
            {
                OUTSTREAM<<"Book selection aborted!";
                PRINTOUT;
                timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
                break;
            }
            else
            {
                OUTSTREAM<<"Book selection aborted!";
                PRINTOUT;
                line -= linesShown;
                lineNb -= linesShown;
                timers::instance().timeout_create(OPER_NOTIF_SHORT_MSDELAY);
            }
        }
    }

    return fresult;
}

INT Library::get_selection_from_members() const
{
    INT fresult = 0;
    INT line=0, linesShown=0;
    static const INT linesPerView = 10;

    while(line < members.size())
    {
        console::settings().clear();
        OUTSTREAM<<endl<<"Listing of all members";
        PRINTOUT;

        linesShown = 0;
        string whole_stock = get_members_columns_name();
        for(INT k=members.size(), maxLines=line+linesPerView; line<k && line<maxLines; line++)
        {
            whole_stock += get_members_row_content(line, line+1);
            linesShown++;
        }

        print_data_table(whole_stock);

        string viewCommand = get_user_text_inserted_by_guidance("Insert entry ID to be selected, "
                 "press ENTER to show next page or 'b'+ENTER to show previous page, 'c'+ENTER aborts listing");

        if(viewCommand == "c")
        {
            break;
        }
        else if(viewCommand == "b")
        {
            line -= linesShown;
            if( (line-linesPerView) >= 0 )
            {
                line -= linesPerView;
            }
        }
        else if(stringToDec(viewCommand) > 0)
        {
            INT entryToSelectIdx = stringToDec(viewCommand)-1;
            //TODO: insert assertion in here to verify if user inserted value is in range of books vector size
            string recordCommand = get_user_text_inserted_by_guidance("Do you want to select member: '"
                +get_members_row_content(entryToSelectIdx, entryToSelectIdx+1)+"'? [Y/N/Any other to continue browsing]");

            if(recordCommand == "Y" || recordCommand == "y")
            {
                fresult = entryToSelectIdx;
                break;
            }
            else if(recordCommand == "N" || recordCommand == "n")
            {
                OUTSTREAM<<"Book selection aborted!";
                PRINTOUT;
                timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
                break;
            }
            else
            {
                OUTSTREAM<<"Book selection aborted!";
                PRINTOUT;
                line -= linesShown;
                timers::instance().timeout_create(OPER_NOTIF_SHORT_MSDELAY);
            }
        }
    }

    return fresult;
}

INT Library::get_selection_from_borrowers() const
{
    INT fresult = -1;
    INT line=0, linesShown=0, lineNb=0, allLines=0;
    static const INT linesPerView = 10;

    while(line < members.size())
    {
        console::settings().clear();
        OUTSTREAM<<endl<<"Listing of all borrowers";
        PRINTOUT;

        linesShown = 0;
        vector<INT> shownLinePosition;
        string whole_stock = get_members_columns_name();
        for(INT k=members.size(), maxLines=line+linesPerView; line<k && linesShown<maxLines; line++)
        {
            if(members[line].get_borrowed_books_amt() > 0)
            {
                lineNb++;
                allLines++;
                linesShown++;
                shownLinePosition.push_back(line);
                whole_stock += get_members_row_content(line, lineNb);
            }
        }

        if(allLines != 0) {
            print_data_table(whole_stock);
            string viewCommand = get_user_text_inserted_by_guidance("Insert entry ID to be selected, "
                     "press ENTER to show next page or 'b'+ENTER to show previous page, 'c'+ENTER aborts listing");

            if(viewCommand == "c")
            {
                break;
            }
            else if(viewCommand == "b")
            {
                line -= linesShown;
                lineNb -= linesShown;
                if( (line-linesPerView) >= 0 )
                {
                    line -= linesPerView;
                    lineNb -= linesPerView;
                }
            }
            else if(stringToDec(viewCommand) > 0)
            {
                INT entryToSelectIdx = shownLinePosition[stringToDec(viewCommand)-1];
                //TODO: insert assertion in here to verify if user inserted value is in range of books vector size
                string recordCommand = get_user_text_inserted_by_guidance("Do you want to select borrower: '"
                    +get_members_row_content(entryToSelectIdx, stringToDec(viewCommand))+"'? [Y/N/Any other to continue browsing]");

                if(recordCommand == "Y" || recordCommand == "y")
                {
                    fresult = entryToSelectIdx;
                    break;
                }
                else if(recordCommand == "N" || recordCommand == "n")
                {
                    OUTSTREAM<<"Borrower selection aborted!";
                    PRINTOUT;
                    timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
                    break;
                }
                else
                {
                    OUTSTREAM<<"Borrower selection aborted!";
                    PRINTOUT;
                    line -= linesShown;
                    timers::instance().timeout_create(OPER_NOTIF_SHORT_MSDELAY);
                }
            }
        }
        else {
            OUTSTREAM<<endl<<"No borrower present!";
            PRINTOUT;
            timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
        }
    }

    return fresult;
}

FunctionSuccesFailure Library::get_view_from_books(INT userID) const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    INT line=0, linesShown=0, lineNb=0;
    static const INT linesPerView = 10;

    if(books_in_stock.size() > 0)
    {
        while(line < books_in_stock.size())
        {
            console::settings().clear();
            OUTSTREAM<<endl<<"Listing of all books";
            PRINTOUT;

            linesShown = 0;
            string whole_stock = get_books_columns_name();
            for(INT k=books_in_stock.size(), maxLines=line+linesPerView; line<k && line<maxLines; line++)
            {
                if(userID == 0 || userID == books_in_stock[line].get_borrower_id())
                {
                    lineNb++;
                    linesShown++;
                    whole_stock += get_books_row_content(line, lineNb);
                }
            }

            print_data_table(whole_stock);
            string viewCommand = get_user_text_inserted_by_guidance("Press ENTER to show next page or 'b'+ENTER to show previous page, "
                                                                    "'c'+ENTER aborts listing");
            if(viewCommand == "c")
            {
                break;
            }
            else if(viewCommand == "b")
            {
                line -= linesShown;
                lineNb -= linesShown;
                if( (line-linesPerView) >= 0 )
                {
                    line -= linesPerView;
                    lineNb -= linesPerView;
                }
            }
            else 
            {
                // no action defined
            }
        }

        fresult = FUNCTION_SUCCESS;
    }
    else
    {
        OUTSTREAM<<"List is empty!"<<endl;
        PRINTOUT;
        timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
    }

    return fresult;
}

FunctionSuccesFailure Library::get_view_from_members() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    INT line=0, linesShown=0;
    static const INT linesPerView = 10;

    if(members.size() > 0)
    {
        while(line < members.size())
        {
            console::settings().clear();
            OUTSTREAM<<endl<<"Listing of all members";
            PRINTOUT;

            linesShown = 0;
            string whole_stock = get_members_columns_name();
            for(INT k=members.size(), maxLines=line+linesPerView; line<k && line<maxLines; line++)
            {
                whole_stock += get_members_row_content(line, line+1);
                linesShown++;
            }

            print_data_table(whole_stock);
            string viewCommand = get_user_text_inserted_by_guidance("press ENTER to show next page or 'b'+ENTER to show previous page, "
                                                                    "'c'+ENTER aborts listing");
            if(viewCommand == "c")
            {
                break;
            }
            else if(viewCommand == "b")
            {
                line -= linesShown;
                if( (line-linesPerView) >= 0 )
                {
                    line -= linesPerView;
                }
            }
            else
            {
                // no action determined
            }
        }

        fresult = FUNCTION_SUCCESS;
    }
    else
    {
        OUTSTREAM<<"List is empty!"<<endl;
        PRINTOUT;
        timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
    }

    return fresult;
}

FunctionSuccesFailure Library::get_view_from_borrowers() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    INT line=0, linesShown=0, allLines=0;
    static const INT linesPerView=10;

    if(members.size() > 0)
    {
        while(line < members.size())
        {
            console::settings().clear();
            OUTSTREAM<<endl<<"Listing of all borrowers";
            PRINTOUT;

            linesShown = 0;
            string whole_stock = get_members_columns_name();
            for(INT i=1, k=members.size(), maxLines=line+linesPerView; line<k && linesShown<maxLines; line++)
            {
                if(members[line].get_borrowed_books_amt() > 0)
                {
                    whole_stock += get_members_row_content(line, i);
                    i++;
                    allLines++;
                    linesShown++;
                }
            }

            if(allLines != 0) {
                print_data_table(whole_stock);
                string viewCommand = get_user_text_inserted_by_guidance("Insert entry ID to be selected, "
                     "press ENTER to show next page or 'b'+ENTER to show previous page, 'c'+ENTER aborts listing");

                if(viewCommand == "c")
                {
                    break;
                }
                else if(viewCommand == "b")
                {
                    line -= linesShown;
                    if( (line-linesPerView) >= 0 )
                    {
                        line -= linesPerView;
                    }
                }
                else
                {
                    // no action determined
                }
            }
            else {
                OUTSTREAM<<endl<<"List is empty!"<<endl;
                PRINTOUT;
                timers::instance().timeout_create(OPER_NOTIF_MSDELAY);
            }
        }

        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

void Library::synchro_with_db()
{
    database_carrier::instance().initialize();

    open_books_table();
    select_books_storage_file();
    database_carrier::instance().synchronize_with_stream(file_carrier::instance().get_whole_content_and_header());

    open_members_table();
    select_members_storage_file();
    database_carrier::instance().synchronize_with_stream(file_carrier::instance().get_whole_content_and_header());

    for(INT p=LIB_CATEGORY_FIRST, r=LIB_CATEGORY_LAST; p<r; p++)
    {
        BOOL isEofReached = FALSE;
        stringstream inputStream;
        string line, member, item_1, item_2, item_3, item_4;
        INT id, item_5 = 0;

        if(p == LIB_CATEGORY_BOOKS)
        {
            open_books_table();
            inputStream = stringstream(database_carrier::instance().get_whole_content());
        }
        else if(p == LIB_CATEGORY_MEMBERS)
        {
            open_members_table();
            inputStream = stringstream(database_carrier::instance().get_whole_content());
        }

        while(isEofReached != TRUE)
        {
            line = EMPTY_STRING;
            isEofReached = getline(inputStream, line, NEW_LINE_TOKEN).eof();
            if(line != EMPTY_STRING)
            {
                item_1 = item_2 = item_3 = item_4 = EMPTY_STRING;
                item_5 = 0;

                line = store_carrier_member(line).getWholeRow();
                stringstream databaseStream(line);
                for(INT i=0, k=store_carrier_member::MEMBER_LAST-store_carrier_member::MEMBER_FIRST; i<k; ++i)
                {
                    member = EMPTY_STRING;
                    getline(databaseStream, member, LINE_FIELDS_DELIMITER);
                    stringstream memberStream(member);

                    switch(i+store_carrier_member::MEMBER_FIRST)
                    {
                        case store_carrier_member::MEMBER_ID:
                            if(isdigit(memberStream.str()[0]) != FALSE)
                            {
                                memberStream>>id;
                                break;
                            }
                            else
                            {
                                // fallthrough to next statement as ID is not provided
                                i++;
                            }
                        case store_carrier_member::MEMBER_ITEM_1:
                            item_1 = memberStream.str();
                            break;
                        case store_carrier_member::MEMBER_ITEM_2:
                            item_2 = memberStream.str();
                            break;
                        case store_carrier_member::MEMBER_ITEM_3:
                            item_3 = memberStream.str();
                            break;
                        case store_carrier_member::MEMBER_ITEM_4:
                            item_4 = memberStream.str();
                            break;
                        case store_carrier_member::MEMBER_ITEM_5:
                            memberStream>>item_5;
                            break;
                        default: { /*no suitable member */ }
                    }
                }

                if(p == LIB_CATEGORY_BOOKS)
                {
                    books_in_stock.push_back( Book(id, item_1, item_2, item_3, Date(item_4), item_5) );
                }
                else if(p == LIB_CATEGORY_MEMBERS)
                {
                    members.push_back( Patron(id, item_1, item_2, Date(item_3), Date(item_4), item_5) );
                }
                else
                {
                    // no action defined
                }
            }
        }
    }
}

FunctionSuccesFailure Library::list_books() const
{
    return get_view_from_books(0);
}

FunctionSuccesFailure Library::list_members() const
{
    return get_view_from_members();
}

FunctionSuccesFailure Library::list_borrowers() const
{
    return get_view_from_borrowers();
}

string Library::get_members_row_content(INT rowNb, INT rowID) const
{
    string output = decToString(rowID)+LINE_FIELDS_DELIMITER
                           +members[rowNb].get_fname()+LINE_FIELDS_DELIMITER
                           +members[rowNb].get_sname()+LINE_FIELDS_DELIMITER
                           +members[rowNb].get_birth().date_to_string()+LINE_FIELDS_DELIMITER
                           +members[rowNb].get_affiliation_date().date_to_string()+LINE_FIELDS_DELIMITER
                           +NEW_LINE_TOKEN;

    return output;
}

string Library::get_books_row_content(INT rowNb, INT rowID) const
{
    string output = decToString(rowID)+LINE_FIELDS_DELIMITER
                           +books_in_stock[rowNb].get_title()+LINE_FIELDS_DELIMITER
                           +books_in_stock[rowNb].get_author_fname()+LINE_FIELDS_DELIMITER
                           +books_in_stock[rowNb].get_author_sname()+LINE_FIELDS_DELIMITER
                           +books_in_stock[rowNb].get_lending_date().date_to_string()+LINE_FIELDS_DELIMITER
                           +NEW_LINE_TOKEN;

    return output;
}

string get_user_text_inserted_by_guidance(const string &guide)
{
    string fresult;
    static const UINT invDataNotifMsDelay = 2000;

    stream::instance().enter_input_section();
    while(1)
    {
        //console::settings().clear();
        OUTSTREAM<<guide<<endl<<PROMPT_PHRASE;
        PRINTOUT;
        if (stream::instance().get_text(fresult) == FUNCTION_SUCCESS) {
            break;
        }
        timers::instance().timeout_create(invDataNotifMsDelay);
    }
    stream::instance().leave_input_section();

    return fresult;
}

INT get_user_sel_from_menu(const string &menu)
{
    INT fresult = 0;
    static const UINT invDataNotifMsDelay = 2000;

    stream::instance().enter_input_section();
    while(1)
    {
        //console::settings().clear();
        OUTSTREAM<<menu<<PROMPT_PHRASE;
        PRINTOUT;
        if (stream::instance().get_int(fresult) == FUNCTION_SUCCESS) {
            break;
        }
        timers::instance().timeout_create(invDataNotifMsDelay);
    }
    stream::instance().leave_input_section();

    return fresult;
}

