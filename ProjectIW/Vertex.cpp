#include "Vertex.h"

namespace MDC
{
	Vertex::Vertex() : parent(0), index(-1), surface_index(-1), flags(0), position(Vector3(0, 0, 0)), normal(Vector3(0, 0, 0)), error(0), euler(0), in_cell(0)
	{
		ZeroMemory(eis, sizeof(int) * 12);
	}

	Vertex::~Vertex()
	{
	}
}
