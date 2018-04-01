/**
  ***************************************************************************************
  * @file    storage_mgmt.cpp
  * @author  £ukasz Kaümierczak
  * @version V1.2
  * @date    20/07/2013
  * @brief   This source file provides support for storage operations on file/sql
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
#include "storage_mgmt.h"
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
// sql add-on
#include <winsock.h>
#include <mysql.h>

#include "stream.h"

#define MOD_STORAGE_MGMT               "storage_mgmt"

using namespace std;

#define INVALID_TEXT_VALUE             UNAVAILABLE_TEXT
#define INVALID_NUM_TEXT_VALUE         "0"
#define INVALID_NUM_VALUE              -1

static string dvaluetostring(const DOUBLE value);
template <typename Type> static string numValueToString(const Type value);
template <typename Type> static BOOL isFloatingPointValuesEqual(const Type value1, const Type value2);

const CHAR *database_carrier::db::server   = "localhost";
const CHAR *database_carrier::db::user     = "lukasz";
const CHAR *database_carrier::db::password = "qwerty1";
const CHAR *database_carrier::db::database = "library_db";

string file_carrier::fileName              = string(fileLocation) + "default.txt";
const CHAR *file_carrier::fileLocation     = "./Storage/";
const CHAR *file_carrier::fileExtension    = ".txt";

INT store_carrier_member::uid = 0;
const CHAR *store_carrier_member::columnName[] = {"Id", "Item_1", "Item_2", "Item_3", "Item_4", "Item_5"};


class sorting {
public:
    enum sort_type { SORT_INVALID=0, SORT_ORDER_ASCENDING, SORT_ORDER_DESCENDING,
                     SORT_BY_BRAND, SORT_BY_TYPE, SORT_BY_PRICE, };

    sorting(): output(EMPTY_STRING), compareAlgorithm(NULL) {}
    sorting(const string &input) { this->sorting::sorting(); if(input != EMPTY_STRING)
        { sort_entries_string(input, SORT_BY_BRAND, SORT_ORDER_ASCENDING ); } }
    sorting(const string &input, sort_type sortBy, sort_type sortOrder)
        { this->sorting::sorting(); if(input != EMPTY_STRING) { sort_entries_string(input, sortBy, sortOrder); } }

    const string &get_outcome() const { return output; }
    FunctionSuccesFailure sort_entries_string(const string &data, sort_type sortedByCategory, sort_type sortInOrder);

private:
    string output;
    BOOL (*compareAlgorithm)(store_carrier_member&, store_carrier_member&);

    static BOOL compare_by_item_1_asc(store_carrier_member &c1, store_carrier_member &c2);
    static BOOL compare_by_item_1_desc(store_carrier_member &c1, store_carrier_member &c2);
    static BOOL compare_by_item_2_asc(store_carrier_member &c1, store_carrier_member &c2);
    static BOOL compare_by_item_2_desc(store_carrier_member &c1, store_carrier_member &c2);
    static BOOL compare_by_item_3_asc(store_carrier_member &c1, store_carrier_member &c2);
    static BOOL compare_by_item_3_desc(store_carrier_member &c1, store_carrier_member &c2);
};

FunctionSuccesFailure
sorting::sort_entries_string(const string &data, sort_type sortedByCategory, sort_type sortInOrder)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(sortInOrder != SORT_ORDER_ASCENDING && sortInOrder != SORT_ORDER_DESCENDING)
    {
        stream::instance().error_message(MOD_STORAGE_MGMT, "unsupported order set, default is to be processed");
    }

    switch(sortedByCategory) {
    case store_carrier_member::MEMBER_ITEM_1:
        if(sortInOrder == SORT_ORDER_DESCENDING) { compareAlgorithm = compare_by_item_1_desc; }
        else { compareAlgorithm = compare_by_item_1_asc; }
        break;
    case store_carrier_member::MEMBER_ITEM_2:
        if(sortInOrder == SORT_ORDER_DESCENDING) { compareAlgorithm = compare_by_item_2_desc; }
        else { compareAlgorithm = compare_by_item_2_asc; }
        break;
    case store_carrier_member::MEMBER_ITEM_3:
        if(sortInOrder == SORT_ORDER_DESCENDING) { compareAlgorithm = compare_by_item_3_desc; }
        else { compareAlgorithm = compare_by_item_3_asc; }
        break;
    default:
        compareAlgorithm = compare_by_item_1_asc;
    }

    if(data != EMPTY_STRING)
    {
        string line;
        BOOL isEofReached = FALSE;
        istringstream stream(data);
        vector<store_carrier_member> entriesVector;

        output = string(EMPTY_STRING);
        while(isEofReached != TRUE)
        {
            line = EMPTY_STRING;
            isEofReached = getline(stream, line).eof();
            if(line != EMPTY_STRING)
            {
                entriesVector.push_back(store_carrier_member(line));
            }
        }

        sort(entriesVector.begin(), entriesVector.end(), compareAlgorithm);

        store_carrier_member::reset_uid();
        for(INT i=0, k=entriesVector.size(); i<k; ++i)
        {
            store_carrier_member::generate_uid();
            output += entriesVector[i].getWholeRow();
        }

        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

BOOL sorting::compare_by_item_1_desc(store_carrier_member &c1, store_carrier_member &c2)
{
    BOOL fresult = FALSE;
    if( c2.get_item_1() < c1.get_item_1() )
    {
        fresult = TRUE;
    }
    else if( c2.get_item_1() == c1.get_item_1() )
    {
        if( c2.get_item_2() < c1.get_item_2() )
        {
            fresult = TRUE;
        }
    }
    else
    {
        // clause not considered
    }

    return fresult;
}

BOOL sorting::compare_by_item_1_asc(store_carrier_member &c1, store_carrier_member &c2)
{
    BOOL fresult = FALSE;
    if( c1.get_item_1() < c2.get_item_1() )
    {
        fresult = TRUE;
    }
    else if( c1.get_item_1() == c2.get_item_1() )
    {
        if( c1.get_item_2() < c2.get_item_2() )
        {
            fresult = TRUE;
        }
    }
    else {
        // clause not considered
    }

    return fresult;
}

BOOL sorting::compare_by_item_2_desc(store_carrier_member &c1, store_carrier_member &c2)
{
    BOOL fresult = FALSE;
    if( c2.get_item_2() < c1.get_item_2() )
    {
        fresult = TRUE;
    }

    return fresult;
}

BOOL sorting::compare_by_item_2_asc(store_carrier_member &c1, store_carrier_member &c2)
{
    BOOL fresult = FALSE;
    if( c1.get_item_2() < c2.get_item_2() )
    {
        fresult = TRUE;
    }

    return fresult;
}

BOOL sorting::compare_by_item_3_desc(store_carrier_member &c1, store_carrier_member &c2)
{
    BOOL fresult = FALSE;
    if( c2.get_item_3() < c1.get_item_3() )
    {
        fresult = TRUE;
    }

    return fresult;
}

BOOL sorting::compare_by_item_3_asc(store_carrier_member &c1, store_carrier_member &c2)
{
    BOOL fresult = FALSE;
    if( c1.get_item_3() < c2.get_item_3() )
    {
        fresult = TRUE;
    }

    return fresult;
}

store_carrier_member::store_carrier_member():
id(0), kind(MEMBER_INVALID), item_1(INVALID_TEXT_VALUE), item_2(INVALID_TEXT_VALUE),
item_3(INVALID_TEXT_VALUE), item_4(INVALID_TEXT_VALUE), item_5(INVALID_NUM_VALUE)
{
    reset_uid();
}

store_carrier_member::
store_carrier_member(const member_type kindType, const string &value_1, const string &value_2,
const string &value_3, const string &value_4, INT value_5)
{
    this->store_carrier_member::store_carrier_member();
    kind   = kindType;
    item_1 = value_1;
    item_2 = value_2;
    item_3 = value_3;
    item_4 = value_4;
    item_5 = value_5;
}

store_carrier_member::
store_carrier_member(const member_type kindType, const INT value_id, const string &value_1,
const string &value_2, const string &value_3, const string &value_4, INT value_5)
{
    this->store_carrier_member::store_carrier_member(kindType, value_1, value_2, value_3, value_4, value_5);
    id = value_id;
}

store_carrier_member::
store_carrier_member(const member_type kindType, const INT uid)
{
    this->store_carrier_member::store_carrier_member();

    kind = kindType;
    switch(kindType)
    {
        case MEMBER_ID:
            id   = uid;
            break;
        default:
            stream::instance().error_message(MOD_STORAGE_MGMT, "passed storage member non-id type is unsupported by constructor");
    }
}

store_carrier_member::
store_carrier_member(const member_type kindType, const INT uid, const std::string &item)
{
    this->store_carrier_member::store_carrier_member(MEMBER_ID, uid);

    kind = kindType;
    switch(kind)
    {
        case MEMBER_ITEM_1:
            item_1 = item;
            break;
        case MEMBER_ITEM_2:
            item_2 = item;
            break;
        case MEMBER_ITEM_3:
            item_3 = item;
            break;
        case MEMBER_ITEM_4:
            item_4 = item;
            break;
        default:
            stream::instance().error_message(MOD_STORAGE_MGMT, "passed storage member numerical type is unsupported by constructor");
    }
}

store_carrier_member::
store_carrier_member(const member_type kindType, const INT uid, const INT item)
{
    this->store_carrier_member::store_carrier_member(MEMBER_ID, uid);

    kind = kindType;
    switch(kind)
    {
        case MEMBER_ITEM_5:
            item_5 = item;
            break;
        default:
            stream::instance().error_message(MOD_STORAGE_MGMT, "passed storage member non-numerical type is unsupported by constructor");
    }
}

store_carrier_member::store_carrier_member(const string &input)
{
    this->store_carrier_member::store_carrier_member();
    decomposeToMembers(input);
}

string store_carrier_member::getColumnRow()
{
    ostringstream output;
    for(INT i= 0, k=(INT)(store_carrier_member::MEMBER_LAST-store_carrier_member::MEMBER_FIRST); i<k; i++)
    {
        output<<string(store_carrier_member::columnName[i])<<LINE_FIELDS_DELIMITER;
    }
    output<<NEW_LINE_TOKEN;
    return output.str();
}

string store_carrier_member::getColumnName(member_type columnType)
{
    return string(store_carrier_member::columnName[columnType-store_carrier_member::MEMBER_FIRST]);
}

string store_carrier_member::getWholeRow() const
{
    ostringstream output;

    output<<id<<LINE_FIELDS_DELIMITER<<getRowWOId();
    return output.str();
}

string store_carrier_member::getRowWOId() const
{
    ostringstream output;

    output<<item_1<<LINE_FIELDS_DELIMITER<<item_2<<LINE_FIELDS_DELIMITER
          <<item_3<<LINE_FIELDS_DELIMITER<<item_4<<LINE_FIELDS_DELIMITER
          <<item_5<<LINE_FIELDS_DELIMITER<<NEW_LINE_TOKEN;

    return output.str();
}

VOID store_carrier_member::decomposeToMembers(const string &data)
{
    if(data != EMPTY_STRING) {
        string member;
        stringstream stream(data);

        kind = MEMBER_ALL;
        for(INT i=0, k=MEMBER_LAST-MEMBER_FIRST; i<k; ++i) {
            member = EMPTY_STRING;
            getline(stream, member, LINE_FIELDS_DELIMITER);

            istringstream memberStream(member);
            switch(i+MEMBER_FIRST) {
                case MEMBER_ID:
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
                case MEMBER_ITEM_1:
                    item_1 = memberStream.str();
                    break;
                case MEMBER_ITEM_2:
                    item_2 = memberStream.str();
                    break;
                case MEMBER_ITEM_3:
                    item_3 = memberStream.str();
                    break;
                case MEMBER_ITEM_4:
                    item_4 = memberStream.str();
                    break;
                case MEMBER_ITEM_5:
                    memberStream>>item_5;
                    break;
                default:
                    stream::instance().error_message(MOD_STORAGE_MGMT, "decomposed item has no suitable member");
            }
        }
    }
}

FunctionSuccesFailure database_carrier::initialize()
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    MYSQL *connection = mysql_init(NULL);

    if( connection == NULL ||
        !mysql_real_connect(connection, credentials.server, credentials.user,
                            credentials.password, credentials.database, 0, NULL, 0) )
    {
        notify_error("initialize");
    }
    else {
        connectionRaw = (UINT_PTR)connection;
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

INT database_carrier::get_last_insert_id()
{
    MYSQL *connection = (MYSQL *)connectionRaw;
    return (INT)mysql_insert_id(connection);
}

FunctionSuccesFailure database_carrier::select_table(const string &name)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    if(is_table_opened(name) == FALSE)
    {
        fresult = add_table(name);

    }
    else  {
        stream::instance().system_message(MOD_STORAGE_MGMT, "given table is already opened");
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure database_carrier::add_entry(store_carrier_member &member)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    if(select_table(selectedTable) != FUNCTION_FAILURE)
    {
        string itemToAdd_1(INVALID_TEXT_VALUE), itemToAdd_2(INVALID_TEXT_VALUE),
                itemToAdd_3(INVALID_TEXT_VALUE), itemToAdd_4(INVALID_TEXT_VALUE);
        INT itemToAdd_5(INVALID_NUM_VALUE);

        switch(member.get_kind()) {
            case store_carrier_member::MEMBER_ALL:
                itemToAdd_1 = member.get_item_1();
                itemToAdd_2 = member.get_item_2();
                itemToAdd_3 = member.get_item_3();
                itemToAdd_4 = member.get_item_4();
                itemToAdd_5 = member.get_item_5();
                break;
            case store_carrier_member::MEMBER_ITEM_1:
                itemToAdd_1 = member.get_item_1();
                break;
            case store_carrier_member::MEMBER_ITEM_2:
                itemToAdd_2 = member.get_item_2();
                break;
            case store_carrier_member::MEMBER_ITEM_3:
                itemToAdd_3 = member.get_item_3();
                break;
            case store_carrier_member::MEMBER_ITEM_4:
                itemToAdd_4 = member.get_item_4();
                break;
            case store_carrier_member::MEMBER_ITEM_5:
                itemToAdd_5 = member.get_item_5();
                break;
            case store_carrier_member::MEMBER_ID:
                stream::instance().error_message(MOD_STORAGE_MGMT, "Abort adding DB entry! Members with values for ["
                                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ID)+
                                "] only cannot be inserted into the table '"+string(selectedTable)+"'");
                return fresult;
            default: { /* no action defined */ }
        };

        MYSQL *connection = (MYSQL *)connectionRaw;
        string addEntryQuery = "INSERT INTO "+selectedTable+
                "("
                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_1)+", "
                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_2)+", "
                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_3)+", "
                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_4)+", "
                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_5)+
                ") VALUES("
                +"'"+itemToAdd_1+"', '"+itemToAdd_2+"', '"+itemToAdd_3+"', '"
                +itemToAdd_4+"', "+decToString(itemToAdd_5)+
                ")";

        if( !mysql_query(connection, addEntryQuery.c_str()) ) {
            fresult = FUNCTION_SUCCESS;
        }
        else {
            notify_error("add_entry");
        }
    }

    return fresult;
}

FunctionSuccesFailure database_carrier::update_entry(store_carrier_member &members)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
     
    if(select_table(selectedTable) != FUNCTION_FAILURE)
    {
        string updateEntryQuery = "UPDATE "+selectedTable+" SET ";
        string item_1(INVALID_TEXT_VALUE), item_2(INVALID_TEXT_VALUE), item_3(INVALID_TEXT_VALUE), item_4(INVALID_TEXT_VALUE);
        INT item_5(INVALID_NUM_VALUE);

        switch(members.get_kind())
        {
            case store_carrier_member::MEMBER_ALL:
                break;
            case store_carrier_member::MEMBER_ITEM_1:
                item_1 = members.get_item_1();
                updateEntryQuery +=  store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_1)+
                                    string("='") + item_1 + string("'");
                break;
            case store_carrier_member::MEMBER_ITEM_2:
                item_2 = members.get_item_2();
                updateEntryQuery +=  store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_2)+
                                    string("='") + item_2 + string("'");
                break;
            case store_carrier_member::MEMBER_ITEM_3:
                item_3 = members.get_item_3();
                updateEntryQuery +=  store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_3)+
                                    string("='") + item_3 + string("'");
                break;
            case store_carrier_member::MEMBER_ITEM_4:
                item_4 = members.get_item_4();
                updateEntryQuery +=  store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_4)+
                                    string("='") + item_4 + string("'");
                break;
            case store_carrier_member::MEMBER_ITEM_5:
                item_5 = members.get_item_5();
                updateEntryQuery +=  store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_5)+
                                    string("=") + decToString(item_5);
                break;
            case store_carrier_member::MEMBER_ID:
                stream::instance().error_message(MOD_STORAGE_MGMT, "Abort updating DB entry! "+
                         store_carrier_member::getColumnName(store_carrier_member::MEMBER_ID) +
                        " only cannot be modified within the table: "+selectedTable);
                return fresult;
            default: { /* no action defined */ }
        }

        updateEntryQuery += " WHERE "+store_carrier_member::getColumnName(store_carrier_member::MEMBER_ID)
                            +  "="+decToString(members.get_id());

        MYSQL *connection = (MYSQL *)connectionRaw;
        if( !mysql_query(connection, updateEntryQuery.c_str()) ) {
            fresult = FUNCTION_SUCCESS;
            stream::instance().system_message(MOD_STORAGE_MGMT, "DB entry updated!");
        }
        else {
            notify_error("DB update_entry");
        }
    }

    return fresult;
}

FunctionSuccesFailure database_carrier::delete_entry(store_carrier_member &members)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    if(select_table(selectedTable) != FUNCTION_FAILURE)
    {
        string itemToAdd_1(INVALID_TEXT_VALUE), itemToAdd_2(INVALID_TEXT_VALUE), 
                itemToAdd_3(INVALID_TEXT_VALUE), itemToAdd_4(INVALID_TEXT_VALUE);
        INT id, itemToAdd_5(INVALID_NUM_VALUE);

        string deleteEntryQuery = "DELETE FROM "+selectedTable+ " WHERE ";

        switch(members.get_kind())
        {
            case store_carrier_member::MEMBER_ALL:
                itemToAdd_1 = members.get_item_1();
                itemToAdd_2 = members.get_item_2();
                itemToAdd_3 = members.get_item_3();
                itemToAdd_4 = members.get_item_4();
                itemToAdd_5 = members.get_item_5();

                deleteEntryQuery += store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_1)
                                    +  "='"+itemToAdd_1+"'"
                                    +  " AND " + store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_2)
                                    +  "='"+itemToAdd_2+"'"
                                    +  " AND " + store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_3)
                                    +  "='"+itemToAdd_3+"'"
                                    +  " AND " + store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_4)
                                    +  "='"+itemToAdd_4+"'"
                                    +  " AND " + store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_5)
                                    +  "="+decToString(itemToAdd_5);
                break;
            case store_carrier_member::MEMBER_ITEM_1:
                itemToAdd_1 = members.get_item_1();
                deleteEntryQuery += store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_1)
                                    +  "='"+itemToAdd_1+"'";
                break;
            case store_carrier_member::MEMBER_ITEM_2:
                itemToAdd_2 = members.get_item_2();
                deleteEntryQuery += store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_2)
                                    +  "='"+itemToAdd_2+"'";
                break;
            case store_carrier_member::MEMBER_ITEM_3:
                itemToAdd_3 = members.get_item_3();
                deleteEntryQuery += store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_3)
                                    +  "='"+itemToAdd_3+"'";
                break;
            case store_carrier_member::MEMBER_ITEM_4:
                itemToAdd_4 = members.get_item_4();
                deleteEntryQuery += store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_4)
                                    +  "='"+itemToAdd_4+"'";
                break;
            case store_carrier_member::MEMBER_ITEM_5:
                itemToAdd_5 = members.get_item_5();
                deleteEntryQuery += store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_5)
                                    +  "="+decToString(itemToAdd_5);
                break;
            case store_carrier_member::MEMBER_ID:
                id = members.get_id();
                deleteEntryQuery += store_carrier_member::getColumnName(store_carrier_member::MEMBER_ID)
                                    +  "="+decToString(id);
                break;
            default: { /* no action defined */ }
        }

        MYSQL *connection = (MYSQL *)connectionRaw;
        if( !mysql_query(connection, deleteEntryQuery.c_str()) )
        {
            fresult = FUNCTION_SUCCESS;
            stream::instance().system_message(MOD_STORAGE_MGMT, "DB Entry deleted!");
        }
        else
        {
            notify_error("DB delete_entry");
        }
    }

    return fresult;
}

VOID database_carrier::set_auto_increment(INT initValue) const
{
    string setAIncrementQuery = "ALTER TABLE "+selectedTable+" AUTO_INCREMENT="+numValueToString(initValue);
    MYSQL *connection = (MYSQL *)connectionRaw;

    mysql_query(connection, setAIncrementQuery.c_str());
}

VOID database_carrier::remove_all_entries_from_table() const
{
    string removeAllEntriesQuery = "DELETE FROM "+selectedTable;
    MYSQL *connection = (MYSQL *)connectionRaw;

    mysql_query(connection, removeAllEntriesQuery.c_str());
    set_auto_increment(1);
}

string database_carrier::get_entries_default_sorted(const string &data) const
{
    sorting sortEntries(data, sorting::SORT_BY_BRAND, sorting::SORT_ORDER_ASCENDING);
    return sortEntries.get_outcome();
}

CHAR *database_carrier::get_column_name(UINT_PTR resultRaw, INT columnNb) const
{
    CHAR *fresult = NULL;
    MYSQL_FIELD *field = NULL;
    MYSQL_RES *result = (MYSQL_RES *)resultRaw;

    if(result != NULL) {
        mysql_field_seek(result, 0);
        for(INT i=0; i<=columnNb; ++i)
        {
            field = mysql_fetch_field(result);
            if(field == NULL) {
                break;
            }

            fresult = field->name;
        }
    }

    return fresult;
}

string database_carrier::get_column_headers_stream(UINT_PTR resultRaw) const
{
    string output;
    MYSQL_RES *result = (MYSQL_RES *)resultRaw;
    INT num_fields = mysql_num_fields(result);

    for(INT i = 0; i < num_fields; i++)
    {
        output += string(get_column_name(resultRaw, i)) + LINE_FIELDS_DELIMITER;
    }
    output += NEW_LINE_TOKEN;

    return output;
}

string database_carrier::get_entries_stream(UINT_PTR resultRaw) const
{
    string output;
    MYSQL_ROW row = {0};
    MYSQL_RES *result = (MYSQL_RES *)resultRaw;
    INT num_fields = mysql_num_fields(result);

    while(row = mysql_fetch_row(result))
    {
        for(INT i = 0; i < num_fields; i++)
        {
            output += string(row[i]) + LINE_FIELDS_DELIMITER;
        }
        output += NEW_LINE_TOKEN;
    }

    return output;
}

string database_carrier::get_whole_content() const
{
    string output = EMPTY_STRING;
    string showAllEntriesQuery = "SELECT * FROM "+selectedTable;
    MYSQL *connection = (MYSQL *)connectionRaw;

    if( !mysql_query(connection, showAllEntriesQuery.c_str()) )
    {
        MYSQL_RES *result = mysql_store_result(connection);
        if(result != NULL)
        {
            UINT_PTR resultRaw = (UINT_PTR)result;
            output = /* get_column_headers_stream(resultRaw) + */ get_entries_stream(resultRaw);
        }
    }

    return output;
}

FunctionSuccesFailure database_carrier::list_all() const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string showAllEntriesQuery = "SELECT * FROM "+selectedTable;
    MYSQL *connection = (MYSQL *)connectionRaw;

    if( !mysql_query(connection, showAllEntriesQuery.c_str()) )
    {
        MYSQL_RES *result = mysql_store_result(connection);

        if(result != NULL)
        {
            UINT_PTR resultRaw = (UINT_PTR)result;
            string tableColumnsLine  = get_column_headers_stream(resultRaw);
            string tableContentLines = get_entries_stream(resultRaw);
            mysql_free_result(result);

            string output = tableColumnsLine + get_entries_default_sorted(tableContentLines);
            OUTSTREAM<<endl<<endl<<"List all entries from table "<<selectedTable<<":"; PRINTOUT;
            print_data_table(output);
            fresult = FUNCTION_SUCCESS;
        }
    }

    if(fresult == FUNCTION_FAILURE) {
        notify_error("DB list_all");
    }

    return fresult;
}

FunctionSuccesFailure database_carrier::get_param_list(const store_carrier_member &params) const
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string columnName;
    string polledEntryName;
    string getPolledParamsQuery = "SELECT * FROM "+ selectedTable;
    MYSQL *connection = (MYSQL *)connectionRaw;

    switch(params.get_kind()) {
        case store_carrier_member::MEMBER_ALL:
            break;
        case store_carrier_member::MEMBER_ITEM_1:
            polledEntryName = params.get_item_1();
            columnName = store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_1);
            getPolledParamsQuery += " WHERE "+columnName+"='"+polledEntryName+"'";
            break;
        case store_carrier_member::MEMBER_ITEM_2:
            polledEntryName = params.get_item_2();
            columnName = store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_2);
            getPolledParamsQuery += " WHERE "+columnName+"='"+polledEntryName+"'";
            break;
        case store_carrier_member::MEMBER_ITEM_3:
            polledEntryName = params.get_item_3();
            columnName = store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_3);
            getPolledParamsQuery += " WHERE "+columnName+"='"+polledEntryName+"'";
            break;
        case store_carrier_member::MEMBER_ITEM_4:
            polledEntryName = params.get_item_4();
            columnName = store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_4);
            getPolledParamsQuery += " WHERE "+columnName+"='"+polledEntryName+"'";
            break;
        case store_carrier_member::MEMBER_ITEM_5:
            polledEntryName = decToString(params.get_item_5());
            columnName = store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_5);
            getPolledParamsQuery += " WHERE "+columnName+"="+polledEntryName;
            break;
        default:
        {
            stream::instance().error_message(MOD_STORAGE_MGMT, "DB parameter type is not supported!");
        }
    }

    if( !mysql_query(connection, getPolledParamsQuery.c_str()) )
    {
        MYSQL_RES *result = mysql_store_result(connection);
        if(result != NULL)
        {
            MYSQL_ROW row = NULL;
            UINT_PTR resultRaw = (UINT_PTR)result;
            INT rowFieldsNb  = mysql_num_fields(result);

            string tableColumnsLine;
            for(INT i = 0; i < rowFieldsNb; i++)
            {
                CHAR *currentColumnName = get_column_name(resultRaw, i);
                if( strcmp(currentColumnName, columnName.c_str()) != 0 )
                {
                    tableColumnsLine += string(currentColumnName) + LINE_FIELDS_DELIMITER;
                }
            }
            tableColumnsLine += NEW_LINE_TOKEN;

            string tableContentLines;
            while( row = mysql_fetch_row(result) )
            {
                for(INT i = 0; i < rowFieldsNb; i++)
                {
                    CHAR *currentColumnName = get_column_name(resultRaw, i);
                    if( row[i] != 0 && strcmp(currentColumnName, columnName.c_str()) != 0 )
                    {
                        tableContentLines += string(row[i]) + LINE_FIELDS_DELIMITER;
                    }
                }
                tableContentLines += NEW_LINE_TOKEN;
            }

            mysql_free_result(result);

            string output = tableColumnsLine + get_entries_default_sorted(tableContentLines);
            OUTSTREAM<<endl<<endl<<"List entries for ["+columnName+"]: '"<<polledEntryName
                     <<"', from table "<<selectedTable<<":"; PRINTOUT;
            print_data_table(output);
            fresult = FUNCTION_SUCCESS;
        }
    }

    if(fresult == FUNCTION_FAILURE) {
        notify_error("DB get_param_list");
    }

    return fresult;
}

FunctionSuccesFailure database_carrier::synchronize_with_stream(const string &input)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    MYSQL *connection = (MYSQL *)connectionRaw;
    string databaseWholeContent, showAllEntriesQuery = "SELECT * FROM "+selectedTable;
    string tableColumnsLine, tableContentLines, inputColumnsLine, inputContentLines;

    for(INT i=0, k=input.size(); i<k; ++i)
    {
        if(input[i] == NEW_LINE_TOKEN)
        {
            i++;
            inputColumnsLine  = string(input.begin(), input.begin()+i);
            inputContentLines = string(input.begin()+i, input.end());
            break;
        }
    }
    //cout<<endl<<"inputContentLines: "<<inputContentLines<<endl;
    if( !mysql_query(connection, showAllEntriesQuery.c_str()) )
    {
        MYSQL_RES *result = mysql_store_result(connection);
        if(result != NULL)
        {
            UINT_PTR resultRaw = (UINT_PTR)result;
            tableColumnsLine  = get_column_headers_stream(resultRaw);
            tableContentLines = get_entries_stream(resultRaw);
            mysql_free_result(result);

            databaseWholeContent = tableColumnsLine + get_entries_default_sorted(tableContentLines);
        }
    }

    if(inputColumnsLine != tableColumnsLine) {
        //cout<<"inputColumnsLine: "<<inputColumnsLine<<"|| tableColumnsLine: "<<tableColumnsLine<<endl;
        stream::instance().error_message(MOD_STORAGE_MGMT, "Aborting DB synchro! Columns not matching!");
        return fresult;
    }
    else
    {
        INT newEntriesCounter = 0;
        BOOL isInputEofReached=FALSE, isDatabaseEofReached=FALSE;
        string inputLine, inputLineFull, inputLineComparePart, databaseLine, 
               tableSortedContentLines=get_entries_default_sorted(tableContentLines),
               inputSortedContentLines=get_entries_default_sorted(inputContentLines);

        istringstream inputStream(inputSortedContentLines);
        while(isInputEofReached != TRUE) 
        {
            isInputEofReached = getline(inputStream, inputLineFull, NEW_LINE_TOKEN).eof();

            inputLine = store_carrier_member(inputLineFull).getRowWOId();
            inputLineComparePart = EMPTY_STRING;
            istringstream inputCompareStream(inputLine);
            for(INT i=0, coreFieldsToComapare = 3; i<coreFieldsToComapare; ++i)
            {
                string temp;
                getline(inputCompareStream, temp, LINE_FIELDS_DELIMITER);
                inputLineComparePart += temp + LINE_FIELDS_DELIMITER;
            }

            stringstream databaseStream(tableSortedContentLines);
            while(1)
            {
                isDatabaseEofReached = getline(databaseStream, databaseLine, NEW_LINE_TOKEN).eof();
                databaseLine = store_carrier_member(databaseLine).getRowWOId();

                if(inputLine == databaseLine)
                {
                    break;
                }
                else if(databaseLine.find(inputLineComparePart) != string::npos)
                {
                    store_carrier_member inputMember(inputLineFull);
                    database_carrier::instance().update_entry( store_carrier_member( store_carrier_member::MEMBER_ITEM_4,
                        inputMember.get_id(), inputMember.get_item_4()) );
                    database_carrier::instance().update_entry( store_carrier_member( store_carrier_member::MEMBER_ITEM_5,
                        inputMember.get_id(), inputMember.get_item_5()) );

                    break;
                }
                else if(isDatabaseEofReached == TRUE)
                {
                    newEntriesCounter++;
                    add_entry(store_carrier_member(inputLine));

                    inputLine = decToString(get_last_insert_id())+LINE_FIELDS_DELIMITER+inputLine;
                    file_carrier::instance().update_entry( store_carrier_member(inputLine) );
                    break;
                }
                else
                {
                    // search next line for a match
                }
            }
        }

        if(newEntriesCounter == 0)
        {
            stream::instance().system_message(MOD_STORAGE_MGMT, "No new entres found. Table same with input stream");
        }

        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

VOID database_carrier::close()
{
    MYSQL *connection = (MYSQL *)connectionRaw;
    mysql_close(connection);

    selectedTable = UNNAMED;
    connectionRaw = NULL;
}

FunctionSuccesFailure database_carrier::add_table(const string &name)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    string tableStructure = store_carrier_member::getColumnName(store_carrier_member::MEMBER_ID)+" INT PRIMARY KEY AUTO_INCREMENT, "
                            +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_1)+" TEXT, "
                            +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_2)+" TEXT, "
                            +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_3)+" TEXT, "
                            +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_4)+" TEXT, "
                            +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_5)+" INT";

    string createTableQuery = "CREATE TABLE " + name + "(" + tableStructure + ")";
    string createTableIfNotExistsQuery = "CREATE TABLE IF NOT EXISTS " + name + "(" + tableStructure + ")";

    MYSQL *connection = (MYSQL *)connectionRaw;
    if( !mysql_query(connection, createTableQuery.c_str()) )
    {
        selectedTable = name;
        fresult = FUNCTION_SUCCESS;
    }
    else
    {
        if( !mysql_query(connection, createTableIfNotExistsQuery.c_str()) )
        {
            stream::instance().system_message(MOD_STORAGE_MGMT ,"given table exists");
            selectedTable = name;
            fresult = FUNCTION_SUCCESS;
        }
        else {
            notify_error("DB add_table");
        }

    }

    return fresult;
}

FunctionSuccesFailure database_carrier::remove_table(const string &name) {
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string removeTableQuery = "DROP TABLE " + name;
    MYSQL *connection = (MYSQL *)connectionRaw;

    if(!mysql_query(connection, removeTableQuery.c_str())) {
        selectedTable = UNNAMED;
        fresult = FUNCTION_SUCCESS;
    }
    else {
        notify_error("DB remove_table");
    }

    return fresult;
}

BOOL database_carrier::is_table_opened(const string &name) const
{
    BOOL fresult = FALSE;
    if(selectedTable == name)
    {
        fresult = TRUE;
    }

    return fresult;
}

BOOL database_carrier::is_member_present(store_carrier_member &member) const
{
    BOOL fresult = FALSE;
    string createTableQuery = "SELECT "
              +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_1)+", "
              +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_2)+", "
              +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_3)+", "
              +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ITEM_4)
              +" FROM "+selectedTable;

    MYSQL *connection = (MYSQL *)connectionRaw;
    if( !mysql_query(connection, createTableQuery.c_str()) )
    {
        MYSQL_RES *result = mysql_store_result(connection);
        if(result != NULL) {
            MYSQL_ROW row = {0};
            INT num_fields = mysql_num_fields(result);

            while ((row = mysql_fetch_row(result)))
            {
                if( strncmp(row[0], member.get_item_1().c_str(), member.get_item_1().size()) == 0 &&
                    strncmp(row[1], member.get_item_2().c_str(), member.get_item_2().size()) == 0 &&
                    strncmp(row[2], member.get_item_3().c_str(), member.get_item_3().size()) == 0 &&
                    strncmp(row[3], member.get_item_4().c_str(), member.get_item_4().size()) == 0  )
                {
                    stream::instance().system_message(MOD_STORAGE_MGMT ,"DB entry is present");
                    fresult = TRUE;
                    break;
                }
            }

            if(fresult != TRUE) {
                stream::instance().system_message(MOD_STORAGE_MGMT ,"entry not found within selected table");
            }

            mysql_free_result(result);
        }
    }
    else
    {
        notify_error("DB is_present");
    }

    return fresult;
}

VOID database_carrier::notify_error(const string &module) const
{
    MYSQL *connection = (MYSQL *)connectionRaw;
    stream::instance().error_message(MOD_STORAGE_MGMT, "["+module +"] "+string(mysql_error(connection)));
}

FunctionSuccesFailure file_carrier::set_storage_file_name(const std::string &newStorageFile)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(newStorageFile != UNNAMED)
    {
        fileName = fileLocation+newStorageFile+fileExtension;
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

string file_carrier::get_whole_content()
{
    string output = /* get_columns_name_line() + */ get_data_lines_content();
    return output;
}

string file_carrier::get_whole_content_and_header()
{
    string output = get_columns_name_line() + get_data_lines_content();
    return output;
}

FunctionSuccesFailure file_carrier::add_entry(store_carrier_member &member)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    BOOL isFileEmpty = file_carrier::instance().is_empty();

    switch(member.get_kind())
    {
        case store_carrier_member::MEMBER_ALL:
        case store_carrier_member::MEMBER_ITEM_1:
        case store_carrier_member::MEMBER_ITEM_2:
        case store_carrier_member::MEMBER_ITEM_3:
        case store_carrier_member::MEMBER_ITEM_4:
        case store_carrier_member::MEMBER_ITEM_5:
            break;
        case store_carrier_member::MEMBER_ID:
            stream::instance().error_message(MOD_STORAGE_MGMT, "Abort adding file entry! Members with values for ["
                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ID)
                +"] only are not to be inserted into the file '"+fileName+"'");
            return fresult;
        default: { /* no action defined */ }
    }

    if( file_carrier::instance().open(FILE_OUTPUT_APP) == FUNCTION_FAILURE &&
        file_carrier::instance().open(FILE_OUTPUT_TRUNC) == FUNCTION_FAILURE )
    {
        stream::instance().error_message(MOD_STORAGE_MGMT, "file could not be opened for output operation");
    }
    else
    {
        fstream *fileObject = (fstream *)fileObjectRaw;
        if(isFileEmpty == TRUE)
        {
            (*fileObject)<<member.getColumnRow();
        }

        (*fileObject)<<member.getWholeRow();

        file_carrier::instance().close();
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure file_carrier::delete_entry(store_carrier_member &member)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string queryName;

    switch(member.get_kind())
    {
        case store_carrier_member::MEMBER_ALL:
            queryName = member.get_item_1()+LINE_FIELDS_DELIMITER
                        +member.get_item_2()+LINE_FIELDS_DELIMITER
                        +member.get_item_3()+LINE_FIELDS_DELIMITER
                        +member.get_item_4()+LINE_FIELDS_DELIMITER
                        +decToString(member.get_item_5());
            break;
        case store_carrier_member::MEMBER_ITEM_1:
            queryName = member.get_item_1();
            break;
        case store_carrier_member::MEMBER_ITEM_2:
            queryName = member.get_item_2();
            break;
        case store_carrier_member::MEMBER_ITEM_3:
            queryName = member.get_item_3();
            break;
        case store_carrier_member::MEMBER_ITEM_4:
            queryName = member.get_item_4();
            break;
        case store_carrier_member::MEMBER_ID:
            break;
        default:
        {
            stream::instance().system_message(MOD_STORAGE_MGMT, "delete_entry: parameter type is not supported!");
            return fresult;
        }
    }

    BOOL isEofReached = FALSE;
    string line, output = file_carrier::instance().get_columns_name_line();
    stringstream fileObjectStream(file_carrier::instance().get_data_lines_content());

    while(isEofReached != TRUE)
    {
        line = EMPTY_STRING;
        isEofReached = getline(fileObjectStream, line, NEW_LINE_TOKEN).eof();
        if(line != EMPTY_STRING)
        {
            if(member.get_kind() == store_carrier_member::MEMBER_ID)
            {
                store_carrier_member memberToDelete(line);
                if(memberToDelete.get_id() != member.get_id())
                {
                    output += line + NEW_LINE_TOKEN;
                }
            }
            else
            {
                if(line.find(queryName) == string::npos)
                {
                    output += line + NEW_LINE_TOKEN;
                }
            }
        }
    }

    if( file_carrier::open(FILE_OUTPUT_TRUNC) == FUNCTION_FAILURE )
    {
        stream::instance().error_message(MOD_STORAGE_MGMT, "file could not be opened for output operation");
    }
    else
    {
        fstream *fileObject = (fstream *)fileObjectRaw;
        (*fileObject).write( output.c_str(), output.size() );
        file_carrier::close();
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

FunctionSuccesFailure file_carrier::list_all() {

    FunctionSuccesFailure fresult = FUNCTION_FAILURE;

    if(file_carrier::instance().is_empty() != TRUE) {
        BOOL isColumnLineExtracted=FALSE, isEofReached=FALSE;
        string fileColumnsLine = file_carrier::instance().get_columns_name_line();
        string fileContentLines = file_carrier::instance().get_data_lines_content();
        string output = fileColumnsLine + get_entries_default_sorted(fileContentLines);

        OUTSTREAM<<endl<<endl<<"List all entries from file '"<<fileName<<"':"; PRINTOUT;
        print_data_table(output);
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

BOOL file_carrier::is_empty()
{
    BOOL fresult = FALSE;

    if( file_carrier::instance().open(FILE_INPUT) != FUNCTION_FAILURE )
    {
        fstream *fileObject = (fstream *)fileObjectRaw;
        (*fileObject).seekg(0, ios::beg);

        if( (*fileObject).peek() == EOF ||
            ((*fileObject).get() == 0x0A && (*fileObject).peek() == EOF) )
        {
            fresult = TRUE;
        }
        file_carrier::instance().close();
    }

    return fresult;
}

FunctionSuccesFailure file_carrier::open(const operationType type)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    INT parameter = ios::out;

    switch(type)
    {
        case FILE_INPUT:
            parameter = ios::in;
            break;
        case FILE_OUTPUT_APP:
            parameter |= ios::app;
            break;
        case FILE_OUTPUT_UPDATE:
            parameter |= ios::in;
            break;
        case FILE_OUTPUT_TRUNC:
            parameter |= ios::trunc;
            break;
        default:
        { /* parameter not supported! */ }
    }

    if(file_carrier::instance().is_open() == TRUE) {
        file_carrier::instance().close();
    }
    
    fileObjectRaw = (UINT)(new fstream(fileName, parameter));
    if(file_carrier::instance().is_open() == TRUE) {
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

BOOL file_carrier::is_open() const
{
    BOOL fresult = FALSE;
    fstream *fileObject = (fstream *)fileObjectRaw;

    if(fileObject != NULL) {
        fresult = (*fileObject).is_open();
    }

    return fresult;
}

void file_carrier::close()
{
    if(file_carrier::instance().is_open() == TRUE)
    {
        fstream *fileObject = (fstream *)fileObjectRaw;
        (*fileObject).close();
    }
    delete (fstream *)fileObjectRaw;
    fileObjectRaw = NULL;
}

string file_carrier::get_columns_name_line()
{
    string output;
    if(file_carrier::instance().open(FILE_INPUT) == FUNCTION_FAILURE)
    {
        stream::instance().error_message(MOD_STORAGE_MGMT, "file could not be opened for input operation");
    }
    else
    {
        fstream *fileObject = (fstream *)fileObjectRaw;
        string line;
        BOOL isEofReached = FALSE;
        istream *fileStream = static_cast<istream *>(fileObject);

        while(isEofReached != TRUE)
        {
            line = EMPTY_STRING;
            isEofReached = getline(*fileStream, line, NEW_LINE_TOKEN).eof();
            if(line != EMPTY_STRING)
            {
                output += line + NEW_LINE_TOKEN;
                break;
            }
        }

        file_carrier::instance().close();
    }

    return output;
}

string file_carrier::get_data_lines_content()
{
    string output;

    if(file_carrier::instance().open(FILE_INPUT) == FUNCTION_FAILURE)
    {
        stream::instance().error_message(MOD_STORAGE_MGMT, "file could not be opened for input operation");
    }
    else
    {
        fstream *fileObject = (fstream *)fileObjectRaw;
        string line;
        BOOL isEofReached = FALSE;
        istream *fileStream = static_cast<istream *>(fileObject);

        isEofReached = getline(*fileStream, line, NEW_LINE_TOKEN).eof();
        while(isEofReached != TRUE)
        {
            line = EMPTY_STRING;
            isEofReached = getline(*fileStream, line, NEW_LINE_TOKEN).eof();
            if(line != EMPTY_STRING)
            {
                output += line + NEW_LINE_TOKEN;
            }
        }

        file_carrier::instance().close();
    }

    return output;
}

BOOL file_carrier::is_member_present(store_carrier_member &member)
{
    BOOL fresult = FALSE;

    if(file_carrier::instance().is_empty() != TRUE) {
        BOOL isEofReached = FALSE;
        string line, memberField, fileLineField;
        istringstream fileObjectStream(file_carrier::instance().get_data_lines_content());

        while(isEofReached != TRUE)
        {
            isEofReached = getline(fileObjectStream, line, NEW_LINE_TOKEN).eof();

            istringstream fileLineStream(line);
            istringstream memberStream(member.getWholeRow());
            for(INT j=0, m=store_carrier_member::MEMBER_LAST-store_carrier_member::MEMBER_FIRST; j<m; ++j)
            {
                getline(memberStream, memberField, LINE_FIELDS_DELIMITER);
                getline(fileLineStream, fileLineField, LINE_FIELDS_DELIMITER);

                if( j+store_carrier_member::MEMBER_FIRST != store_carrier_member::MEMBER_ID )
                {
                    if(memberField != fileLineField)
                    {
                        break;
                    }
                    else if( j+store_carrier_member::MEMBER_FIRST == (store_carrier_member::MEMBER_LAST-1) )
                    {
                        stream::instance().system_message(MOD_STORAGE_MGMT, "member found within file's entries");
                        fresult = TRUE;
                    }
                    else
                    {
                        //fields match, verify next fields
                    }
                }
            }

            if(fresult == TRUE)
            {
                break;
            }
        }
    }

    return fresult;
}

FunctionSuccesFailure file_carrier::update_entry(store_carrier_member &member)
{
    FunctionSuccesFailure fresult = FUNCTION_FAILURE;
    string queryName;

    switch(member.get_kind())
    {
        case store_carrier_member::MEMBER_ALL:
            queryName = member.get_item_1()+LINE_FIELDS_DELIMITER
                        +member.get_item_2()+LINE_FIELDS_DELIMITER
                        +member.get_item_3()+LINE_FIELDS_DELIMITER
                        +member.get_item_4()+LINE_FIELDS_DELIMITER
                        +decToString(member.get_item_5());
            break;
        case store_carrier_member::MEMBER_ITEMS:
        case store_carrier_member::MEMBER_ITEM_1:
        case store_carrier_member::MEMBER_ITEM_2:
        case store_carrier_member::MEMBER_ITEM_3:
        case store_carrier_member::MEMBER_ITEM_4:
        case store_carrier_member::MEMBER_ITEM_5:
            queryName =  member.get_item_1()+LINE_FIELDS_DELIMITER
                        +member.get_item_2()+LINE_FIELDS_DELIMITER
                        +member.get_item_3()+LINE_FIELDS_DELIMITER;
            break;
        case store_carrier_member::MEMBER_ID:
            stream::instance().error_message(MOD_STORAGE_MGMT, "Abort deleting file entry! "
                +store_carrier_member::getColumnName(store_carrier_member::MEMBER_ID)
                +" only cannot be modified in the file: "+fileName);
            return fresult;
        default:
        {
            stream::instance().system_message(MOD_STORAGE_MGMT, "update_entry: parameter type is not supported!");
            return fresult;
        }
    }

    BOOL isEofReached = FALSE;
    string line, output = file_carrier::instance().get_columns_name_line();
    stringstream fileObjectStream(file_carrier::instance().get_data_lines_content());

    while(isEofReached != TRUE)
    {
        line = EMPTY_STRING;
        isEofReached = getline(fileObjectStream, line, NEW_LINE_TOKEN).eof();
        if(line != EMPTY_STRING)
        {
            if(line.find(queryName) != string::npos)
            {
                output += member.getWholeRow();
            }
            else
            {
                output += line + NEW_LINE_TOKEN;
            }
        }
    }

    if( file_carrier::open(FILE_OUTPUT_TRUNC) == FUNCTION_FAILURE )
    {
        stream::instance().error_message(MOD_STORAGE_MGMT, "file could not be opened for output operation");
    }
    else
    {
        fstream *fileObject = (fstream *)fileObjectRaw;
        (*fileObject).write( output.c_str(), output.size() );
        file_carrier::close();
        fresult = FUNCTION_SUCCESS;
    }

    return fresult;
}

INT file_carrier::get_last_entry_id()
{
    INT lastEntryId, fresult = 0;
    BOOL isEofReached = FALSE;
    string line, fileLineField;
    istringstream fileObjectStream(file_carrier::instance().get_data_lines_content());

    while(1)
    {
        line = EMPTY_STRING;
        isEofReached = getline(fileObjectStream, line, NEW_LINE_TOKEN).eof();

        if(line != EMPTY_STRING && isEofReached == FALSE)
        {
            fileLineField = EMPTY_STRING;
            istringstream fileLineStream(line);
            fileLineStream>>lastEntryId;
        }
        else
        {
            fresult = lastEntryId;
            break;
        }
    }

    return fresult;
}

string file_carrier::get_entries_default_sorted(const string &data) const
{
    sorting sortEntries(data, sorting::SORT_BY_BRAND, sorting::SORT_ORDER_ASCENDING);
    return sortEntries.get_outcome();
}



/*******************************************************************
*
*                   Auxliliary functions
*
********************************************************************/
template <typename Type> string numValueToString(const Type value)
{
    ostringstream result;

    if( typeid(Type) != typeid(bool) && typeid(Type) != typeid(string) && typeid(Type) != typeid(wchar_t) )
    {
        const INT defaultPrecision = 10;
        ostringstream os;
        DOUBLE integer = 0;
        DOUBLE fract   = modf(value, &integer);

        os<<fixed<<setprecision(defaultPrecision)<<fract;
        for(INT i=os.str().size()-1; i>=0; i--)
        {
            if( os.str()[i] != '0' || os.str()[i] == '.' ) {
                INT precision = i - ( os.str().size() - defaultPrecision - 1);
                result<<fixed<<setprecision(precision)<<value;
                break;
            }
        }
    }
    else {
        stream::instance().error_message(MOD_STORAGE_MGMT, "numValueToString: Unsupported data type passed to process");
    }

    return result.str();
}

string dvaluetostring(const DOUBLE value)
{
    ostringstream os, result;
    const INT defaultprecision = 10;
    DOUBLE integer = 0;
    DOUBLE fract =  modf(value, &integer);

    os<<fixed<<setprecision(defaultprecision)<<fract;
    for(INT i=os.str().size()-1; i>=0; i--)
    {
        if( os.str()[i] != '0' || os.str()[i] == '.' ) {
            INT precision = i - ( os.str().size()-defaultprecision-1 );
            os.str(EMPTY_STRING);
            os.clear();
            result<<fixed<<setprecision(precision)<<value;
            break;
        }
    }

    return result.str();
}

template <typename Type> BOOL isFloatingPointValuesEqual(const Type value1, const Type value2)
{
    BOOL fresult = FALSE;

    if( (typeid(Type) == typeid(FLOAT)) && (fabs(value1-value2) < FLT_EPSILON) ) {
        fresult = TRUE;
    }
    else if( (typeid(Type) == typeid(DOUBLE)) && (fabs(value1-value2) < DBL_EPSILON) ) {
        fresult = TRUE;
    }
    else {
        // no action determined
    }

    return fresult;
}

VOID print_data_table(string data)
{
    INT dataColumnsNb = 0;
    data.insert(data.begin(), NEW_LINE_TOKEN);

    for(INT i=0; i<static_cast<INT>(data.size()); ++i)
    {
        if(i != 0 && data[i] == LINE_FIELDS_DELIMITER) {
            dataColumnsNb++;
        }
    }

    vector<INT> maxLineFieldLength(dataColumnsNb, 0);
    for(INT i=0, column=0, lineFieldLength=0; i < static_cast<INT>(data.size()); ++i)
    {
        lineFieldLength++;
        if(data[i] == LINE_FIELDS_DELIMITER) {
            if(maxLineFieldLength[column] < lineFieldLength) {
                maxLineFieldLength[column] = lineFieldLength;
            }

            column++;
            lineFieldLength = 0;
        }
        else if(data[i] == NEW_LINE_TOKEN) {
            column = 0;
            lineFieldLength = 0;
        }
        else { }
    }

    for(INT i=0, column=0, lineFieldLength=0; i<static_cast<INT>(data.size()); ++i)
    {
        lineFieldLength++;
        if(data[i] == LINE_FIELDS_DELIMITER) {
            const INT leftVertBorderSpace = 1;
            INT maxLineFieldLengthVal = maxLineFieldLength[column] + leftVertBorderSpace;

            data.erase(data.begin()+i);
            if(data[i] != NEW_LINE_TOKEN) {
                data.insert(i, " | ");
            }

            for(INT k=(maxLineFieldLengthVal - lineFieldLength); k>=0; --k)
            {
                data.insert(i, " ");
                i++;
            }

            column++;
            lineFieldLength = 0;
        }

        if ( data[i] == NEW_LINE_TOKEN ) {
            if( i<static_cast<INT>(data.size()-1) ) {
                data.insert(i+1, "| ");
            }

            if(i != 0) {
                data.insert(i, " |");
                i += 2;
            }

            lineFieldLength = 0;
            column = 0;
        }
    }

    for(INT i=0, horizontalBorderLength=0; i<static_cast<INT>(data.size()); ++i)
    {
        if(data[i] == NEW_LINE_TOKEN)
        {
            if(i != 0)
            {
                for(INT j=0, row=0; j<static_cast<INT>(data.size()); ++j)
                {
                    if(j != 0 && data[j] == NEW_LINE_TOKEN)
                    {
                        row++;
                    }

                    if( row == 0 || row == 2 || j == data.size()-1 )
                    {
                        for(INT k=j+1, m=horizontalBorderLength+k; k<m; ++k)
                        {
                            data.insert(k, "-");
                        }

                        row++;
                        j += horizontalBorderLength+1;
                        data.insert(j, string(1, NEW_LINE_TOKEN));
                    }
                }

                break;
            }
        }
        else
        {
            horizontalBorderLength++;
        }
    }

    OUTSTREAM<<data; PRINTOUT;
}

