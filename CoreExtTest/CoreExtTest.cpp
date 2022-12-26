#include "CoreExtTest.h"

#include <Core/Core.h>
#include <CoreExt/CoreExt.h>
#include <CtrlCoreExt/CtrlCoreExt.h>

#include <cassert>
#include <iostream>
#include <vector>

template <class T>
using pai = lz::ptr_as_int<T>;

TEST(CoreExt, PtrAsIntIncDec)
{
    const char* msg = "hello";
    lz::ptr_as_int p(msg);
    EXPECT_EQ(msg, static_cast<const char*>(p));
    ++p;
    EXPECT_EQ(*p, msg[1]);
    auto t = p++;
    EXPECT_EQ(*p, msg[2]);
    EXPECT_EQ(*t, msg[1]);
    --t;
    EXPECT_EQ(*t, msg[0]);
    t = p--;
    EXPECT_EQ(*p, msg[1]);
    EXPECT_EQ(*t, msg[2]);
}

TEST(CoreExt, PtrAsIntAddSubstract)
{
    int a[5];

    lz::ptr_as_int p(a);
    EXPECT_EQ(a, static_cast<int* const>(p));
    EXPECT_EQ(a[1], *(p += sizeof(int)));
    EXPECT_EQ(a[0], *(p -= sizeof(int)));
    EXPECT_EQ(a[3], *(p + sizeof(int) * 3));
    p += 4 * sizeof(int);
    EXPECT_EQ(a[2], *(p - sizeof(int) * 2));
}

TEST(CoreExt, PtrAsIntBitwise)
{
    int i = 123456;
    lz::ptr_as_int p(&i);
    EXPECT_EQ(0, p & 3);
    p |= 3;
    EXPECT_EQ(3, p & 3);
    EXPECT_EQ(-1, p | ~3);
    p &= ~3;
    EXPECT_EQ(&i, static_cast<int*>(p));
}

TEST(CoreExt, IntOfSize)
{
    EXPECT_EQ(sizeof(lz::IntOfSize<4>), 4);
    if constexpr (sizeof(int) == sizeof(long))
        EXPECT_EQ(typeid(lz::TrimIntegral<int>), typeid(lz::TrimIntegral<long>));
    else if constexpr (sizeof(long) == sizeof(long long))
        EXPECT_EQ(typeid(lz::TrimIntegral<long>), typeid(lz::TrimIntegral<long long>));
}

namespace detail {
struct E {
    E()
    {
    }
};
void DoPostRelocationAdjustment(E* obj) noexcept
{
}

struct F : E {
    F()
    {
    }
};
} // namespace detail

TEST(CoreExt, Relocatable)
{
    using namespace lz;
    EXPECT_TRUE(is_trivially_relocatable_v<char>);
    EXPECT_TRUE(is_trivially_relocatable_v<short>);
    EXPECT_TRUE(is_trivially_relocatable_v<bool>);
    EXPECT_TRUE(is_trivially_relocatable_v<int>);
    EXPECT_TRUE(is_trivially_relocatable_v<long>);
    EXPECT_TRUE(is_trivially_relocatable_v<long long>);
    EXPECT_TRUE(is_trivially_relocatable_v<float>);
    EXPECT_TRUE(is_trivially_relocatable_v<double>);
    EXPECT_TRUE(is_trivially_relocatable_v<long double>);
    struct B {
        int j;
    };
    struct S : B {
        int i;
        double d;
        union {
            char* p;
            long l;
        };
    };
    EXPECT_TRUE(is_trivially_relocatable_v<S>);
    struct D {
        D()
        {
        }
    };
    EXPECT_TRUE(relocatable<Upp::String>);
    EXPECT_TRUE(!is_trivially_relocatable_v<Upp::Ctrl>);
    struct F {
        F()
        {
        }
        void DoPostRelocationAdjustment() noexcept
        {
        }
    };

    EXPECT_TRUE(relocatable<F>);
    struct U {
        U() {};
        U(const U& s) {};
        void DoPostRelocationAdjustment(const U*) noexcept
        {
        }
    };

    struct C : U {
        C()
        {
        }
    };

    EXPECT_TRUE(relocatable<U>);
    EXPECT_FALSE(relocatable<C>);
    EXPECT_TRUE(relocatable<detail::E>);
    // the following need to be fixed
    //
    // EXPECT_FALSE(relocatable<detail::F>);
}

TEST(CoreExtTest, VectorBasics)
{
    using namespace lz;
    for (unsigned i = 1; i < 100; ++i) {
        lz::vcore<4> core;
        core.alloced = i;
        for (int j = 0; j < 80; ++j) {
            auto old = core.alloced;
            core.alloced = core.NewSize();
            EXPECT_LT(old, core.alloced);
            if (core.alloced == core.max_size)
                break;
        }
        EXPECT_EQ(core.max_size, core.alloced);
    }

    vector<int> vi;
    EXPECT_TRUE(vi.empty());
    vi.push_back(5);
    EXPECT_TRUE(!vi.empty());
    EXPECT_EQ(vi[0], 5);
    for (int i = 0; i < 10000; ++i)
        vi.push_back(i);
    EXPECT_EQ(vi.size(), 10001);
    EXPECT_EQ(vi.back(), 9999);
    EXPECT_EQ(vi.front(), 5);
    EXPECT_EQ(vi.data()[0], vi.front());
    EXPECT_EQ(vi.data()[10000], vi.back());
}

TEST(CoreExtTest, VectorIterator)
{
    using namespace lz;

    vector<int> vi;

    for (int i = 1; i <= 100; ++i)
        vi.push_back(i);

    int sum = 0;
    for (auto v : vi)
        sum += v;
    EXPECT_EQ(sum, 5050);
    for (const auto v : vi)
        sum += v;
    EXPECT_EQ(sum, 10100);

    EXPECT_EQ(*vi.rbegin(), 100);
    sum = 0;
    for (auto p = vi.rbegin(); p != vi.rend(); ++p)
        sum += *p;
    EXPECT_EQ(sum, 5050);
}

TEST(CoreExtTest, VectorShrinkToFit)
{
    using namespace lz;

    vector<Upp::String> vs;

    for (int i = 0; i < 100; ++i) {
        vs.emplace_back(Upp::String().Cat() << "String # " << i);
    }
    vs.shrink_to_fit();
    EXPECT_EQ(vs.back(), "String # 99");
    EXPECT_LT(vs.capacity(), 120);

    for (int i = 100; i < 1000; ++i) {
        vs.emplace_back(Upp::String().Cat() << "String # " << i);
    }
    vs.shrink_to_fit();
    EXPECT_LT(vs.capacity(), 1040);
}

TEST(CoreExtTest, VectorSimpleRelocateAdjustmentPushEmplaceBack)
{
    using namespace lz;

    EXPECT_TRUE(relocatable<RelocAdjustSimple>);
    vector<RelocAdjustSimple> vs;

    for (int i = 0; i < 100; ++i) {
        vs.emplace_back().AddNode();
    }
    for (int i = 0; i < 100; ++i)
        EXPECT_TRUE(vs[i].IsConsistent());

    vs.clear();
    for (int i = 0; i < 100; ++i) {
        RelocAdjustSimple ras;
        ras.AddNode();
        vs.push_back(ras);
    }
    for (int i = 0; i < 100; ++i)
        EXPECT_TRUE(vs[i].IsConsistent());
    vs.clear();
    for (int i = 0; i < 100; ++i) {
        RelocAdjustSimple ras;
        ras.AddNode();
        vs.push_back(std::move(ras));
    }
    for (int i = 0; i < 100; ++i)
        EXPECT_TRUE(vs[i].IsConsistent());
}

TEST(CoreExtTest, VectorConstructNValue)
{
    using namespace lz;

    vector<VectorConstructNValueD> vd; // default constructed vector is equivalent to raw memeory

    try {
        VectorConstructNValueD d;
        new (&vd) lz::vector<VectorConstructNValueD>(5000, d);
    } catch (...) {
        EXPECT_EQ(vd.size(), 10);
    }
}
TEST(CoreExtTest, VectorReserve)
{
    using lz::vector;

    vector<int> vi;
    vi.reserve(10000);

    EXPECT_GE(vi.capacity(), 10000);
}

TEST(CoreExtTest, VectorClear)
{
    using lz::vector;
    
    static int cnt;
    
    struct S{
        S(){ ++cnt; }
        ~S(){ --cnt; }
        void DoPostRelocationAdjustment()noexcept{}
    };

    vector<S> vi(1000u);
    EXPECT_EQ(vi.size(), 1000);
    EXPECT_EQ(cnt, 1000);
    vi.clear();
    EXPECT_EQ(vi.size(), 0);
    EXPECT_EQ(cnt, 0);
}

TEST(CoreExtTest, VectorCopy)
{
    using lz::vector;

    vector<int> vi(1000u);
    vector<int> vj(vi);

    EXPECT_EQ(vi.size(), vj.size());
    for (int i = 0; i < vi.size(); ++i)
        EXPECT_EQ(vi[0], vj[0]);
    vector<int> vk(std::move(vi));
    EXPECT_TRUE(vi.empty());
    EXPECT_EQ(vi.data(), nullptr);
    for (int i = 0; i < vi.size(); ++i)
        EXPECT_EQ(vj[0], vk[0]);
}

TEST(CoreExtTest, VectorPopBack)
{
    using namespace lz;

	static int _i;

    struct S {
        S(int i)
            : i(i)
        {
        }
        void DoPostRelocationAdjustment() noexcept
        {
        }
        ~S()
        {
            _i = i;
        }
        int i;
    };

    vector<S> vs;
    vs.emplace_back(1);
    vs.emplace_back(2);
    vs.emplace_back(3);
    vs.pop_back();
    EXPECT_EQ(_i, 3);
    vs.pop_back();
    EXPECT_EQ(_i, 2);
    vs.pop_back();
    EXPECT_EQ(_i, 1);
}

TEST(CoreExtTest, VectorInsertCopy)
{
    using namespace lz;

    vector<int> vi;

    for (int i = 0; i < 100; ++i)
        vi.insert(vi.begin(), i);
    EXPECT_EQ(vi.size(), 100);

    for (int i = 0; i < 100; ++i)
        EXPECT_EQ(vi[i], 99 - i);
}

TEST(CoreExtTest, VectorInsertMoveExceptionSafety)
{
    using namespace lz;

    const int n = 100;
    const int m = 10;

    struct S {
        S(int i)
        {
            p = nullptr;
            if (i == m)
                throw std::runtime_error("fictitious");
            p = new int(i);
        }
        // S(const S& s){ p = nullptr; if(s.p) p = new int(s.Get());}
        S(S&& s)
        {
            p = s.p;
            s.p = nullptr;
        }

        ~S()
        {
            delete p;
        }
        void DoPostRelocationAdjustment() noexcept
        {
        }

        int Get() const
        {
            if (!p)
                throw std::runtime_error("uninitialized S");
            return *p;
        }
        int* p;
    };

    vector<S> vs;

    try {
        for (int i = 0; i < n; ++i) {
            vs.insert(vs.begin(), i);
        }
    } catch (...) {
    }
    EXPECT_EQ(vs.size(), m);

    for (int i = 0; i < m; ++i)
        EXPECT_EQ(vs[i].Get(), m - 1 - i);
}

TEST(CoreExtTest, VectorSimpleRelocateInsert)
{
    using namespace lz;

    vector<RelocAdjustSimple> vs;

    for (int i = 0; i < 100; ++i) {
        vs.emplace_back().AddNode();
    }

    DUMP(relocatable<std::string>);
}
