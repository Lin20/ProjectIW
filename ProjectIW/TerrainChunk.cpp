#include "TerrainChunk.h"
#include "OctreeNode.h"
#include "WorldTypes.h"

namespace MDC
{
	TerrainChunk::TerrainChunk(XMINT3 pos, unsigned int scale)
	{
		octree = new OctreeNode(pos, NODE_CHUNK_RESOLUTION, false, scale);
		vbo = 0;
		ibo = 0;
		empty = false;
	}

	TerrainChunk::~TerrainChunk()
	{
		if (octree)
		{
			delete octree;
			octree = 0;
		}
		if (vbo)
		{
			delete vbo;
			vbo = 0;
		}
		if (ibo)
		{
			delete ibo;
			ibo = 0;
		}
	}

	unsigned int TerrainChunk::GenerateOctree()
	{
		auto start = std::chrono::high_resolution_clock::now();

		octree->ConstructNodes();
		empty = octree->is_leaf;
		
		//octree->ClusterCellBase(0.0f);
		std::vector<VertexPositionColorNormal> verts;
		octree->GenerateVertexBuffer(verts);
		if (verts.size() > 0)
			vbo = new VBO(&verts[0], sizeof(VertexPositionColorNormal) * verts.size(), sizeof(VertexPositionColorNormal), false);

		return (unsigned int)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)).count();
	}

	unsigned int TerrainChunk::GenerateMesh()
	{
		auto start = std::chrono::high_resolution_clock::now();

		std::vector<unsigned int> indexes;

		octree->ProcessCell(indexes, 0.0f);
		if (indexes.size() > 0)
		{
			if (!ibo)
			{
				ibo = new IBO(&indexes[0], sizeof(unsigned int) * indexes.size(), false);
			}
			else
			{
				ibo->SetData(&indexes[0], 0, sizeof(unsigned int) * indexes.size());
			}
		}
		tri_count = indexes.size();

		if (octree)
		{
			delete octree;
			octree = 0;
		}

		return (unsigned int)(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start)).count();
	}
}
