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

    auto yawBytes = encodeInt16(yaw_);
    auto pitchBytes = encodeInt16(pitch_);

    cmd.data = {
        changedMask_,
        yawBytes[0], yawBytes[1],
        pitchBytes[0], pitchBytes[1]
    };

    changedMask_ = 0;

    return cmd;
}