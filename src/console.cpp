/**
  ***************************************************************************************
  * @file    console.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.2
  * @date    8/07/2013
  * @brief   This source file provides support for console operations
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
#include "console.h"
#include <Windows.h>
#include <vector>

#include "stream.h"

#define MOD_CONSOLE               "console"

using namespace std;

#define WM_SETCONSOLEINFO         (WM_USER+201)

static HANDLE getConsoleInputHandle();
static HANDLE getConsoleOutputHandle();
inline WORD colorGetScreenBackground(WORD attribute) { return (attribute & 0xF0)>>4; }
inline WORD colorChangeScreenBackground(WORD attribute, WORD newColor) { return (attribute & 0xFF0F)|newColor<<4; }
inline WORD colorGetText(WORD attribute) { return (attribute & 0x0F); }
inline WORD colorChangeText(WORD attribute, WORD newColor) { return (attribute & 0xFFF0)|newColor; }


class fonts {
public:
    static fonts &instance() { static fonts finstance; return finstance; }
    FunctionSuccesFailure font_add(string name, UINT boldWeight, SHORT sizeX, SHORT sizeY);
    FunctionSuccesFailure font_set_default(string name) { return font_set_first(name); }

    string font_get_name(console::Font fontRaw) const;
    COORD font_get_size(console::Font fontRaw, console::Font_size sizeRaw) const;
    UINT font_get_bold(console::Font fontRaw, console::Font_bold boldType) const;
    VOID list_all() const;

private:
    fonts() {}
    fonts(const fonts&);
    fonts &operator=(const fonts&);

    BOOL isFont() const;
    FunctionSuccesFailure font_add_new() { fonts_vect.push_back(font); return FUNCTION_SUCCESS; }
    FunctionSuccesFailure font_add_existed();
    FunctionSuccesFailure font_set_first(string name);

    struct font_info {
        font_info(): name(UNNAMED), boldWeight(0) {}
        font_info(string name, UINT boldWeight, SHORT sizeX, SHORT sizeY):
            name(name), boldWeight(boldWeight), size(sizeX, sizeY) { size_vect.push_back(size); }

        string name;
        UINT   boldWeight;

        BOOL isSize(SHORT X, SHORT Y) const;
        FunctionSuccesFailure putSize(SHORT X, SHORT Y);

        struct font_size {
            font_size(): dimX(0), dimY(0) {}
            font_size(SHORT X, SHORT Y): dimX(X), dimY(Y) {}

            SHORT dimX;
            SHORT dimY;
        }size;

        vector<font_size> size_vect;
    }font;

    vector<font_info> fonts_vect;
};

VOID fonts::list_all() const
{
    OUTSTREAM<<"Nb of fonts: "<<fonts_vect.size()<<endl;

    for(size_t i=0, k=fonts_vect.size(); i<k; ++i) {
        OUTSTREAM<<"Font: "<<fonts_vect[i].name<<" boldWeight: "<<fonts_vect[i].boldWeight
                 <<" size of size_vect: "<<fonts_vect[i].size_vect.size()<<endl;
        for(size_t j=0, p=fonts_vect[i].size_vect.size(); j<p; ++j)
            OUTSTREAM<<"size x: "<<fonts_vect[i].size_vect[j].dimX<<" y: "<<fonts_vect[i].size_vect[j].dimY<<endl;
    }

    PRINTOUT;
}

FunctionSuccesFailure fonts::font_add(string name, UINT boldWeight, SHORT sizeX, SHORT sizeY)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    font = font_info(name, boldWeight, sizeX, sizeY);
    if(isFont() == FALSE) {
        if( console::FONT_DELIMITER > fonts_vect.size() ) {
            fresult = font_add_new();
        }
        else {
            stream::instance().error_message(MOD_CONSOLE, "Fonts amount limit reached - cannot add more new fonts("+name+")!");
        }
    }
    else {
        fresult = font_add_existed();
    }

    return fresult;
}

string fonts::font_get_name(console::Font fontRaw) const
{ 
    if(fonts_vect.size() > (UINT)fontRaw) {
        return fonts_vect[fontRaw].name; 
    }

    return UNNAMED;
}

COORD fonts::font_get_size(console::Font fontRaw, console::Font_size sizeRaw) const
{
    COORD size = {0};
    if(fonts_vect.size() > (UINT)fontRaw) {
        size_t sizeVectLimit = fonts_vect[fontRaw].size_vect.size();
        if(sizeVectLimit > (UINT)sizeRaw) {
            size.X = fonts_vect[fontRaw].size_vect[sizeRaw].dimX;
            size.Y = fonts_vect[fontRaw].size_vect[sizeRaw].dimY;
        }
        else {
            size.X = fonts_vect[fontRaw].size_vect[sizeVectLimit-1].dimX;
            size.Y = fonts_vect[fontRaw].size_vect[sizeVectLimit-1].dimY;
        }
    }

    return size;
}

UINT fonts::font_get_bold(console::Font fontRaw, console::Font_bold boldType) const
{
    UINT boldWeight = 0;
    if(fonts_vect.size() > (UINT)fontRaw) {
        if(boldType == console::FBOLD_SET) {
            boldWeight = fonts_vect[fontRaw].boldWeight;
        }
    }

    return boldWeight;
}

BOOL fonts::isFont() const
{
    for(size_t i=0, k=fonts_vect.size(); i<k; ++i) {
        if(fonts_vect[i].name == font.name) {
            return TRUE;
        }
    }

    return FALSE;
}

FunctionSuccesFailure fonts::font_add_existed()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    for(size_t i=0, k=fonts_vect.size(); i<k; i++) {
        if(fonts_vect[i].name == font.name) {
            fresult = fonts_vect[i].putSize(font.size.dimX, font.size.dimY);
            break;
        }
    }

    return fresult;
}

FunctionSuccesFailure fonts::font_set_first(string name)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    for(size_t i=0, k=fonts_vect.size(); i<k; i++) {
        if(fonts_vect[i].name == name) {
            font = fonts_vect[i];
            fonts_vect.erase(fonts_vect.begin()+i);
            fonts_vect.insert(fonts_vect.begin(), font);
            fresult = FUNCTION_SUCCESS;
            break;
        }
    }

    return fresult;
}

BOOL fonts::font_info::isSize(SHORT X, SHORT Y) const
{
    BOOL fresult = FALSE;

    for(size_t i=0, k=size_vect.size(); i<k; i++) {
        if(size_vect[i].dimX == X && size_vect[i].dimY == Y) {
            fresult = TRUE;
        }
    }

    return fresult;
}

FunctionSuccesFailure fonts::font_info::putSize(SHORT X, SHORT Y)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if( isSize(X, Y) == FALSE) {
        if( console::FSIZE_DELIMITER > size_vect.size() ) {
            for(size_t i=0, k=size_vect.size(); i<k; i++) {
                if(size_vect[i].dimY > Y) {
                    size_vect.insert(size_vect.begin()+i, font_size(X, Y));
                    break;
                }
                else if(size_vect[i].dimY == Y && size_vect[i].dimX > X) {
                    size_vect.insert(size_vect.begin()+i, font_size(X, Y));
                    break;
                }
                else if(i == (k-1)) {
                    size_vect.push_back(font_size(X, Y));
                }
            }

            fresult = FUNCTION_SUCCESS;
        }
        else {
            stream::instance().error_message(MOD_CONSOLE, "Size entries limit reached for font:"+name+" - cannot add more size positions!");
        }
    }

    return fresult;
}


COLORREF get_color24b(console::Colors const color)
{
    static const COLORREF defalut_colors[16] =
    {
        0x00000000, 0x00800000, 0x00008000, 0x00808000,
        0x00000080, 0x00800080, 0x00008080, 0x00c0c0c0,
        0x00808080, 0x00ff0000, 0x0000ff00, 0x00ffff00,
        0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff
    };

    return defalut_colors[color];
}

#if WIN_OLD
VOID console::console_info_store()
{
    DWORD  dwConsoleOwnerPid;
    HANDLE hProcess, hSection, hDupSection, hThread;
    PVOID  ptrView = NULL;

    GetWindowThreadProcessId(console_info.Hwnd, &dwConsoleOwnerPid);

    hProcess = OpenProcess(MAXIMUM_ALLOWED, FALSE, dwConsoleOwnerPid);
    hSection = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, console_info.Length, 0);
    ptrView  = MapViewOfFile(hSection, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, console_info.Length);

    memcpy(ptrView, &console_info, console_info.Length);
    UnmapViewOfFile(ptrView);

    DuplicateHandle(GetCurrentProcess(), hSection, hProcess, &hDupSection, 0, FALSE, DUPLICATE_SAME_ACCESS);
    SendMessage(console_info.Hwnd, WM_SETCONSOLEINFO, (WPARAM)hDupSection, 0);
    CloseHandle(hSection);

    hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)CloseHandle, hDupSection, 0, 0);
    CloseHandle(hThread);
    CloseHandle(hProcess);
}
#endif

VOID console::initialize()
{
    const string consoleTitle = "Library_2013©";

    fonts::instance().font_add("Lucida Console", 700, 16,16);
    fonts::instance().font_add("Lucida Console", 700, 12,12);
    fonts::instance().font_add("Lucida Console", 700, 10,10);
    fonts::instance().font_add("Lucida Console", 700, 20,20);
    fonts::instance().font_add("Lucida Console", 700, 24,24);

    fonts::instance().font_add("Consolas", 700, 16,16);
    fonts::instance().font_add("Consolas", 700, 12,12);
    fonts::instance().font_add("Consolas", 700, 18,18);
    fonts::instance().font_add("Consolas", 700, 24,24);
    fonts::instance().font_add("Consolas", 700, 22,22);

    fonts::instance().font_add("Default", 0, 8,8);
    fonts::instance().font_add("Default", 0, 12,16);
    fonts::instance().font_add("Default", 0, 5,12);
    fonts::instance().font_add("Default", 0, 8,12);
    fonts::instance().font_add("Default", 0, 6,8);
    fonts::instance().font_set_default("Default");

    //fonts::instance().list_all();

#if WIN_OLD
    COORD Size     = GetLargestConsoleWindowSize(hConsoleOutput);

    memset(&console_info, 0, sizeof(console_info));
    console_info.Hwnd             = GetConsoleWindow();

    console_info.ScreenBufferSize = Size;
    console_info.WindowSize.X     = Size.X - 1;
    console_info.WindowSize.Y     = Size.Y - 1;
    console_info.WindowPosX       = 0;
    console_info.WindowPosY       = 0;
    console_info.FontSize.X       = 12;
    console_info.FontSize.Y       = 12;
    console_info.FontFamily       = 0;
    console_info.FontWeight       = 0;
    swprintf_s(console_info.FaceName, L"Lucida Console");

    //swprintf(console_info.FaceName, 32, L"%hs", "Lucida Console");
    console_info.CursorSize       = 1;
    console_info.FullScreen       = FALSE;
    console_info.QuickEdit        = FALSE;
    console_info.InsertMode       = FALSE;
    console_info.AutoPosition     = 0x0;
    console_info.TextColor        = console::White;
    console_info.ScreenColor      = console::Black;
    console_info.PopupColors      = MAKEWORD(0x0, 0x0);
    console_info.CodePage         = 0;
    console_info.Length           = sizeof(console_info);
    console_info.HistoryNoDup     = FALSE;
    console_info.HistoryBufferSize      = 0;
    console_info.NumberOfHistoryBuffers = 0;
    swprintf(console_info.ConsoleTitle, 0x100, L"%hs", consoleTitle);
    for(int i = 0; i < 16; i++) {
        console_info.ColorTable[i] = get_color24b(console::Colors(i));
    }

    console_info_store();
    console::settings().setTitle( wcToString(console_info.ConsoleTitle) );

#elif WIN_NEW
    setFontType(FONT_3);
    setFontSize(FSIZE_4);
#endif

    fitScreenToWindow();
    setTitle(consoleTitle);
    setScreenColor(White);
    setTextColor(Black);

    disableConsoleMenuBar();
}

VOID console::setTitle(const string &new_title)
{
    SetConsoleTitle(stringToWc(new_title).c_str());

#if WIN_OLD
    lstrcpynW(console_info.ConsoleTitle, stringToWc(new_title).c_str(), 0x100);
#endif
}

VOID console::setScreenColor(Colors new_color)
{
    HANDLE hConsoleOutput = getConsoleOutputHandle();
    COORD coords = {0};
    COORD coordRange = {1,1};
    COORD coordStart = {0,0};
    CHAR_INFO  currentChar  = {0};
    SMALL_RECT srctReadRect = {0};
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};

    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    WORD color = colorChangeScreenBackground(csbi.wAttributes, new_color);
    WORD currentBckColor = colorGetScreenBackground(csbi.wAttributes);

    if(originBckColor != UndefColor) {
        currentBckColor = originBckColor;
    }

    for(coords.Y=0; coords.Y < csbi.dwSize.Y; coords.Y++) {
        for(coords.X=0; coords.X < csbi.dwSize.X; coords.X++) {
            srctReadRect.Top  = coords.Y;
            srctReadRect.Left = coords.X;
            srctReadRect.Bottom = srctReadRect.Top+1;
            srctReadRect.Right  = srctReadRect.Left+1;
            ReadConsoleOutput(hConsoleOutput, &currentChar, coordRange, coordStart, &srctReadRect );

            if(colorGetScreenBackground(currentChar.Attributes) == currentBckColor) {
                DWORD dwWritten = 0;
                color = colorChangeText(color, colorGetText(currentChar.Attributes));
                WriteConsoleOutputAttribute( hConsoleOutput, &color, 1, coords, &dwWritten );
            }
        }
    }

    setTextBckColor(new_color);
    originBckColor = UndefColor;

    /*
    HANDLE hConsoleOutput = getConsoleOutputHandle();
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
    COORD coordScreen = {0};
    DWORD cCharsWritten = 0;

    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    DWORD dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    WORD color = (csbi.wAttributes & SCREEN_COLOR_FILTER) | new_color<<4;
    FillConsoleOutputAttribute(hConsoleOutput, color, dwConSize, coordScreen, &cCharsWritten);
    SetConsoleTextAttribute(hConsoleOutput, color);
    */

#if WIN_OLD
    console_info.ScreenColor = new_color;
#endif
}

VOID console::setTextColor(Colors new_color)
{
    HANDLE hConsoleOutput = getConsoleOutputHandle();
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};

    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    WORD color = colorChangeText(csbi.wAttributes, new_color);
    SetConsoleTextAttribute(hConsoleOutput, color);

#if WIN_OLD
    console_info.TextColor = new_color;
#endif
}

VOID console::setTextBckColor(Colors new_color)
{
    HANDLE hConsoleOutput = getConsoleOutputHandle();
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};

    GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
    WORD color = colorChangeScreenBackground(csbi.wAttributes, new_color);
    SetConsoleTextAttribute(hConsoleOutput, color);

    if(originBckColor == UndefColor) {
        originBckColor = (Colors)colorGetScreenBackground(csbi.wAttributes);
    }
}

VOID console::setFontType(Font type)
{
    currentFont = type;
    currentFontSize = FSIZE_DEFAULT;
    currentFontBold = FBOLD_DEFAULT;
    adjustFont();
}

VOID console::setFontSize(Font_size size)
{
    currentFontSize = size;
    adjustFont();
}

VOID console::setFontBold(Font_bold bold)
{
    currentFontBold = bold;
    adjustFont();
}

VOID console::setConsoleEmpty()
{
    HANDLE hConsole = getConsoleOutputHandle();
    CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
    COORD cursorPosition = {0};
    DWORD nbWrittenChars = 0;
    static const CHAR charToFill = ' ';

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    FillConsoleOutputCharacter(hConsole, charToFill, csbi.dwSize.X*csbi.dwSize.Y, cursorPosition, &nbWrittenChars);
    SetConsoleCursorPosition(hConsole, cursorPosition);
}

VOID console::disableEcho()
{
    DWORD mode;
    HANDLE hConsoleInput = getConsoleInputHandle();

    GetConsoleMode(hConsoleInput, &mode);
    mode &= (DWORD)(~ENABLE_ECHO_INPUT);
    SetConsoleMode(hConsoleInput, mode);
}

VOID console::enableEcho()
{
    DWORD mode;
    HANDLE hConsoleInput = getConsoleInputHandle();

    GetConsoleMode(hConsoleInput, &mode);
    mode |= ENABLE_ECHO_INPUT;
    SetConsoleMode(hConsoleInput, mode);
}

VOID console::adjustFont()
{
    wstring fontName = stringToWc(fonts::instance().font_get_name(currentFont));
    COORD fontSize = fonts::instance().font_get_size(currentFont, currentFontSize);
    UINT fontBold = fonts::instance().font_get_bold(currentFont, currentFontBold);

    HANDLE hConsoleOutput = getConsoleOutputHandle();
    CONSOLE_FONT_INFOEX console_info = {0};
    GetCurrentConsoleFontEx(hConsoleOutput, FALSE, &console_info);

    console_info.cbSize = sizeof(console_info);
    console_info.dwFontSize.X = fontSize.X;
    console_info.dwFontSize.Y = fontSize.Y;

    console_info.FontWeight = fontBold;
    copyWCharString(console_info.FaceName, LF_FACESIZE-SPACE_FOR_NULL, fontName.c_str());

    SetCurrentConsoleFontEx(hConsoleOutput, FALSE, &console_info);
    fitScreenToWindow();
}

VOID console::fitScreenToWindow()
{
    static const UINT fitScreenMaxRetryCounter = 1000U;
    UINT fitScreenRetryCounter = 0;
    HANDLE hConsoleOutput = getConsoleOutputHandle();
    RECT Display = {0};

    GetWindowRect(GetDesktopWindow(), &Display);
    MoveWindow(GetConsoleWindow(), 0, 0, Display.right-Display.left, Display.bottom-Display.top, TRUE);

    while(1) {
        COORD Size = GetLargestConsoleWindowSize(hConsoleOutput);
        SMALL_RECT DisplayArea = {0};
        DisplayArea.Right  = Size.X-1;
        DisplayArea.Bottom = Size.Y-1;
        DisplayArea.Top  = 0;
        DisplayArea.Left = 0;

        SetConsoleScreenBufferSize(hConsoleOutput, Size);
        SetConsoleWindowInfo(hConsoleOutput, FALSE, &DisplayArea);

        CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
        GetConsoleScreenBufferInfo(hConsoleOutput, &csbi);
        ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);
        ShowWindow(GetConsoleWindow(), SW_SHOWMAXIMIZED);

        if( (Size.X == csbi.dwSize.X && Size.Y == csbi.dwSize.Y) || (++fitScreenRetryCounter) >= fitScreenMaxRetryCounter) {
            break;
        }
    }

#if WIN_OLD
   console_info.ScreenBufferSize = Size;
   console_info.WindowPosX   = DisplayArea.Left;
   console_info.WindowPosY   = DisplayArea.Top;
   console_info.WindowSize.X = DisplayArea.Right;
   console_info.WindowSize.Y = DisplayArea.Bottom;
#endif
}

VOID console::disableConsoleMenuBar()
{
    HMENU hMenu = GetSystemMenu(GetConsoleWindow(), FALSE);

    DeleteMenu(hMenu, SC_MINIMIZE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_MOVE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_RESTORE, MF_BYCOMMAND);
    DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND);
    DrawMenuBar(GetConsoleWindow());
}

VOID console::clear()
{
#if WIN_OS
    system("cls");
#elif UNIX
    system("clear");
#endif
}

HANDLE getConsoleOutputHandle()
{
    static HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    return hConsoleOutput;
}

HANDLE getConsoleInputHandle()
{
    static HANDLE hConsoleOutput = GetStdHandle(STD_INPUT_HANDLE);
    return hConsoleOutput;
}

