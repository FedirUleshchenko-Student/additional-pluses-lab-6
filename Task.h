#pragma once
#include <coroutine>
#include <vector>
#include <utility>
#include "global-consts.h"

struct PlayerAwaiter;
struct MasterAwaiter;
struct FinalMasterAwaiter;

struct Query {
	Query() = default;
	Query(int newGuess, int newAnswer) :
		guess(newGuess), answer(newAnswer) {}
	int guess = 0;
	int answer = 0;
};

class TaskBase {
public:
	std::coroutine_handle<> cor{};

	TaskBase() = default;
	explicit TaskBase(std::coroutine_handle<> h);

	TaskBase(const TaskBase&) = delete;
	TaskBase& operator=(const TaskBase&) = delete;
	TaskBase(TaskBase&& t) noexcept;
	TaskBase& operator=(TaskBase&& t) noexcept;
	~TaskBase();

	bool is_done() const;
	void resume();
	void destroy();
};

class PlayerTask;
class MasterTask;