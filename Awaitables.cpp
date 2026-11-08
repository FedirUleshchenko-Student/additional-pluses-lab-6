#include "Awaitables.h"

#include <iostream>

using namespace std;

#pragma region MasterAwaiter
MasterAwaiter::MasterAwaiter(
	std::vector<Task::promise_type::Handle>* newPlayers,
	int nextPlayerId, int newAnswer)
	: last_answer(newAnswer), players(newPlayers), next_player_id(nextPlayerId) {}

Task::promise_type::Handle
MasterAwaiter::await_suspend(Task::promise_type::Handle master_h) noexcept {

	//cout << "await_suspend master | answer = " << last_answer << endl;

	if (last_answer != IGNORED_VALUE) {
		master_h.promise().queries.back().answer = last_answer;
		//for (auto& handle : *players) {
		//	handle.promise().queries.back().answer = last_answer;
		//}
	}
	return (*players)[next_player_id];
}

int MasterAwaiter::await_resume() const noexcept {
	//cout << "await_resume master | guess = " << (*players)[next_player_id].promise().queries.back().guess << endl;
	return (*players)[next_player_id].promise().queries.back().guess;
}
#pragma endregion

#pragma region PlayerAwaiter
PlayerAwaiter::PlayerAwaiter(int newGuess, Task::promise_type::Handle* gameMaster) :
	my_guess(newGuess), master_handle(gameMaster) {
}

Task::promise_type::Handle
PlayerAwaiter::await_suspend(Task::promise_type::Handle player_h) noexcept {
	player_h.promise().queries.push_back({ my_guess, 0 });
	master_handle->promise().queries.push_back({my_guess, 0});
	return *master_handle;
}

pair<vector<Query>::const_iterator, vector<Query>::const_iterator> PlayerAwaiter::await_resume() const noexcept {
	vector<Query>::const_iterator st = (master_handle->promise().queries.size() >= GAMER_CNT) ?
		master_handle->promise().queries.end() - GAMER_CNT : master_handle->promise().queries.begin();

	return make_pair(st, master_handle->promise().queries.end());
}
#pragma endregion

#pragma region WinnerAwaiter
std::coroutine_handle<> FinalMasterAwaiter::await_suspend
(std::coroutine_handle<>) noexcept {
	if (winner_handle)
		return winner_handle;
	// no winner (error?) - go to main
	// blank coroutine
	return std::noop_coroutine();
}
#pragma endregion
