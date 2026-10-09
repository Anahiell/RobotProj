#include "HeadNode.hpp"

void HeadNode::setYaw(int16_t value)
{
    yaw_ = value;
    changedMask_ |= Yaw;
}

void HeadNode::setPitch(int16_t value)
{
    pitch_ = value;
    changedMask_ |= Pitch;
}

Command HeadNode::makeCommand()
{
    Command cmd{};

    cmd.node = Id;
    cmd.command = CommandId::Set;

    cmd.data[0] = changedMask_;

    auto yawBytes = encodeInt16(yaw_);
    auto pitchBytes = encodeInt16(pitch_);

    cmd.data[1] = yawBytes[0];
    cmd.data[2] = yawBytes[1];

    cmd.data[3] = pitchBytes[0];
    cmd.data[4] = pitchBytes[1];

    cmd.dataLength = 5;

    changedMask_ = 0;

    return cmd;
}