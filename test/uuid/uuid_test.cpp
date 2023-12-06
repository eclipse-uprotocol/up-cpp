#include <gtest/gtest.h>

#include "src/main/proto/uuid.pb.h"
#include "UuidSerializer.h"
#include "Uuidv8Factory.h"

using namespace uprotocol::uuid;
using namespace uprotocol::v1;

//UUID create, serilize and deserialize
TEST(UUIDTest, Class)
{
    UUID uuId = Uuidv8Factory::create();
    std::vector<uint8_t> vectUuid = UuidSerializer::instance().serializeToBytes(uuId);
    UUID uuIdFromByteArr = UuidSerializer::instance().deserializeFromBytes(vectUuid);
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

//Negative test - deserializeFromString - Invalid String with more than 32 hex characters
TEST(UUIDTest, NegStringWithMoreThan32HexCharsTest)
{
    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f-1abc";
    UUID uuId = UuidSerializer::instance().deserializeFromString(str);
    EXPECT_NE(UuidSerializer::instance().serializeToString(uuId), str);
    EXPECT_EQ(UuidSerializer::instance().getCount(uuId), uint64_t(0));
}

//Negative test deserializeFromBytes - Empty Byte vector
TEST(UUIDTest, NegEmptyByteVector)
{
    std::vector<uint8_t> vectEmpty;
    UUID uuIdFromByteArr = UuidSerializer::instance().deserializeFromBytes(vectEmpty);
    uint64_t val(0);

    EXPECT_EQ(UuidSerializer::instance().getCount(uuIdFromByteArr), val);
}

//Negative test  deserializeFromBytes - Greater than 16
TEST(UUIDTest, NegGreaterThanDefinedSize)
{
    std::vector<uint8_t> vect(18);
    uint64_t msb(1);
    uint64_t lsb(1);
    for (int i = 0; i < 8; i++) {
        vect[i] = msb ;
        vect[i + 8] = lsb;
    }

    UUID uuIdFromByteArr = UuidSerializer::instance().deserializeFromBytes(vect);
    uint64_t val(0);

    EXPECT_EQ(UuidSerializer::instance().getCount(uuIdFromByteArr), val);
}

//Negative test deserializeFromBytes - Invalid Byte vector
TEST(UUIDTest, NegInvalidByteVector)
{
    std::vector<uint8_t> vect(16);
    uint64_t msb(1);
    uint64_t lsb(1);
    for (int i = 0; i < 8; i++) {
        vect[i] = msb ;
        vect[i + 8] = lsb;
    }
    UUID uuIdFromByteArr = UuidSerializer::instance().deserializeFromBytes(vect);
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