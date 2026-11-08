#include "Awaitables.h"

#include <iostream>

using namespace std;

#pragma region MasterAwaiter
MasterAwaiter::MasterAwaiter(int newAnswer,
	std::vector<Task::promise_type::Handle>* newPlayers,
	int nextPlayerId)
	: last_answer(newAnswer), players(newPlayers), next_player_id(nextPlayerId) {
}

Task::promise_type::Handle
MasterAwaiter::await_suspend(Task::promise_type::Handle master_h) noexcept {

	cout << "await_suspend master | answer = " << last_answer << endl;
	if (last_answer != IGNORED_VALUE) {
		master_h.promise().answers.push_back(last_answer);
		for (auto& handle : *players) {
			handle.promise().answers.push_back(last_answer);
		}
	}
	return (*players)[next_player_id];
}

int MasterAwaiter::await_resume() const noexcept {
	cout << "await_resume master | guess = " << (*players)[next_player_id].promise().guesses.back() << endl;
	return (*players)[next_player_id].promise().guesses.back();
}
#pragma endregion

#pragma region PlayerAwaiter
PlayerAwaiter::PlayerAwaiter(int newGuess, Task::promise_type::Handle* gameMaster) :
	my_guess(newGuess), master_handle(gameMaster) {
}

Task::promise_type::Handle
PlayerAwaiter::await_suspend(Task::promise_type::Handle player_h) noexcept {
	cout << "await_suspend player | guess = " << my_guess << endl;
	player_h.promise().guesses.push_back(my_guess);
	master_handle->promise().guesses.push_back(my_guess);
	return *master_handle;
}

pair<vector<int>::const_iterator, vector<int>::const_iterator> PlayerAwaiter::await_resume() const noexcept {
	//cout << "await_resume player | answer = " << master_handle.promise().answers.back() << endl;
	vector<int>::const_iterator st = (master_handle->promise().answers.size() >= GAMER_CNT) ?
		master_handle->promise().answers.end() - GAMER_CNT : master_handle->promise().answers.begin();

	return make_pair(st, master_handle->promise().answers.end());
}
#pragma endregion
