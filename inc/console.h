/**
  ***************************************************************************************
  * @file    console.h
  * @author  £ukasz Kaümierczak
  * @version V1.2
  * @date    8/07/2013
  * @brief   This header file provides console operations set
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
#ifndef CONSOLE_H
#define CONSOLE_H

#include "common.h"

#define WIN_OLD     0
#define WIN_NEW     (!WIN_OLD)


class console {
public:
    static console &settings() { static console instance; return instance; }

    const enum Colors{ UndefColor=-1, Black, Blue, Green, Aqua, Red, Purple, Yellow, Silver, Gray,
                       LightBlue, LightGreen, LightAqua, LightRed, LightPurple, LightYellow, White, nb_of_colors };

    const enum Font{ FONT_1=0, FONT_2, FONT_3, FONT_DELIMITER, FONT_DEFAULT=FONT_1 };
    const enum Font_size{ FSIZE_1=0, FSIZE_2, FSIZE_3, FSIZE_4, FSIZE_5, FSIZE_DELIMITER, FSIZE_DEFAULT=FSIZE_3 };
    const enum Font_bold{ FBOLD_NONE=0, FBOLD_SET, FBOLD_DELIMITER, FBOLD_DEFAULT=FBOLD_NONE };

    VOID initialize();
    VOID setTitle(const std::string &new_title);
    VOID setTextColor(Colors screen);
    VOID setTextBckColor(Colors screen);
    VOID setScreenColor(Colors screen);
    VOID setFontType(Font type);
    VOID setFontSize(Font_size size);
    VOID setFontBold(Font_bold bold);
    VOID disableConsoleMenuBar();
    VOID fitScreenToWindow();
    VOID setConsoleEmpty();
    VOID disableEcho();
    VOID enableEcho();
    VOID clear();

private:
    console():currentFont(FONT_DEFAULT), currentFontSize(FSIZE_DEFAULT), currentFontBold(FBOLD_DEFAULT), originBckColor(UndefColor) {}
    console(const console&);
    console &operator=(const console&);

    Font currentFont;
    Font_size currentFontSize;
    Font_bold currentFontBold;
    Colors originBckColor;

#if WIN_OLD
    struct {
        ULONG    Length;
        COORD    ScreenBufferSize;
        COORD    WindowSize;
        ULONG    WindowPosX;
        ULONG    WindowPosY;
        COORD    FontSize;
        ULONG    FontFamily;
        ULONG    FontWeight;
        WCHAR    FaceName[32];
        ULONG    CursorSize;
        ULONG    FullScreen;
        ULONG    QuickEdit;
        ULONG    AutoPosition;
        ULONG    InsertMode;
        USHORT   TextColor   : 4;
        USHORT   ScreenColor : 4;
        USHORT               : 8;
        USHORT   PopupColors;
        ULONG    HistoryNoDup;
        ULONG    HistoryBufferSize;
        ULONG    NumberOfHistoryBuffers;
        COLORREF ColorTable[16];
        ULONG    CodePage;
        HWND     Hwnd;
        WCHAR    ConsoleTitle[0x100];
    }console_info;

    void console_info_store();
#endif

    VOID adjustFont();
};


#endif
