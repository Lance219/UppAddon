#ifndef _DataSet_sqlite_h_
#define _DataSet_sqlite_h_

#include "sqlite/sqlite3.h"
#include <DataSet/DataSet.h>

BEGIN_NAMESPACE_LZ
inline namespace sqlite{
	
class Connection
{

	public:
		Connection ( const Connection& )=delete;
		Connection() : _db ( nullptr ) {}

		Connection ( Connection && con )
		{
			_db = con._db;
			con._db = nullptr;
		}

		Connection ( const char * file );

		Connection& operator = ( Connection&& con)noexcept
		{
			this->~Connection();
			new(this)Connection(std::move(con));
			return *this;
		}

		RecordSet Open ( const char *sql )const;


		int ExecSQL ( const char * sql )const;

		Upp::int64 LastInsertRowId()const{
			return _db==nullptr ? Upp::Null : sqlite3_last_insert_rowid(_db);
		}
		
		const char * ErrorMessage() const;

		~Connection (){ if(_db) sqlite3_close(_db); }

		void BeginTrans();
		void CommitTrans();
		void RollBack();

//			Upp::String EscapeLiteral ( const char * str );
		const char * DatabaseName() const;
		
		bool TableExists ( const char * tblename );
		bool TableHasColumn(const char* tblname, const char * colname);
//			Upp::String ExportTableDefs();
//			Upp::String ExportTableDef(Upp::String tble);
	private:
		sqlite3 * _db;
};

}
END_NAMESPACE

#endif
