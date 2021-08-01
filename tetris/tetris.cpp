// tetris.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "GameController.h"
#include "windows.h"
#include <ctime>
#include <algorithm>

struct ActionValue {
    Action a;
    double v;
    ActionValue(Action _a = Action(-1, -1, -1), double _v = -1e9) :a(_a), v(_v) {};
    bool operator < (ActionValue x) {
        return v < x.v;
    }
};

double GetBoardTransitions(const GameController& game) {
    int res = 0;

    for (int y = 0; y < game.Height; y++)
        for (int x = 1; x < game.Width; x++)
            res += game.has_blocks_[x][y] ^ game.has_blocks_[x - 1][y];


    for (int y = 1; y < game.Height; y++)
        for (int x = 0; x < game.Width; x++)
            res += game.has_blocks_[x][y] ^ game.has_blocks_[x][y - 1];

    return res;
}
double GetBoardBuriedHoles(const GameController& game) {
    int res = 0;

    for (int x = 0; x < game.Width; x++) {
        int y;
        for (y = 0; y < game.Height; y++) {
            if (game.has_blocks_[x][y]) break;
        }
        for (; y < game.Height; y++) {
            if (!game.has_blocks_[x][y]) res++;
        }
    }

    return res;
}
int sum_n[21] = { 0,1,3,6,10,15,21,28,36,45,55,66,78,91,105,120,136,153,171,190,210 };
double GetBoardWells(const GameController& game) {
    double res = 0, W;

    for (int x = 0; x < game.Width; x++) {
        int wells = 0, f = 0;
        W = 1.5;
        for (int y = 0; y <= game.Height; y++) {
            if (game.HasBlock(x, y) == 0) {
                if (game.HasBlock(x - 1, y) && game.HasBlock(x + 1, y)) wells++;
            }
            else {
                res += sum_n[wells] * W;
                wells = 0;
                if (f) W += 0.1; else W = 0.8;
                f = 1;
            }
        }
    }

    return res;
}
double Mavis(const GameController& game) {
    double height_value = 0;
    int max = 0;
    double res = 0;
    for (int x = 0; x < game.Width; x++) {
        int y = 0;
        for (; y < game.Height; y++) {
            if (game.HasBlock(x, y)) break;
        }
        y = game.Height - y;
        double cost = y * y;
        int u = x < 6 ? x : x - 5;
        cost *= pow(1.05, u);
        height_value += cost;
        if (max < y) max = y;
    }
    int blocksNumber = 0;
    for (int x = 0; x < game.Width; x++) {
        for (int y = 0; y < game.Height; y++) {
            blocksNumber += game.has_blocks_[x][y];
        }
    }
    double BoardTransitions = GetBoardTransitions(game), BuriedHoles = GetBoardBuriedHoles(game), Wells = GetBoardWells(game);

    res = sqrt(height_value) / 2 + 2.7 * max + 2 * BoardTransitions + 8 * BuriedHoles + Wells;
    if (blocksNumber < 120) {
        res -= blocksNumber * 10.;
    }

    return res;
}
double Evaluate(const GameController& game) {
    double res = game.score_;
    res -= Mavis(game);
    if (game.game_over_) {
        res -= 1e6;
    }
    return res;
}
ActionValue Greedy(const GameController& game) {
    ActionValue res;
    int type = type_list[game.number_];
    for (int r = 0; r < game.RotationNumber[type]; r++) {
        for (int i = 0; i < game.Height; i++) {
            for (int j = 0; j < game.Width; j++) {
                if (game.CanStay(Action(r, j, i))) {
                    GameController tmp_game(game);
                    tmp_game.Step(Action(r, j, i));
                    double value = Evaluate(tmp_game);
                    if (value > res.v) {
                        res = ActionValue(Action(r, j, i), value);
                    }
                }
            }
        }
    }
    return res;
}
ActionValue Search(const GameController& game, int deepth) {
    if (deepth == 0 || game.game_over_) {
        return ActionValue(Action(), Evaluate(game));
    }
    std::vector<ActionValue> action_list;
    int type = type_list[game.number_];
    for (int r = 0; r < game.RotationNumber[type]; r++) {
        for (int i = 0; i < game.Height; i++) {
            for (int j = 0; j < game.Width; j++) {
                if (game.CanStay(Action(r, j, i))) {
                    GameController tmp_game(game);
                    tmp_game.Step(Action(r, j, i));
                    double value = Evaluate(tmp_game);
                    action_list.push_back(ActionValue(Action(r, j, i), value));
                }
            }
        }
    }
    std::sort(action_list.begin(), action_list.end());
    if (deepth == 1) {
        return action_list[action_list.size() - 1];
    }
    ActionValue res;
    for (int i = 0; i < 3 && i < action_list.size(); i++) {
        int index = action_list.size() - 1 - i;
        GameController tmp_game(game);
        Action act = action_list[index].a;
        tmp_game.Step(act);
        tmp_game.CalcData();
        ActionValue av = Search(tmp_game, deepth - 1);
        av.a = act;
        if (res < av) {
            res = av;
        }
    }
    return res;
}
int main()
{
    GameController game;
    game.Restart();
    std::vector<std::string> act_list;
    while (!game.game_over_ && game.number_ < 10000) {
        game.CalcData();
        if (game.game_over_) {
            break;
        }
        //Action act = Greedy(game).a;
        Action act = Search(game, 4).a;
        std::vector<std::string> act_list_tmp = game.Step(act);
        act_list.push_back("N");
        for (int i = act_list_tmp.size() - 1; i >= 0;) {
            int Count = 1;
            i--;
            while (i >= 0 && act_list_tmp[i] == act_list_tmp[i + 1]) {
                Count++;
                i--;
            }
            act_list.push_back(act_list_tmp[i + 1] + std::to_string(Count));
        }
        printf("%d %d\n", game.number_, game.score_);
        if (game.number_ % 30 == 0) {
            game.DebugOutput();
        }
    }
    std::cout << "game.pause();game.playRecord('";
    for (int i = 0; i < act_list.size(); i++) {
        std::cout << act_list[i];
        if (i != act_list.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "'.split(','));";
    printf("\n%d\n", game.score_);
}