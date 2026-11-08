// Working in Visual Studio 2022, so compiler:
// MSVC (Microsoft Visual C++)
//
// Standard (for version of C++):
// Preview - ISO C++23 Standard (/std:c++23preview)
//
// Variant 11

#include <iostream>

#include "global-consts.h"
#include "Task.h"
#include "Awaitables.h"
#include "game-funcs.h"

using namespace std;

int main() {

	// set up all handles
	Task::promise_type::Handle master_h;
	vector<Task::promise_type::Handle> players_h;
	players_h.reserve(GAMER_CNT);

	// set up coroutines (Tasks)
	// SHOULD NOT BE DESTROYED
	Task mT = master( &players_h );
	vector<Task> plT; plT.reserve(GAMER_CNT);
	for (int id = 1; id <= GAMER_CNT; id++) {
		if(id % 2 == 1)
			plT.push_back(gamerSmart(id, &master_h));
		else
			plT.push_back(gamerStupid(id, &master_h));
	}

	//for (int id = 1; id <= GAMER_CNT; id++)
	//	plT.push_back(gamerStupid(id, &master_h));

	// link handles to corespondent coroutines
	// ONLY POINTERS
	master_h = Task::promise_type::Handle::from_address(mT.cor.address());
	int id = 0;
	for (auto& x : plT) {
		players_h.push_back(
			Task::promise_type::Handle::from_address(x.cor.address())
		);
	}

	// start THE GAME
	master_h.resume();
}