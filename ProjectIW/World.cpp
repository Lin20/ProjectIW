#include "World.h"
#include "SimplexNoise.h"
#include <complex>

#define USE_PLANE 0
#define USE_SPHERE 1
#define USE_FRACTAL_2D 2
#define USE_FRACTAL_3D 3
#define USE_FRACTAL_SPHERE 4
#define USE_HEIGHTMAP 5

float World::global_size = 0.0f;
float World::global_size_inverse = 0.0f;
int World::sample_type = USE_HEIGHTMAP;
float World::terrain_scale = 0.0005f;
float World::terrain_height = 1024.0f;

float fractal(double x, double y, int* out_iterations)
{
	using namespace std::complex_literals;
	std::complex<double> z(0, 0);
	std::complex<double> C(x, y);
	const int max_iterations = 1024;
	int iterations = 0;
	for (; z.real() * z.real() + z.imag() * z.imag() < 4.0 && iterations < max_iterations; iterations++)
	{
		z = z * z + C;
	}

	float v = (float)iterations / (float)max_iterations;
	if (out_iterations)
		*out_iterations = iterations;
	return (1.0f - v) * 1.0f + v * (-1.0f);
}

void hypercomplex_multiply(double& x, double& y, double& z)
{
	double pi = 3.14159265;
	double r = sqrt(x*x + y*y + z*z);
	double yang = atan2(sqrt(x*x + y*y), z);
	double zang = atan2(y, x);
	double newx = (r*r) * sin(yang * 2 + 0.5*pi) * cos(zang * 2 + pi);
	double newy = (r*r) * sin(yang * 2 + 0.5*pi) * sin(zang * 2 + pi);
	double newz = (r*r) * cos(yang * 2 + 0.5*pi);
	x = newx;
	y = newy;
	z = newz;
}

void hypercomplex_multiply2(float& x, float& y, float& z)
{
	if (x == 0 && y == 0 && z == 0)
		return;
	float p = sqrt(x*x + y*y);
	float a = 1.0f - z*z / p*p;
	float newx = a * (x*x - y*y);
	float newy = a * (x*y + x*y);
	float newz = p*z + p*z;

	x = newx;
	y = newy;
	z = newz;
}

void hypercomplex_multiply3(float& x, float& y, float& z, float order)
{
	if (x == 0 && y == 0 && z == 0)
		return;
	float r = sqrt(x*x + y*y + z*z);
	float theta = atan2(sqrt(x*x + y*y), z);
	float phi = atan2(y, x);

	float newx = pow(r, order) * sin(theta * order) * cos(phi * order);
	float newy = pow(r, order) * sin(theta * order) * sin(phi * order);
	float newz = pow(r, order) * cos(theta * order);

	x = newx;
	y = newy;
	z = newz;
}

void hypercomplex_multiply4(float p, float& x, float& y, float& z, float zr0, float& dr)
{
	if (x == 0 && y == 0 && z == 0)
		return;

	float zo0 = asinf(z / zr0);
	float zi0 = atan2f(y, x);
	float zr = powf(zr0, p - 1.0f);
	float zo = zo0 * p;
	float zi = zi0*p;
	float czo = cosf(zo);

	dr = zr * dr * p + 1.0f;
	zr *= zr0;

	x = zr * czo * cosf(zi);
	y = zr * czo * sinf(zi);
	z = zr * sinf(zo);
}

float fractal(float init_x, float init_y, float init_z, int* out_iterations, int max_iterations = 12)
{
	using namespace std::complex_literals;

	float z_x = init_x, z_y = init_y, z_z = init_z;
	float d_x = init_x, d_y = init_y, d_z = init_z;
	float dr = 1.0f;
	float r = sqrtf(z_x*z_x + z_y*z_y + z_z*z_z);
	float md = 10000.0f;
	const float power = 8.0f;
	const int clr_iterations = 2;
	const float bailout = 4.0f;

	for (int i = 0; i < max_iterations; i++)
	{
		//hypercomplex_multiply2(z_x, z_y, z_z);
		hypercomplex_multiply3(z_x, z_y, z_z, 8);
		//hypercomplex_multiply4(power, z_x, z_y, z_z, r, dr);
		z_x += init_x;
		z_y += init_y;
		z_z += init_z;

		r = sqrtf(z_x*z_x + z_y*z_y + z_z*z_z);

		if (r > bailout)
			break;
	}

	float distance = 0.5f * r * log(r) / sqrtf(d_x * d_x + d_y * d_y + d_z * d_z);
	return -distance;


	/*for (; z_x*z_x + z_y*z_y + z_z*z_z < 2.0 && iterations < max_iterations; iterations++)
	{
		hypercomplex_multiply3(z_x, z_y, z_z, 8);
		z_x += init_x;
		z_y += init_y;
		z_z += init_z;
	}

	float v = (float)iterations / (float)max_iterations;
	float mag = (float)(z_x*z_x + z_y*z_y + z_z*z_z);
	if (out_iterations)
		*out_iterations = iterations;

	return mag - (float)max_iterations / 2.0f;*/
}

float fractal_sphere(float init_x, float init_y, float init_z, int* out_iterations, int max_iterations = 4)
{
	float z_x = init_x, z_y = init_y, z_z = init_z;
	float d_x = init_x, d_y = init_y, d_z = init_z;
	const float scale = 2.0f;
	float k = scale;
	float d = -10000.0f;
	float d1 = 0, r = 0, md = 100000.0f, cd = 0.0f;
	const float sphere_holes = 4.0f;
	const float sphere_scale = 2.05f;

	for (int i = 0; i < max_iterations; i++)
	{
		float new_x = fmodf(init_x * k, sphere_holes) - 0.5f * sphere_holes;
		float new_y = fmodf(init_y * k, sphere_holes) - 0.5f * sphere_holes;
		float new_z = fmodf(init_z * k, sphere_holes) - 0.5f * sphere_holes;
		r = sqrtf(new_x*new_x + new_y*new_y + new_z*new_z);

		d1 = (sphere_scale - r) / k;
		k *= scale;

		d = max(d, d1);
	}

	return -d;
}

float World::Sample(DirectX::XMINT3 pos, int scale)
{
	return Sample((float)pos.x, (float)pos.y, (float)pos.z, scale);
}

float World::Sample(Vector3 pos, int scale)
{
	return Sample(pos.x, pos.y, pos.z, scale);
}

float World::Sample(float x, float y, float z, int level)
{
	float dx, dy, dz, n;

	switch (sample_type)
	{
	case USE_PLANE:
		return y;

	case USE_SPHERE:
		return Sphere(x, y, z);

	case USE_FRACTAL_2D:
		if (y < 0 || y > 0)
			return 1;
		dx = x / global_size;
		dz = z / global_size;
		return fractal(dx, dz, 0);

	case USE_FRACTAL_3D:
		dx = x / global_size;
		dy = y / global_size;
		dz = z / global_size;
		return fractal(dx, dy, dz, 0);

	case USE_FRACTAL_SPHERE:
		dx = x / global_size;
		dy = y / global_size;
		dz = z / global_size;
		return fractal_sphere(dx, dy, dz, 0);

	case USE_HEIGHTMAP:
		n = Noise(x, z, terrain_scale, 4);
		return y - n * terrain_height - terrain_height * 0.5f;

	default: // plane
		return y;
	}

	float scale = 65535.0f;
	const float kernel[7] = { 0.00598f,	0.060626f,	0.241843f,	0.383103f,	0.241843f,	0.060626f,	0.00598f, };
	int index = 0;
	if (level < 3)
		index = scale;
	for (int i = max(0, level - 3); i < level + 3; i++)
	{
		float weight = kernel[i];
		float new_x = x;
		index++;
	}

	//return base;
	//return y - Noise(x, z, 0.015f, 4) * range * 0.2f - range * 0.15f;
	//return y - Noise(x, y, z, 0.01f, 4) * 128.0f - 64.0f;

	return SimplexNoise::GenerateSimplexNoise(x, y, z);

	//return y;
	//return Sphere(x, y, z);
	//return Cube(x, y, z);
	//if (y < 1)
	//	return -1;
	//return 1;
	//float scaley = 1.0f * 16.0f;
	//return y - Noise(x, 0, z) * scaley - scaley;
	//float n = Noise(x, y, z, 0.005f, 2);
	//return n;
	//return y - (Noise(x, 0, z) + n) * 16.0f - 16.0f;
	//return Cube(x / range, y / range, z / range);

	//float rocks = Noise(x, y, z, 0.1f, 4) * 0.5f;
	//int octaves = max(1, (256 - scale) / 4);
	//float base = y / range * 4.0f - Noise(x, z, 1.0f / range, 16);
	//float true_scale = powf(2, (float)scale);
	//float base = Noise(x, z, 1 / range * scale, 1);
	//base = y / range * 4.0f - base;
	/*if(scale <= 8)
		base += Noise(x + 80000.0f, y + 80000.0f, z + 80000.0f, 1.0f / range * 8.0f, 1) / 2.0f;
	if(scale <= 4)
		base += Noise(x + 40000.0f, y + 40000.0f, z + 40000.0f, 1.0f / range * 16.0f, 1) / 4.0f;
	if (scale <= 2)
		base += Noise(x + 20000.0f, y + 20000.0f, z + 20000.0f, 1.0f / range * 32.0f, 1) / 8.0f*/
}
















float World::Noise(float x, float y, float z)
{
	const float scale = 0.04f;
	x *= scale;
	y *= scale;
	z *= scale;

	return SimplexNoise::GenerateSimplexNoise(x, y, z);
}

float World::Noise(float x, float y, float z, float scale, int octaves)
{
	x *= scale;
	y *= scale;
	z *= scale;

	return SimplexNoise::GenerateSimplexNoise(x, y, z, octaves, 0.5f);
}

float World::Noise(float x, float z, float scale, int octaves)
{
	x *= scale;
	z *= scale;

	return SimplexNoise::GenerateSimplexNoise(x, z, octaves, 0.5f);
}

float World::Sphere(float x, float y, float z)
{
	const float r = global_size / 2.0f;
	float origin[3] = { 0, 0, 0 };
	float dx = x - origin[0];
	float dy = y - origin[1];
	float dz = z - origin[2];

	return dx*dx + dy*dy + dz*dz - r*r;
	//Vector3 v(x, y, z);
	//Vector3 o(0, 0, 0);
	//return (v - o).LengthSquared() - r *r;
}

float World::Cube(float x, float y, float z)
{
	const float r = 224288.0f;
	Vector3 pos(x + r, y + r, z + r);
	Vector3 o(r, r, r);
	Vector3 local = pos - o;
	Vector3 d = Vector3(abs(local.x), abs(local.y), abs(local.z)) - o;
	float m = max(d.x, max(d.y, d.z));
	return min(m, d.Length());
}

Vector3 World::GetIntersection(const Vector3 p1, const Vector3 p2, float d1, float d2)
{
	return p1 + (Vector3)((p2 - p1) / (d2 - d1)) * (-d1);
}

Vector3 World::GetIntersection(const Vector3 pt0, const Vector3 pt1, int quality)
{
	float iso_value = 0.0f;

	Vector3 p0 = pt0;
	float v0 = Sample(pt0, 1);
	Vector3 p1 = pt1;
	float v1 = Sample(pt1, 1);
	if (v1 == v0)
		return pt0;

	float alpha = (0.0f - v0) / (v1 - v0);

	Vector3 pos = p0 + (p1 - p0) * alpha;

	float val = Sample(pos, 1);

	if (fabsf(iso_value - val) < 0.0000001f || quality == 0)
		return pos;

	if (val < 0.0f)
	{
		if (v0 > 0.0f)
			pos = GetIntersection(pos, pt0, quality - 1);
		else if (v1 > 0.0f)
			pos = GetIntersection(pos, pt1, quality - 1);
	}
	else if (val > 0.0f)
	{
		if (v0 < 0.0f)
			pos = GetIntersection(pt0, pos, quality - 1);
		else if (v1 < 0.0f)
			pos = GetIntersection(pt1, pos, quality - 1);
	}

	return pos;
}

Vector3 World::GetNormal(Vector3 v, int scale)
{
	/* A smaller h than this causes normals to have issues */
	if (sample_type == USE_SPHERE)
	{
		v.Normalize();
		return v;
	}
	float h = 0.1f;
	/*if (ImageData != null)
	{
		v = new Vector3((int)Math.Round(v.X), (int)Math.Round(v.Y), (int)Math.Round(v.Z));
		h = 1.0f;
	}*/
	float dxp = Sample(Vector3(v.x + h, v.y, v.z), scale);
	float dxm = Sample(Vector3(v.x - h, v.y, v.z), scale);
	float dyp = Sample(Vector3(v.x, v.y + h, v.z), scale);
	float dym = Sample(Vector3(v.x, v.y - h, v.z), scale);
	float dzp = Sample(Vector3(v.x, v.y, v.z + h), scale);
	float dzm = Sample(Vector3(v.x, v.y, v.z - h), scale);
	//Vector3 gradient = new Vector3(map[x + 1, y] - map[x - 1, y], map[x, y + 1] - map[x, y - 1]);
	Vector3 gradient = Vector3(dxp - dxm, dyp - dym, dzp - dzm);
	gradient.Normalize();
	return gradient;
}

Vector3 World::GetColor(Vector3 pos)
{
	Vector3 out;
	return Vector3(0.2f, 0.5f, 1.0f);
	const double range = pow(2.0, 22);
	//x /= range; y /= range; z /= range;
	double dx = (double)pos.x / (double)range;
	double dz = (double)pos.z / (double)range;
	int iterations;
	fractal(dx, dz, &iterations);

	float i = (float)iterations;
	out.x = i / 1024.0f;
	out.y = 0;
	out.z = 1.0f - i / 1024.0f;

	return out;
}
