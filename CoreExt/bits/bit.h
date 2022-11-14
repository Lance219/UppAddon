class bit{

public:
	bool get()const;
	operator bool()const{ return get(); }

	void * base;
	size_t index;
};