#pragma once
#include <coroutine>
#include <vector>
#include <utility>
#include "global-consts.h"

struct PlayerAwaiter;
struct MasterAwaiter;
struct FinalMasterAwaiter;

struct Query {
	Query() = default;
	Query(int newGuess, int newAnswer) :
		guess(newGuess), answer(newAnswer) {}
	int guess = 0;
	int answer = 0;
};

class Task {
public:
	struct promise_type
	{
		using Handle = std::coroutine_handle<promise_type>;
		std::vector<Query> queries;

		//promise_type() = default;
		// for master ()
		promise_type(std::vector<Handle>* players);

		// Compiler would give args from gamerSmart(id, master)
		promise_type(int id, Handle* master);

		Task get_return_object() { return Task(Handle::from_promise(*this)); }
		std::suspend_always initial_suspend() { return {}; }
		FinalMasterAwaiter final_suspend() noexcept;
		//void return_void() noexcept {}
		void unhandled_exception() { std::terminate(); }

		void return_value(std::pair<int, int> end_query);

		PlayerAwaiter yield_value(int my_guess);
		MasterAwaiter yield_value(std::pair<int, int> waiting_query);

	private:
		std::coroutine_handle<> winner_handle = nullptr;
		Handle* master_h = nullptr;
		std::vector<Handle>* total_players = nullptr;
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
	std::coroutine_handle<> cor{};
};