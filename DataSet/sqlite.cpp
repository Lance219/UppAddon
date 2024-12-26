#include "sqlite.h"
#include <Core/Core.h>

using namespace Upp;

BEGIN_NAMESPACE_LZ
inline namespace sqlite{
	
Connection::Connection ( const char * file )
{
	if(sqlite3_open(file, &_db)!=SQLITE_OK)
	{
		String err="Out of memory";
		if(_db!=nullptr)
		{
			err=sqlite3_errmsg(_db);
			sqlite3_close(_db);
		}
		throw err;
	}
}

static int callback(void *, int argc, char **argv, char **azColName){
    return 1;// we don't process any ROW for ExecSQL
}
int Connection::ExecSQL ( const char * sql )const
{
	char * errMsg;
	if(!_db)
		return 0;
	if( SQLITE_OK != sqlite3_exec(_db,sql,callback,nullptr,&errMsg) )
	{
		Exc e(errMsg);
		sqlite3_free(errMsg);
		throw pick(e);
	}
	return 0; // number of records affected??
}

RecordSet Connection::Open ( const char *sql )const
{
	sqlite3_stmt *stmt;
	RecordSet r;
	int code;
	
	if (_db == nullptr || sql==nullptr)
	{
		return pick(r);
	}

	if(SQLITE_OK != sqlite3_prepare_v2(_db, sql, -1, &stmt, nullptr))
	{
		if(stmt)
			sqlite3_finalize(stmt);
		throw Upp::Exc("Error running sqlite::Connection::Open()");
	}
	bool inited=false;
	while ( (code = sqlite3_step(stmt)) == SQLITE_ROW ) {
		int num_cols = sqlite3_column_count(stmt);

		if(!inited)
		{
			for(int i=0; i < num_cols; ++i)
			{
				FieldTypeId tid;
				switch(sqlite3_column_type(stmt,i))
				{
				case SQLITE3_TEXT:
					tid = FT_STRING;
					break;
					
				case SQLITE_INTEGER:
					tid = FT_INT8;
					break;
					
				case SQLITE_FLOAT:
					tid = FT_DOUBLE;
					break;
					
				case SQLITE_BLOB:
					tid = FT_BLOB;
					break;
					
				default:
					tid = FT_STRING;
					break;
				}
				r.AddField(sqlite3_column_name(stmt,i),tid);
			}
			const_cast<RecordDef&>(r.GetRecordDef()).PhysicalLayout();
			inited = true;
		}

		r.Append();
		for (int i = 0; i < num_cols; i++)
		{
			const char *p=(const char*)sqlite3_column_text(stmt,i);
			if(p==nullptr)
				r(i).SetNull();
			else
				r(i).Set(p);
		}
	}

	sqlite3_finalize(stmt);

	return pick(r);
}

bool Connection::TableExists ( const char * tblname )
{
	if(!_db)
		return false;
	return Open(String().Cat()<<"SELECT 1 AS cnt FROM sqlite_master "
		"WHERE type='table' and LOWER(name)="<<SqlFieldValue(ToLower(tblname))).RecordCount()!=0;
}

bool Connection::TableHasColumn(const char* tblname, const char * colname)
{
	if(!_db)
		return false;
	return Open(String().Cat()<<"SELECT 1 AS cnt FROM pragma_table_info("
			<<SqlFieldValue(tblname)<<") WHERE LOWER(name)="
			<<SqlFieldValue(ToLower(colname))
		).RecordCount() !=0;
}

void Connection::BeginTrans()
{
	ExecSQL("BEGIN TRANSACTION");
}

void Connection::CommitTrans()
{
	ExecSQL("COMMIT TRANSACTION");
}

void Connection::RollBack()
{
	ExecSQL("ROLLBACK");
}


}
END_NAMESPACE