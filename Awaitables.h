#pragma once
#include <coroutine>
#include <vector>
#include <utility>
#include "Task.h"
#include "global-consts.h"

struct MasterAwaiter {
	int last_answer;
	std::vector<Task::promise_type::Handle>* players;
	int next_player_id;
	static const int IGNORED_VALUE = 2;

	MasterAwaiter(
		std::vector<Task::promise_type::Handle>* newPlayers,
		int nextPlayerId, int newAnswer);

	constexpr bool await_ready() const noexcept { return false; }

	Task::promise_type::Handle
		await_suspend(Task::promise_type::Handle master_h) noexcept;

	int await_resume() const noexcept;
};

struct PlayerAwaiter {
	int my_guess;
	Task::promise_type::Handle* master_handle;

	PlayerAwaiter(int newGuess, Task::promise_type::Handle* gameMaster);

	constexpr bool await_ready() const noexcept { return false; }

	Task::promise_type::Handle
		await_suspend(Task::promise_type::Handle player_h) noexcept;

	std::pair<std::vector<Query>::const_iterator, std::vector<Query>::const_iterator>
		await_resume() const noexcept;
};

struct FinalMasterAwaiter {
	std::coroutine_handle<> winner_handle;

	FinalMasterAwaiter(std::coroutine_handle<> x) :
		winner_handle(x) {}

	constexpr bool await_ready() const noexcept { return false; }
	std::coroutine_handle<> await_suspend(std::coroutine_handle<>) noexcept;
	constexpr void await_resume() const noexcept {}
};