#pragma once
#include "Task.h"

class PlayerTask : public TaskBase {
public:
	struct promise_type {
		using Handle = std::coroutine_handle<promise_type>;
		std::vector<Query> queries;

		promise_type() = default;

		// Compiler would give args from gamerSmart(id, master)
		promise_type(int id, std::coroutine_handle<>* master);

		PlayerTask get_return_object() { return PlayerTask(Handle::from_promise(*this)); }
		std::suspend_always initial_suspend() noexcept { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void unhandled_exception() { std::terminate(); }
		void return_void() noexcept {}

		PlayerAwaiter yield_value(int my_guess);
	private:
		std::coroutine_handle<>* master_h = nullptr;
	};

	// constructor inheritance
	using TaskBase::TaskBase;
};