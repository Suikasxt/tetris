#pragma once
#include <vector>
#include <string>

extern int type_list[];
struct Action {
	int r;
	int x;
	int y;
	Action(int _r = -1, int _x = -1, int _y = -1) :r(_r), x(_x), y(_y) {};
};
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
	int pop_count_[5];
	int number_;
	int score_;
	int block_number_;
	bool game_over_;

	void Restart();
	void CalcData();
	bool HasBlock(int x, int y) const;
	bool CanStay(Action) const;
	std::vector<std::string> Step(Action act, bool is_simulation = true);
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