#ifndef SUDOKU_H
#define SUDOKU_H

const bool DEBUG_MODE = false;
enum { ROW=9, COL=9, N = 81, NEIGHBOR = 20 };
const int NUM = 9;
const int Task_num=5000000;//创建任务最大数量

void init_neighbors();//初始化neighbors数组
int input(const char in[N], int board[N], int spaces[N]);
void init_cache(int board[N]);

bool available(int guess, int cell);

bool solve_sudoku_basic(int which_space,int board[N],int nspaces,int spaces[N]);
bool solved(int board[N]);
#endif
