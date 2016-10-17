#pragma once

#include "SimpleMath.h"

using namespace DirectX::SimpleMath;

class World
{
public:
	static float Sample(DirectX::XMINT3 pos, int scale);
	static float Sample(Vector3 pos, int scale);
	static float Sample(float x, float y, float z, int scale);
	static float Noise(float x, float y, float z);
	static float Noise(float x, float y, float z, float scale, int octaves);
	static float Noise(float x, float z, float scale, int octaves);
	static Vector3 GetIntersection(const Vector3 p1, const Vector3 p2, float d1, float d2);
	static Vector3 GetIntersection(const Vector3 p1, const Vector3 p2, int quality);
	static float Sphere(float x, float y, float z);
	static float Cube(float x, float y, float z);
	static Vector3 GetNormal(Vector3 pos, int scale);
	static Vector3 GetColor(Vector3 pos);

	static float global_size;
	static float global_size_inverse;
	static int sample_type;
	static float terrain_scale;
	static float terrain_height;
};
