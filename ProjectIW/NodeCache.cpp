#include "NodeCache.h"
#include "WorldNode.h"

#define MAX_CACHED_NODES 1048576

NodeCache::NodeCache() : stop_delete(false), cache(MAX_CACHED_NODES), delete_thread(&NodeCache::ProcessDeleteQueue, this)
{
}

NodeCache::~NodeCache()
{
	stop_delete = true;

	delete_condition.notify_all();
	delete_thread.join();


	// Cleanup the delete queue
	while (delete_queue.size() > 0)
	{
		WorldNode* node = delete_queue.front();
		delete_queue.pop();

		if (node)
		{
			int stage = false;
			do
			{
				stage = node->GetStage();
			} while (stage == NODE_STAGE_MESH_WAITING || stage == NODE_STAGE_MESH_GENERATING);
			delete node;
		}
	}

	// Cleanup the cache
	auto it = cache.get_items_list().begin();
	for (auto it = cache.get_items_list().begin(); it != cache.get_items_list().end(); ++it)
	{
		WorldNode* node = it->second;

		if (node)
		{
			int stage = 0;
			do
			{
				stage = node->GetStage().load();
			} while (stage != NODE_STAGE_READY);
			delete node;
		}
	}
}

WorldNode* NodeCache::Find(int64_t hash, XMINT3 expected_pos, unsigned int expected_size)
{
	if (MAX_CACHED_NODES == 0)
		return 0;

	WorldNode* node = 0;
	if (cache.exists(hash))
	{
		node = cache.get(hash);

		if (node->position.x != expected_pos.x || node->position.y != expected_pos.y || node->position.z != expected_pos.z || node->size != expected_size)
		{
			node = 0;
		}
		else
		{
			cache.remove(hash);
		}
	}

	return node;
}

void NodeCache::Add(WorldNode* node)
{
	if (!node || MAX_CACHED_NODES == 0)
		return;

	WorldNode* oldest = 0;
	WorldNode* duplicate = 0;
	cache.put(node->Hash(), node, &oldest, &duplicate);
	if (oldest || duplicate)
	{
		{
			std::unique_lock<std::mutex> delete_lock(delete_mutex);
			if (oldest)
			{
				delete_queue.push(oldest);
				oldest->request_abandon = true;
			}
			if (duplicate)
			{
				delete_queue.push(duplicate);
				duplicate->request_abandon = true;
			}
		}
		delete_condition.notify_one();
	}
}

void NodeCache::ProcessDeleteQueue()
{
	while (!stop_delete)
	{
		{
			std::unique_lock<std::mutex> lock(delete_mutex);
			while (!stop_delete && delete_queue.empty())
				delete_condition.wait(lock);

			if (stop_delete)
				return;
			WorldNode* node = delete_queue.front();
			delete_queue.pop();

			// The node should always exist.
			// If it does, only delete it if it's not generating. If it is, re-add it to the delete queue for further processing.
			if (node)
			{
				int stage = node->GetStage();
				if (stage == NODE_STAGE_READY)
				{
					// Unlock the lock so other threads aren't waiting on delete
					lock.unlock();
					delete node;
				}
				else
					delete_queue.push(node);
			}
		}
	}
}
