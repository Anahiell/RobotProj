#include "HeadController.hpp"

HeadController::HeadController() 
	: yaw(YAW_CENTER), pitch(PITCH_CENTER)
{
}
void HeadController::setYaw(int yaw)
{
	if(yaw <= YAW_MAX && yaw >= YAW_MIN)
	this->yaw = yaw;
	else
		{
		if(yaw > YAW_MAX)
			this->yaw = YAW_MAX;
		else
			this->yaw = YAW_MIN;
	}
}
void HeadController::setPitch(int pitch)
{
	this->pitch = pitch;
}

int HeadController::getYaw() const
{
	return yaw;
}
int HeadController::getPitch() const
{
	return pitch;
}
