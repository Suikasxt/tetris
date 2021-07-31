// tetris.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "GameController.h"

double evaluate(GameController game) {
    double res = 0;
    for (int i = 0; i < game.Width; i++) {
        double weight = 1;
        for (int j = game.Height-1; j >=0; j--) {
            if (game.has_blocks_[i][j]) {
                res -= weight;
                weight += 1;
            }
            else {
                weight += 8;
            }
        }
    }
    return res;
}

int main()
{
    GameController game;
    game.Restart();
    while (!game.game_over_ && game.number_ < 200) {
        double value_max = -1e9;
        int ar = -1, ax = -1, ay = -1;
        for (int r = 0; r < 4; r++) {
            for (int i = game.Height - 1; i >= 0; i--) {
                for (int j = 0; j < game.Width; j++) {
                    if (game.CanStay(r, j, i)) {
                        GameController tmp_game(game);
                        tmp_game.Step(r, j, i);
                        double value = evaluate(tmp_game);
                        if (value > value_max) {
                            value_max = value;
                            ar = r;
                            ax = j;
                            ay = i;
                        }
                    }
                }
            }
        }
        printf("%d %d %d %d\n", game.number_, ar, ax, ay);
        game.Step(ar, ax, ay);
        //game.DebugOutput();
    }
    std::cout << "game.pause();game.playRecord('";
    for (int i = 0; i < game.act_list_.size(); i++) {
        std::cout << game.act_list_[i];
        if (i != game.act_list_.size() - 1) {
            std::cout << ",";
        }
    }
    std::cout << "'.split(','));";
    printf("\n%d\n", game.score_);
}