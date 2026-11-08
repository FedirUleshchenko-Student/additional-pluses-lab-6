#include "Task.h"
#include "Awaitables.h"
#include "PlayerTask.h"
#include "MasterTask.h"

#include <iostream>

using namespace std;

#pragma region TaskBase
TaskBase::TaskBase(std::coroutine_handle<> h) : cor(h) {}

TaskBase::TaskBase(TaskBase&& t) noexcept : cor(t.cor) { t.cor = nullptr; }
TaskBase& TaskBase::operator=(TaskBase&& t) noexcept {
	if (this != &t) {
		destroy();
		cor = t.cor;
		t.cor = nullptr;
	}
	return *this;
}

TaskBase::~TaskBase() { destroy(); }

bool TaskBase::is_done() const { return !cor || cor.done(); }
void TaskBase::resume() { if (cor && !cor.done()) cor.resume(); }
void TaskBase::destroy() {
	if (cor) {
		cor.destroy();
		cor = nullptr;
	}
}

#pragma endregion


#pragma region MasterTask - promise
MasterTask::promise_type::promise_type(std::vector<std::coroutine_handle<>>* players)
	: total_players(players) {
}

//MasterTask::promise_type::promise_type(int id, Handle* master)
//	: master_h(master) {
//}

FinalMasterAwaiter MasterTask::promise_type::final_suspend() noexcept {
	return FinalMasterAwaiter{ winner_handle };
}

void MasterTask::promise_type::return_value(std::pair<int, int> end_query) {
	int player_id = end_query.first;
	int answer = end_query.second;

	// only if last answer = 0
	if (answer == 0 && total_players
		&& player_id >= 0 && player_id < total_players->size())
		winner_handle = (*total_players)[player_id];
}

//PlayerAwaiter MasterTask::promise_type::yield_value(int my_guess) {
//	return PlayerAwaiter{ my_guess, master_h };
//}

MasterAwaiter MasterTask::promise_type::yield_value(pair<int, int> waiting_query) {
	return MasterAwaiter{ total_players, waiting_query.first, waiting_query.second};
}
#pragma endregion

#pragma region PlayerTask - promise
PlayerTask::promise_type::promise_type(int id, std::coroutine_handle<>* master)
	: master_h(master) {
}

PlayerAwaiter PlayerTask::promise_type::yield_value(int my_guess) {
	return PlayerAwaiter{ my_guess, master_h };
}
#pragma endregion
