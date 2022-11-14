#include <Core/Core.h>
#include <CoreExt/CoreExt.h>

#include <vector>

using namespace Upp;

const int N = 400000;
const int M = 30;
const size_t buffsize = 128;

struct Buff{
	Buff()=default;
	Buff(const Buff&)=default;
	Buff(Buff&)=default;
	char buff[buffsize];
};

namespace Upp{
NTL_MOVEABLE(Buff);
}

void TestInt();
void TestIntInsert();
void TestString();
void TestStdString();
void TestCharBuffer();

CONSOLE_APP_MAIN
{
	TestCharBuffer();
//	TestInt();
//	TestIntInsert();
}

void TestCharBuffer()
{
	for(int i=0; i < M; ++i)
	{
		{
			RTIMING("std::vector<Buff>::push_back");
			std::vector<Buff> v;
			for(int i = 0; i < N; i++){
				Buff b;
				v.push_back(b);
			}
		}
		{
			RTIMING("lz::vector<Buff>::push_back");
			lz::vector<Buff> v;
			for(int i = 0; i < N; i++){
				Buff b;
				v.push_back(b);
			}
		}
		{
			RTIMING("Upp::Vector<Buff>::push_back");
			Upp::Vector<Buff> v;
			for(int i = 0; i < N; i++){
				Buff b;
				v.Add(b);
			}
		}

	}
}

void TestInt()
{
	for(int i=0; i < M; ++i)
	{
		{
			RTIMING("std::vector<int>::push_back");
			std::vector<int> v;
			for(int i = 0; i < N; i++)
				v.push_back(i);
		}
		{
			RTIMING("lz::vector<int>::push_back");
			lz::vector<int> v;
			for(int i = 0; i < N; i++)
				v.push_back(i);
		}
		{
			RTIMING("Upp::Vector<int>::push_back");
			Upp::Vector<int> v;
			for(int i = 0; i < N; i++)
				v.push_back(i);
		}

	}
}

void TestIntInsert()
{
	for(int i=0; i < M; ++i)
	{
		{
			RTIMING("std::vector<int>::insert");
			std::vector<int> v;
			for(int i = 0; i < N; i++)
				v.insert(v.begin(), i);
		}
		{
			RTIMING("lz::vector<int>::insert");
			lz::vector<int> v;
			for(int i = 0; i < N; i++)
				v.insert(v.begin(), i);
		}
		{
			RTIMING("Upp::Vector<int>::insert");
			Upp::Vector<int> v;
			for(int i = 0; i < N; i++)
				v.Insert(0, i);
		}

	}
}

void TestStdString()
{
	using namespace std;
	Vector<String> data;
	Buffer<std::string> sdata(N);
	
	for(int i = 0; i < N; i++) {
		data << AsString(i);
		sdata[i] = data[i].ToStd();
	}
	
	for(int i=0; i < 1000; ++i)
	{
		{
			RTIMING("vector<string>::push_back");
			std::vector<string> v;
			for(int i = 0; i < N; i++)
				v.push_back(sdata[i]);
			LOG(v[N-1]);
		}
		{
			RTIMING("lz::vector<string>::push_back");
			lz::vector<string> v;
			for(int i = 0; i < N; i++)
				v.push_back(sdata[i]);
			LOG(v[N-1]);
		}
//		{
//			RTIMING("Upp::Vector<String>::push_back");
//			Upp::Vector<int> v;
//			for(int i = 0; i < N; i++)
//				v.push_back(i);
//		}
//
	}
}


//#include <Core/Core.h>
//
//#include <vector>
//#include <string>
//#include <CoreExt/CoreExt.h>
//
//using namespace Upp;
//
//CONSOLE_APP_MAIN
//{
//	const int N = 10000;
//	const int M = 1000;
//	
//	Vector<String> data;
//	Buffer<std::string> sdata(N);
//	
//	for(int i = 0; i < N; i++) {
//		data << AsString(i);
//		sdata[i] = data[i].ToStd();
//	}
//	
//	RDUMP(sizeof(std::string));
//	RDUMP(sizeof(String));
//	
//	for(int q = M; q--;) {
//		{
//			RTIMING("vector<string>::emplace_back");
//			std::vector<std::string> v;
//			for(int i = 0; i < N; i++)
//				v.emplace_back(sdata[i]);
//		}
//		{
//			RTIMING("lz::vector<string>::push_back");
//			lz::vector<std::string> v;
//			for(int i = 0; i < N; i++)
//				v.emplace_back(sdata[i]);
//		}
//		{
//			RTIMING("vector<String>::emplace_back");
//			std::vector<String> v;
//			for(int i = 0; i < N; i++)
//				v.emplace_back(data[i]);
//		}
//		{
//			RTIMING("lz::vector<String>::emplace_back");
//			lz::vector<String> v;
//			for(int i = 0; i < N; i++)
//				v.emplace_back(data[i]);
//		}
//		{
//			RTIMING("Vector<String>::Add");
//			Vector<String> v;
//			for(int i = 0; i < N; i++)
//				v.Add(data[i]);
//		}
//		{
//			RTIMING("vector<string>::insert");
//			std::vector<std::string> v;
//			for(int i = 0; i < N; i++)
//				v.insert(v.begin(), sdata[i]);
//		}
//		{
//			RTIMING("lz::vector<string>::insert");
//			lz::vector<std::string> v;
//			for(int i = 0; i < N; i++)
//				v.insert(v.begin(), sdata[i]);
//		}
//		{
//			RTIMING("vector<String>::insert");
//			std::vector<String> v;
//			for(int i = 0; i < N; i++)
//				v.insert(v.begin(), sdata[i]);
//		}
//		{
//			RTIMING("lz::vector<String>::insert");
//			lz::vector<String> v;
//			for(int i = 0; i < N; i++)
//				v.insert(v.begin(), sdata[i]);
//		}
//		{
//			RTIMING("Vector<String>::Insert");
//			Vector<String> v;
//			for(int i = 0; i < N; i++)
//				v.Insert(0, data[i]);
//		}
//		{
//			std::vector<std::string> v;
//			for(int i = 0; i < N; i++)
//				v.insert(v.begin(), sdata[i]);
//			RTIMING("vector<string>::erase");
//			while(v.size())
//				v.erase(v.begin());
//		}
//		{
//			std::vector<String> v(~sdata, ~sdata + N);
//			RTIMING("vector<String>::erase");
//			while(v.size())
//				v.erase(v.begin());
//		}
//		{
//			Vector<String> v;
//			for(int i = 0; i < N; i++)
//				v.Insert(0, data[i]);
//			RTIMING("Vector<String>::Remove");
//			while(v.GetCount())
//				v.Remove(0);
//		}
//		for(int j = 0; j < 100; j++) {
//			{
//				std::vector<std::string> v(~sdata, ~sdata + N);
//				RTIMING("std::remove_if");
//				v.erase(std::remove_if(v.begin(), v.end(), [](const std::string& h) { return h[0] == '1'; }), v.end());
//			}
//			{
//				Vector<String> v = clone(data);
//				RTIMING("Vector::RemoveIf");
//				v.RemoveIf([&](int i) { return v[i][0] == '1'; });
//			}
//		}
//	}
//}

/*
sizeof(std::string) = 32
sizeof(String) = 16
TIMING Vector::RemoveIf: 97.81 ms -  9.78 us (98.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING std::remove_if : 306.81 ms - 30.68 us (307.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING Vector<String>::Remove:  1.37 s  - 13.69 ms ( 1.37 s  / 100 ), min: 13.00 ms, max: 18.00 ms, nesting: 0 - 100
TIMING vector<String>::erase:  3.53 s  - 35.34 ms ( 3.53 s  / 100 ), min: 33.00 ms, max: 42.00 ms, nesting: 0 - 100
TIMING vector<string>::erase: 14.58 s  - 145.76 ms (14.58 s  / 100 ), min: 138.00 ms, max: 161.00 ms, nesting: 0 - 100
TIMING Vector<String>::Insert:  1.22 s  - 12.23 ms ( 1.22 s  / 100 ), min: 11.00 ms, max: 17.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::insert:  1.24 s  - 12.40 ms ( 1.24 s  / 100 ), min: 11.00 ms, max: 17.00 ms, nesting: 0 - 100
TIMING vector<String>::insert:  3.56 s  - 35.65 ms ( 3.56 s  / 100 ), min: 34.00 ms, max: 39.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::insert:  6.09 s  - 60.87 ms ( 6.09 s  / 100 ), min: 58.00 ms, max: 89.00 ms, nesting: 0 - 100
TIMING vector<string>::insert: 16.74 s  - 167.44 ms (16.74 s  / 100 ), min: 157.00 ms, max: 360.00 ms, nesting: 0 - 100
TIMING Vector<String>::Add:  4.00 ms - 39.98 us ( 4.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::push_back:  6.00 ms - 59.98 us ( 6.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<String>::push_back:  8.00 ms - 79.98 us ( 8.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::push_back:  5.00 ms - 49.98 us ( 5.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<string>::push_back: 14.00 ms - 139.98 us (14.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100

* /home/lance/.cache/upp.out/benchmarks/CLANG.Blitz.Shared/Vector 13.11.2022 14:16:31, user: lance

sizeof(std::string) = 32
sizeof(String) = 16
TIMING Vector::RemoveIf: 117.82 ms - 11.78 us (118.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING std::remove_if : 302.82 ms - 30.28 us (303.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING Vector<String>::Remove:  1.18 s  - 11.81 ms ( 1.18 s  / 100 ), min: 11.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING vector<String>::erase:  3.47 s  - 34.69 ms ( 3.47 s  / 100 ), min: 33.00 ms, max: 38.00 ms, nesting: 0 - 100
TIMING vector<string>::erase: 14.51 s  - 145.07 ms (14.51 s  / 100 ), min: 137.00 ms, max: 162.00 ms, nesting: 0 - 100
TIMING Vector<String>::Insert:  1.15 s  - 11.51 ms ( 1.15 s  / 100 ), min: 10.00 ms, max: 13.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::insert:  1.15 s  - 11.48 ms ( 1.15 s  / 100 ), min: 10.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING vector<String>::insert:  3.51 s  - 35.14 ms ( 3.51 s  / 100 ), min: 33.00 ms, max: 38.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::insert:  5.82 s  - 58.23 ms ( 5.82 s  / 100 ), min: 55.00 ms, max: 62.00 ms, nesting: 0 - 100
TIMING vector<string>::insert: 16.43 s  - 164.31 ms (16.43 s  / 100 ), min: 156.00 ms, max: 190.00 ms, nesting: 0 - 100
TIMING Vector<String>::Add:  3.00 ms - 29.98 us ( 3.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::push_back:  3.00 ms - 29.98 us ( 3.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<String>::push_back:  5.00 ms - 49.98 us ( 5.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::push_back: 12.00 ms - 119.98 us (12.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<string>::push_back: 20.00 ms - 199.98 us (20.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100

After memcpy_record using memcpy
TIMING Vector::RemoveIf: 171.84 ms - 17.18 us (172.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING std::remove_if : 368.84 ms - 36.88 us (369.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING Vector<String>::Remove:  1.20 s  - 11.98 ms ( 1.20 s  / 100 ), min: 11.00 ms, max: 15.00 ms, nesting: 0 - 100
TIMING vector<String>::erase:  3.50 s  - 35.00 ms ( 3.50 s  / 100 ), min: 32.00 ms, max: 38.00 ms, nesting: 0 - 100
TIMING vector<string>::erase: 16.58 s  - 165.79 ms (16.58 s  / 100 ), min: 157.00 ms, max: 186.00 ms, nesting: 0 - 100
TIMING Vector<String>::Insert:  1.16 s  - 11.58 ms ( 1.16 s  / 100 ), min: 10.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::insert:  1.17 s  - 11.74 ms ( 1.17 s  / 100 ), min: 11.00 ms, max: 13.00 ms, nesting: 0 - 100
TIMING vector<String>::insert:  3.52 s  - 35.16 ms ( 3.52 s  / 100 ), min: 33.00 ms, max: 38.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::insert:  4.75 s  - 47.46 ms ( 4.75 s  / 100 ), min: 45.00 ms, max: 52.00 ms, nesting: 0 - 100
TIMING vector<string>::insert: 18.98 s  - 189.85 ms (18.98 s  / 100 ), min: 182.00 ms, max: 208.00 ms, nesting: 0 - 100
TIMING Vector<String>::Add:  3.00 ms - 29.98 us ( 3.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::push_back:  4.00 ms - 39.98 us ( 4.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<String>::push_back:  4.00 ms - 39.98 us ( 4.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::push_back: 15.00 ms - 149.98 us (15.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<string>::push_back: 16.00 ms - 159.98 us (16.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100

Change memcpy_record back to mirek's way
let vector and lz::vector do emplace_back instead
sizeof(std::string) = 32
sizeof(String) = 16
TIMING Vector::RemoveIf: 146.83 ms - 14.68 us (147.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING std::remove_if : 342.83 ms - 34.28 us (343.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING Vector<String>::Remove:  1.21 s  - 12.07 ms ( 1.21 s  / 100 ), min: 11.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING vector<String>::erase:  3.50 s  - 34.99 ms ( 3.50 s  / 100 ), min: 33.00 ms, max: 40.00 ms, nesting: 0 - 100
TIMING vector<string>::erase: 16.73 s  - 167.34 ms (16.73 s  / 100 ), min: 159.00 ms, max: 182.00 ms, nesting: 0 - 100
TIMING Vector<String>::Insert:  1.17 s  - 11.67 ms ( 1.17 s  / 100 ), min: 11.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::insert:  1.20 s  - 11.97 ms ( 1.20 s  / 100 ), min: 11.00 ms, max: 17.00 ms, nesting: 0 - 100
TIMING vector<String>::insert:  3.54 s  - 35.40 ms ( 3.54 s  / 100 ), min: 33.00 ms, max: 39.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::insert:  6.08 s  - 60.77 ms ( 6.08 s  / 100 ), min: 58.00 ms, max: 68.00 ms, nesting: 0 - 100
TIMING vector<string>::insert: 17.74 s  - 177.43 ms (17.74 s  / 100 ), min: 166.00 ms, max: 318.00 ms, nesting: 0 - 100
TIMING Vector<String>::Add:  4.00 ms - 39.98 us ( 4.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::emplace_back:  6.00 ms - 59.98 us ( 6.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<String>::emplace_back:  4.00 ms - 39.98 us ( 4.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::push_back:  7.00 ms - 69.98 us ( 7.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<string>::emplace_back: 14.00 ms - 139.98 us (14.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100

* /home/lance/.cache/upp.out/benchmarks/CLANG.Blitz.Shared/Vector 13.11.2022 14:46:58, user: lance

sizeof(std::string) = 32
sizeof(String) = 16
TIMING Vector::RemoveIf: 152.79 ms - 15.28 us (153.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING std::remove_if : 346.79 ms - 34.68 us (347.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING Vector<String>::Remove:  1.23 s  - 12.26 ms ( 1.23 s  / 100 ), min: 11.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING vector<String>::erase:  3.55 s  - 35.49 ms ( 3.55 s  / 100 ), min: 33.00 ms, max: 40.00 ms, nesting: 0 - 100
TIMING vector<string>::erase: 16.81 s  - 168.09 ms (16.81 s  / 100 ), min: 157.00 ms, max: 180.00 ms, nesting: 0 - 100
TIMING Vector<String>::Insert:  1.18 s  - 11.78 ms ( 1.18 s  / 100 ), min: 11.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::insert:  1.19 s  - 11.94 ms ( 1.19 s  / 100 ), min: 11.00 ms, max: 13.00 ms, nesting: 0 - 100
TIMING vector<String>::insert:  3.60 s  - 36.00 ms ( 3.60 s  / 100 ), min: 33.00 ms, max: 40.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::insert:  6.16 s  - 61.65 ms ( 6.17 s  / 100 ), min: 59.00 ms, max: 67.00 ms, nesting: 0 - 100
TIMING vector<string>::insert: 17.64 s  - 176.45 ms (17.64 s  / 100 ), min: 166.00 ms, max: 190.00 ms, nesting: 0 - 100
TIMING Vector<String>::Add:  3.00 ms - 29.98 us ( 3.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::emplace_back: 997.93 us -  9.98 us ( 1.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<String>::emplace_back:  3.00 ms - 29.98 us ( 3.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::push_back: 11.00 ms - 109.98 us (11.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<string>::emplace_back: 17.00 ms - 169.98 us (17.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100


sizeof(std::string) = 32
sizeof(String) = 16
TIMING Vector::RemoveIf: 150.84 ms - 15.08 us (151.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING std::remove_if : 329.84 ms - 32.98 us (330.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING Vector<String>::Remove:  1.18 s  - 11.84 ms ( 1.18 s  / 100 ), min: 11.00 ms, max: 14.00 ms, nesting: 0 - 100
TIMING vector<String>::erase:  3.50 s  - 35.04 ms ( 3.50 s  / 100 ), min: 32.00 ms, max: 38.00 ms, nesting: 0 - 100
TIMING vector<string>::erase: 16.26 s  - 162.63 ms (16.26 s  / 100 ), min: 152.00 ms, max: 176.00 ms, nesting: 0 - 100
TIMING Vector<String>::Insert:  1.15 s  - 11.48 ms ( 1.15 s  / 100 ), min: 11.00 ms, max: 16.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::insert:  1.17 s  - 11.73 ms ( 1.17 s  / 100 ), min: 11.00 ms, max: 13.00 ms, nesting: 0 - 100
TIMING vector<String>::insert:  3.56 s  - 35.65 ms ( 3.56 s  / 100 ), min: 33.00 ms, max: 39.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::insert:  5.94 s  - 59.45 ms ( 5.95 s  / 100 ), min: 57.00 ms, max: 65.00 ms, nesting: 0 - 100
TIMING vector<string>::insert: 17.07 s  - 170.67 ms (17.07 s  / 100 ), min: 161.00 ms, max: 197.00 ms, nesting: 0 - 100
TIMING Vector<String>::Add:  4.00 ms - 39.98 us ( 4.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::emplace_back:  5.00 ms - 49.98 us ( 5.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<String>::emplace_back: 998.41 us -  9.98 us ( 1.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::push_back:  9.00 ms - 89.98 us ( 9.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<string>::emplace_back: 23.00 ms - 229.98 us (23.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100

TIMING Vector::RemoveIf: 154.83 ms - 15.48 us (155.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING std::remove_if : 312.83 ms - 31.28 us (313.00 ms / 10000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 10000
TIMING Vector<String>::Remove:  1.09 s  - 10.91 ms ( 1.09 s  / 100 ), min: 10.00 ms, max: 11.00 ms, nesting: 0 - 100
TIMING vector<String>::erase:  3.22 s  - 32.22 ms ( 3.22 s  / 100 ), min: 32.00 ms, max: 34.00 ms, nesting: 0 - 100
TIMING vector<string>::erase: 14.94 s  - 149.38 ms (14.94 s  / 100 ), min: 148.00 ms, max: 157.00 ms, nesting: 0 - 100
TIMING Vector<String>::Insert:  1.08 s  - 10.84 ms ( 1.08 s  / 100 ), min: 10.00 ms, max: 12.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::insert:  1.10 s  - 11.03 ms ( 1.10 s  / 100 ), min: 10.00 ms, max: 12.00 ms, nesting: 0 - 100
TIMING vector<String>::insert:  3.24 s  - 32.44 ms ( 3.24 s  / 100 ), min: 32.00 ms, max: 34.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::insert:  5.50 s  - 55.00 ms ( 5.50 s  / 100 ), min: 54.00 ms, max: 57.00 ms, nesting: 0 - 100
TIMING vector<string>::insert: 15.79 s  - 157.92 ms (15.79 s  / 100 ), min: 157.00 ms, max: 170.00 ms, nesting: 0 - 100
TIMING Vector<String>::Add:  4.00 ms - 39.98 us ( 4.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<String>::emplace_back:  3.00 ms - 29.98 us ( 3.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<String>::emplace_back:  6.00 ms - 59.98 us ( 6.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING lz::vector<string>::push_back: 998.31 us -  9.98 us ( 1.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100
TIMING vector<string>::emplace_back: 11.00 ms - 109.98 us (11.00 ms / 100 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100


RUN With 10 times more test, and all non essential job close, CPU usage under 3%
sizeof(std::string) = 32
sizeof(String) = 16
TIMING Vector::RemoveIf:  1.51 s  - 15.11 us ( 1.51 s  / 100000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100000
TIMING std::remove_if :  3.21 s  - 32.09 us ( 3.21 s  / 100000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 100000
TIMING Vector<String>::Remove: 11.41 s  - 11.41 ms (11.41 s  / 1000 ), min: 11.00 ms, max: 19.00 ms, nesting: 0 - 1000
TIMING vector<String>::erase: 34.00 s  - 34.00 ms (34.00 s  / 1000 ), min: 32.00 ms, max: 42.00 ms, nesting: 0 - 1000
TIMING vector<string>::erase: 155.70 s  - 155.70 ms (155.70 s  / 1000 ), min: 148.00 ms, max: 183.00 ms, nesting: 0 - 1000
TIMING Vector<String>::Insert: 10.85 s  - 10.85 ms (10.85 s  / 1000 ), min: 10.00 ms, max: 13.00 ms, nesting: 0 - 1000
TIMING lz::vector<String>::insert: 11.20 s  - 11.20 ms (11.20 s  / 1000 ), min: 10.00 ms, max: 13.00 ms, nesting: 0 - 1000
TIMING vector<String>::insert: 34.78 s  - 34.78 ms (34.78 s  / 1000 ), min: 32.00 ms, max: 40.00 ms, nesting: 0 - 1000
TIMING lz::vector<string>::insert: 57.48 s  - 57.48 ms (57.48 s  / 1000 ), min: 56.00 ms, max: 67.00 ms, nesting: 0 - 1000
TIMING vector<string>::insert: 163.62 s  - 163.62 ms (163.62 s  / 1000 ), min: 157.00 ms, max: 236.00 ms, nesting: 0 - 1000
TIMING Vector<String>::Add: 27.98 ms - 27.98 us (28.00 ms / 1000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 1000
TIMING lz::vector<String>::emplace_back: 27.98 ms - 27.98 us (28.00 ms / 1000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 1000
TIMING vector<String>::emplace_back: 45.98 ms - 45.98 us (46.00 ms / 1000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 1000
TIMING lz::vector<string>::push_back: 63.98 ms - 63.98 us (64.00 ms / 1000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 1000
TIMING vector<string>::emplace_back: 87.98 ms - 87.98 us (88.00 ms / 1000 ), min:  0.00 ns, max:  1.00 ms, nesting: 0 - 1000

*/
