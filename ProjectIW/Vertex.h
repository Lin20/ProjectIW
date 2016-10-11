#pragma once

#include "SimpleMath.h"
#include "QEFSolver.h"

using namespace DirectX::SimpleMath;

namespace MDC
{
	enum VertexFlags
	{
		NONE = 0,
		COLLAPSIBLE = 1,
		FACEPROP2 = 2
	};

	class Vertex
	{
	public:
		Vertex();
		~Vertex();

		QEFSolver qef;
		Vertex* parent;
		Vector3 position;
		Vector3 normal;

		unsigned int index;
		int surface_index;
		unsigned char flags;
		float error;
		int euler;
		int eis[12];
		unsigned char in_cell;

		inline bool IsCollapsible() { return (flags & VertexFlags::COLLAPSIBLE) != 0; }
		inline bool IsManifold() { return euler == 1 && (flags & VertexFlags::FACEPROP2) != 0; }
	};
}
