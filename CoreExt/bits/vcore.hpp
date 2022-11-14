
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;
using int64 = std::int64_t;

// give them a different name so that we can switch and try different
// memory management utilities, eg malloc, c++ new(bad, you will need
// to allocate char[], wasting room on cookie that keeps track of number of
// chars, which is redundant in vector's case) , etc.
inline void * raw_alloc(size_t bytes){  return Upp::MemoryAllocSz ( bytes ); }
inline void raw_free(void * ptr)noexcept{ Upp::MemoryFree(ptr); }
template <unsigned size>
inline void memcpy_record ( void * dest, const void * src, size_t count )
{
	memcpy(dest, src, count*size);
	return;
	using namespace Upp;
#ifdef CPU_X86

	if constexpr ( size % 16 == 0 )
		memcpy128 ( dest, src, count * ( size >> 4 ) );
	else
		if constexpr ( size % 8 == 0 )
			memcpy64 ( dest, src, count * ( size >> 3 ) );
		else
#endif
			if constexpr ( size % 4 == 0 )
				memcpy32 ( dest, src, count * ( size >> 2 ) );
			else
				if constexpr ( size % 2 == 0 )
					memcpy16 ( dest, src, count * ( size >> 2 ) );
				else
					memcpy8 ( dest, src, count * size );
}

// a workhourse for vector where typename T is not important
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
		using std::max;
#ifdef _DEBUG
		return max ( alloced + 1, alloced >= max_size / 2 ? max_size : 2*alloced );
#else
		return max ( alloced + 1, alloced >= uint32 ( ( uint64 ) max_size*2 / 3 ) ? max_size
					 : alloced + ( alloced >> 1 ) );
#endif
	}

	bool ReAlloc ( unsigned newalloc );

	bool GrowSz()
	{
		if ( alloced == max_size )
			throw std::length_error("vector: items out of implementation limit");
			//Upp::Panic ( "Number of items in vector exceeds implementation limit" );

		return ReAlloc ( NewSize() );
	}

	void GrowF()
	{
		void * prev = data;

		if ( GrowSz() )
			raw_free ( prev );
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
};

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

	data = newdata;

	return alloc;
}
