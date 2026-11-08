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

PlayerAwaiter Task::promise_type::yield_value(int my_guess) {
	return PlayerAwaiter{ my_guess, master_h };
}

MasterAwaiter Task::promise_type::yield_value(pair<int, int> waiting_query) {
	return MasterAwaiter{ waiting_query.first, total_players, waiting_query.second };
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
	cout << "destructor called" << endl;
	destroy();
}
#pragma endregion

#pragma region another methods
Task::Task(promise_type::Handle coroutine) : cor(coroutine) {}

bool Task::is_done() const { return !cor || cor.done(); }
void Task::resume() { if (cor && !cor.done()) cor.resume(); }
void Task::destroy() {
	cout << "destroy called" << endl;
	if (cor) {
		cor.destroy();
		cor = nullptr;
	}
}
#pragma endregion