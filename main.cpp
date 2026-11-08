// Working in Visual Studio 2022, so compiler:
// MSVC (Microsoft Visual C++)
//
// Standard (for version of C++):
// Preview - ISO C++23 Standard (/std:c++23preview)
//
// Variant 11

#include <iostream>
#include <random>
#include <chrono>
#include <coroutine>
#include <memory>
#include <thread>

#include <set>

using namespace std;

#include "global-consts.h"
#include "Task.h"
#include "Awaitables.h"

Task master(vector<Task::promise_type::Handle>* players)
{
	uniform_int_distribution<int> dist(MIN_VAL, MAX_VAL);
	mt19937 gen(GLOBAL_SEED);
	int seeked = dist(gen);

	cout << "generated = " << seeked << endl;
	int guess = 0;
	int lastAnswer = MasterAwaiter::IGNORED_VALUE;
	int id = 0;
	while (true) {
		//guess = co_await MasterAwaiter{ lastAnswer, players, i++ };
		guess = co_yield {lastAnswer, id };
		cout << "awaited" << endl;

		if (seeked == guess) break;
		if (++id == players->size()) id = 0;


		lastAnswer = (seeked > guess) ? 1 : -1;
		cout << "current asnwer = " << lastAnswer << endl;
	}

	cout << "game ended : founded = " << (seeked == guess) << " last id = " << id << endl;

	co_await MasterAwaiter{ 0, players, id };

	co_return;
}

void updateRanges(int& l, int& r, const int& pos, const int& ans) {
	if (!(l <= pos && pos <= r)) return;
	// maybe swap ??
	if (ans == 1)
		l = pos;
	else
		r = pos;
}

Task gamerSmart(int id, Task::promise_type::Handle* master) {
	static int TEST_GUESS = 1;
	int l = MIN_VAL;
	int r = MIN_VAL;

	vector<int>::const_iterator  g_it1;
	vector<int>::const_iterator  g_it2;
	vector<int>::const_iterator  a_it1;
	vector<int>::const_iterator  a_it2;

	g_it1 = master->promise().guesses.begin();
	g_it2 = master->promise().guesses.end();
	a_it1 = master->promise().answers.begin();
	a_it2 = master->promise().answers.end();

	for (; g_it1 != g_it2 && a_it1 != a_it2;
		g_it1++, a_it1++) {

	}

	cout << "id = " << id << " started guesses : ";
	for (auto& x : master->promise().guesses)
		cout << x << " "; cout << endl;
	cout << "id = " << id << " started answers : ";
	for (auto& x : master->promise().answers)
		cout << x << " "; cout << endl;
	//while (l + 1 < r) {
	//	int guess = (l + r) / 2;
	//	co_await PlayerAwaiter{ ++TEST_GUESS, master };
	//}
	TEST_GUESS *= 15;
	cout << "player id = " << id << " pushing = " << TEST_GUESS << endl;
	//auto x = co_await PlayerAwaiter { TEST_GUESS, master };
	auto x = co_yield TEST_GUESS;
	cout << "id = " << id << " getter ans = [";
	
	for (auto v = x.first; v != x.second; v++)
		cout << *v << ((v+1 != x.second) ? " " : "");
	cout << "]" << endl;

	cout << "guesses : ";
	for (auto& x : master->promise().guesses)
		cout << x << " "; cout << endl;
	cout << "answers : ";
	for (auto& x : master->promise().answers)
		cout << x << " "; cout << endl;
	//co_await PlayerAwaiter{ TEST_GUESS * 10, master };
	co_yield TEST_GUESS * 10;
}

int main() {

	// set up all handles
	Task::promise_type::Handle master_h;
	vector<Task::promise_type::Handle> players_h;
	players_h.reserve(GAMER_CNT);

	// set up coroutines (Tasks)
	// SHOULD NOT BE DESTROYED
	Task mT = master( &players_h );
	vector<Task> plT; plT.reserve(GAMER_CNT);
	for (int id = 1; id <= players_h.capacity(); id++)
		plT.push_back(gamerSmart(id, &master_h));

	// link handles to corespondent coroutines
	// ONLY REFERENCES (OR POINTERS ?)
	master_h = mT.cor;
	int id = 0;
	for (auto& x : plT) {
		//players_h[id++] = ref(x.cor);
		players_h.push_back(x.cor);
	}

	// start THE GAME
	master_h.resume();

	cout << "game ended" << endl;

	//master_h.destroy();
	//master_h = nullptr;

	if (!mT.is_done()) {
		cout << "ERROR : game still running" << endl;
		mT.destroy();
		mT.destroy();
	}

	//// TEMP CODE, should not work 
	//cout << "sleepy time" << endl;
	//this_thread::sleep_for(3s);
	//// this would break the app
	//mT.resume();
}