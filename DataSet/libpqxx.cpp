//---------------------------------------------------------------------------
#include <libpq-fe.h>
extern "C" PGresult* PQexecParams(PGconn* conn, const char* command, int nParams,
                                  const Oid* paramTypes, const char* const* paramValues,
                                  const int* paramLengths, const int* paramFormats,
                                  int resultFormat); //	typedef enum

// #include <libpq/libpq-fs.h>
//  copy content of libpq-fs.h below
/*
 *	Read/write mode flags for inversion (large object) calls
 */

#define INV_WRITE 0x00020000
#define INV_READ 0x00040000

#include "libpqxx.h"

#ifndef _MSC_VER
long long ntohll(long long d)
{
#ifdef CPU_LE
	char h[8];
	int q;
	h[7] = *reinterpret_cast<char*>(&d);
	h[6] = *(reinterpret_cast<char*>(&d) + 1);
	h[5] = *(reinterpret_cast<char*>(&d) + 2);
	h[4] = *(reinterpret_cast<char*>(&d) + 3);
	h[3] = *(reinterpret_cast<char*>(&d) + 4);
	h[2] = *(reinterpret_cast<char*>(&d) + 5);
	h[1] = *(reinterpret_cast<char*>(&d) + 6);
	h[0] = *(reinterpret_cast<char*>(&d) + 7);
	return *reinterpret_cast<long long*>(h);
#else
	return d;
#endif
}

// don't know how to do it portably, do nothing for now
float ntohf(float f) { return f; }

double ntohd(double d) { return d; }

#endif

//---------------------------------------------------------------------------
using namespace Upp;

// for simplicity, convert numeric binary representation to double
static double numeric(int16* p, int len)
{
	int16 numdigit = ntohs(p[0]);
	int16 weight = ntohs(p[1]);

	ASSERT(numdigit == 0 || len == numdigit + 4);
	if(numdigit == 0)
		return len == 4 ? 0.0 : nan("");
	double v = ntohs(p[4]);
	for(int i = 5; i < len; ++i)
		v *= 10000, v += ntohs(p[i]);
	v *= pow(10000, weight + 1 - numdigit);

	int sign = ntohs(p[2]);
	ASSERT(sign == 0 || sign == 16384);
	if(sign)
		v *= -1;
	return v;
}

BEGIN_NAMESPACE_LZ
inline namespace pg{

Connection::Result Connection::Exec(const char* sql) const
{
	for(int retry = 7; retry != 0; --retry) {
		auto _res = PQexecParams(_conn, sql, 0, nullptr, nullptr, nullptr, nullptr, 1);
		if(auto _stat = PQresultStatus(_res);
		   _stat != PGRES_BAD_RESPONSE && _stat != PGRES_FATAL_ERROR) [[likely]]
			return _res;

		PQclear(_res);

		if(PQstatus(_conn) != CONNECTION_BAD)
			break;
		Sleep(300);
		PQreset(_conn);
	}
	throw Exc(PQerrorMessage(_conn));

	// original implementation, rewrite to get rid of 'goto'
	//_tryagain:
	//    _res = PQexecParams(_conn, sql, 0, nullptr,nullptr,nullptr,nullptr,1);
	//
	//    ExecStatusType _stat=PQresultStatus(_res);
	//    if(_stat == PGRES_BAD_RESPONSE || _stat== PGRES_FATAL_ERROR){
	//		if( PQstatus(_conn)==CONNECTION_BAD && ++retry <7 ){
	//			PQreset (_conn);
	//			Sleep(300);
	//			if(_res)
	//				PQclear(_res);
	//			goto _tryagain;
	//		}
	//        Exc e(PQerrorMessage(_conn));
	//        if(_res)
	//			PQclear(_res);
	//        throw e;
	//    }
	//    return _res;
	//
}

bool Connection::TryOpen(const char* sql, RecordSet& r) const
{
	try {
		r = Open(sql);
	}
	catch(Exc& e) {
		RLOG(e);
		return false;
	}
	return true;
}

bool Connection::TryExecSQL(const char* sql, int& rowsAffected)
{
	try {
		rowsAffected = ExecSQL(sql);
	}
	catch(Exc& e) {
		RLOG(e);
		return false;
	}
	return true;
}

void Connection::Result::Clear()
{
	if(_res) {
		PQclear(_res);
		_res = nullptr;
	}
}

int Connection::Result::Status() { return (int)PQresultStatus(_res); }

int Connection::Result::FieldCount() const { return PQnfields(_res); }

char* Connection::Result::FieldName(int field_index) const
{
	return PQfname(_res, field_index);
}

int Connection::Result::FieldNumber(const char* fieldName) const
{
	return PQfnumber(_res, fieldName);
}

int Connection::Result::FieldIndex(const char* fieldName) const
{
	return FieldNumber(fieldName);
}

int Connection::Result::RowsAffected() { return atoi(PQcmdTuples(_res)); }

int Connection::Result::RecordCount() const { return PQntuples(_res); }

char* Connection::Result::GetValue(int row, int col) const
{
	return PQgetvalue(_res, row, col);
}

bool Connection::Result::IsNull(int row, int col) const
{
	return PQgetisnull(_res, row, col) == 1;
}

Oid Connection::lo_upload_file(const char* fn)
{
	Result r;
	Oid oid;
	Exec("BEGIN", r);
	oid = ::lo_import(_conn, fn);
	if(oid == InvalidOid) {
		Exec("ROLLBACK", r);
		Exc e(PQerrorMessage(_conn));
		throw e;
	}
	else {
		Exec("COMMIT", r);
	}
	return oid;
}
// the SQL to check if a table exists (can refine based on this)
/*
SELECT * FROM pg_catalog.pg_class WHERE relkind = 'r' AND
    relname = 't_inventory_item' AND
    pg_catalog.pg_table_is_visible(oid)
*/
bool Connection::TableExists(const char* tblename)
{
	String sql = String().Cat() << R"(
		SELECT EXISTS (
			SELECT FROM pg_tables
			WHERE  schemaname = 'public'
			AND tablename=)" << sql_quote(tblename)
	                            << ");";

	return Open(sql)(0).Get<bit>();
}

RecordSet Connection::ListTables() const
{
	return Open("select relname from pg_class where relkind='r' and relowner<>10");
}

bool Connection::TableHasColumn(const char* tblname, const char* colname)
{
	String sql =
		String().Cat()
		<< "SELECT CAST(count(*) AS INTEGER) FROM information_schema.columns WHERE table_name="
		<< sql_quote(tblname) << "AND column_name=" << sql_quote(colname);
	return Open(sql)(0).Get<int32>() > 0;
}

#ifdef _DEBUG
RecordSet Connection::Open(const char* sql) const
{
	Result r = Exec(sql);
	ASSERT(PQresultStatus(r.GetRes()) == PGRES_TUPLES_OK); // note it works for empty tuples
	return r;
}

int Connection::ExecSQL(const char* sql)
{
	Result r = Exec(sql);
	ASSERT(PQresultStatus(r.GetRes()) == PGRES_COMMAND_OK);
	return r.RowsAffected();
}
#endif

// use related metadata to recreate the SQL the creates the tables in the current database
//
String Connection::ExportTableDefs()
{
	String s;
	auto r = Open("select relname from pg_class where relkind='r' and relowner<>10");
	while(!r.Eof()) {
		s << ExportTableDef(r(0).Get<String>());
		r.Next();
	}
	return s;
}

String Connection::ExportTableDef(String tbl)
{
	String s;
	auto r = Open(String().Cat() << "SELECT * FROM information_schema.columns where table_name="
	                             << sql_quote(tbl) << " order by ordinal_position");

	auto t =
		Open(String().Cat()
	         << "SELECT column_name FROM information_schema.key_column_usage WHERE table_name="
	         << sql_quote(tbl)
	         << " AND position_in_unique_constraint IS NULL ORDER BY ordinal_position");

	// refer to https://stackoverflow.com/questions/1152260/how-to-list-table-foreign-keys
	// query to retrieve foreign keys
	//
	// multiple column foreign keys not tested.
	VectorMap<String, Tuple<String, String>> forkey;
	s =
		R"(SELECT
	tc.table_schema,
    tc.constraint_name,
	tc.table_name,
	kcu.column_name,
	ccu.table_schema AS foreign_table_schema,
	ccu.table_name AS foreign_table_name,
	ccu.column_name AS foreign_column_name
FROM information_schema.table_constraints AS tc
    JOIN information_schema.key_column_usage AS kcu
      ON tc.constraint_name = kcu.constraint_name
      AND tc.table_schema = kcu.table_schema
	JOIN information_schema.constraint_column_usage AS ccu
      ON ccu.constraint_name = tc.constraint_name
      AND ccu.table_schema = tc.table_schema
WHERE tc.constraint_type = 'FOREIGN KEY' AND tc.table_name=)"
		<< sql_quote(tbl);

	RecordSet u = Open(s);
	s.Clear();
	while(!u.Eof()) {
		String fld = u(3).Get<String>();
		String ftbl = u(5).Get<String>();
		String ffld = u(6).Get<String>();
		forkey.Add(fld, MakeTuple(ftbl, ffld));
		u.Next();
	}
	// refer to https://dataedo.com/kb/query/postgresql/list-all-primary-keys-and-their-columns
	// List all primary keys (PKs) and their columns in PostgreSQL database
	auto p = Open(String().Cat() <<
	              R"(SELECT kcu.column_name AS col
FROM information_schema.table_constraints tco
JOIN information_schema.key_column_usage kcu 
     ON kcu.constraint_name = tco.constraint_name
     AND kcu.constraint_schema = tco.constraint_schema
     AND kcu.constraint_name = tco.constraint_name
WHERE tco.table_name=)" << sql_quote(tbl)
	                             <<
	              R"( AND tco.constraint_type = 'PRIMARY KEY'
ORDER BY ordinal_position)");

	if(!r.Eof()) {
		s << "CREATE TABLE " << SqlQuoteIdentifier(tbl) << '(';
		while(!r.Eof()) {
			String col = r["column_name"].Get<String>();
			if(r.RecordNo() != 0)
				s << ',';
			s << '\n';
			s << '\t' << SqlQuoteIdentifier(col) << ' ';
			{
				String type = Filter(r["data_type"].Get<String>(), CharFilterToUpper);
				if(type == "CHARACTER VARYING") {
					type = "VARCHAR";
					if(!r["character_maximum_length"].IsNull())
						type << '(' << r["character_maximum_length"].Get<int32>() << ')';
				}
				else if(type == "NUMERIC") {
					type << '(' << r["numeric_precision"].Get<int32>() << ','
						 << r["numeric_scale"].Get<int32>() << ')';
				}
				s << type;
			}
			if(r["is_nullable"].Get<String>() == "NO")
				s << " NOT NULL";
			if(p.RecordCount() == 1 && col == p(0).Get<String>())
				s << " PRIMARY KEY";
			if(r["is_identity"].Get<String>() == "YES")
				s << " GENERATED " << r["identity_generation"].Get<String>() << " AS IDENTITY";
			int i = forkey.Find(col);
			if(i >= 0)
				s << " REFERENCES " << SqlQuoteIdentifier(forkey[i].a) << '('
				  << SqlQuoteIdentifier(forkey[i].b) << ')';
			r.Next();
		}
		if(p.RecordCount() > 1) {
			s << ",\n\tPRIMARY KEY (";
			while(!p.Eof()) {
				if(p.RecordNo() != 0)
					s << ',';
				s << SqlQuoteIdentifier(p(0).Get<String>());
				p.Next();
			}
			s << ')';
		}
		s << "\n);\n\n";
	}
	return s;
}

Connection::Result::operator RecordSet() const
{
	RecordSet rs;
	AssignTo(rs);
	return std::move(rs);
}

void Connection::Result::AssignTo(RecordSet& rs) const
{
	String name = rs.GetName();
	rs.Clear();
	rs.SetName(name);
	RecordDef rdef;
	if(this->N() == 0)
		return;

	for(int i = 0; i < this->N(); ++i) {
		Oid oid = PQftype(_res, i);
		FieldTypeId tid;
		const char* fn = PQfname(_res, i);
		switch(oid) {
		case 16: // boolean
			tid = FT_BIT;
			break;
			// rs.AddField<bit>(fn);
		// 17;"bytea"
		// 18;"char"
		case 20: //;"int64"
			tid = FT_INT64;
			break;
		case 21: //;"int16"
			tid = FT_INT16;
			break;
		// 22;"int2vector"
		case 23: //"int32"
			tid = FT_INT32;
			break;
		// 24: //"regproc"
		case 19:   // "name"
		case 25:   //"text"
		case 1042: // Char[n]
		case 1043: // varchar
			tid = FT_STRING;
			break;

		// 26: //"oid"
		// 27: //"tid"
		// 28: //"xid"
		// 29: //"cid"
		// 30: //"oidvector"
		// 71: //"pg_type"
		// 75: //"pg_attribute"
		// 81: //"pg_proc"
		// 83: //"pg_class"
		// 142: //"xml"
		// 143: //"_xml"
		// 210: //"smgr"
		// 600: //"point"
		// 601: //"lseg"
		// 602: //"path"
		// 603: //"box"
		// 604: //"polygon"
		// 628: //"line"
		// 629: //"_line"
		// 650: //"cidr"
		// 651: //"_cidr"
		case 700: //"float4"
			tid = FT_FLOAT;
			break;
		case 701:  //"float8"
		case 1700: // numeric
			tid = FT_DOUBLE;
			break;
		// case 702: //"abstime"
		// case 703: //"reltime"
		case 1114: // TIMESTAMP
			tid = FT_TIME;
			break;
		default:
			LOG(String().Cat() << fn << " of unknow type id " << oid);
			continue;
		}
		rdef.Add(fn, tid);
	}
	rs.SetRecordDef(std::move(rdef.PhysicalLayout()));
	// const_cast<RecordDef&>(rs.GetRecordDef()).PhysicalLayout();
	//
	// DUMP(rs.GetRecordDef()); //works correctly and as expected.

	for(int r = 0; r < this->M(); ++r) {
		rs.Append();
		for(int c = 0; c < (int)rs.GetRecordDef().GetFieldCount(); ++c) {
			if(PQgetisnull(_res, r, (int)c))
				continue;
			const char* s = PQgetvalue(_res, r, (int)c);

			if(1 == PQfformat(_res, c) /*binary*/) {
				switch(PQftype(_res, c)) {
				case 16: // boolean // 1 byte for binary boolean value PG
					ASSERT(PQgetlength(_res, r, c) == 1);
					rs(c).SetBit(*s != 0);
					break;
					// rs.AddField<bit>(fn);
				// 17;"bytea"
				// 18;"char"
				// 19;"name"
				case 20: //;"int8" 64bit integer
					rs(c).As<int64>() = ntohll(*(int64*)s);
					break;
				case 21: //;"int2"
					rs(c).As<int16>() = ntohs(*(int16*)s);
					break;
				// 22;"int2vector"
				case 23: //"int4"
					rs(c).As<int32>() = ntohl(*(int32*)s);
					break;
				// 24: //"regproc"
				case 19:   //"name"
				case 25:   //"text"
				case 1042: // Char[n]
				case 1043: // varchar
					rs(c) = s;
					break;
				//	break;

				// 26: //"oid"
				// 27: //"tid"
				// 28: //"xid"
				// 29: //"cid"
				// 30: //"oidvector"
				// 71: //"pg_type"
				// 75: //"pg_attribute"
				// 81: //"pg_proc"
				// 83: //"pg_class"
				// 142: //"xml"
				// 143: //"_xml"
				// 210: //"smgr"
				// 600: //"point"
				// 601: //"lseg"
				// 602: //"path"
				// 603: //"box"
				// 604: //"polygon"
				// 628: //"line"
				// 629: //"_line"
				// 650: //"cidr"
				// 651: //"_cidr"
				case 700: //"float4"
					rs(c).As<float>() = ntohf(*(unsigned*)s);
					break;
				case 701: //"float8"
					rs(c).As<double>() = ntohd(*(int64*)s);
					break;
				case 1700: // numeric
					ASSERT(PQgetlength(_res, r, c) % 2 == 0);
					rs(c).As<double>() = numeric((int16*)s, PQgetlength(_res, r, c) / 2);
					break;
				// case 702: //"abstime"
				// case 703: //"reltime"
				case 1114: // TIMESTAMP
					rs(c).As<Time>().Set(ntohll(*(int64*)s) / 1000000 +
					                     (2000LL * 365 + 485) * 24 * 3600);
					break;
				default:
					LOG("UNPROCESSED FIELD ASSIGNMENT in Connection::AssignTo!");
					continue;
				}
			}
			else if(rs.GetRecordDef().GetFieldDef(c).GetFieldType() == FT_TIME)
				; // rs(c) = ScanTime("ymd", s,Null);
			else
				rs(c) = s;
			rs(c).MarkAsSet();
		} // else
		//	rs[c].SetNull();
	}
	rs.First();
}

Connection::Connection(const char* conn_str)
	: _conn(PQconnectdb(conn_str))
	, _cancel(NULL)
{
	if(PQstatus(_conn) != (int)CONNECTION_OK) {
		Exc e(PQerrorMessage(_conn));
		PQfinish(_conn);
		throw e;
	}
}

// don't free returned pointer.
const char* Connection::ErrorMessage() const { return PQerrorMessage(_conn); }

Oid Connection::lo_import(const char* filename) { return ::lo_import(_conn, filename); }

void Connection::lo_export(Oid oidBlob, const char* filename)
{
	if(-1 == ::lo_export(_conn, oidBlob, filename))
		throw Exc("export oid failed.");
}

int Connection::lo_open(Oid oidBlob) { return ::lo_open(_conn, oidBlob, INV_READ); }

int Connection::lo_close(int fd) { return ::lo_close(_conn, fd); }

int Connection::lo_read(int fd, char* buff, size_t len)
{
	return ::lo_read(_conn, fd, buff, len);
}

int Connection::lo_size(int fd)
{
	int size;
	lo_lseek(_conn, fd, 0, SEEK_END);
	size = lo_tell(_conn, fd);
	lo_lseek(_conn, fd, 0, SEEK_SET);
	return size;
}

void Connection::SetClientEncoding(const char* encoding)
{
	if(PQsetClientEncoding(_conn, encoding) == -1)
		throw Exc("setClientEncoding failed!");
}

Connection::~Connection()
{
	PQfinish(_conn);
	if(_cancel)
		PQfreeCancel(_cancel);
}

void Connection::BeginTrans() { (void)Exec("BEGIN"); }
void Connection::CommitTrans() { (void)Exec("COMMIT"); }
void Connection::RollBack() { (void)Exec("ROLLBACK"); }

String Connection::EscapeLiteral(const char* str)
{
	char * p=PQescapeLiteral(_conn, str, size_t(-1));
	String s(p);
	PQfreemem(p);
	return s;
}

const char* Connection::DatabaseName() const { return PQdb(_conn); }
const char* Connection::User() const { return PQuser(_conn); }
const char* Connection::Password() const { return PQpass(_conn); }
const char* Connection::Host() const { return PQhost(_conn); }
const char* Connection::Port() const { return PQport(_conn); }
// asynchronous processing
bool Connection::SendQuery(const char* command) { return PQsendQuery(_conn, command) == 1; }

Connection::Result Connection::GetResult() { return Result(PQgetResult(_conn)); }

bool Connection::ConsumeInput() { return PQconsumeInput(_conn) == 1; }

bool Connection::IsBusy() { return PQisBusy(_conn) == 1; }

void Connection::Cancel()
{
	if(!_cancel) {
		if((_cancel = PQgetCancel(_conn)) == NULL) {
			return;
		}
	}
	char err[256];
	PQcancel(_cancel, err, 256);
}

static String SqlFieldValue(RecordSet& rs, int i)
{
	String s;
	if(rs(i).IsNull())
		return "NULL";
	switch(rs.GetRecordDef().GetFieldDef(i).GetFieldType()) {
	case FT_BIT:
	case FT_BOOL: {
		bool v;
		rs(i).Get(v);
		return v ? "true" : "false";
	}
	case FT_INT8:
	case FT_INT16:
	case FT_INT32:
	case FT_INT64:
	case FT_UINT8:
	case FT_UINT16:
	case FT_UINT32:
	case FT_UINT64:
	case FT_FLOAT:
	case FT_DOUBLE:
		rs(i).Get(s);
		break;
	case FT_STRING:
		s = sql_quote(rs(i).Get<String>());
		break;
	case FT_DATE:
		s << '\'' << FormatDate(rs(i).Get<Date>(), "YYYY-M-D") << '\'';
		break; // Upp::Date
	case FT_TIME:
		s << '\'' << FormatTime(rs(i).Get<Time>(), "YYYY-M-D h:m:s") << '\'';
		break; // Upp::Time
	default:
		throw "type not handled!";
		//	    case FT_BLOB:
		//			break;
		//		case FT_PCHAR:
		//			break; // char *
		//		case FT_VALUE:
		//			break; //u++ value
	}
	return s;
}

bool Connection::Save(RecordSet& rs, const char* _tbl, int pkey_index)
{
	if(_tbl == nullptr || _tbl[0] == '\0')
		if((_tbl = rs.GetName())[0] == '\0')
			return false;
	_tbl = PQescapeIdentifier(_conn, _tbl, size_t(-1));
	String tbl = _tbl;
	PQfreemem(const_cast<char*>(_tbl));

	const RecordDef& rdef = rs.GetRecordDef();
	String sql, s;
	bool newrec = false;

again:
	if(rs(pkey_index).IsNull()) // new record
	{
		newrec = true;
		sql << "INSERT INTO " << tbl << '(';
		for(int i = 0; i < (int)rdef.GetFieldCount(); ++i) {
			if(i != pkey_index) {
				sql << rdef.GetFieldDef(i).GetFieldName() << ',';
				s << SqlFieldValue(rs, i) << ',';
			}
		}
		sql.Remove(sql.GetLength() - 1);
		s.Remove(s.GetLength() - 1);
		sql << ")VALUES(" << s << ") RETURNING " << rdef.GetFieldDef(pkey_index).GetFieldName();
		try {
			auto t = Open(sql);
			rs(pkey_index) = t(0).Get<int32>(); // might be some other types!!!
		}
		catch(...) {
			DUMP(sql);
			return false;
		}
	}
	else {
		sql << "UPDATE " << tbl << " SET ";
		for(int i = 0; i < (int)rdef.GetFieldCount(); ++i)
			if(i != pkey_index)
				sql << rdef.GetFieldDef(i).GetFieldName() << '=' << SqlFieldValue(rs, i) << ',';
		sql.Remove(sql.GetLength() - 1);
		sql << " WHERE " << rdef.GetFieldDef(pkey_index).GetFieldName() << '='
			<< SqlFieldValue(rs, pkey_index);
		try {
			int rowsaffected = ExecSQL(sql);
			if(rowsaffected == 0) // update with no record
			{
				rs(pkey_index).SetNull();
				sql.Clear();
				goto again;
			}
		}
		catch(Exc& e) {
			DUMP(sql);
			RLOG(e);
			return false;
		}
	}
	return true;
}

RecordSet Connection::RecordSetFromTable(const char* tbl)
{
	String sql;
	tbl = PQescapeIdentifier(_conn, tbl, size_t(-1));
	sql << "SELECT * FROM " << tbl << " WHERE FALSE";
	PQfreemem(const_cast<char*>(tbl));
	return Exec(sql);
}

}
END_NAMESPACE
