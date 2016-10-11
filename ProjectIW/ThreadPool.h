#pragma once

#include <thread>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "Task.h"

using namespace std;

class ThreadPool
{
public:
	ThreadPool(int n_threads);
	~ThreadPool();

	inline bool IsActive() { return active; }

	void AddTask(Task t);
	void ThreadWork();
	void Quit();
	void JoinAllAndAbort();

private:
	int num_threads;
	vector<thread> worker_threads;

	deque<Task> task_queue;
	mutex queue_mutex;
	condition_variable condition;

	bool active;
};
