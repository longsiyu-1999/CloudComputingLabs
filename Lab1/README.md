#### Lab1最终实验成果保存路径→CloudComputingLabs/Lab1

# Lab 1: “超高速”数独解题

进入实验git资源库克隆的文件夹，并提取最新提交的资源。 

`git pull`

您可以在lab1/readme.md中找到本实验1的说明。

lab1的所有材料都在lab1/文件夹中。

## 1. 1.	概述

使用多个线程或多个进程，在一台机器上运行，实现数独解算程序。
试着利用你所有的CPU核心，让你的程序尽可能快地运行！

### 实验目的：

* 练习基本的并行编程技能，例如使用多线程/进程。
* 熟悉Unix操作系统环境开发(如文件I/O、获取时间戳)。
* 熟悉源代码版本控制工具(GIT)，学会使用GitHub与他人协作。
* 练习如何进行性能测试，并撰写高质量的性能测试报告。

## 2. 背景

### 2.1 ①.	数独介绍

Sudoku (originally called Number Place) is a logic-based combinatorial number-placement puzzle. 

You are given a 9×9 board of 81 squares logically separated into 9 columsn, 9 rows, and 9 3×3 subsquares. The goal is, given a board with some initial numbers filled in (we call it a **Sudoku puzzle**), fill in the rest of the board so that every column, row, and subsquare have all the digits from 1 to 9 and each digit appears only once (we call it a **Sudoku solution**).


 <u>An example Sudoku puzzle:</u>

<img src="src/Sudoku_puzzle.png" alt="Sudoku" title="Sudoku puzzle" style="zoom:67%;" />

 <u>An example Sudoku solution to above puzzle:</u>

<img src="src/Sudoku_answer.png" alt="Sudoku" title="Sudoku answer" style="zoom:67%;" />

### 2.2 可用资源

If you have no idea about what algorithms can be used to solve Sudoku puzzles, we suggest you read [this](https://rafal.io/posts/solving-sudoku-with-dancing-links.html). To simplify your work, we have provided a simple [implementation](src/Sudoku/) `(Lab1/src/Sudoku`) of 4 Sudoku solving algorithms (some are slow, some are fast), but without using multiple threads/processes. The two files *test1* and *test1000* contain many puzzles for you to test. 

Of course, you are always encouraged (not mandatory) to implement those algorithms by yourselves and even your own algorithms (if you have time).

## 3. 3.	实验任务

### 3.1 写一个程序 

Implement a program which satisfies the following requirements:

#### 3.1.1 程序输入和输出

##### **3.1.1.1 输入** 

1. Your program should have no arguments during start. For example, if your program is called *sudoku_solve*,  just typing `./sudoku_solve` and your program will run correctly.
2. But after start, your program should be able to read multiple strings from ***stdin***, where each string is separated by a line-break. Each string is a **name of a file**, which contains one or more Sudoku puzzles that your program is going to solve. 
3. In the input file, **each line** is a Sudoku puzzle that needs to be solved. Each line contains 81 decimal digits. The 1st-9th digits are the 1st row of the 9×9 grid, and the 10th-18th digits are the 2nd row of the 9×9 grid, and so on. Digit 0 means this digit is unknown and your program needs to figure it out according to the Sudoku rules described above.

<u>Example input to your program</u> may be like this (start your program and read 3 input file names from stdin)

<img src="src/ExampleInput.png" alt="ExampleInput" style="zoom:50%;" />

<u>Example input file format</u>, 

```
./test1 is an input file that contains one Sudoku puzzle problem that need to be solved:
310000085000903000905000307804000106000401000690000073030502010000804000020706090

The first Sudoku puzzle problem (first line) actually looks like this when being placed on the 9×9 grid:
---------------------
3 1 0 | 0 0 0 | 0 8 5
0 0 0 | 9 0 3 | 0 0 0
9 0 5 | 0 0 0 | 3 0 7
---------------------
8 0 4 | 0 0 0 | 1 0 6 
0 0 0 | 4 0 1 | 0 0 0 
6 9 0 | 0 0 0 | 0 7 3
---------------------
0 3 0 | 5 0 2 | 0 1 0
0 0 0 | 8 0 4 | 0 0 0
0 2 0 | 7 0 6 | 0 9 0
---------------------
```
##### 3.1.1.2 输出

1. You should calculate the solutions of all the Sudoku puzzles in all the input files, and output these solutions into ***stdout***.
2. The solutions should be outputed **in the same sequence** as the puzzles are inputted in. For example, if  there are two input files and the first input file contains 2 puzzles, then, the 1st output solution should be of the 1st puzzle in the first input file, and the 2nd output solution should be of the 2nd puzzle in the first input file, and the 3rd output solution should be of the 1st puzzle in the second input file, and so on.

<u>Example output</u>:

```
Assuming your program has been inputted with two file names from stdin: 
./test1
./test2

./test1 has the following content:
310000085000903000905000307804000106000401000690000073030502010000804000020706090

./test2 has the following content:
000000010400000000020000000000050407008000300001090000300400200050100000000806000
000000013000030080070000000000206000030000900000010000600500204000400700100000000

Then your program should output the following results to stdout:
312647985786953241945128367854379126273461859691285473437592618569814732128736594
693784512487512936125963874932651487568247391741398625319475268856129743274836159
869725413512934687374168529798246135231857946456319872683571294925483761147692358

Where the 1st line in the output is the solution to puzzle
310000085000903000905000307804000106000401000690000073030502010000804000020706090
and the 2nd line in the output is the solution to puzzle
000000010400000000020000000000050407008000300001090000300400200050100000000806000
and the 3rd line in the output is the solution to puzzle
000000013000030080070000000000206000030000900000010000600500204000400700100000000
```
#### 3.1.3 分级要求 

##### 3.1.3.1 基础版

Your program should be able to: 

1. 接受 **一个** 输入文件名，并且输入文件大小小于100MB。
2. 成功解出输入文件要求的数独题，并将结果按要求输出。
3. 使用多线程/进程来利用您机器的大部分CPU内核。

\[Tips\]: 1) 1．	使用事件队列将任务分派到工作线程，并将结果合并到工作线程或从工作线程合并结果. 2) 动态检测您的计算机上有多少个CPU核心，以便确定您的程序使用了多少线程/进程. 3) 注意多线程/进程之间的争用问题.

##### 3.1.3.2 进阶版

Your program should be able to: 

1. 完成基础版的所有要求。
2. 接受 **任意数量的输入文件名**，输入文件的大小可以是 **任意大小**(只要可以存储在您的磁盘上)。
3. 当程序在解决先前输入文件中的谜题时，**程序可以同时接受来自 *stdin*** 的更多输入文件名。

\[Tips\]: 1) 使用专用线程接受输入; 2) 为避免占用全部内存，将文件的不同部分读入内存，逐一求解; 3) 3．	尝试更多优化，如缓存一致性处理.

### 3.2. 完成性能测试报告

请先测试您的代码，并将测试报告和实验代码一起提交到小组的课程GitHub Repo中。

测试报告应该描述测试输入，以及在各种测试条件下的性能结果。
具体至少应该包含以下两点：

1. 在相同的输入、相同的环境下，将您的超快数独解算程序与简单的单线程解算程序进行性能对比。
2. 更改输入(例如，更改文件大小)和环境(例如，使用不同CPU和硬盘的机器)，并绘制程序在各种条件下的性能曲线。

## 4. 实验提交

请将您的所有代码放在文件夹Lab1中，并编写一个Makefile，这样我们就可以在一个命令make中编译您的代码。
编译后的可运行可执行二进制文件应该命名为sudoku_solve，并位于文件夹Lab1中。
如果您不知道如何编写Makefile，可以在lab1/src/Sudoku中找到一个简单的示例。
请仔细遵循以上规则，以便助教自动测试您的代码！


请按照总体实验说明(../readme.md)中的指导提交您的实验计划和性能测试报告

## 5. 评分标准

1. 38分标准：
* 完成基础版的所有要求。
* 您的性能测试报告已经完成了前面描述的两个要求。如果你错过了一些部分，你只会得到部分分，取决于完成度。

2. 40分（满分）标准：
* 完成高级版的所有要求。
* 您的性能测试报告已经完成了前面描述的两个要求。如果你错过了一些部分，你会得到一部分分，取决于完成度。
