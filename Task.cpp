#include "Task.h"
#include "Awaitables.h"

#include <iostream>

using namespace std;

#pragma region Task - promise
Task::promise_type::promise_type(std::vector<Handle>* players)
	: total_players(players) {
}

Task::promise_type::promise_type(int id, Handle* master)
	: master_h(master) {
}

FinalMasterAwaiter Task::promise_type::final_suspend() noexcept {
	return FinalMasterAwaiter{ winner_handle };
}

void Task::promise_type::return_value(std::pair<int, int> end_query) {
	int player_id = end_query.first;
	int answer = end_query.second;

	// only if last answer = 0
	if (answer == 0 && total_players
		&& player_id >= 0 && player_id < total_players->size())
		winner_handle = (*total_players)[player_id];
}

PlayerAwaiter Task::promise_type::yield_value(int my_guess) {
	return PlayerAwaiter{ my_guess, master_h };
}

MasterAwaiter Task::promise_type::yield_value(pair<int, int> waiting_query) {
	return MasterAwaiter{ total_players, waiting_query.first, waiting_query.second };
}
#pragma endregion

#pragma region Task - Rule of 5
Task::Task(Task&& t) noexcept : cor(t.cor) { t.cor = {}; }
Task& Task::operator=(Task&& t) noexcept {
	if (this != &t) {
		if (cor) cor.destroy();
		cor = t.cor;
		t.cor = {};
	}
	return *this;
}
Task::~Task() {
	destroy();
}
#pragma endregion

#pragma region another methods
Task::Task(promise_type::Handle coroutine) : cor(coroutine) {}

bool Task::is_done() const { return !cor || cor.done(); }
void Task::resume() { if (cor && !cor.done()) cor.resume(); }
void Task::destroy() {
	if (cor) {
		cor.destroy();
		cor = nullptr;
	}
}
#pragma endregion