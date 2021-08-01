
#include <time.h>
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <string.h>
#include <iomanip>
#include <Windows.h>

#pragma warning(disable : 4996)

using namespace std;

void buff_clr(void)
{

    char junk;
    do {
        junk = getchar();
    } while (junk != '\n');
}

struct coord
{
    int y;
    int x;

}coords;

int randgen(vector<vector<int>>& Ships , int n)
{
    int i, j, count = 0;
    srand((unsigned)time(NULL));
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            Ships[i][j] = rand() % 2;
            if (Ships[i][j] == 1)
            {
                count++;
            }
        }
    }
    return count;
}

void draw_gui(vector<vector<char>> &Pseudo_gui , int n)
{
    int i, j;

    Pseudo_gui[0][0] = ' ';
    for (i = 1; i < (n + 1); i++)
    {
        Pseudo_gui[0][i] = i + 48;
        Pseudo_gui[i][0] = i + 48;
    }

    for (i = 1; i < (n + 1); i++)
    {
        for (j = 1; j < (n + 1); j++)
        {
            Pseudo_gui[i][j] = '+';
        }
    }
}

void battle(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui , int n, struct coord x, int* count, int* miss)
{

    int i, j;

    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (x.x - 1 == i && x.y - 1 == j)
            {
                if (Ships[i][j] == 1)
                {
                    if (Pseudo_gui[i + 1][j + 1] == 'O')
                    {
                        cout << setw(10) << "You've already uncovered this field!" << endl;
                        break;
                    }
                    cout << setw(10) << "Hit!" << endl;
                    Pseudo_gui[i + 1][j + 1] = 'O';
                    (*count)--;
                }
                else
                {
                    if (Pseudo_gui[i + 1][j + 1] == 'X')
                    {
                        cout << setw(10) << "You've already uncovered this field!" << endl;
                        break;
                    }
                    cout << setw(10) << "Miss!" << endl;
                    Pseudo_gui[i + 1][j + 1] = 'X';
                    (*miss)++;
                }
            }
        }
    }
}

void result(vector<vector<char>> Pseudo_gui, int n)
{
    cout << endl;
    cout << setw(50);
    for (int i = 0; i < (n + 1); i++)
    {
        for (int j = 0; j < (n + 1); j++)
        {
            cout<< Pseudo_gui[i][j]<<"     ";
        }
        cout << endl;
        cout << endl;
        cout << setw(50);
    }
}

int main() {

    vector<vector<int>> Ships;
    vector<vector<char>> Pseudo_gui;
    int i;
    int n;
    char switch_size, switch_difficulty;
    int difficulty = 0;
    int shipcount = 0;
    int x_count = 0;
    string s;


    cout << setw(10) << "Sink the ships v0.1b" << endl;
    cout << endl;

    cout << setw(10) << "Choose size(S, M, L) :"<<endl;
    cout << setw(10) << "=>>";
    cin>>switch_size;
    buff_clr();

    switch (switch_size)
    {
    case 's':
    case 'S':n = 3; break;
    case 'm':
    case 'M':n = 5; break;
    case 'l':
    case 'L':n = 8; break;
    default:cout << endl << "You've choosen poorly!" << endl;
        system("pause");
        exit(EXIT_FAILURE);
    }
    cout << endl;
    cout << setw(10) << "Choose difficulty(E, H) :"<<endl;
    cout << setw(10) << "=>>";
    cin>>switch_difficulty;

    buff_clr();

    switch (switch_difficulty)
    {
    case 'e':
    case 'E':difficulty = (n * 2) - 2; break;
    case 'h':
    case 'H':difficulty = (n / 2); break;
    default:cout << endl<<setw(10) << "You've choosen poorly!";
        system("pause");
        exit(EXIT_FAILURE);
    }

    Ships.resize(n);

    for (i = 0; i < n; i++)
    {
        Ships[i].resize(n);
    }

    Pseudo_gui.resize(n+1);

    for (i = 0; i < (n + 1); i++)
    {
        Pseudo_gui[i].resize(n+1);
    }

    shipcount = randgen(Ships, n);
    cout << endl;
    cout << setw(10) << "Number of ships to be sunk:"<< shipcount<<endl;
    cout << endl;
    cout << setw(10) << "Number of misses allowed: "<< difficulty<<endl;

    draw_gui(Pseudo_gui, n);
    result(Pseudo_gui, n);

    while (shipcount != 0 && x_count != difficulty)
    {

        cout << endl << "Attack at (x,y):";
        cout << " =>>";
        cin >> coords.x;
        cin >> coords.y;
        /*buff_clr();*/

        system("cls");

        battle(Ships, Pseudo_gui, n, coords, &shipcount, &x_count);
        result(Pseudo_gui, n);

        cout << setw(10) << "Number of ships to be sunk:"<<shipcount<<endl;
        cout << endl;
        cout << setw(10) << "Number of misses(out of " << difficulty << "): " << x_count << endl;

    }
    if (shipcount == 0)
    {
        cout << endl << "Winner!";
        system("pause");
    }
    else if (x_count == difficulty)
    {
        cout << endl << "You Lose!";
        system("pause");
    }
    return 0;
}