#include "Awaitables.h"

#include <iostream>

using namespace std;

#pragma region MasterAwaiter
MasterAwaiter::MasterAwaiter(
	std::vector<std::coroutine_handle<>>* newPlayers,
	int nextPlayerId, int newAnswer)
	: last_answer(newAnswer), players(newPlayers), next_player_id(nextPlayerId) {}

coroutine_handle<>
MasterAwaiter::await_suspend(MasterTask::promise_type::Handle master_h) noexcept {

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
	return
		PlayerTask::promise_type::Handle::from_address((*players)[next_player_id].address())
		.promise().queries.back().guess;
}
#pragma endregion

#pragma region PlayerAwaiter
PlayerAwaiter::PlayerAwaiter(int newGuess, std::coroutine_handle<>* gameMaster) :
	my_guess(newGuess), master_handle(gameMaster) {
}

std::coroutine_handle<>
PlayerAwaiter::await_suspend(PlayerTask::promise_type::Handle player_h) noexcept {
	player_h.promise().queries.push_back({ my_guess, 0 });
	MasterTask::promise_type::Handle::from_address(master_handle->address())
		.promise().queries.push_back({my_guess, 0});
	return *master_handle;
}

pair<vector<Query>::const_iterator, vector<Query>::const_iterator> PlayerAwaiter::await_resume() const noexcept {
	auto master = MasterTask::promise_type::Handle::from_address(master_handle->address());
	vector<Query>::const_iterator st = (master.promise().queries.size() >= GAMER_CNT) ?
		master.promise().queries.end() - GAMER_CNT : master.promise().queries.begin();

	return make_pair(st, master.promise().queries.end());
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
