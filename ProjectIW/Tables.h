#pragma once

#include <DirectXMath.h>
#include "SimpleMath.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace MDC
{
	extern const int num_vertices[256];
	extern const int edge_table[256][16];

	extern const int corner_deltas[8];
	extern const Vector3 corner_deltas_f[8];
	extern const int edge_pairs[12][3];

	extern const int cell_proc_edge_mask[6][5];
	extern const int face_proc_face_mask[3][4][3];
	extern const int face_proc_edge_mask[3][4][6];
	extern const int edge_proc_edge_mask[3][2][5];
	extern const int process_edge_mask[3][4];

	extern const int external_edges[8][3];
	extern const int internal_edges[8][9];
	extern const int faces[6][4];
}