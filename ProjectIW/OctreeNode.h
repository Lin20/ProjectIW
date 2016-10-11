#pragma once

#include <vector>
#include <DirectXMath.h>
#include "General.h"
#include "SimpleMath.h"
#include "VertexPositionColorNormal.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace MDC
{
	class OctreeNode
	{
	public:
		OctreeNode();
		OctreeNode(XMINT3 position, unsigned short size, bool is_leaf, unsigned int scale);
		~OctreeNode();

		bool ConstructNodes();
		bool ConstructLeaf();
		void GenerateVertexBuffer(std::vector<VertexPositionColorNormal>& vertices);

		void ProcessCell(std::vector<unsigned int>& indexes, float threshold);
		static void ProcessFace(OctreeNode** nodes, int direction, std::vector<unsigned int>& indexes, float threshold);
		static void ProcessEdge(OctreeNode** nodes, int direction, std::vector<unsigned int>& indexes, float threshold);
		static void ProcessIndexes(OctreeNode** nodes, int direction, std::vector<unsigned int>& indexes, float threshold);

		void ClusterCellBase(float error);
		void ClusterCell(float error);
		static void ClusterFace(OctreeNode** nodes, int direction, int& surface_index, std::vector<Vertex*>& collected_vertices);
		static void ClusterEdge(OctreeNode** nodes, int direction, int& surface_index, std::vector<Vertex*>& collected_vertices);
		static void ClusterIndexes(OctreeNode** nodes, int direction, int& max_surface_index, std::vector<Vertex*>& collected_vertices);
		static void AssignSurface(std::vector<Vertex*>& vertices, int from, int to);


		OctreeNode* children[8];
		Vertex* vertices;

		XMINT3 position;
		int scale;
		short size;
		unsigned char child_index;
		bool is_leaf;
		unsigned char corners;
		unsigned char vertex_count;
	};
}
