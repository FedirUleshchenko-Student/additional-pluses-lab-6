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

	MasterAwaiter(int newAnswer,
		std::vector<Task::promise_type::Handle>* newPlayers,
		int nextPlayerId);

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

	std::pair<std::vector<int>::const_iterator, std::vector<int>::const_iterator>
		await_resume() const noexcept;
};
