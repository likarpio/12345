#pragma once
#include <array>

struct Vector2
{
	float x, y;
};

struct Vector3
{
	float x, y, z;
};

class Math
{
public:
	bool WorldToScreen(Vector3 pos, Vector2& screen, std::array<float, 16> viewMatrix, int screenWidth, int screenHeight);
	bool getRectPos(Vector3 locPlayerPos, Vector3 entPos, Vector2 screen, Vector2& start, Vector2& end);
	bool calcViewAngles(float& yaw, float& pitch, Vector3 locPlayerPos, Vector3 entPos);

	float DistanceTo(const Vector3& entPos, Vector3 locPlayerPos);
};


