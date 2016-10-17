#include "ThreadPool.h"

ThreadPool::ThreadPool(int n_threads)
{
	num_threads = n_threads;
	active = true;
	for (int i = 0; i < n_threads; i++)
		worker_threads.push_back(thread(&ThreadPool::ThreadWork, this));
}

ThreadPool::~ThreadPool()
{
	if (active)
		Quit();
	
}

void ThreadPool::AddTask(Task t)
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		if (!active)
			return;
		task_queue.push_back(t);
	}

	condition.notify_one();
}

void ThreadPool::AddTaskBatch(std::vector<Task>& batch)
{
	if (batch.size() == 0)
		return;

	{
		std::unique_lock<std::mutex> lock(queue_mutex);

		if (!active)
			return;
		task_queue.insert(task_queue.end(), batch.begin(), batch.end());
	}

	condition.notify_one();
}

void ThreadPool::ThreadWork()
{
	//thanks to http://progsch.net/wordpress/?p=81 for a good idea on how to do this
	//note: there might be deactivating problems
	//TODO: fix if it becomes an issue
	Task t;
	while (active)
	{
		{
			std::unique_lock<std::mutex> lock(queue_mutex);
			while (active && task_queue.empty())
				condition.wait(lock);

			if (!active)
				return;
			t = task_queue.front();
			task_queue.pop_front();
		}

		if (!active)
			return;
		t.Execute();
		if (!active)
			return;
		t.Callback();
	}
}

void ThreadPool::Quit()
{
	{
		std::unique_lock<std::mutex> lock(queue_mutex);
		active = false;
	}
	condition.notify_all();
	for (int i = 0; i < num_threads; i++)
	{
		worker_threads[i].join();
	}
}

void ThreadPool::JoinAllAndAbort()
{
	condition.notify_all();
	for (int i = 0; i < num_threads; i++)
	{
		worker_threads[i].join();
	}
}
