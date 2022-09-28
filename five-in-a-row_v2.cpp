/** add "compiler other options": -fexec-charset=BIG-5 **/

#include <cstdio>
#include <cstdlib>

#define N 15
#define MAX_LAYER 2 // max simulated layer
#define N_DEF 99999999

enum {EMPTY, BLACK, WHITE};
enum {MAX, MIN};

const int LIVE[6] = {0, 1, 10, 1200, 30000, 10000000}; // 活i(1~5)
const int DEAD[5] = {0, 0, 1, 10, 30000}; // 死i(1~4)
const double exploiRate = 0.2;
const double explorRate = 0.8;
const int distance = 1; // distance from existed chess that can play
const int weight = 2;

void simulation(int board[N*N]);
void printBoard(int board[N*N]);

int max(int a, int b);
int min(int a, int b);
int isWin(int board[N*N], int turn); // 該turn玩家是否五連線?
int getElement(int board[N*N], int row, int col); // 取得陣列元素，若out of bound則return -1
int getScore(int board[N*N], int currTeam);
int maximin(int board[N*N], int currTeam, int maxOrMin, int layer); // return exploration score
int hasNeighbor(int board[N*N], int row, int col); // 判斷棋盤周邊是否有其他棋子

int AI_team, playerTeam;

int main()
{
    int board[225];
    int col, row, i, j;
    int turn = BLACK;
    int isFirst = 1;
    char c_row, c_team;

    for(i = 0; i < 15; i++)
        for(j = 0; j < 15; j++)
            board[i*N+j] = EMPTY;

    printf("select your team(b/w): ");
    scanf(" %c", &c_team);
    if(c_team == 'b')
    {
        playerTeam = BLACK;
        AI_team = WHITE;
    }
    else if(c_team == 'w')
    {
        playerTeam = WHITE;
        AI_team = BLACK;
    }
    else
    {
        printf("illegal format.");
        return -1;
    }

    printf("note: the board has been rotated 90 degree counterclockwise.\n");
    printf("  ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ\n");
    for(i = 0; i < 15; i++)
    {
        if(i < 10) printf(" ");
        printf("%d", i);
        for(j = 0; j < 15; j++)
            if(j != 0)
                printf(" ┼");
            else
                printf("┼");
        printf("\n");
    }

    turn = BLACK;
    while(1)
    {
        if(isFirst == 1 && AI_team == BLACK)
        {
            board[7*N+7] = turn;
            isFirst = 0;
        }
        else if(turn == playerTeam)
        {
            scanf(" %c%d", &c_row, &col);
            row = c_row - 'A';

            if(col < 0 || col > 14 || row < 0 || row > 14)
            {
                printf("illegal format.\n");
                continue;
            }
            if(board[col*N+row] != EMPTY)
            {
                printf("location is occupied.\n");
                continue;
            }
            board[col*N+row] = turn;
        }
        else
        {
            simulation(board);
        }

        printBoard(board);
        if(isWin(board, turn))
            break;
        turn = (turn == BLACK) ? WHITE : BLACK;
    }

    printf("%s Win!\n", (turn == BLACK) ? "black" : "white");
    system("pause");

}

void printBoard(int board[N*N])
{
    int i, j;

    /// draw graph
    printf("  ＡＢＣＤＥＦＧＨＩＪＫＬＭＮＯ\n");
    for(i = 0; i < 15; i++)
    {
        if(i < 10) printf(" ");
        printf("%d", i);
        for(j = 0; j < 15; j++)
        {
            // due to console background playerTeam, ● looks like white
            if(board[i*N+j] == EMPTY) {
                if(j != 0 && board[i*N+j-1] == EMPTY)
                    printf(" ");
                printf("┼");
            } else if(board[i*N+j] == BLACK) {
                if(j != 0 && board[i*N+j-1] == EMPTY)
                    printf(" ");
                printf("○");
            }
            else if(board[i*N+j] == WHITE) {
                if(j != 0 && board[i*N+j-1] == EMPTY)
                    printf(" ");
                printf("●");
            }
        }

        printf("\n");
    }
}

int max(int a, int b)
{
    if(a == N_DEF)
        return b;
    else if(b == N_DEF)
        return a;
    return a > b ? a : b;
}

int min(int a, int b)
{
    if(a == N_DEF)
        return b;
    else if(b == N_DEF)
        return a;
    return a < b ? a : b;
}

/** (暴力法)reference: https://zhidao.baidu.com/question/204146100.html  **/
int isWin(int board[N*N], int turn)
{
    int i,j;

    for(i=0; i<15; i++)
        for(j=0; j<11; j++)
            if(board[i*N+j]==turn&&board[i*N+j+1]==turn&&board[i*N+j+2]==turn&&board[i*N+j+3]==turn&&board[i*N+j+4]==turn)
                return 1;
    for(j=0; j<15; j++)
        for(i=0; i<11; i++)
            if(board[i*N+j]==turn&&board[(i+1)*N+j]==turn&&board[(i+2)*N+j]==turn&&board[(i+3)*N+j]==turn&&board[(i+4)*N+j]==turn)
                return 1;
    for(i=0; i<11; i++)
        for(j=0; j<11; j++)
            if(board[i*N+j]==turn&&board[(i+1)*N+j+1]==turn&&board[(i+2)*N+j+2]==turn&&board[(i+3)*N+j+3]==turn&&board[(i+4)*N+j+4]==turn)
                return 1;
    for(i=0; i<11; i++)
        for(j=14; j>3; j--)
            if(board[i*N+j]==turn&&board[(i+1)*N+j-1]==turn&&board[(i+2)*N+j-2]==turn&&board[(i+3)*N+j-3]==turn&&board[(i+4)*N+j-4]==turn)
                return 1;
    return 0;
}

int getElement(int board[N*N], int row, int col) // 取得陣列元素，若out of bound則return -1
{
    if(row < 0 || row >= N || col < 0 || col >= N)
        return -1;
    else return board[row*N+col];
}

int getScore(int board[N*N], int currTeam)
{
    int score = 0;
    int i, j, cnt;
    int chessCount;
    int opponent = (currTeam == BLACK) ? WHITE : BLACK;

    // judge score points
    if(isWin(board, currTeam))
        score = LIVE[5];
    else if(isWin(board, opponent))
        score = -LIVE[5];
    else {
        for(i = 0; i < N; i++)
        {
            for(j = 0; j < N; j++)
            {
                if(board[i*N+j] == EMPTY)
                    continue;
                cnt = 0;

                /// 判斷橫排連線
                while(j+cnt < N)
                {
                    if(board[i*N+j+cnt] == board[i*N+j+cnt+1])
                        cnt++;
                    else break;
                }

                chessCount = cnt+1;
                if(chessCount > 1)
                {
                    if(getElement(board, i, j-1) == EMPTY && getElement(board, i, j+chessCount) == EMPTY)
                        score += (board[i*N+j] == currTeam) ? LIVE[chessCount] : -LIVE[chessCount] * weight;
                    else if(getElement(board, i, j-1) == EMPTY || getElement(board, i, j+chessCount) == EMPTY)
                        if(getElement(board, i, j-1) != AI_team && getElement(board, i, j+chessCount) != AI_team)
                            score += (board[i*N+j] == currTeam) ? DEAD[chessCount] : -DEAD[chessCount] * weight;
                }

                /// 判斷直排連線
                cnt = 0;
                while(i+cnt < N)
                {
                    if(board[(i+cnt)*N+j] == board[(i+cnt+1)*N+j])
                        cnt++;
                    else break;
                }

                chessCount = cnt+1;
                if(chessCount > 1)
                {
                    if(getElement(board, i-1, j) == EMPTY && getElement(board, i+chessCount, j) == EMPTY)
                        score += (board[i*N+j] == currTeam) ? LIVE[chessCount] : -LIVE[chessCount] * weight;
                    else if(getElement(board, i-1, j) == EMPTY || getElement(board, i+chessCount, j) == EMPTY)
                        if(getElement(board, i-1, j) != AI_team && getElement(board, i+chessCount, j) != AI_team)
                            score += (board[i*N+j] == currTeam) ? DEAD[chessCount] : -DEAD[chessCount] * weight;
                }

                /// 判斷"\"向連線
                cnt = 0;
                while(i+cnt < N && j+cnt < N)
                {
                    if(board[(i+cnt)*N+j+cnt] == board[(i+cnt+1)*N+j+cnt+1])
                        cnt++;
                    else break;
                }

                chessCount = cnt+1;
                if(chessCount > 1)
                {
                    if(chessCount == 4) {
                        //printf("on (%d, %d) has \\ 4\n", i, j);
                        //printBoard(board);
                        //system("pause");
                    }
                    if(getElement(board, i-1, j-1) == EMPTY && getElement(board, i+chessCount, j+chessCount) == EMPTY)
                        score += (board[i*N+j] == currTeam) ? LIVE[chessCount] : -LIVE[chessCount] * weight;
                    else if(getElement(board, i-1, j-1) == EMPTY || getElement(board, i+chessCount, j+chessCount) == EMPTY)
                        if(getElement(board, i-1, j-1) != AI_team && getElement(board, i+chessCount, j+chessCount) != AI_team)
                            score += (board[i*N+j] == currTeam) ? DEAD[chessCount] : -DEAD[chessCount] * weight;
                }


                /// 判斷"/"向連線
                cnt = 0;
                while(i+cnt < N && j-cnt >= 0)
                {
                    if(board[(i+cnt)*N+j-cnt] == board[(i+cnt+1)*N+j-cnt-1])
                        cnt++;
                    else break;
                }

                chessCount = cnt+1;
                if(chessCount > 1)
                {
                    if(getElement(board, i-1, j+1) == EMPTY && getElement(board, i+chessCount, j-chessCount) == EMPTY)
                        score += (board[i*N+j] == currTeam) ? LIVE[chessCount] : -LIVE[chessCount] * weight;
                    else if(getElement(board, i-1, j+1) == EMPTY || getElement(board, i+chessCount, j-chessCount) == EMPTY)
                        if(getElement(board, i-1, j+1) != AI_team && getElement(board, i+chessCount, j-chessCount) != AI_team)
                            score += (board[i*N+j] == currTeam) ? DEAD[chessCount] : -DEAD[chessCount] * weight;
                }
            } // end for j
        } // end for i
    } // end else
    return score;
}

void simulation(int board[N*N])
{
    double score, bestScore = N_DEF;
    int best_row, best_col;
    int exploiScore, explorScore;
    int i, j, k, l;

    /// judge the best step and do
    for(i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            if(board[i*N+j] != EMPTY)
                continue;

            if(!hasNeighbor(board, i, j))
                continue;

            /// simulate current position
            board[i*N+j] = AI_team;
            exploiScore = getScore(board, AI_team);
            explorScore = maximin(board, playerTeam, MIN, 1);
            score = exploiScore*exploiRate + explorScore*explorRate;
            //printf("(%d, %d) has score %.2f(%d && %d)\n", i, j, score, exploiScore, explorScore);
            board[i*N+j] = EMPTY;

            if(bestScore == N_DEF || bestScore < score)
            {
                //printf("bestScore: %.2f -> %.2f\n", bestScore, score);
                bestScore = score;
                best_row = i;
                best_col = j;
            }
        } // end for j
    } // end for i

    board[best_row*N+best_col] = AI_team;
}

int maximin(int board[N*N], int currTeam, int maxOrMin, int layer)
{
    if(layer == MAX_LAYER)
    {
        return getScore(board, currTeam);
    }
    else if(isWin(board, currTeam))
    {
        //printf("layer = %d, possibility: \n", layer);
        //printBoard(board);
        //system("pause");
        return getScore(board, currTeam);
    }

    //if(getScore(board, currTeam) >=70000 || getScore(board, currTeam) <= -70000) {
    //printf("layer = %d\n", layer);
    //printBoard(board);
    //system("pause");
    //}

    int i, j;
    int score = N_DEF;
    int nextTeam = (currTeam == BLACK) ? WHITE : BLACK;

    /// start searching child node
    for(i = 0; i < N; i++)
    {
        for(j = 0; j < N; j++)
        {
            if(board[i*N+j] != EMPTY)
                continue;

            if(!hasNeighbor(board, i, j))
                continue;

            /// simulate current position
            board[i*N+j] = currTeam;

            if(maxOrMin == MAX)
                score = max(score, maximin(board, nextTeam, MIN, layer+1));
            else if(maxOrMin == MIN)
                score = min(score, maximin(board, nextTeam, MAX, layer+1));

            board[i*N+j] = EMPTY;
        }
    }

    return score;
}

int hasNeighbor(int board[N*N], int row, int col)
{
    int i, j;
    int result = 0;
    for(i = -distance; i <= distance; i++)
    {
        for(j = -distance; j <= distance; j++)
        {
            if(getElement(board, row+i, col+j) == BLACK || getElement(board, row+i, col+j) == WHITE)
            {
                result = 1;
                break;
            }
        }
        if(result) break;
    }

    return result;
}
