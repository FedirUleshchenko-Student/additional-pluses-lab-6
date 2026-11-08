#pragma once
#include <coroutine>
#include <vector>
#include <utility>
#include "global-consts.h"

struct PlayerAwaiter;
struct MasterAwaiter;

class Task {
public:
	struct promise_type
	{
		using Handle = std::coroutine_handle<promise_type>;
		std::vector<int> guesses;
		std::vector<int> answers;

		// for master ()
		promise_type() = default;
		promise_type(std::vector<Handle>* players);

		// Compiler would give args from gamerSmart(id, master)
		promise_type(int id, Handle* master);

		Task get_return_object() { return Task(Handle::from_promise(*this)); }
		std::suspend_always initial_suspend() { return {}; }
		std::suspend_always final_suspend() noexcept { return {}; }
		void return_void() noexcept {}
		void unhandled_exception() { terminate(); }

		PlayerAwaiter yield_value(int my_guess);
		MasterAwaiter yield_value(std::pair<int, int> waiting_query);

	private:
		Handle* master_h{};
		std::vector<Handle>* total_players{};
	};

#pragma region Rule of 5
	Task(const Task&) = delete;
	Task& operator=(const Task&) = delete;
	Task(Task&& t) noexcept;
	Task& operator=(Task&& t) noexcept;
	~Task();
#pragma endregion

	explicit Task(promise_type::Handle coroutine);

	bool is_done() const;
	void resume();
	void destroy();

	// is a pointer?
	promise_type::Handle cor{};
};