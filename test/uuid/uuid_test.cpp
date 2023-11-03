#include <gtest/gtest.h>

#include "uuid.h"

using namespace uprotocol::uuid;

//UUID constructors, toByteArray(),getTime(), getCount(),toString()
TEST(UUIDTest, Class)
{
    UUID uuId;

    UUID uuIdFromByteArr(uuId.toByteArray().data());
    EXPECT_TRUE(uuId.getTime() == uuIdFromByteArr.getTime());
    EXPECT_TRUE(uuId.getCount() == uuIdFromByteArr.getCount());

    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew(str);
    EXPECT_EQ(uuIdNew.toString(), str);
}

TEST(UUIDTest, NegStringConstructor)
{
    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew(str);
    str = "test" +str;
    EXPECT_NE(uuIdNew.toString(), str);
}

TEST(UUIDTest, NegEmptyString)
{
    //Empty String
    std::string str = "";
    UUID uuId(str);
    EXPECT_NE(uuId.toString(), str);
    EXPECT_EQ(uuId.getCount(), uint64_t(0));
}

TEST(UUIDTest, NegEmptyByteArray)
{
    std::array<uint8_t, 16> buff{};
    UUID uuIdFromByteArr(buff.data());
    uint64_t val(0);

    EXPECT_EQ(uuIdFromByteArr.getCount(), val);
}

TEST(UUIDTest, NegInvalidByteArray)
{
    std::array<uint8_t, 16> buff{};
    uint64_t msb(1);
    uint64_t lsb(1);
    for (int i = 0; i < 8; i++) {
        buff[i] = msb ;
        buff[i + 8] = lsb;
    }
    UUID uuIdFromByteArr(buff.data());
    uint64_t val(1);
    std::string str = "";

    EXPECT_NE(uuIdFromByteArr.toString(), str);
    EXPECT_NE(uuIdFromByteArr.getCount(), val);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}