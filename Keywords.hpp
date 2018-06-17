//
//  Keywords.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Keywords_h
#define Keywords_h

const int gKeywordExpected=100;
const int gValueExpected=110;
const int gIdentifierExpected=120;
const int gSyntaxError=130;

const int gStatementExpected=200;
const int gNoDatabaseSpecified=300;
const int gUnknownDatabase=400;

const int gTableExists=500;
const int gUnknownTable=501;

const int gUnknownType=600;
const int gInvalidAttribute=610;
const int gInvalidArguments=620;

const int gReadError=700;
const int gWriteError=800;
const int gSeekError=900;
const int gStorageFull=1000;

const int gFilePathDNE = 2000;


const int gUnknownCommand = 3000;
const int gInvalidCommand = 3100;

const int gNotImplemented = 9998;
const int gEOF=9999;

namespace SF {
  
  enum class Keywords {
    add_kw, all_kw, alter_kw, and_kw, auto_increment_kw,
    between_kw, by_kw,
    char_kw, column_kw, create_kw,
    database_kw, databases_kw, datetime_kw, decimal_kw, delete_kw, describe_kw, distinct_kw, double_kw, drop_kw,
    float_kw, foreign_kw, from_kw, group_kw, 
    in_kw, insert_kw, integer_kw, into_kw,
    key_kw,  not_kw,  null_kw, or_kw, order_kw, primary_kw,
    references_kw, select_kw, set_kw, show_kw, table_kw, tables_kw,
    unknown_kw, unique_kw, update_kw, use_kw,
    values_kw, varchar_kw, where_kw,
    help_kw , idxRecord_kw, idxBlockNum_kw,
      
    run1000_kw, dump_kw
  };
  
  enum class Operators {
    equal_op, notequal_op, lt_op, lte_op, gt_op, gte_op, between_op, or_op,
    nor_op, and_op, not_op,
    unknown_op
  };

  struct StatusResult {
    bool handled;
    int  code;
    
    StatusResult(bool aHandled=true, int aCode=0) :
    handled(aHandled), code(aCode) {}
    
    operator bool() {return handled;}
  };
  
}
#endif /* Keywords_h */
