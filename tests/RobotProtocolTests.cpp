
#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <random>
#include <limits>

#include "RobotProtocol.hpp"

TEST(RobotProtocolTests, Int16RoundTrip_AllValues)
{
    for (int value = INT16_MIN; value <= INT16_MAX; ++value)
    {
        const auto original = static_cast<int16_t>(value);

        const auto encoded = encodeInt16(original);
        const auto decoded = decodeInt16(encoded);

        EXPECT_EQ(decoded, original)
            << "Failed for value: " << value;
    }
}

TEST(RobotProtocolTests, Int16WireFormat)
{
    const auto bytes = encodeInt16(1600);

    EXPECT_EQ(bytes[0], 0x06);
    EXPECT_EQ(bytes[1], 0x40);
}



TEST(RobotProtocolTests, Int32RandomRoundTrip)
{
    constexpr uint32_t seed = 0xC0FFEE;

    std::mt19937 rng(seed);

    std::uniform_int_distribution<int32_t> dist(
        INT32_MIN,
        INT32_MAX
    );

    for (int i = 0; i < 10000; ++i)
    {
        const int32_t original = dist(rng);

        const auto encoded = encodeInt32(original);
        const auto decoded = decodeInt32(encoded);

        EXPECT_EQ(decoded, original)
            << "Iteration: " << i
            << ", value: " << original
            << ", seed: " << seed;
    }
}
