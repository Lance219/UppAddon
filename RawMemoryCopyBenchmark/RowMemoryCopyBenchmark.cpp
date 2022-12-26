#include <Core/Core.h>
#include <CoreExt/CoreExt.h>

using namespace Upp;

const int N = 100000;
const int M = 3*1024*1024;

struct S{
    S()noexcept = default;
    S(const S&)noexcept = default;
    char buff[ M ];
};

void CheckEqual(S& s, char * buff)
{
	if(s.buff[0] == buff[0] && s.buff[M-1]== buff[M-1] && s.buff[M/2]==buff[M/2])
		;
	else
		RLOG("INVALID");
}

CONSOLE_APP_MAIN
{
    S s;

    int64 a = 0, b = 0, c = 0, d = 0, e = 0;
    for(int i = 0; i < N; ++i)
    {
   		{
//   			S t;
   			RTIMING("No copying, common overhed");
            for(int j = 0; j < M; ++j)
                e += s.buff[j];
   		}
        {
            RTIMING("std::vector");
            char buff[M];
            new(buff)S(s);
            for(int j = 0; j < M; ++j)
                a += buff[j];
            
//            if( Random(N) == 0 )
//                CheckEqual(s, buff);
//            a += buff[j1];
//            a += buff[j2];
//            a += buff[j3];
        }
        {
            RTIMING("Upp::Vector");
            char buff[M];
            memcpy_t((S*)buff, &s, 1);
            for(int j = 0; j < M; ++j)
                b += buff[j];
//            if( Random(N) == 0 )
//                CheckEqual(s, buff);
//            b += buff[j1];
//            b += buff[j2];
//            b += buff[j3];
        }
        {
            RTIMING("lz::vector");
            char buff[M];
            lz::memcpy_record<sizeof(S)>(buff, &s, 1);
            for(int j = 0; j < M; ++j)
                c += buff[j];

//            if( Random(N) == 0 )
//                CheckEqual(s, buff);

//            c += buff[j1];
//            c += buff[j2];
//            c += buff[j3];
        }
        {
            RTIMING("plain memcpy");
            char buff[M];
            memcpy(buff, &s, M);
            for(int j = 0; j < M; ++j)
                d += buff[j];
//            if( Random(N) == 0 )
//                CheckEqual(s, buff);

//            d += buff[j1];
//            d += buff[j2];
//            d += buff[j3];
        }

    }

    RLOG( String().Cat() << "Expecting values equal: "
    		<< a << ',' << b << ',' << c << ',' << d <<","<<e);
    RLOG("Testing speed of various memory block copy utilities");
}
//Expecting values equal: 4932700000,4932700000,4932700000,4932700000,4932650673
//Testing speed of various memory block copy utilities
//TIMING plain memcpy   : 123.84 s  -  1.24 ms (123.84 s  / 100000 ), min:  1.00 ms, max:  4.00 ms, nesting: 0 - 100000
//TIMING lz::vector     : 96.77 s  - 967.69 us (96.77 s  / 100000 ), min:  0.00 ns, max:  4.00 ms, nesting: 0 - 100000
//TIMING Upp::Vector    : 97.25 s  - 972.47 us (97.25 s  / 100000 ), min:  0.00 ns, max:  4.00 ms, nesting: 0 - 100000
//TIMING std::vector    : 112.84 s  -  1.13 ms (112.84 s  / 100000 ), min:  1.00 ms, max:  5.00 ms, nesting: 0 - 100000
//TIMING No copying, common overhed: 87.95 s  - 879.53 us (87.95 s  / 100000 ), min:  0.00 ns, max:  4.00 ms, nesting: 0 - 100000

//Expecting values equal: 2688100000,2688100000,2688100000,2688100000,2688073119
//Testing speed of various memory block copy utilities
//TIMING plain memcpy   : 123.53 s  -  1.24 ms (123.53 s  / 100000 ), min:  1.00 ms, max:  4.00 ms, nesting: 0 - 100000
//TIMING lz::vector     : 96.71 s  - 967.06 us (96.71 s  / 100000 ), min:  0.00 ns, max:  4.00 ms, nesting: 0 - 100000
//TIMING Upp::Vector    : 97.82 s  - 978.23 us (97.82 s  / 100000 ), min:  0.00 ns, max:  4.00 ms, nesting: 0 - 100000
//TIMING std::vector    : 113.39 s  -  1.13 ms (113.39 s  / 100000 ), min:  1.00 ms, max:  5.00 ms, nesting: 0 - 100000
//TIMING No copying, common overhed: 88.46 s  - 884.60 us (88.46 s  / 100000 ), min:  0.00 ns, max:  3.00 ms, nesting: 0 - 100000
