#include "WorldProcessor.h"
#include "World.h"

WorldProcessor::WorldProcessor(unsigned int res) : main_tree(res), node_creation()
{
}

WorldProcessor::~WorldProcessor()
{
}

void WorldProcessor::Cleanup()
{
	MeshGeneration::Shutdown();
	node_creation.Shutdown();
	main_tree.Shutdown();
	Vector3 pos(0, 0, 0);
	while (!main_tree.CanExit())
	{
		main_tree.Update(pos);
		node_creation.TransferBatch(main_tree.GetDirtyNodes());
	}

}

unsigned int WorldProcessor::Update(Vector3 pos)
{
	unsigned int total_time = 0;

	total_time += main_tree.Update(pos);
	total_time += node_creation.TransferBatch(main_tree.GetDirtyNodes());

	return total_time;
}