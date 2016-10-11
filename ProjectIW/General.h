#pragma once

#include <DirectXMath.h>

#define PI 3.1415926535897932384626433832795f
#define CROSS(u, v) XMFLOAT3(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x)
#define DOT(u, v) u.x * v.x + u.y * v.y + u.z * v.z
#define ADD(u, v) XMFLOAT3(u.x + v.x, u.y + v.y, u.z + v.z)
#define ABS(x) (x >= 0 ? x : -x);

#define SDELETE(x) if(x){delete x;x=0;}
#define SDELETEARRAY(x, size) for(int i=0;i<size;i++){if(x[i]){delete x[i];x[i]=0;}}

enum ShaderStages { VS, PS, GS };

extern int BUFFER_WIDTH;
extern int BUFFER_HEIGHT;
extern int edgeTable[256];

class Engine;
class RenderEngine;
class Scene;
class Camera;
class VBO;
class IBO;
class Shader;
class ShaderGroup;
class ConstantsBuffer;
class DirectInput;
class InputController;
class SimplexNoise;

namespace MDC
{
	class OctreeNode;
	class TerrainChunk;
	class Vertex;
}

class WorldNode;

extern Engine* engine;
bool RayIntersectsTriangle(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end, DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b, DirectX::XMFLOAT3 c);