
#include <gtest/gtest.h>
#include <cstdint>
#include <limits>
#include <random>
#include <vector>
#include <cstddef>

#include "RobotProtocol.hpp"
#include "CommandEncoder.hpp"
#include "CommandDecoder.hpp"
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

constexpr uint32_t seed = 0xC0FFEE;


TEST(RobotProtocolTests, Int32RandomRoundTrip)
{

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

TEST(RobotProtocolTests, SingleSevenBytes)
{
	Command cmd{};
	cmd.node = NodeId::Head;
	cmd.command = CommandId::Set;
	cmd.data = { 0, 1, 2, 3, 4, 5, 6 };

	const auto frames = encodeCommand(
		cmd,
		static_cast<CanPriority>(0)
	);

	ASSERT_EQ(frames.size(), 1u);

	EXPECT_EQ(
		decodeCanId(frames[0].id).type,
		FrameType::Single
	);

	EXPECT_EQ(frames[0].length, 8);
	CommandDecoder decoder;
	const auto result = decoder.push(frames[0]);

	EXPECT_EQ(result.dStatus, DecodeStatus::Complete);
	ASSERT_TRUE(result.cmd.has_value());
	EXPECT_EQ(result.cmd->data, cmd.data);

}
TEST(RobotProtocolTests, SwitchToSequenceAtEightBytes)
{
	Command cmd{};
	cmd.node = NodeId::Head;
	cmd.command = CommandId::Set;
	cmd.data = { 0, 1, 2, 3, 4, 5, 6, 7 };

	const auto frames = encodeCommand(
		cmd,
		static_cast<CanPriority>(0)
	);

	ASSERT_GT(frames.size(), 1u);

	EXPECT_EQ(
		decodeCanId(frames[0].id).type,
		FrameType::SequenceStart
	);
}
TEST(RobotProtocolTests, SequenceEightBytes)
{
	std::mt19937 rng(seed);
	std::uniform_int_distribution<int> randomByte(0, 255);

	for (std::size_t length = 8; length <= 1534; ++length) {
		SCOPED_TRACE(::testing::Message() << "Payload length: " << length);


		Command cmd{};
		cmd.node = NodeId::Head;//TODO:rng
		cmd.command = CommandId::Set;
		cmd.data.resize(length);

		//gen rnd payload
		for (auto& byte : cmd.data) {
			byte = static_cast<uint8_t>(randomByte(rng));
		}
		//encode cmd
		const auto frames = encodeCommand(
			cmd,
			static_cast<CanPriority>(0)
		);
		ASSERT_GE(frames.size(), 2u);
		ASSERT_LE(frames.size(),256u);

		//verify frames and restore payload

		std::vector<uint8_t> restored;

		const uint8_t sequenceId = frames[0].data[1];

		for (std::size_t i = 0; i < frames.size(); ++i)
		{
			const auto& frame = frames[i];
			const auto id = decodeCanId(frame.id);

			const FrameType expectedType =
				i == 0 ? FrameType::SequenceStart :
				i == frames.size() - 1 ? FrameType::SequenceEnd :
				FrameType::SequenceData;

			EXPECT_EQ(id.type, expectedType);
			EXPECT_EQ(id.node, cmd.node);

			const std::size_t headerSize = (i == 0) ? 4u : 2u;

			ASSERT_GE(frame.length, headerSize);
			ASSERT_LE(frame.length, 8u);

			if (i == 0)
			{
				EXPECT_EQ(
					frame.data[0],
					static_cast<uint8_t>(cmd.command)
				);

				const std::size_t declaredLength =
					(static_cast<std::size_t>(frame.data[2]) << 8) |
					frame.data[3];

				EXPECT_EQ(declaredLength, length);
			}
			else
			{
				EXPECT_EQ(frame.data[0], sequenceId);
				EXPECT_EQ(frame.data[1], static_cast<uint8_t>(i));
			}

			for (std::size_t j = headerSize; j < frame.length; ++j)
			{
				restored.push_back(frame.data[j]);
			}
		}

		// 4. Verify original payload was preserved
		EXPECT_EQ(restored, cmd.data);
	}
}
TEST(RobotProtocol, SequenceRoundTrip)
{
	std::mt19937 rng(seed);
	std::uniform_int_distribution<int> randomByte(0, 255);

	Command original{};
	original.node = NodeId::Head;
	original.command = CommandId::Set;
	

	for (std::size_t length = 8; length <= 1534; ++length)
	{
		SCOPED_TRACE(::testing::Message()
			<< "Payload length: " << length);

		original.data.resize(length);

		for (auto& byte : original.data)
		{
			byte = static_cast<uint8_t>(randomByte(rng));
		}

		const auto frames = encodeCommand(original);

		CommandDecoder decoder;

		for (std::size_t i = 0; i < frames.size(); ++i)
		{
			const DecodeResult result = decoder.push(frames[i]);

			const bool isLast = (i == frames.size() - 1);

			if (!isLast)
			{
				EXPECT_EQ(result.dStatus, DecodeStatus::InProgress);
				EXPECT_FALSE(result.cmd.has_value());
			}
			else
			{
				ASSERT_EQ(result.dStatus, DecodeStatus::Complete);
				ASSERT_TRUE(result.cmd.has_value());

				EXPECT_EQ(result.cmd->node, original.node);
				EXPECT_EQ(result.cmd->command, original.command);
				EXPECT_EQ(result.cmd->data, original.data);
			}
		}
	}
}