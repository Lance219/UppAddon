
class RecordDef;

class FieldDef
{

	public:
		class FieldValue;
		
		FieldDef ( FieldTypeId id = FT_UNKNOWN, const char * name = "", unsigned count = 1, bool nullable = true )
				: nullbit ( unsigned ( -1 ) ),
				tid ( id ),
				nullable(nullable)
		{
			strncpy ( this->name, name, max_name_len );
			this->name[max_name_len] = '\0';
		}

		FieldDef ( const FieldDef& aFdef )
		{
//			struct S{
//				virtual ~S();
//				FieldTypeId tid;
//				std::int8_t nullable; // fixed sized bool
//				char        name[max_name_len+1];
//
//				unsigned	start;
//				unsigned	nullbit;
//			};
//			static_assert(std::is_polymorphic_v<FieldDef>);
//			static_assert(sizeof(*this) == sizeof(S) );
			memcpy(&tid, &aFdef.tid, sizeof(*this));
			// basically copy bitwise but leave the vptr alone.
//			if constexpr (lz::vptr_at_beginning)
//			{
//				memcpy(&tid, &aFdef.tid, sizeof(*this)-sizeof(void*));
//			}else{
//				memcpy((char*)this+sizeof(void*), (const char*)&aFdef+sizeof(void*),
//					sizeof(*this)-sizeof(void*));
//			}
		}


		// intended to be extended
		//
		//virtual ~FieldDef() {}

		FieldDef& SetNullable(bool v = true){
			nullable = static_cast<std::int8_t>(v);
			return *this;
		}

		bool IsNullable() const
		{
			return nullable;
		}

		size_t SizeInBits() const
		{
			return tid == FT_BIT ? 1 : 8*TypeSize();
		}

		size_t TypeAlign() const
		{
			return lz::TypeAlign ( tid );
		}

		size_t TypeSize() const
		{
			return lz::TypeSize ( tid );
		}

		unsigned GetStart() const
		{
			return start;
		}

		const char * GetFieldName() const
		{
			return name;
		}


		size_t GetFieldSize() const
		{
			return TypeSize();
		}


		bool FieldIsNull ( const void * base ) const
		{
			return /*IsNullable() &&*/ !lz::GetBit ( base, nullbit );
		}

		void MarkAsSet ( void * base )noexcept
		{
			if ( IsNullable() )
			{
				SetBit ( base, nullbit );
			}
		}

		void MarkAsNotSet ( void *base )
		{
			if ( IsNullable() )
				UnsetBit ( base, nullbit );
		}

		const static size_t max_name_len = 61;

		FieldTypeId GetFieldType() const
		{
			return tid;
		}

	protected:
		FieldTypeId tid;
		std::int8_t nullable;
		char        name[max_name_len+1];

		unsigned	start;
		unsigned	nullbit;


		friend class RecordDef;

		friend class FieldValue;

		friend class RecordSet;
};

