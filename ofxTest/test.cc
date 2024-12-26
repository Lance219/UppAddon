#include "test.h"

namespace lz{

TEST(ParserTest, TagsAndKeys)
{
	lz::Parser _;
	for(auto k : _.keys)
		EXPECT_TRUE(_.IndexOfKey(k)!=-1);
	for(auto t : _.tags)
		EXPECT_TRUE(_.IndexOfTag(t)!=-1);
	EXPECT_EQ(_.IndexOfKey("OFX"), -1);
	EXPECT_EQ(_.IndexOfTag("OFXHEADER"), -1);
}

}