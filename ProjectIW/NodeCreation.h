#pragma once

#include <vector>
#include <atomic>
#include "WorldTypes.h"
#include "ThreadPool.h"
#include "NodeCache.h"

/// <summary>
/// This is the node creation stage of the world pipeline.
/// It accepts a vector of DirtyNodeOutput containing nodes that either need to be divided or grouped and adds them to another queue, which is then processed to create/destroy nodes.
/// </summary>
class NodeCreation
{
public:
	NodeCreation();
	~NodeCreation();
	
	/// <summary>
	/// Sets the shutdown flag to allow full grouping of nodes.
	/// </summary>
	void Shutdown();
	
	/// <summary>
	/// Transfers the batch of dirty nodes to the processor to handle them. To be ran on same thread as WorldTree's initial processing.
	/// </summary>
	/// <param name="batch">Batch of dirty nodes to be divided or grouped.</param>
	/// <returns>The amount of time in us it took.</returns>
	unsigned int TransferBatch(std::vector<DirtyNodeOutput>& batch);
	
	/// <summary>
	/// Processes the node.
	/// </summary>
	/// <param name="n">The node to be divided or grouped.</param>
	/// <param name="last_node">The list containing the last node.</param>
	static void ProcessNode(DirtyNodeOutput n, NodeCache* cache, std::atomic<bool>& shutdown);
	
	/// <summary>
	/// Passes the node to the mesh generation stage.
	/// </summary>
	/// <param name="node">The node to pass.</param>
	static void PassNodeToMeshGeneration(WorldNode* node);

private:
	/// <summary>
	/// The threadpool that handles executing node divisions and groups.
	/// </summary>
	ThreadPool threadpool;
		
	/// <summary>
	/// The cache used for looking up previously-generated nodes.
	/// </summary>
	NodeCache cache;

	std::vector<Task> task_queue;
	std::atomic<bool> shutdown;
};

