/**
  ***************************************************************************************
  * @file    interface.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.5
  * @date    26/07/2013
  * @brief   This source file provides support for 'library' interface operations
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
#include "interface.h"
#include "stream.h"
#include "console.h"
#include "process_mgmt.h"
#include "time_mgmt.h"
#include "libraries.h"

#define MOD_INTERFACE               "interface"

using namespace std;

#define ERROR_TEXT_MSDELAY          4000
#define MAIN_INTERFACE_TXT          "--------------------------------------------------\n" \
                                    "|        Library [ main  interface ] 2013        |\n" \
                                    "--------------------------------------------------\n"

#define USER_INTERFACE_TXT          "-------- Library [  member's zone  ] 2013 --------\n"
#define SERVICE_INTERFACE_TXT       "-------- Library [  service panel  ] 2013 --------\n"
#define BOOKS_INTERFACE_TXT         "-------- Library [   books panel   ] 2013 --------\n"
#define MEMBERS_INTERFACE_TXT       "-------- Library [  members panel  ] 2013 --------\n"
#define TRANSACTIONS_INTERFACE_TXT  "------ Library [  transactions panel  ] 2013 -----\n"

const enum menu_action{ menu_exit=0, menu_previous };

static VOID welcome_screen();
static VOID main_interface();
static menu_action user_interface();
static menu_action operator_interface();
static menu_action books_menu();
static menu_action book_delete_menu();
static menu_action members_menu();
static menu_action member_delete_menu();
static menu_action lendings_menu();
static FunctionSuccesFailure operator_authorization();

static DWORD WINAPI welcome_animation(LPVOID lpParam);
static INT get_user_sel_from_menu(const string &menu);


DWORD WINAPI welcome_animation(LPVOID lpParam)
{
#define WELCOME_ANIMATION_TXT      "|===========================================================|\n" \
                                   "                                                             \n" \
                                   "       WELCOME TO LIBRARY_2013© TERMINAL...[PRESS ENTER]     \n" \
                                   "                  "+get_current_time().full +                    \
                                   "  |  "           +get_today_date().full+  "                  \n" \
                                   "                                                             \n" \
                                   "|===========================================================|\n" 

    static const string libraryLogInTxt =  "|=================================|\n"
                                           "   LOGGING IN TO THE APPLICATION   \n"
                                           "|=================================|\n";

    static const INT animTxtSize              = 8;
    static const INT animUpdateTxtMsDelay     = 1000;
    static const INT animRepositionTxtMsDelay = 3000;
    static const string animPipeName = "\\\\.\\pipe\\aniamtionpipe";
    static const string process_name   = "animation.exe";
    static const string process_path   = "K:\\Private\\Work\\C++ Stroustrup course\\9\\HW\\animation\\animation\\Debug";
    static const string process_params = "anim -p " + animPipeName + " -s "+decToString(animTxtSize)
                                +" -u "+decToString(animUpdateTxtMsDelay)+" -rp "+decToString(animRepositionTxtMsDelay);

    processes::thread_ready();
    console::settings().clear();

    UINT_PTR hPipeNamedRaw = processes::pipe_named_create(TRUE, TRUE, NULL, animPipeName);

    UINT_PTR hAnimationProcessRaw = processes::process_create(process_name, process_path, process_params,
                                        processes::PROC_CONSOLE_DISABLED, processes::PROC_PRIORITY_NORMAL, NULL, NULL, NULL); 

    if(processes::pipe_named_client_wait(hPipeNamedRaw) == FUNCTION_SUCCESS) {
        stream::instance().system_message(MOD_INTERFACE, "pipe connected!");
    }

    if(hPipeNamedRaw != NULL) {
        static const INT pipeMsgUpdateFactor = animUpdateTxtMsDelay * 99.0/100;
        BOOL isAnimationProcessPending = TRUE;
        string msgBuffer;

        while(isAnimationProcessPending == TRUE) {
            do {
                if( processes::process_wait(hAnimationProcessRaw, 0) == FUNCTION_SUCCESS ||
                    msgBuffer == decToString(FUNCTION_FAILURE) )
                {
                    while(processes::process_wait(hAnimationProcessRaw, 0) == FUNCTION_FAILURE);
                    isAnimationProcessPending = FALSE;
                    break;
                }

                processes::pipe_named_read(hPipeNamedRaw, msgBuffer);
                if(msgBuffer == decToString(FUNCTION_FAILURE)) {
                    while(processes::pipe_named_write(hPipeNamedRaw, libraryLogInTxt) == FUNCTION_FAILURE);
                    stream::instance().system_message(MOD_INTERFACE, "program termianted!");
                    break;
                }
                else {
                    msgBuffer = "";
                }
            }while(processes::pipe_named_write(hPipeNamedRaw, WELCOME_ANIMATION_TXT) == FUNCTION_FAILURE);

            timers::instance().timeout_create(pipeMsgUpdateFactor);
        }
    }

    return EXIT_SUCCESS;
}

VOID library_interface()
{
    welcome_screen();
    main_interface();
}

VOID welcome_screen()
{
    UINT_PTR hWelcomeThdRaw = processes::thread_create(welcome_animation, 0, 0);
    while(processes::thread_wait(hWelcomeThdRaw, 100) == FUNCTION_FAILURE);
}

VOID main_interface()
{
static const string main_interface_menu = "Select assignment:\n"
                                          " 1. Employee\n"
                                          " 2. Member/Client\n"
                                          " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum position{undef=0, employee, user};

    while(1) {
        switch( position(get_user_sel_from_menu(MAIN_INTERFACE_TXT+main_interface_menu)) )
        {
            case employee:
                if(operator_authorization() == FUNCTION_SUCCESS)
                {
                    if(operator_interface()==menu_previous){
                        break;
                    }
                }
                return;
            case user:
                if(user_interface()==menu_previous) {
                    break;
                }
                return;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to your role");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }
}

menu_action operator_interface()
{
static const string service_interface_menu =  "Select section:\n"
                                              " 1. Books\n"
                                              " 2. Members\n"
                                              " 3. Lendings\n"
                                              " 0. Previous menu\n"
                                              " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum sections{up=0, books, members, lendings, };

    while(1) {
        switch( sections(get_user_sel_from_menu(SERVICE_INTERFACE_TXT+service_interface_menu)) )
        {
            case up:
                return menu_previous;
            case books:
                if(books_menu()==menu_previous) {
                    break;
                }
                return menu_exit;
            case members:
                if(members_menu()==menu_previous) {
                    break;
                }
                return menu_exit;
            case lendings:
                if(lendings_menu()==menu_previous) {
                    break;
                }
                return menu_exit;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to proper section");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }  
}

menu_action books_menu()
{
    static const string books_interface_menu = "Select operation:\n"
                                                 " 1. Add book\n"
                                                 " 2. Delete book\n"
                                                 " 3. Find book\n"
                                                 " 4. List all books\n"
                                                 " 0. Previous menu\n"
                                                 " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum search{up=0, add_book, delete_book, find_book, list_books,};

    while(1) {
        switch( search(get_user_sel_from_menu(BOOKS_INTERFACE_TXT+books_interface_menu)) )
        {
            case up:
                return menu_previous;
            case add_book:
            {
                Library::instance().add_book();
                break;
            }
            case delete_book:
                if(book_delete_menu()==menu_previous) {
                    break;
                }
                return menu_exit;
            case find_book:
            {
                Library::instance().find_book();
                break;
                return menu_exit;
            }
            case list_books:
                Library::instance().list_books();
                break;
                return menu_exit;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to search method");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }
    return menu_exit;
}

menu_action book_delete_menu()
{
    static const string delete_interface_menu = "Select delete mode:\n"
                                                " 1. By entering data\n"
                                                " 2. By selecting position\n"
                                                " 0. Previous menu\n"
                                                " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum sections{up=0, delete_by_data, delete_by_position, };

    while(1) {
        switch( sections(get_user_sel_from_menu(BOOKS_INTERFACE_TXT+delete_interface_menu)) )
        {
            case up:
                return menu_previous;
            case delete_by_data:
                if(Library::instance().delete_book_by_data() != FUNCTION_SUCCESS) {
                    break;
                }
                return menu_previous;
            case delete_by_position:
                if(Library::instance().delete_book_by_position() != FUNCTION_SUCCESS) {
                    break;
                }
                return menu_previous;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to proper section");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }  
}

menu_action members_menu()
{
    static const string members_interface_menu = "Select operation:\n"
                                                 " 1. Add member\n"
                                                 " 2. Delete member\n"
                                                 " 3. Find member\n"
                                                 " 4. List all members\n"
                                                 " 0. Previous menu\n"
                                                 " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum search{up=0, add_member, delete_member, find_member, list_members,};

    while(1) {
        switch( search(get_user_sel_from_menu(MEMBERS_INTERFACE_TXT+members_interface_menu)) )
        {
            case up:
                return menu_previous;
            case add_member:
            {
                Library::instance().add_member();
                break;
            }
            case delete_member:
            {
                if(member_delete_menu()==menu_previous) {
                    break;
                }
                return menu_exit;
            }
            case find_member:
                Library::instance().find_member();
                break;
            case list_members:
                Library::instance().list_members();
                break;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to search method");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }

    return menu_exit;
}

menu_action member_delete_menu()
{
    static const string delete_interface_menu = "Select delete mode:\n"
                                                " 1. By entering data\n"
                                                " 2. By selecting position\n"
                                                " 0. Previous menu\n"
                                                " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum sections{up=0, delete_by_data, delete_by_position, };

    while(1) {
        switch( sections(get_user_sel_from_menu(MEMBERS_INTERFACE_TXT+delete_interface_menu)) )
        {
            case up:
                return menu_previous;
            case delete_by_data:
                if(Library::instance().delete_member_by_data() != FUNCTION_SUCCESS)
                {
                    break;
                }
                return menu_previous;
            case delete_by_position:
                if(Library::instance().delete_member_by_position() != FUNCTION_SUCCESS)
                {
                    break;
                }
                return menu_previous;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to proper section");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }
}


menu_action lendings_menu()
{
    static const string lendings_interface_menu = "Select operation:\n"
                                                 " 1. Add borrowed book\n"
                                                 " 2. Release borrowed book\n"
                                                 " 3. Find borrower\n"
                                                 " 4. List all borrowers\n"
                                                 " 0. Previous menu\n"
                                                 " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum sections{up=0, lending_add_book, lending_release_book, lending_find_borrower, lending_show_borrowers };

    while(1) {
        switch( sections(get_user_sel_from_menu(TRANSACTIONS_INTERFACE_TXT+lendings_interface_menu)) )
        {
            case up:
                return menu_previous;
            case lending_add_book:
                Library::instance().add_lending();
                break;
                return menu_previous;
            case lending_release_book:
                Library::instance().release_lending();
                break;
                return menu_previous;
            case lending_find_borrower:
                Library::instance().find_borrower();
                break;
                return menu_previous;
            case lending_show_borrowers:
                Library::instance().list_borrowers();
                break;
                return menu_previous;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to proper section");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }

    return menu_exit;
}

menu_action user_interface()
{
    static const string user_interface_menu = "Select operation:\n"
                                              " 1. Find book\n"
                                              " 2. List all books\n"
                                              " 0. Previous menu\n"
                                              " Logout: press '"+string(1,QUIT_PROCCESS)+"'\n";

    enum user_search{up=0, find_book, list_books, };

    while(1) {
        switch( user_search(get_user_sel_from_menu(USER_INTERFACE_TXT+user_interface_menu)) )
        {
            case up:
                return menu_previous;
            case find_book:
                Library::instance().find_book();
                break;
                return menu_exit;
            case list_books:
                Library::instance().list_books();
                break;
                return menu_exit;
            default:
                stream::instance().error_message(MOD_INTERFACE, "Unsupported option! Select number corresponding to search method");
                timers::instance().timeout_create(ERROR_TEXT_MSDELAY);
        }
    }
}

INT get_user_sel_from_menu(const string &menu)
{
    INT fresult = 0;
    static const UINT invDataNotifMsDelay = 2000;

    stream::instance().enter_input_section();
    while(1)
    {
        console::settings().clear();
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

FunctionSuccesFailure operator_authorization()
{
    static const string operator_password = "qwerty";
    static const UINT incorrPassNotifMsDelay = 2000;

    stream::instance().enter_input_section();

    string authorization_key;
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    OUTSTREAM<<endl<<"please insert operator password: ";
    PRINTOUT;
    console::settings().disableEcho();
    stream::instance().get_text(authorization_key);
    console::settings().enableEcho();

    if(authorization_key == operator_password)
    {
        fresult = FUNCTION_SUCCESS;
    }
    else
    {
        stream::instance().error_message(MOD_INTERFACE, "Password incorrect. Aborting!");
        timers::instance().timeout_create(incorrPassNotifMsDelay);
    }

    stream::instance().leave_input_section();

    return fresult;
}

