#include "GameController.h"
#include <cstring>
#include <queue>

const int MAX_NUMBER = 10000;
int type_list[MAX_NUMBER];

void GameController::Restart()
{
	std::memset(has_blocks_, 0, sizeof(has_blocks_));
	std::memset(pop_count_, 0, sizeof(pop_count_));
	int random_number = v;
	for (int i = 0; i < MAX_NUMBER; i++) {
		random_number = (random_number * a + C) % M;
		int weightIndex = random_number % 29;
		int shapeIndex;
		if (weightIndex >= 0 && weightIndex <= 1) {
			shapeIndex = 0;
		}
		else if (weightIndex > 1 && weightIndex <= 4) {
			shapeIndex = 1;
		}
		else if (weightIndex > 4 && weightIndex <= 7) {
			shapeIndex = 2;
		}
		else if (weightIndex > 7 && weightIndex <= 11) {
			shapeIndex = 3;
		}
		else if (weightIndex > 11 && weightIndex <= 16) {
			shapeIndex = 4;
		}
		else if (weightIndex > 16 && weightIndex <= 22) {
			shapeIndex = 5;
		}
		else if (weightIndex > 22) {
			shapeIndex = 6;
		}
		type_list[i] = shapeIndex;
	}
	number_ = 0;
	score_ = 0;
	game_over_ = false;
}

void GameController::CalcData()
{
	if (number_ >= MAX_NUMBER) {
		return;
	}
	int type = type_list[number_];
	for (int r = 0; r < 4; r++) {
		for (int i = 0; i < Width; i++) {
			for (int j = 0; j < Height; j++) {
				is_valid_[r][i][j] = true;
				for (int k = 0; is_valid_[r][i][j] && k < 4; k++) {
					if (HasBlock(i + Shapes[type][r][k][0], j + Shapes[type][r][k][1])) {
						is_valid_[r][i][j] = false;
					}
				}
			}
		}
	}
	memset(can_arrive_, 0, sizeof(can_arrive_));
	std::queue<Action> q;
	int r = number_ & 3;
	if (is_valid_[r][4][0]) {
		for (int i = 0; i < 4; i++) {
			int r_ext = (r + i * RotationNumber[type]) & 3;
			if (!can_arrive_[r_ext][4][0]) {
				q.push(Action(r_ext, 4, 0));
				from_[r_ext][4][0] = -1;
				can_arrive_[r_ext][4][0] = true;
			}
		}
	}
	else {
		game_over_ = true;
	}
	while (!q.empty()) {
		Action k = q.front();
		q.pop();
		for (int i = 0; i < 4; i++) {
			Action next(k);
			(next.r += dir[i][0]) &= 3;
			next.x += dir[i][1];
			next.y += dir[i][2];
			if (!HasBlock(next.x, next.y) && next.y >= 0 && is_valid_[next.r][next.x][next.y] && can_arrive_[next.r][next.x][next.y] == false) {
				can_arrive_[next.r][next.x][next.y] = true;
				from_[next.r][next.x][next.y] = i;
				q.push(next);
			}
		}
	}
}

bool GameController::HasBlock(int x, int y) const
{
	if (x < 0 || x >= Width) {
		return true;
	}
	if (y < 0) {
		return false;
	}
	if (y >= Height) {
		return true;
	}
	return has_blocks_[x][y];
}

bool GameController::CanStay(Action act) const
{
	if (!can_arrive_[act.r][act.x][act.y]) {
		return false;
	}
	if (act.y + 1 < Height && can_arrive_[act.r][act.x][act.y + 1]) {
		return false;
	}
	return true;
}

std::vector<std::string> GameController::Step(Action act, bool is_simulation)
{
	std::vector<std::string> act_list;
	if (game_over_) {
		return act_list;
	}
	if (!can_arrive_[act.r][act.x][act.y]) {
		return act_list;
	}
	int type = type_list[number_];
	number_++;
	if (number_ >= MAX_NUMBER) {
		game_over_ = true;
	}
	for (int k = 0; k < 4; k++) {
		if (act.y + Shapes[type][act.r][k][1] >= 0) {
			has_blocks_[act.x + Shapes[type][act.r][k][0]][act.y + Shapes[type][act.r][k][1]] = true;
		}
		if (act.y + Shapes[type][act.r][k][1] <= 0) {
			game_over_ = true;
		}
	}
	if (!is_simulation) {
		int rotation = act.r;
		int px = act.x;
		int py = act.y;
		while (from_[rotation][px][py] != -1) {
			int dir_index = from_[rotation][px][py];
			act_list.push_back(dir_string[dir_index]);
			rotation = (rotation - dir[dir_index][0] + 4) & 3;
			px -= dir[dir_index][1];
			py -= dir[dir_index][2];
		}
	}

	int pop_number = 0;
	block_number_ = 0;
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			block_number_ += has_blocks_[j][i];
		}
	}
	for (int i = Height - 1; i >= 0;) {
		bool full = true;
		for (int j = 0; j < Width && full; j++) {
			full &= has_blocks_[j][i];
		}
		if (!full) {
			i--;
			continue;
		}
		pop_number++;
		for (int k = i; k > 0; k--) {
			for (int j = 0; j < Width; j++) {
				has_blocks_[j][k] = has_blocks_[j][k - 1];
			}
		}
		for (int j = 0; j < Width; j++) {
			has_blocks_[j][0] = false;
		}
	}
	switch (pop_number)
	{
	case 1:
		score_ += block_number_ * 1;
		break;
	case 2:
		score_ += block_number_ * 3;
		break;
	case 3:
		score_ += block_number_ * 6;
		break;
	case 4:
		score_ += block_number_ * 10;
		break;
	default:
		break;
	}

	pop_count_[pop_number]++;
	return act_list;
}

void GameController::DebugOutput()
{
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			printf(has_blocks_[j][i] ? "*" : ".");
		}
		printf("\n");
	}
}
