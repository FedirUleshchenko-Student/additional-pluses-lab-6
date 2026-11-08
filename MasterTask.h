#pragma once
#include "Task.h"

class MasterTask : public TaskBase {
public:
	struct promise_type
	{
		using Handle = std::coroutine_handle<promise_type>;
		std::vector<Query> queries;

		promise_type() = default;
		// for master ()
		promise_type(std::vector<std::coroutine_handle<>>* players);

		MasterTask get_return_object() { return MasterTask(Handle::from_promise(*this)); }
		std::suspend_always initial_suspend() { return {}; }
		FinalMasterAwaiter final_suspend() noexcept;
		void unhandled_exception() { std::terminate(); }

		void return_value(std::pair<int, int> end_query);

		MasterAwaiter yield_value(std::pair<int, int> waiting_query);

	private:
		std::coroutine_handle<> winner_handle = nullptr;
		std::vector<std::coroutine_handle<>>* total_players = nullptr;
	};

	// constructor inheritance
	using TaskBase::TaskBase;
};

