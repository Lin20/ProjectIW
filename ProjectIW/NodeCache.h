#pragma once

#include <mutex>
#include <queue>
#include <atomic>
#include <DirectXMath.h>
#include "LRUCache.hpp"
#include "WorldTypes.h"

using namespace Utilities;
using namespace DirectX;

/// <summary>
/// This class handles creating, destroying, and retrieving world nodes, using a LRU system.
/// When a node is being divided, it first looks here to try and find a previously-generated node, and returns and removes it from the cache if found.
/// When nodes are grouped, their children are recursively added to the cache.
/// This class also handles the deletion of world nodes.
/// </summary>
class NodeCache
{
public:
	NodeCache();
	~NodeCache();

	WorldNode* Find(int64_t hash, XMINT3 expected_pos, unsigned int expected_size);
	void Add(WorldNode* node);

private:
	void ProcessDeleteQueue();

	std::mutex delete_mutex;

	std::queue<WorldNode*> delete_queue;
	std::condition_variable delete_condition;
	std::atomic<bool> stop_delete;

	lru_cache<int64_t, WorldNode*> cache;

	// IMPORTANT! This thread MUST come after the mutexes
	std::thread delete_thread;
};

