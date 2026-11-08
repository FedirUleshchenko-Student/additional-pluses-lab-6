#include "game-funcs.h"
#include "Task.h"
#include "Awaitables.h"
#include "global-consts.h"
#include <iostream>

using namespace std;

MasterTask master(vector<std::coroutine_handle<>>* playersCor)
{
	uniform_int_distribution<int> dist(MIN_VAL, MAX_VAL);
	mt19937 gen(GLOBAL_SEED);
	int seeked = dist(gen);

	cout << "[Master] Generated number = " << seeked << endl << endl;
	int guess = 0;
	int lastAnswer = MasterAwaiter::IGNORED_VALUE;
	int id = 0;

	while (true) {

		guess = co_yield{ id, lastAnswer };

		if (seeked == guess) {
			cout << endl << "[Master] number " << guess <<
				" was guessed CORRECTLY by a " << id << " gamer (id in a list)" << endl;
			break;
		}

		if (++id == playersCor->size()) id = 0;
		lastAnswer = (seeked > guess) ? 1 : -1;
	}

	cout << "[Master] Game ended. Ending master's coroutine" << endl << endl;

	co_return{ id, 0 };
}

void updateRange(int& l, int& r, const int& guess, const int& ans) {
	if (!(l <= guess && guess <= r)) return;
	// maybe swap ??
	if (ans == -1)
		r = guess;
	else
		l = guess;
}

bool updateRanges(int& l, int& r,
	vector<Query>::const_iterator& it1,
	vector<Query>::const_iterator& it2) {
	bool flag = true;
	for (; it1 != it2; it1++) {
		updateRange(l, r, it1->guess, it1->answer);
		if (it1->answer == 0) { flag = false; break; }
	}
	return flag;
}

PlayerTask gamerSmart(int id, std::coroutine_handle<>* masterCor) {
	static int TEST_GUESS = 1;
	int l = MIN_VAL;
	int r = MAX_VAL + 1;

	vector<Query>::const_iterator q_it1;
	vector<Query>::const_iterator q_it2;

	auto master = MasterTask::promise_type::Handle::from_address(masterCor->address());

	q_it1 = master.promise().queries.begin();
	q_it2 = master.promise().queries.end();

	updateRanges(l, r, q_it1, q_it2);

	//auto outputAllQueries = [&]() {
	//	cout << "id = " << id << " guesses : ";
	//	for (auto& x : master->promise().queries) {
	//		cout << "[" << x.guess << ", " << x.answer << "] ";
	//	}
	//	cout << endl;
	//	};

	int questions = 0;
	while (l + 1 < r) {
		questions++;
		int guess = (l + r) / 2;
		cout << "[Gamer " << id
			<< "] asked " << questions
			<< " time = " << guess
			<< " (l = " << l
			<< ", r = " << r
			<< ")" << endl;
		auto x = co_yield guess;
		cout << "[Gamer " << id
			<< "] got answer = " << (x.second - 1)->answer
			<< " on guess = " << guess
			<< " (l now = " << l
			<< ", r now = " << r
			<< ")" << endl;
		if (!updateRanges(l, r, x.first, x.second))
			break;
		//outputAllQueries();
	}

	cout << "[Gamer " << id << "] YIPPE!!!!!!" << endl;
	cout << "[Gamer " << id << "] I've won by bin search, with = "
		<< questions << " queries (counded only MY queries)" << endl;
	cout << "[Gamer " << id << "] I've won. Ending this gamer's coroutine" << endl;
	//outputAllQueries();
	co_return;
	/*
	TEST_GUESS *= 15;
	cout << "player id = " << id << " pushing = " << TEST_GUESS << endl;
	//auto x = co_await PlayerAwaiter { TEST_GUESS, master };
	auto x = co_yield TEST_GUESS;
	cout << "id = " << id << " getter ans = [";

	for (auto v = x.first; v != x.second; v++)
		cout << (*v).answer << ((v+1 != x.second) ? " " : "");
	cout << "]" << endl;

	cout << "guesses : ";
	cout << "id = " << id << " started guesses : ";
	for (auto& [x, y] : master->promise().queries)
		cout << x << " "; cout << endl;
	cout << "id = " << id << " started answers : ";
	for (auto& [y, x] : master->promise().queries)
		cout << x << " "; cout << endl;
	//co_await PlayerAwaiter{ TEST_GUESS * 10, master };
	co_yield TEST_GUESS * 10;
	*/
}

bool addIncluded(
	set<int>& s,
	vector<Query>::const_iterator& it1,
	vector<Query>::const_iterator& it2) {
	bool flag = true;
	for (; it1 != it2; it1++) {
		s.insert(it1->guess);
		if (it1->answer == 0) { flag = false; break; }
	}
	return flag;
}

int getRandomGuess(set<int>& included) {
	uniform_int_distribution<int> dist(MIN_VAL, MAX_VAL - included.size());
	static mt19937 gen(RANDOM_SEEKER_SEED);
	int seeked = dist(gen);
	int curr = seeked;

	auto getLowerCnt = [&](int t) -> int {
		int ans = 0;
		for (auto& x : included) {
			if (t > x) ans++;
			else break;
		}
		return ans;
		};

	while (included.count(curr) || curr - getLowerCnt(curr) < seeked) curr++;
	return curr;
}
	
PlayerTask gamerStupid(int id, std::coroutine_handle<>* masterCor) {
	static int TEST_GUESS = 1;

	set<int> included;

	vector<Query>::const_iterator q_it1;
	vector<Query>::const_iterator q_it2;

	auto master = MasterTask::promise_type::Handle::from_address(masterCor->address());

	q_it1 = master.promise().queries.begin();
	q_it2 = master.promise().queries.end();

	addIncluded(included, q_it1, q_it2);

	int questions = 0;
	while (true) {
		questions++;
		int guess = getRandomGuess(included);
		cout << "[Gamer " << id
			<< "] asked " << questions
			<< " time = " << guess
			<< " (just random)"
			<< endl;
		auto x = co_yield guess;
		cout << "[Gamer " << id
			<< "] got answer = " << (x.second - 1)->answer
			<< " on guess = " << guess
			<< " (just random)"
			<< endl;
		if (!addIncluded(included, x.first, x.second))
			break;
	}

	cout << "[Gamer " << id << "] YIPPE!!!!!!" << endl;
	cout << "[Gamer " << id << "] I've won by random, with = "
		<< questions << " queries (counded only MY queries)" << endl;
	cout << "[Gamer " << id << "] I've won. Ending this gamer's coroutine" << endl;
	co_return;
}
