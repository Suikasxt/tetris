// tetris.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "GameController.h"
#include "windows.h"
#include <ctime>
#include<cassert>
#include <algorithm>

struct ActionValue {
    Action a;
    double v;
    ActionValue(Action _a = Action(-1, -1, -1), double _v = -1e9) :a(_a), v(_v) {};
    bool operator < (ActionValue x) {
        return v < x.v;
    }
};
double weight = 10;
namespace MCTS {
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
        double res = 0;
        int max = 0;
        double height_value = 0;

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
        int blocks_number = 0;
        for (int x = 0; x < game.Width; x++) {
            for (int y = 0; y < game.Height; y++) {
                blocks_number += game.has_blocks_[x][y];
            }
        }
        double BoardTransitions = GetBoardTransitions(game), BuriedHoles = GetBoardBuriedHoles(game), Wells = GetBoardWells(game);

        res = sqrt(height_value) / 3000 + 5.7 * max + 2 * BoardTransitions + 300 * BuriedHoles;
        res -= min(blocks_number, 180) * 29.5;

        return res;
    }
    double Evaluate(const GameController& game) {
        double res = game.score_ * 1.;
        res += game.pop_count_[4] * 800.;
        res += game.pop_count_[3] * 100.;
        res -= game.pop_count_[2] * 0.;
        res -= game.pop_count_[1] * 50.;
        res -= Mavis(game);
        if (game.game_over_) {
            res -= 1e7;
        }
        return res;
    }
    ActionValue Greedy(const GameController& game, double random_range = 0) {
        ActionValue res;
        int type = type_list[game.number_];
        for (int r = 0; r < game.RotationNumber[type]; r++) {
            for (int i = 0; i < game.Height; i++) {
                for (int j = 0; j < game.Width; j++) {
                    if (game.CanStay(Action(r, j, i))) {
                        GameController tmp_game(game);
                        tmp_game.Step(Action(r, j, i));
                        double value = Evaluate(tmp_game);
                        value += random_range * rand() / RAND_MAX;
                        if (value > res.v) {
                            res = ActionValue(Action(r, j, i), value);
                        }
                    }
                }
            }
        }
        return res;
    }
    ActionValue Search(const GameController& game, int deepth, int width) {
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
        for (int i = 0; i < width && i < action_list.size(); i++) {
            int index = action_list.size() - 1 - i;
            GameController tmp_game(game);
            Action act = action_list[index].a;
            tmp_game.Step(act);
            tmp_game.CalcData();
            ActionValue av = Search(tmp_game, deepth - 1, width);
            av.a = act;
            if (res < av) {
                res = av;
            }
        }
        return res;
    }
    ActionValue MCSearch(const GameController& game, int deepth, bool out = false);
    double MCEvaluate(const GameController& game) {
        double res = 0;
        for (int x = 0; x < game.Width; x++) {
            bool flag = false;
            for (int y = 0; y < game.Height; y++) {
                if (game.has_blocks_[x][y]) {
                    flag = true;
                }
                if (flag && !game.has_blocks_[x][y]) {
                    res -= game.Height - y;
                }
            }
        }
        for (int y = 0; y < game.Height; y++) {
            if (game.has_blocks_[0][y]) {
                res -= game.Height - y;
            }
        }
        return res;
    }
    double MCSimulation(GameController& game, int width, int deepth) {
        double value = game.score_;
        double addition = MCEvaluate(game);
        for (int i = 0; i < width && !game.game_over_; i++) {

            Action act;
            if (deepth == 0) {
                //act = Greedy(game, 0).a;
                act = Search(game, 3, 5).a;
            }
            else {
                act = MCSearch(game, deepth - 1).a;
            }
            game.Step(act);
            game.CalcData();
            value = value * 0.8 + game.score_;
            addition += MCEvaluate(game) * pow(0.92, i);
        }
        //value += game.score_ * 2.5;
        value += addition / 3;
        return value;
    }
    ActionValue MCSearch(const GameController& game, int deepth, bool out) {
        std::vector<ActionValue> action_list;
        int type = type_list[game.number_];
        for (int r = 0; r < game.RotationNumber[type]; r++) {
            for (int i = 0; i < game.Height; i++) {
                for (int j = 0; j < game.Width; j++) {
                    if (game.CanStay(Action(r, j, i))) {
                        GameController tmp_game(game);
                        tmp_game.Step(Action(r, j, i));
                        tmp_game.CalcData();
                        //double value = Evaluate(tmp_game);
                        double value = Search(tmp_game, 3, 5).v;
                        action_list.push_back(ActionValue(Action(r, j, i), value));
                    }
                }
            }
        }
        std::sort(action_list.begin(), action_list.end());
        ActionValue res;
        int width = 5;
        if (out) {
            width = 10;
        }
        for (int i = 0; i < width && i < action_list.size(); i++) {
            double total_value = 0;
            int index = action_list.size() - 1 - i;
            Action act = action_list[index].a;
            for (int j = 0; j < 1; j++) {
                GameController tmp_game(game);
                tmp_game.Step(act);
                tmp_game.CalcData();
                total_value += MCSimulation(tmp_game, 30, deepth);
            }
            if (res.v < total_value) {
                res = ActionValue(act, total_value);
            }
        }
        return res;
    }
}

namespace Search {
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
        double res = 0;
        int max = 0;
        double height_value = 0;

        for (int x = 0; x < game.Width; x++) {
            int y = 0;
            for (; y < game.Height; y++) {
                if (game.HasBlock(x, y)) break;
            }
            y = game.Height - y;
            double cost = y * y;
            int u = x < 6 ? x : x - 5;
            //cost *= pow(1.25, x);
            height_value += cost;
            if (max < y) max = y;
        }
        int blocks_number = 0;
        for (int x = 0; x < game.Width; x++) {
            for (int y = 0; y < game.Height; y++) {
                blocks_number += game.has_blocks_[x][y];
            }
        }
        double BoardTransitions = GetBoardTransitions(game), BuriedHoles = GetBoardBuriedHoles(game), Wells = GetBoardWells(game);

        res = sqrt(height_value) / 3  + 5.7 * max + 2 * BoardTransitions + 200 * BuriedHoles;
        res -= min(blocks_number, 120) * 20.;

        return res;
    }
    double Evaluate(const GameController& game) {
        double res = game.score_ * 0.01;
        if (game.block_number_ > 120) {
            res += game.pop_count_[4] * 50.;
        }
        /*
        res += game.pop_count_[4] * 800.;
        res += game.pop_count_[3] * 100.;
        res -= game.pop_count_[2] * 0.;
        res -= game.pop_count_[1] * 50.;
        */
        res -= Mavis(game);
        if (game.game_over_) {
            res -= 1e7;
        }
        return res;
    }
    ActionValue Search(const GameController& game, int deepth, int width, bool out = false);
    typedef struct __THREAD_DATA
    {
        GameController game;
        int deepth;
        int width;
        ActionValue& res;

        __THREAD_DATA(const GameController& _game, int _deepth, int _width, ActionValue& _res) : game(_game), deepth(_deepth), width(_width), res(_res) {};
    }THREAD_DATA;
    DWORD WINAPI ThreadProc(LPVOID lpParameter)
    {
        THREAD_DATA* pThreadData = (THREAD_DATA*)lpParameter;
        pThreadData->res = Search(pThreadData->game, pThreadData->deepth, pThreadData->width);
        //printf("calc %d %lf\n", pThreadData->i, tmp_res[pThreadData->i].v);
        return 0L;
    }
    ActionValue Search(const GameController& game, int deepth, int width, bool out) {
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
                        double value;
                        //value = Evaluate(tmp_game);
                        
                        if (deepth < 6) {
                            value = Evaluate(tmp_game);
                        }
                        else {
                            tmp_game.CalcData();
                            value = Search(tmp_game, deepth - 5, width).v;
                        }
                        
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
        int real_width = width;
        if (out) {
            real_width = 5;
        }
        if (deepth >= 10) {
            HANDLE thread[100];
            __THREAD_DATA* tmp_data[100];
            ActionValue tmp_res[100];
            //double maxvalue = action_list[action_list.size() - 1].v;
            //maxvalue -= maxvalue - abs(maxvalue / 2);
            for (int i = 0; i < real_width && i < action_list.size(); i++) {
                int index = action_list.size() - 1 - i;
                //if (action_list[index].v < maxvalue) continue;
                GameController tmp_game(game);
                Action act = action_list[index].a;
                tmp_game.Step(act);
                tmp_game.CalcData();
                tmp_data[i] = new __THREAD_DATA(tmp_game, deepth - 1, width, tmp_res[i]);
                thread[i] = CreateThread(NULL, 0, ThreadProc, tmp_data[i], 0, NULL);
                //printf("ask %d\n", i);
            }
            for (int i = 0; i < real_width && i < action_list.size(); i++) {
                int index = action_list.size() - 1 - i;
                //if (action_list[index].v < maxvalue) continue;
                Action act = action_list[index].a;
                WaitForSingleObject(thread[i], INFINITE);
                delete tmp_data[i];
                ActionValue av = tmp_res[i];
                //printf("get %d %lf\n", i, av.v);
                av.a = act;
                av.v = action_list[index].v + av.v * 2;
                if (res < av) {
                    res = av;
                }
            }
            return res;
        }
        for (int i = 0; i < real_width && i < action_list.size(); i++) {
            int index = action_list.size() - 1 - i;
            GameController tmp_game(game);
            Action act = action_list[index].a;
            tmp_game.Step(act);
            tmp_game.CalcData();
            ActionValue av = Search(tmp_game, deepth - 1, width);
            av.a = act;
            av.v = action_list[index].v + av.v * 2;
            if (res < av) {
                res = av;
            }
        }
        return res;
    }
}
void work() {
    GameController game;
    game.Restart();
    std::vector<std::string> act_list;
    while (!game.game_over_ && game.number_ < 10000) {
        game.CalcData();
        if (game.game_over_) {
            break;
        }
        //Action act = Greedy(game).a;
        //Action act = Search::Search(game, game.number_<100?11:11, 3, true).a;
        //Action act = Search::Search(game, game.number_ < 100 ? 11 : 13, 3, true).a;
        Action act = Search::Search(game, game.number_ < 100 ? 11 : 13, 3, true).a;
        //Action act = MCTS::MCSearch(game, 0, true).a;
        std::vector<std::string> act_list_tmp = game.Step(act, false);
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
        printf("%d %d %lf %lf %lf\n", game.number_, game.score_, 1. * game.score_ * 10000 / game.number_, 1. * clock() / CLOCKS_PER_SEC, 1. * clock() / CLOCKS_PER_SEC * 10000 / game.number_);
        if (game.number_ % 10 == 0) {
            game.DebugOutput(); //Sleep(50);
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
    
    printf("%d\n", game.score_);
}
int main()
{
    srand(time(0));
    work();
    return 0;
    for (weight = 15; weight <= 30; weight += 0.5) {
        printf("%lf ", weight);
        work();
    }
}