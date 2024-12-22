//---------------------------------------------------------------------------

#ifndef libpqxxH
#define libpqxxH
#include <stdexcept>
//---------------------------------------------------------------------------
#include <Core/Core.h>
#include <DataSet/DataSet.h>

// using namespace Upp;

extern "C" {

typedef struct pg_result PGresult;
typedef struct pg_conn PGconn;
typedef struct pg_cancel PGcancel;
typedef unsigned int Oid;

struct PGconnection;
}

namespace pg {

/*  wraps PGconn
 *
 */
class Connection {
private:
	/* Wraps PGresult
	 *
	 */
	class Result {
		friend class Connection;

	public:
		Result(PGresult* r = NULL)
			: _res(r)
		{
		}
		~Result() { Clear(); }
		Result(Result&& rhs)
		{
			_res = rhs._res;
			rhs._res = nullptr;
		}

		Result& operator=(Result&& rhs)
		{
			Clear();
			Upp::Swap(_res, rhs._res);
			return *this;
		}

		Result(const Result& rhs) = delete;

		void Clear();
		int Status();
		int FieldCount() const;
		char* FieldName(int field_index) const;
		int FieldNumber(const char* fieldName) const;
		int FieldIndex(const char* fieldName) const;
		int RowsAffected();
		int N() const { return FieldCount(); }
		int RecordCount() const;
		int M() const { return RecordCount(); }
		char* GetValue(int row, int col) const;
		char* operator()(int row, int col) const { return GetValue(row, col); }

		char* operator()(int row, const char* fldName) const { return GetValue(row, fldName); }

		char* GetValue(int row, const char* fieldName) const
		{
			int col = FieldNumber(fieldName);

			if(col == -1)
				throw Upp::Exc("Specified field name not found");

			return GetValue(row, col);
		}

		bool IsNull(int row, int col) const;

		bool IsNull(int row, const char* fieldName) const
		{
			int col = const_cast<Result*>(this)->FieldIndex(fieldName);

			if(col == -1)
				throw Upp::Exc("field name not present");

			return IsNull(row, col);
		}

	private:
		void Set(PGresult* res)
		{
			Clear();
			_res = res;
		}

		// convert to RecordSet;
		operator Upp::RecordSet() const;
		// operator RecordSet(){ return RecordSet(const_cast<Result&>(*this));}

		void AssignTo(Upp::RecordSet& rs) const;

		PGresult* GetRes() { return _res; }

		const PGresult* GetRes() const { return _res; }

	private:
		PGresult* _res;
		Upp::String errMsg;
	};

public:
	Connection()
		: _conn(nullptr)
		, _cancel(nullptr)
	{
	}

	Connection(const Connection&) = delete;

	Connection(Connection&& con)
		: _conn(con._conn)
		, _cancel(con._cancel)
	{
		con._conn = nullptr;
		con._cancel = nullptr;
	}

	Connection(const char* conn_str);

	Connection& operator=(Connection&& con)
	{
		this->~Connection();
		new (this) Connection(std::move(con));
		return *this;
	}

	Upp::RecordSet Open(const char* sql) const
#ifdef _DEBUG
		;
#else
	{
		return Exec(sql);
	}
#endif

	int ExecSQL(const char* sql)
#ifdef _DEBUG
		;
#else
	{
		return Exec(sql).RowsAffected();
	}
#endif

	bool TryOpen(const char* sql, Upp::RecordSet& r) const;
	bool TryExecSQL(const char* sql, int& rowsAffected);

	const char* ErrorMessage() const;

	// if return'ed value == InvalidOid,
	// the action failed.
	//
	Oid lo_import(const char* filename);

	Oid lo_upload_file(const char* fn);

	void lo_export(Oid oidBlob, const char* filename);

	int lo_open(Oid oidBlob);

	int lo_close(int fd);
	int lo_read(int fd, char* buff, size_t len);

	int lo_size(int fd);

	void SetClientEncoding(const char* encoding);

	~Connection();

	void BeginTrans();
	void CommitTrans();
	void RollBack();

	Upp::String EscapeLiteral(const char* str);
	const char* DatabaseName() const;
	const char* User() const;
	const char* Password() const;
	const char* Host() const;
	const char* Port() const;
	// asynchronous processing
	bool SendQuery(const char* command);
	Result GetResult();

	bool ConsumeInput();
	bool IsBusy();

	void Cancel();
	// void ExecuteAsyn(const char * cmd,

	bool Save(Upp::RecordSet& rs, const char* tbl, int pkey_index = 0);
	Upp::RecordSet RecordSetFromTable(const char* tbl);

	bool Save(Upp::DataSet& ds);

	bool TableExists(const char* tblename);
	bool TableHasColumn(const char* tblname, const char* colname);
	Upp::RecordSet ListTables() const;
	Upp::String ExportTableDefs();
	Upp::String ExportTableDef(Upp::String tble);

private:
	PGconn* _conn;
	PGcancel* _cancel;

	// public:
	Result Exec(const char* sql) const;
	void Exec(const char* sql, Result& r) const { r = Exec(sql); }
};

} // namespace pg

#endif
