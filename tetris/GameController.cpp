#include "GameController.h"
#include <cstring>
#include <queue>
#include <tuple>

void GameController::Restart()
{
	std::memset(has_blocks_, 0, sizeof(has_blocks_));
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
		type_list_[i] = shapeIndex;
	}
	number_ = 0;
	score_ = 0;
	game_over_ = false;
	CalcData();
}

void GameController::CalcData()
{
	if (number_ >= MAX_NUMBER) {
		return;
	}
	int type = type_list_[number_];
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
	std::queue<std::tuple<int, int, int> > q;
	int r = number_ & 3;
	if (is_valid_[r][4][0]) {
		q.push(std::make_tuple(r, 4, 0));
		from_[r][4][0] = -1;
		can_arrive_[r][4][0] = true;
	}
	else {
		game_over_ = true;
	}
	while (!q.empty()) {
		std::tuple<int, int, int> k = q.front();
		q.pop();
		for (int i = 0; i < 4; i++) {
			std::tuple<int, int, int> next(k);
			(std::get<0>(next) += dir[i][0]) &= 3;
			std::get<1>(next) += dir[i][1];
			std::get<2>(next) += dir[i][2];
			int r = std::get<0>(next);
			int x = std::get<1>(next);
			int y = std::get<2>(next);
			if (!HasBlock(x, y) && y >= 0 && is_valid_[r][x][y] && can_arrive_[r][x][y] == false) {
				can_arrive_[r][x][y] = true;
				from_[r][x][y] = i;
				q.push(next);
			}
		}
	}
}

bool GameController::HasBlock(int x, int y)
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

bool GameController::CanStay(int r, int px, int py)
{
	if (!can_arrive_[r][px][py]) {
		return false;
	}
	if (py + 1 < Height && can_arrive_[r][px][py + 1]) {
		return false;
	}
	return true;
}

void GameController::Step(int rotation, int px, int py)
{
	if (game_over_) {
		return;
	}
	if (!CanStay(rotation, px, py)) {
		return;
	}
	int type = type_list_[number_];
	number_++;
	for (int k = 0; k < 4; k++) {
		if (py + Shapes[type][rotation][k][1] >= 0) {
			has_blocks_[px + Shapes[type][rotation][k][0]][py + Shapes[type][rotation][k][1]] = true;
		}
		else {
			game_over_ = true;
		}
	}
	std::vector<std::string> act_list;
	while (from_[rotation][px][py] != -1) {
		int dir_index = from_[rotation][px][py];
		act_list.push_back(dir_string[dir_index]);
		rotation = (rotation - dir[dir_index][0] + 4) & 3;
		px -= dir[dir_index][1];
		py -= dir[dir_index][2];
	}
	act_list_.push_back("N");
	for (int i = act_list.size() - 1; i >= 0;) {
		int Count = 1;
		i--;
		while (i >= 0 && act_list[i] == act_list[i + 1]) {
			Count++;
			i--;
		}
		act_list_.push_back(act_list[i+1] + std::to_string(Count));
	}

	int pop_number = 0, block_number = 0;
	for (int i = 0; i < Height; i++) {
		for (int j = 0; j < Width; j++) {
			block_number += has_blocks_[j][i];
		}
	}
	for (int i = Height - 1; i >= 0; i--) {
		bool full = true;
		for (int j = 0; j < Width && full; j++) {
			full &= has_blocks_[j][i];
		}
		if (!full) {
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
		score_ += block_number * 1;
		break;
	case 2:
		score_ += block_number * 3;
		break;
	case 3:
		score_ += block_number * 6;
		break;
	case 4:
		score_ += block_number * 10;
		break;
	default:
		break;
	}
	CalcData();
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
