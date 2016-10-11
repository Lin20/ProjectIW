#pragma once

#include <chrono>
#include <DirectXMath.h>
#include "General.h"
#include "IBO.h"
#include "VBO.h"

using namespace DirectX;

#define CHUNK_SIZE 32

namespace MDC
{
	class TerrainChunk
	{
	public:
		TerrainChunk(XMINT3 pos, unsigned int scale);
		~TerrainChunk();

		unsigned int GenerateOctree();
		unsigned int GenerateMesh();

		inline VBO* GetVBO() { return vbo; }
		inline IBO* GetIBO() { return ibo; }
		inline unsigned int GetTriangleCount() { return tri_count; }
		inline bool IsEmpty() { return empty; }

	private:
		OctreeNode* octree;
		VBO* vbo;
		IBO* ibo;
		unsigned int tri_count;
		bool empty;
	};
}
