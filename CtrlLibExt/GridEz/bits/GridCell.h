template <class T> class CellT;

class CellData
{

	public:
		virtual ~CellData() {}

		virtual const char * GetDataPChar() const
		{
			return nullptr;
		}

		virtual String GetDataString() const
		{
			return Null;
		}

		virtual Value GetData() const
		{
			return Null;
		}

		virtual Ctrl* GetCtrl() const
		{
			return nullptr;
		}

		virtual const CellFormat* GetFormat() const
		{
			return nullptr;
		}

};


class Cell : public Moveable<Cell>
{

	public:
		~Cell();
		Cell() : pCellData ( nullptr ) {}
		Cell ( std::nullptr_t ) : Cell() {}

		template <class T>
		Cell ( T&& t )
		{
			if constexpr (std::is_convertible_v< std::remove_cvref_t<T>, Ctrl*>)
			{
				std::construct_at(this, (Ctrl*)t, false);
			}else{
				pCellData = new CellT ( std::forward<T>(t) );
			}
		}

		Cell ( StrLiteral text, bool isT = false );
		Cell ( String s );

		Cell ( Ctrl * ctrl, bool owned = false )
		{
			if ( ctrl != nullptr )
				MaskAsCtrlPointer ( MarkAsOwned ( ctrl, owned ) );
			else
				pCellData = nullptr;
		}

		Cell ( Cell && cell ) : Cell()
		{
			pCellData      = cell.pCellData;
			cell.pCellData = nullptr;
		}

		Cell ( const Cell& ) = delete;
		Cell& operator = ( Cell && cell )
		{
			this->~Cell();
			std::construct_at(this, std::move(cell));
			return *this;
		}

		template<CtrlClass T, typename...Args>
		T& Create ( Args && ...args )
		{
			T * t;
			{
				One<T> p;

				p.Create ( std::forward(args)... );
				t = p.Detach();
			}

			this->~Cell();
			std::construct_at(this, t, true);
			return *t;
		}

		const char * GetDataPChar() const;

		String GetDataString() const
		{
			return pCellData == nullptr ? String() :
				   ( lz::AsInt ( pCellData ) & 3 ) == 0 ?
				   pCellData->GetDataString() :
				   String ( GetDataPChar() );
		}

		Value GetData() const
		{
			if ( !pCellData )
				return Null;

			switch ( AsInt ( pCellData ) & 3 )
			{

				case 1: //StrLiteral
					return Value ( ( const char* ) pCellData - 1 );

				case 2:
					return reinterpret_cast<const Ctrl*> (
							   ( const char* ) pCellData - 2
						   );

				case 3:
					return reinterpret_cast<const Ctrl*> (
							   ( const char* ) pCellData - 3
						   );
			};

			return pCellData->GetData();
		}

		Ctrl* GetCtrl() const
		{
			switch ( int f = GetFlags() )
			{

				case 0:
					return pCellData ? pCellData->GetCtrl() : nullptr;

				case 2:
				case 3:
					return reinterpret_cast<Ctrl*> (
							   lz::AsInt ( pCellData ) - f
						   );
//
//				case 3:
//					return reinterpret_cast<Ctrl*> (
//							   lz::AsInt ( pCellData ) - 3
//						   );
			}

			return nullptr;
		}

		const CellFormat& GetFormat() const
		{
			return pCellData == nullptr ||
				   GetFlags() != 0 ?
				   CellFormat::NullCellFormat() :
				   *pCellData->GetFormat();

		}

		const static Cell& NullCell()
		{
			static Cell cell;
			return cell;
		}

	private:
		CellData * pCellData = nullptr;
	#ifdef CPU_64
		int GetFlags()const{
			return AsInt( pCellData ) & 7;
		}
		void SetFlags(int flag){
			ASSERT(flag>=0 && flag <=7);
			*reinterpret_cast<ptrint*>(&pCellData) |= ((1<<3)-1);
//			pCellData = ptr_as_int(pCellData) |= ((1<<2)-1);
		}
	#elif defined(CPU_32)
		int32		flag;
		int GetFlags()const{
			return flags;
		}
		void SetFlags(int _flag){
			flag = _flag;
		}
	#endif

		void MaskAsASCIIZPointer ( const char* p )
		{
			lz::AsInt ( pCellData ) = lz::AsInt ( p ) + 1;
		}

		void MaskAsCtrlPointer ( Ctrl* ctrl )
		{
			lz::AsUInt ( pCellData ) = lz::AsUInt ( ctrl ) + 3;
		}

		// 2021-12-05 AsUInt(ctrl)+3 now free to be taken!!
		void MaskAsOwnedCtrlPointer ( Ctrl* ctrl )
		{
			MaskAsCtrlPointer ( MarkAsOwned ( ctrl ) );
		}

//		int PiggyAttribute() const
//		{
//			return lz::AsInt ( pCellData ) & 3;
//		}

		bool IsCtrlPointer() const
		{
			return ( lz::AsInt ( pCellData ) & 3 ) == 2;
		}

		//bool IsOwnedCtrlPointer()const	{ return ( AsInt(pCellData) & 3 )==3; }
		bool IsASCIIZPointer() const
		{
			return ( lz::AsInt ( pCellData ) & 3 ) == 1;
		}
};

template <class T>

class CellT : public CellData
{

	public:
		//CellT(const T& t) :t(t){}
		CellT ( T t ) : t ( t ) {}

		virtual Upp::Value GetData() const
		{
			return t;
		}

		virtual Upp::String GetDataString() const
		{
			return String().Cat() << t;
		}

	private:
		T t;
};


class CellPChar : public CellData
{

	public:
		CellPChar ( const char * text, bool isT = false )
				: text ( text ), isT ( isT )
		{
		}

		const char * GetDataPChar() const override
		{
			return isT ? t_ ( text ) : text;
	}

		Upp::Value GetData() const override
		{
			return CellPChar::GetDataPChar();
	}

		Upp::String GetDataString() const override
		{
			return CellPChar::GetDataPChar();
	}

	private:
		const char * text;
		bool		 isT;
};


template<>

class CellT<Value> : public CellData
{

	public:
		CellT ( const Upp::Value& v, const Upp::Convert& conv = Upp::StdConvert() )
				: value ( v ), convert ( &conv )
		{
		}

		virtual Upp::Value GetData() const
		{
			return value;
		}

		virtual Upp::String GetDataString() const
		{
			return convert->Format ( value );
		}

	private:
		Upp::Value value;
		const Upp::Convert* convert;
};

template<>

class CellT<String> : public CellData
{

	public:
		CellT ( const Upp::String& text, bool isT = false )
				: text ( text ), isT ( isT )
		{
		}

		virtual const char * GetDataPChar() const
		{
			return isT ? t_ ( text ) : static_cast<const char*> ( text );
		}

		virtual Upp::Value GetData() const
		{
			return GetDataString();
		}

		virtual Upp::String GetDataString() const
		{
			return isT ? Upp::String ( t_ ( text ) ) : Upp::String ( text );
		}

	private:
		Upp::String text;
		bool	isT;
};

