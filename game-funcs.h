#pragma once
#include "Task.h"
#include <vector>
#include <set>

Task master(std::vector<Task::promise_type::Handle>* players);

void updateRange(int& l, int& r, const int& guess, const int& ans);

bool updateRanges(int& l, int& r,
	std::vector<Query>::const_iterator& it1,
	std::vector<Query>::const_iterator& it2);

Task gamerSmart(int id, Task::promise_type::Handle* master);

bool addIncluded(
	std::set<int>& s,
	std::vector<Query>::const_iterator& it1,
	std::vector<Query>::const_iterator& it2);

int getRandomGuess(std::set<int>& included);

Task gamerStupid(int id, Task::promise_type::Handle* master);