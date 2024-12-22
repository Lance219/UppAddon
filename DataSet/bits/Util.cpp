#include "../DataSet.h"
#include <algorithm>


int TypeAlign(FieldTypeId fid)
{
	if(fid<FT_PCHAR)
	{
		ASSERT(basic_types[fid].id==fid);
		return basic_types[fid].align;
	}
	return 1;
}
// will need to revise to accomodate UDTs
int TypeSize(FieldTypeId fid)
{
	if(fid<FT_PCHAR)
	{
		ASSERT(basic_types[fid].id==fid);
		return basic_types[fid].size;
	}
	return 0;
}
const char * TypeName(FieldTypeId fid)
{
	if(fid<FT_PCHAR)
	{
		ASSERT(basic_types[fid].id==fid);
		return basic_types[fid].name;
	}
	return "unknown";
}

//  state machine
//  0  ON 0/1 to 1
//     ON Yy  to 2
//     ON Tt  to 3
//     ON
bool ParseBool(const char * input, bool& output)
{
	//int c;
	//bool success=false;
	bool value;
	if(!input)
		return false;

	// remove preceding spaces
	while(isspace(*input))
		++input;
	
	switch( tolower(*input))
	{
	case '0':
		value=false;
		break;
	case '1':
		value=true;
		break;
	case 't':
		value=true;
		if(stricmp(input, "true")==0)
			input+=3;
		break;
	case 'f':
		value=false;
		if(stricmp(input, "false")==0)
			input+=4;
		break;
	case 'y':
		value=true;
		if(stricmp(input, "yes")==0)
			input+=2;
		break;
	case 'n':
		value=false;
		if(tolower(input[1])=='o')
			++input;
		break;
	default:
		return false;
	}
	// ignore trailing space
	while(isspace(*++input))
		;
	output=value;
	return *input=='\0';
}

void SetBit(void * start, size_t offset)
{
	typedef int8 t;
	start = (void*) ((lz::ptrint)start+offset/(8*sizeof(t)));
	offset=offset%(8*sizeof(t));
	*(t*)start |= (1<<offset);
}

void UnsetBit(void * start, size_t offset)
{
	typedef int8 t;
	
	start = (void*) ((lz::ptrint)start+offset/(8*sizeof(t)));
	offset=offset%8;
	*(t*)start &= ~(1<<offset);
}

void SetBit(void * start, size_t offset, bool v)
{
	if(v)
		SetBit(start, offset);
	else
		UnsetBit(start, offset);
}

bool GetBit(const void * start, size_t offset)
{
	typedef int8 t;
	
	start = (void*) ((lz::ptrint)start+offset/(8*sizeof(t)));
	offset%=8*sizeof(t);
	return 0!=( *(const t*)start & (((t)1)<<offset) );
}


int compare(FieldTypeId fid, const void * lhs, const void * rhs)
{
#define CASE(ftype, type) case ftype:\
  return *(const type*)lhs<*(const type*)rhs?-1:\
	*(const type*)lhs==*(const type*)rhs?0:1
	
	switch(fid)
	{
	case FT_BIT:
		return int(lz::ptrint(lhs)-lz::ptrint(rhs));
	case FT_BOOL:
		return *(const lz::boolint*)lhs<*(const lz::boolint*)rhs;
	CASE(FT_INT8, int8);
	CASE(FT_INT16, int16);
	CASE(FT_INT32, int32);
	CASE(FT_INT64, int64);
	CASE(FT_UINT8, uint8);
	CASE(FT_UINT16, uint16);
	CASE(FT_UINT32, uint32);
	CASE(FT_UINT64, uint64);
	CASE(FT_FLOAT, float);
	CASE(FT_DOUBLE, double);
	CASE(FT_STRING, String);
	CASE(FT_TIME, Time);
	//LZ_ENTRY(FT_VALUE, Value);
	default:
		;
	}
	return 0; // or shall we throw?
#undef CASE
}

/*
* 2020-08-24 No need to escape \ ???
*
*/

String sql_quote(const char * s)
{
	if(s==0 || s[0]=='\0')
		return String("''");
	
	String t;
	t<<'\'';
	while(*s!='\0'){
		t<<*s;
		if(*s=='\'' || *s=='\\')
			t<<*s;
		
		++s;
	}
	t<<"'";
	// for testing, compare with
	
	return t;
}


String sql_like(const char *s)
{
	if(s==0 || s[0]=='\0')
		return String("''");
	
	String t;
	while(*s!='\0'){
		t<<*s;
		if(*s=='\'' || *s=='%')
			t<<*s;
		++s;
	}
	return t;
}

String SqlQuoteIdentifier(const char *s)
{
	static const char* keywords[]={
		"A",		"ABORT",		"ABS",		"ABSENT",
		"ABSOLUTE",	"ACCESS",		"ACCORDING","ACOS",
		"ACTION",	"ADA",		"ADD",		"ADMIN",
		"AFTER",	"AGGREGATE",	"ALL",	"ALLOCATE",
		"ALSO",		"ALTER",		"ALWAYS","ANALYSE",
		"ANALYZE",	"AND",		"ANY",		"ARE",
		"ARRAY",	"ARRAY_AGG",	"ARRAY_​MAX_​CARDINALITY",
		"AS",		"ASC",		"ASENSITIVE",		"ASIN",
		"ASSERTION",	"ASSIGNMENT",		"ASYMMETRIC",
		"AT",		"ATAN",		"ATOMIC",		"ATTACH",
		"ATTRIBUTE",		"ATTRIBUTES",		"AUTHORIZATION",
		"AVG",		"BACKWARD",		"BASE64",		"BEFORE",
		"BEGIN",		"BEGIN_FRAME",		"BEGIN_PARTITION",
		"BERNOULLI",		"BETWEEN",		"BIGINT",
		"BINARY",		"BIT",		"BIT_LENGTH",
		"BLOB",		"BLOCKED",		"BOM",		"BOOLEAN",
		"BOTH",		"BREADTH",		"BY",		"C",
		"CACHE",		"CALL",		"CALLED",		"CARDINALITY",
		"CASCADE",		"CASCADED",		"CASE",		"CAST",
		"CATALOG",		"CATALOG_NAME",		"CEIL",		"CEILING",
		"CHAIN",		"CHAINING",		"CHAR",		"CHARACTER",
		"CHARACTERISTICS",		"CHARACTERS",		"CHARACTER_LENGTH",
		"CHARACTER_​SET_​CATALOG",		"CHARACTER_SET_NAME",
		"CHARACTER_SET_SCHEMA",		"CHAR_LENGTH",
		"CHECK",		"CHECKPOINT",		"CLASS",		"CLASSIFIER",
		"CLASS_ORIGIN",		"CLOB",		"CLOSE",		"CLUSTER",
		"COALESCE",		"COBOL",		"COLLATE",		"COLLATION",
		"COLLATION_CATALOG",		"COLLATION_NAME",		"COLLATION_SCHEMA",
		"COLLECT",		"COLUMN",		"COLUMNS",		"COLUMN_NAME",
		"COMMAND_FUNCTION",		"COMMAND_​FUNCTION_​CODE",		"COMMENT",
		"COMMENTS",		"COMMIT",		"COMMITTED",		"CONCURRENTLY",
		"CONDITION",		"CONDITIONAL",		"CONDITION_NUMBER",
		"CONFIGURATION",		"CONFLICT",		"CONNECT",
		"CONNECTION",		"CONNECTION_NAME",		"CONSTRAINT",
		"CONSTRAINTS",		"CONSTRAINT_CATALOG",		"CONSTRAINT_NAME",
		"CONSTRAINT_SCHEMA",		"CONSTRUCTOR",		"CONTAINS",
		"CONTENT",		"CONTINUE",		"CONTROL",		"CONVERSION",
		"CONVERT",		"COPY",		"CORR",		"CORRESPONDING",
		"COS",		"COSH",		"COST",	"COUNT",		"COVAR_POP",
		"COVAR_SAMP",		"CREATE",		"CROSS",		"CSV",
		"CUBE",		"CUME_DIST",		"CURRENT",		"CURRENT_CATALOG",
		"CURRENT_DATE",	"CURRENT_​DEFAULT_​TRANSFORM_​GROUP",	"CURRENT_PATH",
		"CURRENT_ROLE",		"CURRENT_ROW",	 "CURRENT_SCHEMA", "CURRENT_TIME",
		"CURRENT_TIMESTAMP", "CURRENT_​TRANSFORM_​GROUP_​FOR_​TYPE",
		"CURRENT_USER", "CURSOR",		"CURSOR_NAME",		"CYCLE",
		"DATA",		"DATABASE",		"DATALINK",		"DATE",
		"DATETIME_​INTERVAL_​CODE",		"DATETIME_​INTERVAL_​PRECISION",
		"DAY",		"DB",		"DEALLOCATE",		"DEC",		"DECFLOAT",
		"DECIMAL",		"DECLARE",		"DEFAULT",		"DEFAULTS",
		"DEFERRABLE",		"DEFERRED",		"DEFINE",		"DEFINED",
		"DEFINER",		"DEGREE",		"DELETE",		"DELIMITER",
		"DELIMITERS",		"DENSE_RANK",		"DEPENDS",		"DEPTH",
		"DEREF",		"DERIVED",		"DESC",		"DESCRIBE",
		"DESCRIPTOR",		"DETACH",		"DETERMINISTIC",	"DIAGNOSTICS",
		"DICTIONARY",		"DISABLE",		"DISCARD",		"DISCONNECT",
		"DISPATCH",		"DISTINCT",		"DLNEWCOPY",		"DLPREVIOUSCOPY",
		"DLURLCOMPLETE",		"DLURLCOMPLETEONLY",		"DLURLCOMPLETEWRITE",
		"DLURLPATH",		"DLURLPATHONLY",		"DLURLPATHWRITE",
		"DLURLSCHEME",		"DLURLSERVER",		"DLVALUE",
		"DO",		"DOCUMENT",		"DOMAIN",		"DOUBLE",
		"DROP",		"DYNAMIC",		"DYNAMIC_FUNCTION",
		"DYNAMIC_​FUNCTION_​CODE",		"EACH",		"ELEMENT",
		"ELSE",		"EMPTY",		"ENABLE",		"ENCODING",
		"ENCRYPTED",		"END",		"END-EXEC",		"END_FRAME",
		"END_PARTITION",		"ENFORCED",		"ENUM",		"EQUALS",
		"ERROR",		"ESCAPE",		"EVENT",		"EVERY",
		"EXCEPT",		"EXCEPTION",		"EXCLUDE",		"EXCLUDING",
		"EXCLUSIVE",		"EXEC",		"EXECUTE",		"EXISTS",
		"EXP",		"EXPLAIN",		"EXPRESSION",		"EXTENSION",
		"EXTERNAL",		"EXTRACT",		"FALSE",		"FAMILY",
		"FETCH",		"FILE",		"FILTER",		"FINAL",
		"FINISH",		"FIRST",		"FIRST_VALUE",		"FLAG",
		"FLOAT",		"FLOOR",		"FOLLOWING",		"FOR",
		"FORCE",		"FOREIGN",		"FORMAT",		"FORTRAN",
		"FORWARD",		"FOUND",		"FRAME_ROW",		"FREE",
		"FREEZE",		"FROM",		"FS",		"FULFILL",
		"FULL",		"FUNCTION",		"FUNCTIONS",		"FUSION",
		"G",		"GENERAL",		"GENERATED",		"GET",
		"GLOBAL",		"GO",		"GOTO",		"GRANT",
		"GRANTED",		"GREATEST",		"GROUP",		"GROUPING",
		"GROUPS",		"HANDLER",		"HAVING",		"HEADER",
		"HEX",		"HIERARCHY",		"HOLD",		"HOUR",
		"ID",		"IDENTITY",		"IF",		"IGNORE",
		"ILIKE",		"IMMEDIATE",		"IMMEDIATELY",		"IMMUTABLE",
		"IMPLEMENTATION",		"IMPLICIT",		"IMPORT",		"IN",
		"INCLUDE",		"INCLUDING",		"INCREMENT",		"INDENT",
		"INDEX",		"INDEXES",		"INDICATOR",		"INHERIT",
		"INHERITS",		"INITIAL",		"INITIALLY",		"INLINE",
		"INNER",		"INOUT","INPUT",		"INSENSITIVE",
		"INSERT",		"INSTANCE",		"INSTANTIABLE",		"INSTEAD",
		"INT",		"INTEGER",		"INTEGRITY",		"INTERSECT",
		"INTERSECTION",		"INTERVAL",		"INTO",		"INVOKER",
		"IS",		"ISNULL",		"ISOLATION",		"JOIN",
		"JSON",		"JSON_ARRAY",		"JSON_ARRAYAGG",	"JSON_EXISTS",
		"JSON_OBJECT",		"JSON_OBJECTAGG",		"JSON_QUERY",
		"JSON_TABLE",		"JSON_TABLE_PRIMITIVE",		"JSON_VALUE",
		"K",		"KEEP",		"KEY",		"KEYS",		"KEY_MEMBER",
		"KEY_TYPE",		"LABEL",		"LAG",		"LANGUAGE",
		"LARGE",		"LAST",		"LAST_VALUE",		"LATERAL",
		"LEAD",		"LEADING",		"LEAKPROOF",		"LEAST",
		"LEFT",		"LENGTH",		"LEVEL",		"LIBRARY",
		"LIKE",		"LIKE_REGEX",		"LIMIT",		"LINK",
		"LISTAGG",		"LISTEN",		"LN",		"LOAD",
		"LOCAL",		"LOCALTIME",		"LOCALTIMESTAMP",
		"LOCATION",		"LOCATOR",		"LOCK",		"LOCKED",
		"LOG",		"LOG10",		"LOGGED",		"LOWER",
		"M",		"MAP",		"MAPPING",		"MATCH",	"MATCHED",
		"MATCHES",		"MATCH_NUMBER",		"MATCH_RECOGNIZE",
		"MATERIALIZED",		"MAX",		"MAXVALUE",		"MEASURES",
		"MEMBER", "MERGE", "MESSAGE_LENGTH", "MESSAGE_OCTET_LENGTH",
		"MESSAGE_TXT",		"METHOD",		"MIN",		"MINUTE",
		"MINVALUE",		"MOD",		"MODE",		"MODIFIES",
		"MODULE",		"MONTH",		"MORE",		"MOVE",
		"MULTISET",		"MUMPS",		"NAME",		"NAMES",
		"NAMESPACE",		"NATIONAL",		"NATURAL",		"NCHAR",
		"NCLOB",		"NESTED",		"NESTING",		"NEW",
		"NEXT",		"NFC",		"NFD",		"NFKC",
		"NFKD",		"NIL",		"NO",		"NONE",
		"NORMALIZE",		"NORMALIZED",		"NOT",	"NOTHING",
		"NOTIFY",		"NOTNULL",		"NOWAIT",		"NTH_VALUE",
		"NTILE",		"NULL",		"NULLABLE",		"NULLIF",
		"NULLS",		"NUMBER",		"NUMERIC",		"OBJECT",
		"OCCURRENCES_REGEX",		"OCTETS",		"OCTET_LENGTH",
		"OF",		"OFF",		"OFFSET",		"OIDS",
		"OLD",		"OMIT",		"ON",		"ONE",		"ONLY",
		"OPEN",		"OPERATOR",		"OPTION",		"OPTIONS",
		"OR",		"ORDER",		"ORDERING",		"ORDINALITY",
		"OTHERS",		"OUT",		"OUTER",		"OUTPUT",
		"OVER",		"OVERFLOW",		"OVERLAPS",		"OVERLAY",
		"OVERRIDING",		"OWNED",		"OWNER",	"P",
		"PAD",		"PARALLEL",		"PARAMETER",	"PARAMETER_MODE",
		"PARAMETER_NAME",
		"PARAMETER_​ORDINAL_​POSITION",	"PARAMETER_​SPECIFIC_​CATALOG",
		"PARAMETER_​SPECIFIC_​NAME",	"PARAMETER_​SPECIFIC_​SCHEMA",
		"PARSER",		"PARTIAL",		"PARTITION",		"PASCAL",
		"PASS",		"PASSING",		"PASSTHROUGH",		"PASSWORD",
		"PAST",		"PATH",		"PATTERN",		"PER",
		"PERCENT",		"PERCENTILE_CONT",		"PERCENTILE_DISC",
		"PERCENT_RANK",		"PERIOD",		"PERMISSION",
		"PERMUTE",		"PLACING",		"PLAN",		"PLANS",
		"PLI",		"POLICY",		"PORTION",		"POSITION",
		"POSITION_REGEX",		"POWER",		"PRECEDES",
		"PRECEDING",		"PRECISION",		"PREPARE",
		"PREPARED",		"PRESERVE",		"PRIMARY",
		"PRIOR",		"PRIVATE",		"PRIVILEGES",
		"PROCEDURAL",		"PROCEDURE",	"PROCEDURES",
		"PROGRAM",		"PRUNE",		"PTF",		"PUBLIC",
		"PUBLICATION",		"QUOTE",		"QUOTES",
		"RANGE",		"RANK",		"READ",
		"READS",		"REAL",		"REASSIGN",
		"RECHECK",		"RECOVERY",		"RECURSIVE",
		"REF",		"REFERENCES",		"REFERENCING",
		"REFRESH",		"REGR_AVGX",		"REGR_AVGY",
		"REGR_COUNT",		"REGR_INTERCEPT",		"REGR_R2",
		"REGR_SLOPE",		"REGR_SXX",		"REGR_SXY",
		"REGR_SYY",		"REINDEX",		"RELATIVE",
		"RELEASE",		"RENAME",		"REPEATABLE",
		"REPLACE",		"REPLICA",		"REQUIRING",
		"RESET",		"RESPECT",		"RESTART",
		"RESTORE",		"RESTRICT",		"RESULT",
		"RETURN",		"RETURNED_CARDINALITY",
		"RETURNED_LENGTH",		"RETURNED_​OCTET_​LENGTH",
		"RETURNED_SQLSTATE",		"RETURNING",
		"RETURNS",		"REVOKE",		"RIGHT",
		"ROLE",		"ROLLBACK",		"ROLLUP",
		"ROUTINE",		"ROUTINES",		"ROUTINE_CATALOG",
		"ROUTINE_NAME",		"ROUTINE_SCHEMA",		"ROW",
		"ROWS",		"ROW_COUNT",		"ROW_NUMBER",
		"RULE",		"RUNNING",		"SAVEPOINT",
		"SCALAR",		"SCALE",		"SCHEMA",
		"SCHEMAS",		"SCHEMA_NAME",		"SCOPE",
		"SCOPE_CATALOG",	"SCOPE_NAME",	"SCOPE_SCHEMA",
		"SCROLL",		"SEARCH",		"SECOND",
		"SECTION",		"SECURITY",		"SEEK",
		"SELECT",		"SELECTIVE",		"SELF",
		"SENSITIVE",		"SEQUENCE",		"SEQUENCES",
		"SERIALIZABLE",		"SERVER",		"SERVER_NAME",
		"SESSION",		"SESSION_USER",		"SET",
		"SETOF",		"SETS",		"SHARE",
		"SHOW",		"SIMILAR",		"SIMPLE",
		"SIN",		"SINH",		"SIZE",
		"SKIP",		"SMALLINT",		"SNAPSHOT",
		"SOME",		"SOURCE",		"SPACE",
		"SPECIFIC",		"SPECIFICTYPE",		"SPECIFIC_NAME",
		"SQL",		"SQLCODE",		"SQLERROR",
		"SQLEXCEPTION",		"SQLSTATE",		"SQLWARNING",
		"SQRT",		"STABLE",		"STANDALONE",
		"START",		"STATE",		"STATEMENT",
		"STATIC",		"STATISTICS",		"STDDEV_POP",
		"STDDEV_SAMP",		"STDIN",		"STDOUT",
		"STORAGE",		"STORED",		"STRICT",
		"STRING",		"STRIP",		"STRUCTURE",
		"STYLE",		"SUBCLASS_ORIGIN",		"SUBMULTISET",
		"SUBSCRIPTION",		"SUBSET",		"SUBSTRING",
		"SUBSTRING_REGEX",		"SUCCEEDS",		"SUM",
		"SUPPORT",		"SYMMETRIC",		"SYSID",
		"SYSTEM",		"SYSTEM_TIME",		"SYSTEM_USER",
		"T",		"TABLE",		"TABLES",	"TABLESAMPLE",
		"TABLESPACE",		"TABLE_NAME",		"TAN",
		"TANH",		"TEMP",		"TEMPLATE",		"TEMPORARY",
		"TEXT",		"THEN",		"THROUGH",		"TIES",
		"TIME",		"TIMESTAMP",		"TIMEZONE_HOUR",
		"TIMEZONE_MINUTE",		"TO",		"TOKEN",
		"TOP_LEVEL_COUNT",		"TRAILING",		"TRANSACTION",
		"TRANSACTIONS_​COMMITTED",		"TRANSACTIONS_​ROLLED_​BACK",
		"TRANSACTION_ACTIVE",		"TRANSFORM",
		"TRANSFORMS",		"TRANSLATE",		"TRANSLATE_REGEX",
		"TRANSLATION",		"TREAT",		"TRIGGER",
		"TRIGGER_CATALOG",		"TRIGGER_NAME",		"TRIGGER_SCHEMA",
		"TRIM",		"TRIM_ARRAY",		"TRUE",		"TRUNCATE",
		"TRUSTED",		"TYPE",		"TYPES",		"UESCAPE",
		"UNBOUNDED",		"UNCOMMITTED",		"UNCONDITIONAL",
		"UNDER",		"UNENCRYPTED",		"UNION",
		"UNIQUE",		"UNKNOWN",		"UNLINK",
		"UNLISTEN",		"UNLOGGED",		"UNMATCHED",
		"UNNAMED",		"UNNEST",		"UNTIL",
		"UNTYPED",		"UPDATE",		"UPPER",
		"URI",		"USAGE",		"USER",
		"USER_​DEFINED_​TYPE​CATALOG",		"USER_​DEFINED_​TYPE_​CODE",
		"USER_​DEFINED_​TYPE_​NAME",		"USER_​DEFINED_​TYPE_​SCHEMA",
		"USING",		"UTF16",		"UTF32",		"UTF8",
		"VACUUM",		"VALID",	"VALIDATE",		"VALIDATOR",
		"VALUE",		"VALUES",		"VALUE_OF",		"VARBINARY",
		"VARCHAR",		"VARIADIC",		"VARYING",		"VAR_POP",
		"VAR_SAMP",		"VERBOSE",		"VERSION",		"VERSIONING",
		"VIEW",		"VIEWS",		"VOLATILE",		"WHEN",
		"WHENEVER",		"WHERE",		"WHITESPACE",		"WIDTH_BUCKET",
		"WINDOW",		"WITH",		"WITHIN",		"WITHOUT",
		"WORK",		"WRAPPER",		"WRITE",		"XML",
		"XMLAGG",		"XMLATTRIBUTES",		"XMLBINARY",	"XMLCAST",
		"XMLCOMMENT",		"XMLCONCAT",		"XMLDECLARATION",
		"XMLDOCUMENT",		"XMLELEMENT",		"XMLEXISTS",
		"XMLFOREST",		"XMLITERATE",		"XMLNAMESPACES",
		"XMLPARSE",		"XMLPI",		"XMLQUERY",
		"XMLROOT",		"XMLSCHEMA",		"XMLSERIALIZE",
		"XMLTABLE",		"XMLTEXT",		"XMLVALIDATE",
		"YEAR",		"YES",		"ZONE"
	};

	int c;
	bool normal=true;
	const char *p=s;
	while( (c=*p++)!='\0' )
	{
		if( (c<'0' || c> '9') && (c<'a' || c>'z') && (c<'A' || c>'Z')
			&& (c!='_') && (c!='$') )
		{
			normal=false; break;
		}
	}
	if(*s>='0' && *s<='9')
		normal=false;
	if(normal){
		const int n=sizeof(keywords)/sizeof(keywords[0]);
		const char**q=std::lower_bound(
			keywords,keywords+n, s,
			[](const char*a, const char*b)
				{ return stricmp(a,b)<0; }
		);
		if(q<keywords+n && stricmp(*q,s)==0 )
			normal=false;
	}
	String t;
	if(!normal)//do actual quoting
	{
		t<<'"';
		while( (c=*s++)!='\0' )
		{
			if(c=='\\' || c=='"')
				t<<'\\';
			t<<(char)c;
		}
		t<<'"';
	}else
		t=s;
	return t;
}
