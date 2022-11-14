#include "relocatable.hpp"
#include "vcore.hpp"

template <relocatable T/*, class _Alloc  */>
class vector
{
public:
      typedef T					value_type;
      typedef T*				pointer;
      typedef const T*			const_pointer;
      typedef T&				reference;
      typedef const T&			const_reference;
      typedef T*				iterator;
      typedef const T*			const_iterator;
      typedef std::reverse_iterator<const_iterator>	const_reverse_iterator;
      typedef std::reverse_iterator<iterator>		reverse_iterator;
      typedef size_t					size_type;
      typedef ptrdiff_t					difference_type;
//      typedef _Alloc					allocator_type;


public:
	constexpr vector() noexcept = default;
	constexpr vector( size_type count,
	                  const T& value/*,
	                  const _Alloc& alloc = _Alloc()*/ );
	constexpr explicit vector( size_type count/*,
				const _Alloc& alloc = _Alloc()*/ );
	template< class InputIt >
	constexpr vector( InputIt first, InputIt last/*,
	                  const _Alloc& alloc = _Alloc()*/ );
	constexpr vector( const vector& other/*, const _Alloc& alloc*/ )
	{
		reserve(other.size());
		for( ; core.used < other.size(); ++core.used){
			new( data()+core.used ) T( other[core.used] );
		}
	}
	constexpr vector( vector&& other ) noexcept{
		memcpy(this, &other, sizeof(other));
		memset(&other, 0, sizeof(other) );
	}

//	constexpr vector( vector&& other, const _Alloc& alloc );
	constexpr vector( std::initializer_list<T> init/*,
                  const _Alloc& alloc = _Alloc()*/ );
                  
	constexpr ~vector(){
	   for(uint32 i = 0; i < core.used; ++i)
	   {
	       (*this)[i].~T();
	   }
	   Upp::MemoryFree(core.data);
	}
	
	constexpr reference at( size_type pos ){
		if( pos >= core.used )
			throw std::out_of_range("vector::at(): index out of range");
		return data()[ pos ];
	}
	constexpr const_reference at( size_type pos ) const{
	   return const_cast<const_reference>(const_cast<reference>(*this).at(pos));
	}
	
	constexpr reference operator[]( size_type pos ){
		return data()[pos];
	}
	
	constexpr const_reference operator[]( size_type pos )const{
		return data()[pos];
	}
	
	constexpr reference front(){ return (*this)[0]; }
	constexpr const_reference front()const{ return (*this)[0]; }
	constexpr reference back(){ return (*this)[size()-1]; }
	constexpr const_reference back()const{ return (*this)[size()-1]; }
	
	constexpr T* data() noexcept{ return reinterpret_cast<T*>(core.data); }
	constexpr const T* data()const noexcept{ return reinterpret_cast<T const*>(core.data); }
	
	constexpr iterator begin() noexcept { return &front(); }
	constexpr const_iterator begin() const noexcept{ return &front(); }
	constexpr const_iterator cbegin() const noexcept{ return &front(); }
	constexpr iterator end() noexcept { return (T*)core.end(); }
	constexpr const_iterator end() const noexcept{ return (const T*)core.end(); }
	constexpr const_iterator cend() const noexcept{ return (const T*)core.end(); }
 
	constexpr reverse_iterator rbegin() noexcept{ return std::reverse_iterator(end()); }
	constexpr const_reverse_iterator rbegin() const noexcept{ return std::reverse_iterator(end);}
	constexpr const_reverse_iterator crbegin() const noexcept{ return rbegin(); }
	constexpr reverse_iterator rend() noexcept{ return std::reverse_iterator(begin()); }
	constexpr const_reverse_iterator rend() const noexcept{ return std::reverse_iterator(begin());}
	constexpr const_reverse_iterator crend() const noexcept{ return rend(); }
  
	[[nodiscard]] constexpr bool empty() const noexcept{ return core.used == 0; }
	constexpr size_type size() const noexcept{ return core.used; }
	constexpr size_type max_size() const noexcept{ return core.max_size; }
	constexpr void reserve( size_type new_cap );
	constexpr size_type capacity() const noexcept{ return core.alloced; }
	constexpr void shrink_to_fit(){ core.shrink_to_fit(); }
	
	constexpr void clear() noexcept;
	constexpr iterator insert( const_iterator pos, const T& value )
	{
		return insert_one(pos, value);
	}
	constexpr iterator insert( const_iterator pos, T&& value )
	{
		return insert_one(pos, std::move(value));
	}
	template< class InputIt >
	constexpr iterator insert( const_iterator pos, InputIt first, InputIt last );
	constexpr iterator insert( const_iterator pos,
	                       std::initializer_list<T> ilist );
	                       
	template< class... Args >
	constexpr iterator emplace( const_iterator pos, Args&&... args );
	
	constexpr iterator erase( const_iterator pos );
	constexpr iterator erase( const_iterator first, const_iterator last );
	
	constexpr void push_back( const T& value )
	{
		using trait = relocate_traits<T>;
		const T * prev;
		if constexpr( trait::value != 0 ){
			prev = data();
		}
		core.push_back();
		do_relocate_adjust(data(), prev, core.used-1, prev, prev+core.used-1);

		if constexpr ( std::is_nothrow_constructible_v<T, const T&> )
			new(core.back())T(value);
		else{
			try{
				new(core.back())T(value);
			}catch(...){
				--core.used;
				throw;
			}
		}
	}
	constexpr void push_back( T&& value )
	{
		using trait = relocate_traits<T>;
		const T * prev;
		if constexpr( trait::value != 0 ){
			prev = data();
		}
		
		core.push_back();
		do_relocate_adjust(data(), prev, core.used-1, prev, prev+core.used-1);
		
		if constexpr ( std::is_nothrow_constructible_v<T, T&&> )
			new( core.back() )T( std::move(value) );
		else{
			try{
				new(core.back() )T( std::move(value) );
			}catch(...){
				--core.used;
				throw;
			}
		}
	}
	
	template< class... Args >
	constexpr reference emplace_back( Args&&... args )
	{
		using trait = relocate_traits<T>;
		const T * prev;
		if constexpr( trait::value != 0 ){
			prev = data();
		}
		core.push_back();
		do_relocate_adjust(data(), prev, core.used-1, prev, prev+core.used-1);

		if constexpr ( std::is_nothrow_constructible_v<T, Args...> )
			new( core.back() )T( std::forward<Args>(args)... );
		else{
			try{
				new( core.back() )T( std::forward<Args>(args)... );
			}catch(...){
				--core.used;
				throw;
			}
		}

		return back();
	}
	
	constexpr void pop_back(){
		ASSERT( size()>0 );
		--core.used;
		data()[core.used].~T();
	}
	
	constexpr void resize( size_type count );
	constexpr void resize( size_type count, const value_type& value );
	
	constexpr void swap( vector& other ) noexcept
	{
		char buff[sizeof(other)];
		memcpy(buff, this, sizeof(other));
		memcpy(this, &other, sizeof(other));
		memcpy(&other, buff, sizeof(other));
	}
	
protected:
	vcore<sizeof(T)> core;
	
	using trait = relocate_traits<T>;
	
	void do_relocate_adjust(T* obj, const T* old,
		unsigned n, const T* prev_data_start, const T* prev_data_end);
		
	void prepare_insert_one(const_iterator pos);
	
	template <class U>
	constexpr iterator insert_one(const_iterator  pos, U u);
};

template <relocatable T>
constexpr vector<T>::vector( typename vector<T>::size_type count/*,
	                  const _Alloc& alloc = _Alloc()*/ )
{
	reserve(count);
	for(; core.used < count; ++core.used)
//	{
		new( data() + core.used ) T;
//		if constexpr (!std::is_nothrow_constructible_v<T, const T&>){
//			++ core.used; // exception saftey concern
//		}
//	}
//	if constexpr (std::is_nothrow_constructible_v<T, const T&>){
//		core.used = count;
//	}
	
}

template <relocatable T>
constexpr vector<T>::vector( typename vector<T>::size_type count, const T& value/*,
	                  const _Alloc& alloc = _Alloc()*/ )
{
	reserve(count);
	for(decltype(count) i=0; i<count; ++i)
	{
		new( (char*)core.data + i * core.element_size() ) T(value);
		if constexpr (!std::is_nothrow_constructible_v<T, const T&>){
			++ core.used; // exception saftey concern
		}
	}
	if constexpr (std::is_nothrow_constructible_v<T, const T&>){
		core.used = count;
	}
	
}

template <relocatable T>
constexpr void vector<T>::reserve( typename vector<T>::size_type new_cap )
{
	if( new_cap <= core.alloced)
		return;
	T* prev;
	
	if constexpr( trait::value != 0 ){
		prev = data();
	}

	core.reserve( new_cap );

	//if constexpr( trait::value != 0 ){
	do_relocate_adjust(data(), prev, core.used, prev, prev + core.used);
	
}

template <relocatable T>
constexpr void vector<T>::clear() noexcept
{
	for(int i = 0; i<core.used; ++i){
		(*this)[i].~T();
	}
	core.used = 0;
}

template< relocatable T>
template <class U>
constexpr typename vector<T>::iterator
vector<T>::insert_one(typename vector<T>::const_iterator  pos, U u)
{
	auto rbefore = pos - begin(); // # of record before insertion point

	ASSERT( rbefore >= 0 && rbefore <= size() );
	
	T * prev = data();

	if( size() == capacity() )[[unlikely]] // insertion will result in `prev` different from `data()`
	{
		T * newdata = (T*) raw_alloc ( sizeof(T) * core.NewSize() );
		try{
			new( newdata + rbefore ) T ( std::forward<U>(u) );
		}catch(...){
			raw_free ( newdata );
			throw;
		}
		memcpy_record<sizeof(T)>(newdata, data(), rbefore);
		memcpy_record<sizeof(T)>(newdata + rbefore +1, &*pos, core.used-rbefore);
		do_relocate_adjust(newdata, data(), rbefore, prev, prev + core.used);
		do_relocate_adjust(newdata + rbefore +1, &*pos, core.used - rbefore, prev, prev + core.used);
		core.data = newdata;
		core.alloced = core.NewSize();
		raw_free (prev);
	}else{
		memmove(const_cast<T*>(&*pos+1), &*pos, (core.used - rbefore)*sizeof(T) ); // make room
		try{
			new ( const_cast<T*>(&*pos) + rbefore ) T (std::forward<U>(u));
		}catch(...){ // restoration
			memmove( const_cast<T*>(&*pos), &*pos+1, (core.used - rbefore)*sizeof(T) );
			throw;
		}
		do_relocate_adjust(const_cast<T*>(&*pos)+1, &*pos, core.used - rbefore, prev, prev + core.used);
	}
	++core.used;
	return begin() + rbefore;
}

//template< relocatable T>
//constexpr typename vector<T>::iterator
//vector<T>::insert( typename vector<T>::const_iterator pos, const T& value )
//{
//	auto rbefore = pos - begin(); // # of record before insertion point
//
//	ASSERT( rbefore >= 0 && rbefore <= size() );
//	
//	T * prev = data();
//
//	if( size() == capacity() )[[unlikely]] // insertion will result in `prev` different from `data()`
//	{
//		T * newdata = (T*) raw_alloc ( sizeof(T) * core.NewSize() );
//		try{
//			new( newdata + rbefore ) T (value);
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
//			new ( const_cast<T*>(&*pos) + rbefore ) T (value);
//		}catch(...){ // restoration
//			memmove( const_cast<T*>(&*pos), &*pos+1, (core.used - rbefore)*sizeof(T) );
//			throw;
//		}
//		do_relocate_adjust(const_cast<T*>(&*pos)+1, &*pos, core.used - rbefore, prev, prev + core.used);
//	}
//	++core.used;
//	return begin() + rbefore;
//}
//
//template< relocatable T>
//constexpr typename vector<T>::iterator
//vector<T>::insert( typename vector<T>::const_iterator pos, T&& value )
//{
//	auto rbefore = pos - begin(); // # of record before insertion point
//
//	ASSERT( rbefore >= 0 && rbefore <= size() );
//	
//	T * prev = data();
//
//	if( size() == capacity() )[[unlikely]] // insertion will result in `prev` different from `data()`
//	{
//		T * newdata = (T*) raw_alloc ( sizeof(T) * core.NewSize() );
//		try{
//			new( newdata + rbefore ) T (std::move(value) );
//		}catch(...){
//			raw_free ( newdata );
//			throw;
//		}
//		memcpy_record<sizeof(T)>(newdata, data(), rbefore);
//		memcpy_record<sizeof(T)>(newdata + rbefore +1, &*pos, core.used-rbefore);
//		do_relocate_adjust(newdata, data(), rbefore, prev, prev + core.used);
//		do_relocate_adjust(newdata + rbefore +1, &*pos, core.used - rbefore, prev, prev + core.used);
//		//++core.used;
//		core.data = newdata;
//		core.alloced = core.NewSize();
//		raw_free (prev);
//	}else{
//		memmove(const_cast<T*>(&*pos+1), &*pos, (core.used - rbefore)*sizeof(T) ); // make room
//		try{
//			new ( const_cast<T*>(&*pos) + rbefore ) T (std::move(value) );
//		}catch(...){ // restoration
//			memmove( const_cast<T*>(&*pos), &*pos+1, (core.used - rbefore)*sizeof(T) );
//			throw;
//		}
//		do_relocate_adjust(const_cast<T*>(&*pos)+1, &*pos, core.used - rbefore, prev, prev + core.used);
//		//++core.used
//	}
//	++core.used;
//	return begin() + rbefore;
//	
//}

template< relocatable T>
	template<class InputIt >
constexpr typename vector<T>::iterator 
vector<T>::insert( typename vector<T>::const_iterator pos, InputIt first, InputIt last )
{
	// to be optimized with iterator_traits<InputIt>::iterator_category
	// eg. for random access iterator, we can reserve space first.
	
}

// `n` consecutive T objects have been moved from memory block pointed by `old`
// to memory block pointed by `obj`, now, please do adjustment to make all of them
// consistent.
//
// `prev_start` is this->data() before relocation,
// `prev_start` is (this->data() + core.used) befoe relocation
template< relocatable T>
inline void vector<T>::
do_relocate_adjust(T* obj, const T* old, unsigned n, const T* prev_start, const T* prev_end)
{
//	ASSERT(obj != old);
	if constexpr (trait::value == 0)
		return;
	else{
		if( old != obj && old ) [[likely]] // actually relocated
		{
			for(unsigned i=0; i < n; ++i)
				if constexpr (trait::value == 1)
					trait::Do(obj+i);
				else if constexpr (trait::value == 2 )
					trait::Do(obj+i, old+i);
				else if constexpr (trait::value == 4 )
					trait::Do(obj+i, old+i, prev_start, prev_end);
		}
	}
}
