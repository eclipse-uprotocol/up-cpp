#include <gtest/gtest.h>

#include "uuid.h"
#include "UUIDSerializer.h"
#include "UUIDv8Factory.h"

using namespace uprotocol::uuid;

//UUID create, serilize and deserialize
TEST(UUIDTest, Class)
{
    UUID uuId = UUIDv8Factory::create();
    std::vector<uint8_t> vectUuid = UUIDSerializer::instance().serializeToBytes(uuId);
    UUID uuIdFromByteArr = UUIDSerializer::instance().deserialize(vectUuid.data());
    EXPECT_TRUE(uuId.getTime() == uuIdFromByteArr.getTime());
    EXPECT_TRUE(uuId.getCount() == uuIdFromByteArr.getCount());

    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew = UUIDSerializer::instance().deserialize(str);
    EXPECT_EQ(UUIDSerializer::instance().serializeToString(uuIdNew), str);
}
//Negative test - serialize and deserialize
TEST(UUIDTest, NegStringConstructor)
{
    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew = UUIDSerializer::instance().deserialize(str);
    str = "test" +str;
    EXPECT_NE(UUIDSerializer::instance().serializeToString(uuIdNew), str);
}
//Negative test - empty string
TEST(UUIDTest, NegEmptyString)
{
    //Empty String
    std::string str = "";
    UUID uuId = UUIDSerializer::instance().deserialize(str);
    EXPECT_NE(UUIDSerializer::instance().serializeToString(uuId), str);
    EXPECT_EQ(uuId.getCount(), uint64_t(0));
}
//Negative test - Empty Byte Array
TEST(UUIDTest, NegEmptyByteArray)
{
    std::array<uint8_t, 16> buff{};
    UUID uuIdFromByteArr = UUIDSerializer::instance().deserialize( buff.data() );
    uint64_t val(0);

    EXPECT_EQ(uuIdFromByteArr.getCount(), val);
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
    UUID uuIdFromByteArr = UUIDSerializer::instance().deserialize( buff.data() );
    uint64_t val(1);
    std::string str = "";

    EXPECT_NE(UUIDSerializer::instance().serializeToString(uuIdFromByteArr), str);
    EXPECT_NE(uuIdFromByteArr.getCount(), val);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}