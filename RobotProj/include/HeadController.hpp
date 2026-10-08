#pragma once

class HeadController {

private:
	int yaw;
	int pitch;
	static constexpr int YAW_CENTER = 1600;
	static constexpr int PITCH_CENTER = 1500;
	static constexpr int YAW_DELTA = 500;

	static constexpr int YAW_MAX = YAW_CENTER + YAW_DELTA;  // 2000
	static constexpr int YAW_MIN = YAW_CENTER - YAW_DELTA;
public:
	HeadController();
	void setYaw(int yaw);
	void setPitch(int pitch);
	int getYaw() const;
	int getPitch() const;
};