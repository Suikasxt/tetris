#pragma once
#include <vector>
#include <string>

const int MAX_NUMBER = 10000;
class GameController
{
public:
	static const int a = 27073;
	static const int M = 32749;
	static const int C = 17713;
	static const int v = 12358;
	static const int Height = 20;
	static const int Width = 10;
	const int dir[4][3] = { {0, -1, 0}, {0, 1, 0}, {0, 0, 1}, {1, 0, 0} };
	std::string dir_string[4] = { "L", "R", "D", "C" };
	const int RotationNumber[7] = { 2, 4, 4, 4, 1, 2, 2};
	bool has_blocks_[Width][Height];
	bool is_valid_[4][Width][Height];
	bool can_arrive_[4][Width][Height];
	int from_[4][Width][Height];
	int type_list_[MAX_NUMBER];
	int number_;
	int score_;
	bool game_over_;
	std::vector<std::string> act_list_;

	void Restart();
	void CalcData();
	bool HasBlock(int x, int y);
	bool CanStay(int r, int px, int py);
	void Step(int rotation, int px, int py);
	void DebugOutput();

	const int Shapes[7][4][4][2] = {
		{
			// I 型
			{ {0, 0}, {0, -1}, {0, -2}, {0, 1}, },
			{ {0, 0}, {1, 0}, {2, 0}, {-1, 0}, },
			{ {0, 0}, {0, -1}, {0, -2}, {0, 1}, },
			{ {0, 0}, {1, 0}, {2, 0}, {-1, 0}, },
		},
		{
			// L 型
			{ {0, 0}, {0, -1}, {0, -2}, {1, 0}, },
			{ {0, 0}, {1, 0}, {2, 0}, {0, 1}, },
			{ {0, 0}, {-1, 0}, {0, 1}, {0, 2}, },
			{ {0, 0}, {0, -1}, {-1, 0}, {-2, 0}, },
		},
		{
			// J 型
			{ {0, 0}, {0, -1}, {0, -2}, {-1, 0}, },
			{ {0, 0}, {0, -1}, {1, 0}, {2, 0}, },
			{ {0, 0}, {1, 0}, {0, 1}, {0, 2}, },
			{ {0, 0}, {-1, 0}, {-2, 0}, {0, 1}, },
		},
		{
			// T 型
			{ {0, 0}, {1, 0}, {0, 1}, {-1, 0}, },
			{ {0, 0}, {0, -1}, {0, 1}, {-1, 0}, },
			{ {0, 0}, {0, -1}, {1, 0}, {-1, 0}, },
			{ {0, 0}, {0, -1}, {1, 0}, {0, 1}, },
		},
		{
			// O 型
			{ {0, 0}, {0, -1}, {1, -1}, {1, 0}, },
			{ {0, 0}, {0, -1}, {1, -1}, {1, 0}, },
			{ {0, 0}, {0, -1}, {1, -1}, {1, 0}, },
			{ {0, 0}, {0, -1}, {1, -1}, {1, 0}, },
		},
		{
			// S 型
			{ {0, 0}, {0, -1}, {1, -1}, {-1, 0}, },
			{ {0, 0}, {-1, 0}, {-1, -1}, {0, 1}, },
			{ {0, 0}, {0, -1}, {1, -1}, {-1, 0}, },
			{ {0, 0}, {-1, 0}, {-1, -1}, {0, 1}, },
		},
		{
			// Z 型
			{ {0, 0}, {0, -1}, {1, 0}, {-1, -1}, },
			{ {0, 0}, {0, -1}, {-1, 1}, {-1, 0}, },
			{ {0, 0}, {0, -1}, {1, 0}, {-1, -1}, },
			{ {0, 0}, {0, -1}, {-1, 1}, {-1, 0}, },
		},
	};
};