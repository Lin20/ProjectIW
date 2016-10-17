#pragma once

#include <atomic>

class WorldNode;
class NodeCache;

namespace MDC
{
	class TerrainChunk;
}

enum NodeOutputType
{
	NODE_OUTPUT_DIVIDE = 0,
	NODE_OUTPUT_GROUP = 1
};
struct DirtyNodeOutput
{
	NodeOutputType type;
	WorldNode* node;
};

/// <summary>
/// The possible node stages.
/// 0 = unspecified
/// 1 = in the NodeCreation stage to be split or grouped
/// 2 = in the MeshGeneration queue waiting to be processed
/// 3 = in the MeshGeneration stage and being generated
/// 4 = fully complete and ready to render as a leaf node
/// 5 = in the NodeCreation stage with full mesh
/// </summary>
/// <remarks>
/// There are 2 mesh stages simply for debugging purposes.
/// </remarks>
enum WorldNodeStages
{
	NODE_STAGE_UNSPECIFIED = 0,
	NODE_STAGE_CREATION = 1,
	NODE_STAGE_MESH_WAITING = 2,
	NODE_STAGE_MESH_GENERATING = 3,
	NODE_STAGE_READY = 4,
	NODE_STAGE_SPLITTABLE = 5
};

// Uniform resolution of a chunk
#define NODE_CHUNK_RESOLUTION 8
//How many chunks to have before a LOD split
#define NODE_LOD_THRESHOLD 1.0f