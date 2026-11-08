#pragma once
#include "Task.h"
#include "PlayerTask.h"
#include "MasterTask.h"
#include <vector>
#include <set>

MasterTask master(std::vector<std::coroutine_handle<>>* playersCor);

void updateRange(int& l, int& r, const int& guess, const int& ans);

bool updateRanges(int& l, int& r,
	std::vector<Query>::const_iterator& it1,
	std::vector<Query>::const_iterator& it2);

PlayerTask gamerSmart(int id, std::coroutine_handle<>* masterCor);

bool addIncluded(
	std::set<int>& s,
	std::vector<Query>::const_iterator& it1,
	std::vector<Query>::const_iterator& it2);

int getRandomGuess(std::set<int>& included);

PlayerTask gamerStupid(int id, std::coroutine_handle<>* masterCor);