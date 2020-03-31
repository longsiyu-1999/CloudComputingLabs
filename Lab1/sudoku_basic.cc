#include <assert.h>
#include <stdio.h>
#include <strings.h>
#include <algorithm>

#include "sudoku.h"
int neighbors[N][NEIGHBOR];

static void print_neighbors(const bool adjacent[ROW][COL], int row, int col, int myneighbors[NEIGHBOR])
{
  for (int y = 0; y < ROW; ++y) {
    for (int x = 0; x < COL; ++x) {
      if (y == row && x == col)
        putchar('X');
      else
        putchar(adjacent[y][x] ? 'o' : '.');
    }
    printf("\n");
  }
  for (int i = 0; i < NEIGHBOR; ++i) {
    printf("%2d, ", myneighbors[i]);
  }
  puts("\n");
}
static void collect_neighbors(const bool adjacent[ROW][COL], int row, int col, int myneighbors[NEIGHBOR])
{
  int n = 0;
  for (int y = 0; y < ROW; ++y) {
    for (int x = 0; x < COL; ++x) {
      if (adjacent[y][x] && !(y == row && x == col)) {
        assert(n < NEIGHBOR);
        myneighbors[n++] = y*COL + x;
      }
    }
  }
  assert(n == NEIGHBOR);
}

static void mark_adjacent(bool adjacent[ROW][COL], int row, int col)
{
  for (int i = 0; i < NUM; ++i) {
    adjacent[row][i] = true;
    adjacent[i][col] = true;
  }
  int top = (row/3)*3;
  int left = (col/3)*3;
  adjacent[top][left] = true;
  adjacent[top][left+1] = true;
  adjacent[top][left+2] = true;
  adjacent[top+1][left] = true;
  adjacent[top+1][left+1] = true;
  adjacent[top+1][left+2] = true;
  adjacent[top+2][left] = true;
  adjacent[top+2][left+1] = true;
  adjacent[top+2][left+2] = true;
}

/*public*/ void init_neighbors()
{
  for (int row = 0; row < ROW; ++row) {
    for (int col = 0; col < COL; ++col) {
      bool adjacent[ROW][COL];
      bzero(adjacent, sizeof adjacent);
      mark_adjacent(adjacent, row, col);

      int me = row*COL + col;
      collect_neighbors(adjacent, row, col, neighbors[me]);

      if (DEBUG_MODE)
        print_neighbors(adjacent, row, col, neighbors[me]);
    }
  }
}

//校验可行解
bool solved(int board[N])
{
  int (*chess)[COL] = (int (*)[COL])board;
  for (int row = 0; row < ROW; ++row) {
    // check row
    int occurs[10] = { 0 };
    for (int col = 0; col < COL; ++col) {
      int val = chess[row][col];	
      assert(1 <= val && val <= NUM);	
      ++occurs[val];
    }
   //对于每一行  1到9都要出现一次 那么occurs[1]到occurs[9]都应该是1 加起来是NUM=9
    if (std::count(occurs, occurs+10, 1) != NUM)
      return false;
  }
  for (int col = 0; col < COL; ++col) {
    int occurs[10] = { 0 };
    for (int row = 0; row < ROW; ++row) {
      int val = chess[row][col];
      // assert(1 <= val && val <= NUM);
      ++occurs[val];
    }
	//同理校验列
    if (std::count(occurs, occurs+10, 1) != NUM)
      return false;
  }
	//校验九宫格  （这里涉及到二维数组的随机访问 我感觉可能是一个可以优化的地方）
  for (int row = 0; row < ROW; row += 3) {
    for (int col = 0; col < COL; col += 3) {
      int occurs[10] = { 0 };
      ++occurs[chess[row  ][col]];
      ++occurs[chess[row  ][col+1]];
      ++occurs[chess[row  ][col+2]];
      ++occurs[chess[row+1][col]];
      ++occurs[chess[row+1][col+1]];
      ++occurs[chess[row+1][col+2]];
      ++occurs[chess[row+2][col]];
      ++occurs[chess[row+2][col+1]];
      ++occurs[chess[row+2][col+2]];

      if (std::count(occurs, occurs+10, 1) != NUM)
        return false;
    }
  }
  //校验通过
  return true;
}

//在main.cc中调用  作用是记录输入题中空格的位置情况
static int find_spaces(int board[N], int spaces[N],int nspaces)
{
  nspaces=0;
  for (int cell = 0; cell < N; ++cell) {
    if (board[cell] == 0)		//board[]存的是初始题 但会随着递归的过程慢慢变化 也就是为0的位置慢慢填上正确值
      spaces[nspaces++] = cell; //nspaces是初始题目的空格数 spaces[n] 存的是第n个空格对应原题目中的格子索引
  }
  return nspaces;
}

//全局通过这个函数进行数据读入 （不涉及I/O 只是把I/O读入的字符串转化成int数组）
int input(const char in[N],int board[N], int spaces[N])
{
  for (int cell = 0; cell < N; ++cell) {
    board[cell] = in[cell] - '0';
    assert(0 <= board[cell] && board[cell] <= NUM);
    //printf("%d",board[cell]);	//打印读入的题
  }
  int t=find_spaces(board, spaces,0);
  return t;
}

//检查某一个单元格 当选填guess这个数字时 是否与邻居单元格冲突 sudoku.h中定义neighbor为20个 即横纵16+四个角=20
bool available(int guess, int cell,int board[N])
{
  for (int i = 0; i < NEIGHBOR; ++i) {
    int neighbor = neighbors[cell][i];
    if (board[neighbor] == guess)
    {
      return false;
    }
  }
  return true;
}

//递归求解
bool solve_sudoku_basic(int which_space,int board[N],int nspaces,int spaces[N])
{
	
  //这里返回true说明找到了可行解 
  if (which_space >= nspaces) {
  
    return true;
  }

  int cell = spaces[which_space];//which_space表示的是本次递归讨论的是第几个空格子 （spaces[wich_space]表示这个空格子在原题目数组中的索引）
  //依次猜测本空格可填的数字 1到9
  for (int guess = 1; guess <= NUM; ++guess) {
  //校验这个猜测数字是否可行
    if (available(guess,cell,board)) {
      // hold
      assert(board[cell] == 0);	//断言  如果此时的cell位置不为0 那就终止程序 （这个情况理论上不会出现）
      board[cell] = guess;  //当前空填上guess 
      // try
      //递归下一空  如果整个后续都返回了true 说明这一轮的guess是有可以满足可行解的 那么这一空也返回true
      if (solve_sudoku_basic(which_space+1,board,nspaces,spaces))
      {
        return true;
      }
      //如果刚刚下一空之后的递归失败了 说明本轮的guess不满足可行解
      // unhold
      //断言  确保刚刚的递归没有修改board的值 也就是每一次失败的递归 都会不会导致参数发生改变
      assert(board[cell] == guess);	
      //本次尝试归0 进行下一个guess的验证
      board[cell]=0;
    }
  }
  //如果当前空的9个数都构成不了可行解 说明此题无解 返回false
  return false;
}
