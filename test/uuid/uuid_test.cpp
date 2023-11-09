#include <gtest/gtest.h>

#include "uuid.pb.h"
#include "UuidSerializer.h"
#include "Uuidv8Factory.h"

using namespace uprotocol::uuid;

//UUID create, serilize and deserialize
TEST(UUIDTest, Class)
{
    UUID uuId = Uuidv8Factory::create();
    std::vector<uint8_t> vectUuid = UuidSerializer::instance().serializeToBytes(uuId);
    UUID uuIdFromByteArr = UuidSerializer::instance().deserializeFromBytes(vectUuid.data());
    EXPECT_TRUE(UuidSerializer::instance().getTime(uuId) == UuidSerializer::instance().getTime(uuIdFromByteArr));
    EXPECT_TRUE(UuidSerializer::instance().getCount(uuId) == UuidSerializer::instance().getCount(uuIdFromByteArr));

    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew = UuidSerializer::instance().deserializeFromString(str);
    EXPECT_EQ(UuidSerializer::instance().serializeToString(uuIdNew), str);
}
//Negative test - serialize and deserialize
TEST(UUIDTest, NegStringConstructor)
{
    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew = UuidSerializer::instance().deserializeFromString(str);
    str = "test" +str;
    EXPECT_NE(UuidSerializer::instance().serializeToString(uuIdNew), str);
}
//Negative test - empty string
TEST(UUIDTest, NegEmptyString)
{
    //Empty String
    std::string str = "";
    UUID uuId = UuidSerializer::instance().deserializeFromString(str);
    EXPECT_NE(UuidSerializer::instance().serializeToString(uuId), str);
    EXPECT_EQ(UuidSerializer::instance().getCount(uuId), uint64_t(0));
}
//Negative test - Empty Byte Array
TEST(UUIDTest, NegEmptyByteArray)
{
    std::array<uint8_t, 16> buff{};
    UUID uuIdFromByteArr = UuidSerializer::instance().deserializeFromBytes( buff.data() );
    uint64_t val(0);

    EXPECT_EQ(UuidSerializer::instance().getCount(uuIdFromByteArr), val);
}
//Negative test - Invalid Byte array
TEST(UUIDTest, NegInvalidByteArray)
{
    std::array<uint8_t, 16> buff{};
    uint64_t msb(1);
    uint64_t lsb(1);
    for (int i = 0; i < 8; i++) {
        buff[i] = msb ;
        buff[i + 8] = lsb;
    }
    UUID uuIdFromByteArr = UuidSerializer::instance().deserializeFromBytes( buff.data() );
    uint64_t val(1);
    std::string str = "";

    EXPECT_NE(UuidSerializer::instance().serializeToString(uuIdFromByteArr), str);
    EXPECT_NE(UuidSerializer::instance().getCount(uuIdFromByteArr), val);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}