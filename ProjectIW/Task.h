#pragma once

#include <functional>

using namespace std;

enum TaskPriority { Low, Medium, High, Urgent };

class Task
{
public:
	inline Task()
	{
	}

	inline Task(function<void()> t_function, function <void()> t_callback, TaskPriority p)
	{
		task_function = t_function;
		callback_function = t_callback;
		priority = p;
	}

	inline ~Task() { }

	inline TaskPriority GetPriority() { return priority; }

	inline void Execute() { if (task_function != nullptr) task_function(); }
	inline void Callback() { if (callback_function != nullptr) callback_function(); }

private:
	function<void()> task_function;
	function<void()> callback_function;

	TaskPriority priority;
};
