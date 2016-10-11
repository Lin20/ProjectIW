#pragma once

#include "General.h"
#include <math.h>

extern const unsigned int perm[512];
static const int grad3[12][3] = {
	{ 1, 1, 0 }, { -1, 1, 0 }, { 1, -1, 0 }, { -1, -1, 0 },
	{ 1, 0, 1 }, { -1, 0, 1 }, { 1, 0, -1 }, { -1, 0, -1 },
	{ 0, 1, 1 }, { 0, -1, 1 }, { 0, 1, -1 }, { 0, -1, -1 }
};
static const int grad4[32][4] = {
	{ 0, 1, 1, 1 }, { 0, 1, 1, -1 }, { 0, 1, -1, 1 }, { 0, 1, -1, -1 },
	{ 0, -1, 1, 1 }, { 0, -1, 1, -1 }, { 0, -1, -1, 1 }, { 0, -1, -1, -1 },
	{ 1, 0, 1, 1 }, { 1, 0, 1, -1 }, { 1, 0, -1, 1 }, { 1, 0, -1, -1 },
	{ -1, 0, 1, 1 }, { -1, 0, 1, -1 }, { -1, 0, -1, 1 }, { -1, 0, -1, -1 },
	{ 1, 1, 0, 1 }, { 1, 1, 0, -1 }, { 1, -1, 0, 1 }, { 1, -1, 0, -1 },
	{ -1, 1, 0, 1 }, { -1, 1, 0, -1 }, { -1, -1, 0, 1 }, { -1, -1, 0, -1 },
	{ 1, 1, 1, 0 }, { 1, 1, -1, 0 }, { 1, -1, 1, 0 }, { 1, -1, -1, 0 },
	{ -1, 1, 1, 0 }, { -1, 1, -1, 0 }, { -1, -1, 1, 0 }, { -1, -1, -1, 0 }
};
static const int simplex[64][4] = {
	{ 0, 1, 2, 3 }, { 0, 1, 3, 2 }, { 0, 0, 0, 0 }, { 0, 2, 3, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 1, 2, 3, 0 },
	{ 0, 2, 1, 3 }, { 0, 0, 0, 0 }, { 0, 3, 1, 2 }, { 0, 3, 2, 1 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 1, 3, 2, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 1, 2, 0, 3 }, { 0, 0, 0, 0 }, { 1, 3, 0, 2 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 3, 0, 1 }, { 2, 3, 1, 0 },
	{ 1, 0, 2, 3 }, { 1, 0, 3, 2 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 2, 0, 3, 1 }, { 0, 0, 0, 0 }, { 2, 1, 3, 0 },
	{ 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 },
	{ 2, 0, 1, 3 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 3, 0, 1, 2 }, { 3, 0, 2, 1 }, { 0, 0, 0, 0 }, { 3, 1, 2, 0 },
	{ 2, 1, 0, 3 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 0, 0, 0, 0 }, { 3, 1, 0, 2 }, { 0, 0, 0, 0 }, { 3, 2, 0, 1 }, { 3, 2, 1, 0 }
};

class SimplexNoise
{
public:
	SimplexNoise();
	~SimplexNoise();
	static float GenerateSimplexNoise(float x);
	static float GenerateSimplexNoise(float x, float y);
	static float GenerateSimplexNoise(float x, float y, int octaves, float pers);
	static float GenerateSimplexNoise(float x, float y, float z, int octaves, float pers);
	static float GenerateSimplexNoise(float x, float y, float z, float w);
	static float SeamlessNoise(float x, float y, float dx, float dy, float xyOffset, int octaves, float pers);
	static float SeamlessNoise(float x, float y, float dx, float dy, float xyOffset);
	inline static float GenerateSimplexNoise(float x, float y, float z)
	{
		{
			float n0, n1, n2, n3; // Noise contributions from the four corners

								  // Skew the input space to determine which simplex cell we're in
			float F3 = 1.0f / 3.0f;
			float s = (x + y + z)*F3; // Very nice and simple skew factor for 3D
			int i = FastFloor(x + s);
			int j = FastFloor(y + s);
			int k = FastFloor(z + s);

			float G3 = 1.0f / 6.0f; // Very nice and simple unskew factor, too
			float t = (i + j + k)*G3;
			float X0 = i - t; // Unskew the cell origin back to (x,y,z) space
			float Y0 = j - t;
			float Z0 = k - t;
			float x0 = x - X0; // The x,y,z distances from the cell origin
			float y0 = y - Y0;
			float z0 = z - Z0;

			// For the 3D case, the simplex shape is a slightly irregular tetrahedron.
			// Determine which simplex we are in.
			int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
			int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords

			if (x0 >= y0) {
				if (y0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // X Y Z order
				else if (x0 >= z0) { i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; } // X Z Y order
				else { i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; } // Z X Y order
			}
			else { // x0<y0
				if (y0 < z0) { i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; } // Z Y X order
				else if (x0 < z0) { i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; } // Y Z X order
				else { i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; } // Y X Z order
			}

			// A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
			// a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
			// a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
			// c = 1/6.
			float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
			float y1 = y0 - j1 + G3;
			float z1 = z0 - k1 + G3;
			float x2 = x0 - i2 + 2.0f*G3; // Offsets for third corner in (x,y,z) coords
			float y2 = y0 - j2 + 2.0f*G3;
			float z2 = z0 - k2 + 2.0f*G3;
			float x3 = x0 - 1.0f + 3.0f*G3; // Offsets for last corner in (x,y,z) coords
			float y3 = y0 - 1.0f + 3.0f*G3;
			float z3 = z0 - 1.0f + 3.0f*G3;

			// Work out the hashed gradient indices of the four simplex corners
			int ii = i & 255;
			int jj = j & 255;
			int kk = k & 255;
			int gi0 = perm[ii + perm[jj + perm[kk]]] % 12;
			int gi1 = perm[ii + i1 + perm[jj + j1 + perm[kk + k1]]] % 12;
			int gi2 = perm[ii + i2 + perm[jj + j2 + perm[kk + k2]]] % 12;
			int gi3 = perm[ii + 1 + perm[jj + 1 + perm[kk + 1]]] % 12;

			// Calculate the contribution from the four corners
			float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;
			if (t0 < 0) n0 = 0.0f;
			else {
				t0 *= t0;
				n0 = t0 * t0 * Dot(grad3[gi0], x0, y0, z0);
			}

			float t1 = 0.6f - x1*x1 - y1*y1 - z1*z1;
			if (t1 < 0) n1 = 0.0f;
			else {
				t1 *= t1;
				n1 = t1 * t1 * Dot(grad3[gi1], x1, y1, z1);
			}

			float t2 = 0.6f - x2*x2 - y2*y2 - z2*z2;
			if (t2 < 0) n2 = 0.0f;
			else {
				t2 *= t2;
				n2 = t2 * t2 * Dot(grad3[gi2], x2, y2, z2);
			}

			float t3 = 0.6f - x3*x3 - y3*y3 - z3*z3;
			if (t3 < 0) n3 = 0.0f;
			else {
				t3 *= t3;
				n3 = t3 * t3 * Dot(grad3[gi3], x3, y3, z3);
			}

			// Add contributions from each corner to get the final noise value.
			// The result is scaled to stay just inside [-1,1]
			return 32.0f*(n0 + n1 + n2 + n3);

		}
	}

private:

	static int Mod(int x, int m);

	inline static float grad(int hash, float x)
	{
		int h = hash & 15;
		float grad = 1.0f + (h & 7);   // Gradient value 1.0, 2.0, ..., 8.0
		if ((h & 8)) grad = -grad;         // Set a random sign for the gradient
		return (grad * x);           // Multiply the gradient with the distance
	}

	inline static float grad(int hash, float x, float y)
	{
		/*int h = hash & 7;      // Convert low 3 bits of hash code
		float u = h < 4 ? x : y;  // into 8 simple gradient directions,
		float v = h < 4 ? y : x;  // and compute the dot product with (x,y).
		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);*/

		switch (hash & 7)
		{
		case 0: return x + 2.0f * y;
		case 1: return -x + 2.0f * y;
		case 2: return x - 2.0f * y;
		case 3: return -x - 2.0f * y;
		case 4: return y + 2.0f * x;
		case 5: return -y + 2.0f * x;
		case 6: return y - 2.0f * x;
		case 7: return -y - 2.0f * x;
		}
	}

	inline static float grad(int hash, float x, float y, float z)
	{
		switch (hash & 0xF)
		{
		case 0x0: return  x + y;
		case 0x1: return -x + y;
		case 0x2: return  x - y;
		case 0x3: return -x - y;
		case 0x4: return  x + z;
		case 0x5: return -x + z;
		case 0x6: return  x - z;
		case 0x7: return -x - z;
		case 0x8: return  y + z;
		case 0x9: return -y + z;
		case 0xA: return  y - z;
		case 0xB: return -y - z;
		case 0xC: return  y + x;
		case 0xD: return -y + z;
		case 0xE: return  y - x;
		case 0xF: return -y - z;
		default: return 0; // never happens
		}
	}

	inline static float grad(int hash, float x, float y, float z, float t)
	{
		int h = hash & 31;      // Convert low 5 bits of hash code into 32 simple
		float u = h < 24 ? x : y; // gradient directions, and compute dot product.
		float v = h < 16 ? y : z;
		float w = h < 8 ? z : t;
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v) + ((h & 4) ? -w : w);
	}

	inline static float Dot(const int* g, const float x, const float y, const float z) { return g[0] * x + g[1] * y + g[2] * z; }
	inline static float Dot(const int* g, const float x, const float y, const float z, const float w) { return g[0] * x + g[1] * y + g[2] * z + g[3] * w; }
	inline static int FastFloor(const float x) { return (int)x - (x < (int)x); }
};