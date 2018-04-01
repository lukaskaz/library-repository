/**
  ***************************************************************************************
  * @file    storage_mgmt.h
  * @author  £ukasz Kaümierczak
  * @version V1.2
  * @date    20/07/2013
  * @brief   This header file provides storage operations set for file/sql
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
#ifndef STORAGE_MGMT_H
#define STORAGE_MGMT_H

#include "common.h"

#define NEW_LINE_TOKEN                 '\n'
#define LINE_FIELDS_DELIMITER          ';'


class store_carrier_member {
public:
    enum member_type { MEMBER_INVALID, MEMBER_ALL, MEMBER_ITEMS, MEMBER_FIRST, MEMBER_ID=MEMBER_FIRST,
                        MEMBER_ITEM_1, MEMBER_ITEM_2, MEMBER_ITEM_3, MEMBER_ITEM_4, MEMBER_ITEM_5, MEMBER_LAST };

    store_carrier_member();
    store_carrier_member(const member_type, const std::string &, const std::string &,
                                    const std::string &, const std::string &, const INT);
    store_carrier_member(const member_type, const INT, const std::string &, const std::string &,
                                    const std::string &, const std::string &, const INT);
    store_carrier_member(const member_type, const INT);
    store_carrier_member(const member_type, const INT, const std::string &);
    store_carrier_member(const member_type, const INT, const INT);
    store_carrier_member(const std::string &);

    static const CHAR *columnName[];

    static VOID reset_uid()    { uid=0; }
    static UINT generate_uid() { uid++; return uid; }
    static std::string getColumnRow();
    static std::string getColumnName(member_type );

    std::string getWholeRow()       const;
    std::string getRowWOId()        const;
    member_type get_kind()          const { return kind;   }
    UINT get_id()                   const { return id;     }
    const std::string &get_item_1() const { return item_1; }
    const std::string &get_item_2() const { return item_2; }
    const std::string &get_item_3() const { return item_3; }
    const std::string &get_item_4() const { return item_4; }
    const INT get_item_5()          const { return item_5; }

private:
    static INT  uid;
    INT id;
    member_type kind;
    std::string item_1;
    std::string item_2;
    std::string item_3;
    std::string item_4;
    INT item_5;

    VOID decomposeToMembers(const std::string &data);
};

class database_carrier {
public:
    static database_carrier &instance() { static database_carrier db_carrier_instance; return db_carrier_instance; }

    FunctionSuccesFailure initialize();
    VOID close();

    INT get_last_insert_id();
    FunctionSuccesFailure select_table(const std::string &name);
    FunctionSuccesFailure remove_table(const std::string &name);
    FunctionSuccesFailure add_entry(store_carrier_member &member);
    FunctionSuccesFailure update_entry(store_carrier_member &members);
    FunctionSuccesFailure delete_entry(store_carrier_member &members);

    std::string get_whole_content()                                          const;
    FunctionSuccesFailure list_all()                                         const;
    FunctionSuccesFailure get_param_list(const store_carrier_member &params) const;
    std::string get_entries_default_sorted(const std::string &data)          const;

    FunctionSuccesFailure synchronize_with_stream(const std::string &input);

private:
    database_carrier(): connectionRaw(NULL), selectedTable(UNNAMED) {}
    database_carrier(const database_carrier &);
    database_carrier &operator=(const database_carrier);

    UINT connectionRaw;
    std::string selectedTable;

    static const struct db{
        static const CHAR *server;
        static const CHAR *database;
        static const CHAR *user;
        static const CHAR *password;
    }credentials;

    FunctionSuccesFailure add_table(const std::string &name);

    BOOL is_table_opened(const std::string &name)                 const;
    BOOL is_member_present(store_carrier_member &member)          const;
    std::string get_column_headers_stream(UINT_PTR resultRaw)     const;
    std::string get_entries_stream(UINT_PTR resultRaw)            const;
    CHAR *get_column_name(UINT_PTR resultRaw, INT columnNb)       const;
    VOID set_auto_increment(INT initValue)                        const;
    VOID remove_all_entries_from_table()                          const;
    VOID notify_error(const std::string &module)                  const;
};

class file_carrier {
    enum operationType { FILE_INVALID=0, FILE_INPUT, FILE_OUTPUT_APP, FILE_OUTPUT_TRUNC, FILE_OUTPUT_UPDATE };

public:
    static file_carrier &instance() { static file_carrier file_carrier_instance; return file_carrier_instance; }

    BOOL is_empty();
    std::string get_whole_content();
    FunctionSuccesFailure list_all();
    std::string file_carrier::get_whole_content_and_header();
    FunctionSuccesFailure add_entry(store_carrier_member &member);
    FunctionSuccesFailure set_storage_file_name(const std::string &);
    FunctionSuccesFailure update_entry(store_carrier_member &member);
    FunctionSuccesFailure delete_entry(store_carrier_member &member);

private:
    file_carrier(): fileObjectRaw(NULL) { }
    file_carrier(const file_carrier &);
    file_carrier &operator=(const file_carrier);

    UINT fileObjectRaw;
    static std::string fileName;
    static const CHAR *fileLocation;
    static const CHAR *fileExtension;

    VOID close();
    INT get_last_entry_id();
    FunctionSuccesFailure open(const operationType type);
    std::string get_columns_name_line();
    std::string get_data_lines_content();
    BOOL is_member_present(store_carrier_member &member);

    BOOL is_open()                                                  const;
    std::string get_entries_default_sorted(const std::string &data) const;
};

VOID print_data_table(std::string data);


#endif
