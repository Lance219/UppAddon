
using uint32 = Upp::uint32;
using uint64 = Upp::uint64;
using int64 = Upp::int64;

// give them a different name so that we can switch and try different
// memory management utilities, eg malloc, c++ new(bad, you will need
// to allocate char[], wasting room on cookie that keeps track of number of
// chars, which is redundant in vector's case) , etc.
//inline void * raw_alloc(size_t bytes){  return Upp::MemoryAllocSz ( bytes ); }
//inline void raw_free(void * ptr)noexcept{ Upp::MemoryFree(ptr); }
//inline void * raw_alloc(size_t bytes){  return std::malloc ( bytes ); }
//inline void raw_free(void * ptr)noexcept{ std::free(ptr); }
inline void * raw_alloc(size_t bytes){  return new char[bytes]; }
inline void raw_free(void * ptr)noexcept{ delete[] (char*)ptr; }

template <unsigned size> inline constexpr void
memcpy_record ( void * dest, const void * src, size_t count )
{
//	memcpy(dest, src, count*size);
//	return;
	using namespace Upp;
#ifdef CPU_X86

	if constexpr ( size % 16 == 0 )
		memcpy128 ( dest, src, count * ( size >> 4 ) );
	else if constexpr ( size % 8 == 0 )
		memcpy64 ( dest, src, count * ( size >> 3 ) );
	else
#endif
	if constexpr ( size % 4 == 0 )
		memcpy32 ( dest, src, count * ( size >> 2 ) );
	else if constexpr ( size % 2 == 0 )
		memcpy16 ( dest, src, count * ( size >> 1 ) );
	else
		memcpy8 ( dest, src, count * size );
}

// a workhourse for vector where typename T is not relevant
//
// we aim to put all the more labour intensive work in this helper class,
// so that, for example, vector<long>, vector<unsigned long>, vector<long long>,
// vector<unsigned long long>, vector<double>, vector<T*>, and other builtin/user
// defined class of size 8 bytes will share the same vcore. In `vector`,
// most functions should be inline'd and deliver work load to its vcore object.
//
// The purpose of this design is to minimize executable size.
//
template < unsigned size/*, class _Alloc*/ >
struct vcore
{
	void		*data = nullptr;
	uint32		used = 0;
	uint32		alloced = 0;
//	[[no_unique_address]] _Alloc alloc;

	static constexpr auto max_size = std::numeric_limits < decltype ( alloced ) >::max();
	
	static constexpr auto element_size(){ return size; }

	auto	NewSize() const
	{
//		using std::max;
//#ifdef _DEBUG
//		return max ( alloced + 1, alloced >= max_size / 2 ? max_size : 2*alloced );
//#else
//		return max ( alloced + 1, alloced >= uint32 ( ( uint64 ) max_size*2 / 3 ) ? max_size
//					 : alloced + ( alloced >> 1 ) );
//#endif
		return alloced <=4 ? 8 : alloced >= max_size / 2 ? max_size : 2*alloced;
	}

	bool ReAlloc ( unsigned newalloc );

	void GrowSz()
	{
		if ( alloced == max_size )[[unlikely]]
			throw std::length_error("vector: items exceed implementation limit");
			//Upp::Panic ( "Number of items in vector exceeds implementation limit" );

		auto newalloc = NewSize();
		
		ASSERT( newalloc > 0 && newalloc >= used );
		
		auto sz = (size_t) newalloc * size;
		void * p = raw_alloc ( sz );
		if( data ){
			memcpy_record<size> ( p, data, used );
			raw_free ( data );
		}
		data = p;
		alloced = newalloc;
	}

	void GrowF()
	{
//		void * prev = data;

		GrowSz();
//		if ( GrowSz() )
//			raw_free ( prev );
	}

	void push_back()
	{
		if ( used == alloced )[[ unlikely ]]
		{
			GrowF();
		}

		++used;
	}

	void reserve(size_t new_cap)
	{
		void * prev = data;
		if( new_cap > max_size )
			throw std::length_error("vector: items out of implementation limit");
		if( new_cap > alloced ) [[likely]]
		{
			if( ReAlloc ( new_cap ) )
				raw_free ( prev );
		}
	}

	void * operator[] ( unsigned n )
	{
		assert ( n<= used );
		return ( char* ) data + n * size;
	}

//	void * element(unsigned n){ assert( n<= used ); return (char*)data + n * size; }
	void * back()
	{
		assert ( used != 0 );
		return ( *this ) [used-1];
	}

	void * begin()
	{
		return data;
	}

	void * end()
	{
		return ( char* ) data + used* size;
	}

	void shrink_to_fit()
	{
		(void)ReAlloc ( used );
	}
	
	// return data if no new allocation needed
	//        newly allocated block if one is allocated
	//		  may throw when allocation failed
	void * insert_one(decltype(used) n);
};

//template < unsigned size/*, class _Alloc*/ >
//void vcore < size/*, _Alloc*/ >::ReAlloc ( unsigned newalloc )
//{
//}

template < unsigned size/*, class _Alloc*/ >
bool vcore < size/*, _Alloc*/ >::ReAlloc ( unsigned newalloc )
{
	ASSERT ( newalloc >= used );
	// is there actual use case with `newalloc` == 0 ?
	// if not, the following check can be done without.

	if ( newalloc == 0 )[[ unlikely ]]
		return false;

	auto sz0 = ( size_t ) newalloc * size;

	auto sz = sz0;

	void * newdata;

	bool alloc = true;

	if ( Upp::MemoryTryRealloc ( data, sz ) )
	{
		newdata = data;
		data = nullptr;
		alloc = false;
	}
	else
		newdata = raw_alloc ( sz );

	alloced = newalloc == max_size ? max_size
			  : ( uint32 ) ( ( ( int64 ) sz - sz0 ) / size + newalloc );

	if ( data && newdata )
		memcpy_record<size> ( newdata, data, used );
//		memcpy ( newdata, data, used * size );

	data = newdata;

	return alloc;
}

//template <unsigned size>
//void * vcore < size/*, _Alloc*/ >::insert_one(decltype(used) n)
//{
//
//	ASSERT( n <= used );
//
//	void * prev = data;
//	
//	if( used == alloced && ReAlloc( NewSize() ) )[[unlikely]]
//	// insertion results in `prev` different from `data`
//	{
//		T * newdata = (T*) raw_alloc ( sizeof(T) * core.NewSize() );
//		try{
//			new( newdata + rbefore ) T ( std::forward<U>(u) );
//		}catch(...){
//			raw_free ( newdata );
//			throw;
//		}
//		memcpy_record<sizeof(T)>(newdata, data(), rbefore);
//		memcpy_record<sizeof(T)>(newdata + rbefore +1, &*pos, core.used-rbefore);
//		do_relocate_adjust(newdata, data(), rbefore, prev, prev + core.used);
//		do_relocate_adjust(newdata + rbefore +1, &*pos, core.used - rbefore, prev, prev + core.used);
//		core.data = newdata;
//		core.alloced = core.NewSize();
//		raw_free (prev);
//	}else{
//		memmove(const_cast<T*>(&*pos+1), &*pos, (core.used - rbefore)*sizeof(T) ); // make room
//		try{
//			new ( const_cast<T*>(&*pos) + rbefore ) T (std::forward<U>(u));
//		}catch(...){ // restoration
//			memmove( const_cast<T*>(&*pos), &*pos+1, (core.used - rbefore)*sizeof(T) );
//			throw;
//		}
//		do_relocate_adjust(const_cast<T*>(&*pos)+1, &*pos, core.used - rbefore, prev, prev + core.used);
//	}
//	++core.used;
//	return begin() + rbefore;
//}
