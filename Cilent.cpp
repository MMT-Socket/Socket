#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <string.h>
#include <thread>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <Windows.h>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <conio.h>

using namespace std;
#pragma warning(disable : 4996)
#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define IP_ADDRESS "192.168.1.7"
#define DEFAULT_PORT "3504"

struct client_type
{
    SOCKET socket;
    int id;
    char received_message[DEFAULT_BUFLEN];
};

struct User {

    string Account;
    string Password;
    string Encrypt;
};

struct coord
{
    int y;
    int x;

}coords;

int FlagRev(string s) {
    // Game part
    // Take User online list 
    if (s.find("ENOUGH_USER") != -1)
    {
        return 22;
    }
    else if (s.find("ONLINE_LIST") != -1)
    {
        return 21;
    }
    else if (s.find("INVITE") != -1)
    {
        return 23;
    }
    else if (s.find("COMPERTITOR_ACCEPT") != -1)
    {
        return 24;
    }
    else if (s.find("COMPERTITOR_REJECT") != -1)
    {
        return 25;
    }
    else if (s.find("REV_SEND_MAP") != -1)
    {
        return 26;
    }
    else if (s.find("UPLOAD_MAP") != -1)
    {
        return 27;
    }
    else if (s.find("ATTACK_SHIP ") != -1)
    {
        return 28;
    }
    else if (s.find("MY_TURN") != -1)
    {
        return 29;
    }
    else if (s.find("LOSE_GAME") != -1)
    {
        return 30;
    }
    else if (s.find("ENCRYPT_") != -1)
    {
        return 30;
    }

    // Sign and user info part
    else if (s == "LOGIN_SUCCESS")
    {
        return 1;
    }
    else if (s == "LOGIN_FAIL")
    {
        return 2;
    }
    else if (s == "REGISTER_SUCCESS")
    {
        return 3;
    }
    else if (s == "REGISTER_FAIL")
    {
        return 4;
    }
    else if (s == "CHANGE_SUCCESS")
    {
        return 5;
    }
    else if (s == "CHANGE_FAIL")
    {
        return 6;
    }
    else if (s.find("CHECK_USER") != -1) {
        return 7;
    }
    else if (s.find("NotFound") != -1) {
        return 8;
    }
    else if (s.find("NotExist") != -1) {
        return 9;
    }
    else if (s.find("SETUP_INFO") != -1) {
        return 10;
    }
    else if (s.find("USER_FAIL_ONLINE") != -1) {
        return 11;
    }
    else if (s == "LOGOUT")
    {
        return 100;
    }
    return 0;

}

// Game send flag
int FlagGameSend(string s) {

    // Game part
    // Format CREATE_ROOM (room_number) (User_ID) 
    if (s == "YES")
    {
        return 26;
    }
    else if (s == "NO")
    {
        return 27;
    }
    if (s.find("CREATE_ROOM") != -1)
    {
        return 21;
    }
    else if (s.find("ACCEPT,P") != -1)
    {
        return 22;
    }
    else if (s.find("REJECT,P") != -1)
    {
        return 23;
    }
    //Upload Map
    else if (s.find(".txt") != -1)
    {
        return 24;
    }
    else if (s.find("ATTACK_SHIP ") != -1)
    {
        return 25;
    }
    else {
        return -1;
    }

}

string FlagSend(int flag) {

    switch (flag)
    {
    case 1:
        return "LOGIN";
    case 2:
        return "REGISTER";
    case 3:
        return "CHANGE_PASSWORD";
    case 20:
        return "START_GAME";

    case 100:
        return "LOGOUT";
    }

    return "";
}
// Setting Window
void SetWindowSize(SHORT width, SHORT height) {

    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    SMALL_RECT WindowSize;
    WindowSize.Top = 0;
    WindowSize.Left = 0;
    WindowSize.Right = width;
    WindowSize.Bottom = height;

    SetConsoleWindowInfo(hStdout, 1, &WindowSize);

}

void SetScreenBufferSize(SHORT width, SHORT height)
{
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    COORD NewSize;
    NewSize.X = width;
    NewSize.Y = height;

    SetConsoleScreenBufferSize(hStdout, NewSize);
}
void DisableResizeWindow()
{
    HWND hWnd = GetConsoleWindow();
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);
}
void ShowScrollbar(BOOL Show)
{
    HWND hWnd = GetConsoleWindow();
    ShowScrollBar(hWnd, SB_BOTH, Show);
}

void  Setting_Console() {
    SetWindowSize(100, 50);
    SetScreenBufferSize(100, 50);
    DisableResizeWindow();
}


string string_to_hex(const string& in) {
    stringstream ss;

    ss << hex << setfill('0');
    for (size_t i = 0; in.length() > i; ++i) {
        ss << setw(2) << static_cast<unsigned int>(static_cast<unsigned char>(in[i]));
    }

    return ss.str();
}

string hex_to_string(const string& in) {
    string output;

    if ((in.length() % 2) != 0) {
        throw runtime_error("String is not valid length ...");
    }

    size_t cnt = in.length() / 2;

    for (size_t i = 0; cnt > i; ++i) {
        uint32_t s = 0;
        stringstream ss;
        ss << hex << in.substr(i * 2, 2);
        ss >> s;

        output.push_back(static_cast<unsigned char>(s));
    }

    return output;
}

void SentEncrytMsg(SOCKET client, string msg, bool encrypt) {
    // Encry before sent
    if (encrypt == true)
    {
        msg = string_to_hex(msg);
        int iResults = send(client, msg.c_str(), strlen(msg.c_str()), 0);
        if (iResults == SOCKET_ERROR)
        {
            cout << "send() failed: " << WSAGetLastError() << endl;
            return;
        }
    }
    // No enrypt
    else {
        int iResults = send(client, msg.c_str(), strlen(msg.c_str()), 0);
        if (iResults == SOCKET_ERROR)
        {
            cout << "send() failed: " << WSAGetLastError() << endl;
            return;
        }
    }

    return;

}


string RevEncrytMsg(SOCKET client) {

    char tempmsg[DEFAULT_BUFLEN] = "";
    memset(tempmsg, 0, DEFAULT_BUFLEN);
    int iResults = recv(client, tempmsg, DEFAULT_BUFLEN, 0);

    return hex_to_string(tempmsg);

}

string RevMsg(SOCKET client) {

    char tempmsg[DEFAULT_BUFLEN] = "";
    memset(tempmsg, 0, DEFAULT_BUFLEN);
    int iResults = recv(client, tempmsg, DEFAULT_BUFLEN, 0);

    return tempmsg;

}

void Clean(int i) {
    if (i == 1)
    {
        system("cls");
        cout << endl;
        cout << endl;
        cout << endl;
    }
    else {
        cout << endl;
        cout << endl;
        cout << endl;
    }
};

void Shutdown(SOCKET& client) {

    int iResult = shutdown(client, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown() failed with error: " << WSAGetLastError() << endl;
        closesocket(client);
        WSACleanup();
        system("pause");
        return;
    }
    cout << setw(70) << "Shutting down socket..." << endl;
    Clean(2);
    cout << setw(69);

    closesocket(client);
    WSACleanup();
    system("pause");
    return;
}

void InputPassword(SOCKET& client, bool encrypt , User& user,int purpose ) {
    string sent_message;
    if (purpose == 1)
    {
        cout << setw(70) << "Input Password:" << endl;
    }
    else {
        cout << setw(69) << "Input new Password:" << endl;
    }
    
    cout << setw(56) << "=>>";

    /*char p[100];
    int i = 0;
    while ((p[i] = _getch()) != 13 && i++ < 15) {
        if (i > 1) printf("\b\b");
        printf("* %d", i);
    }
    p[i] = '\0';*/
    char ch;
    char buf[100] = { 0 };
    //int count = 1;
    int i;
    for (i = 0; i < 100; i++)
    {
        ch = getch();

        if (ch == 13)break;//13=Enter
        if (ch == 27)exit(0);//27=ESC
        if (ch == 8)//Backspace
        {
            i--;
            if (i < 0)continue;
            buf[i--] = 0;
            cprintf("%c", char(8));
            cprintf("%c", char(32));
            cprintf("%c", char(8));
            continue;
        }
        printf("*");
        buf[i] = ch;
    }
    sent_message.resize(i);
    for (int u = 0; u < i; u++)
    {
        sent_message[u] = buf[u];
    }

    user.Password = sent_message;
}
void EncryptMenu() {

    // Init user's Menu
    cout << setw(70) << "====== Encrypt =====" << endl;
    Clean(2);
    cout << setw(75) << "Do you want to encrypt message (Y/N)" << endl;
    Clean(2);
    cout << setw(65) << "=>>  ";

}
void Login(SOCKET& client, bool encrypt,User& user) {

    Clean(1);

    string sent_message = FlagSend(1);
    // Send Flag LOGIN 
    SentEncrytMsg(client, sent_message, encrypt);
    cout << setw(70) << "=====LOGIN=====" << endl;

    //  Input Account
    cout << setw(70) << "Input Account :" << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, user.Account);

    //  Input Password
    InputPassword(client, encrypt,user,1);

    Clean(1);
    
    // Sent mess

    EncryptMenu();
    getline(cin, user.Encrypt);

    if (user.Encrypt == "Y")
    {
        encrypt = true;
    }

    // Sent mess
    SentEncrytMsg(client, user.Encrypt, false);
    string s;
    s = to_string(user.Account.length()) + user.Account + user.Password;
    SentEncrytMsg(client, s, encrypt);

}

void Register(SOCKET& client, bool encrypt) {

    User user;
    Clean(1);

    string sent_message = FlagSend(2);
    // Send Flag REGISTER 
    SentEncrytMsg(client, sent_message, encrypt);
    
    cout << setw(70) << "====REGISTER====" << endl;

    //  Input Account
    cout << setw(70) << "Input Account : " << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, user.Account);

    //  Input Password
    InputPassword(client, encrypt,user,1);

    Clean(1);


    EncryptMenu();
    getline(cin, user.Encrypt);


    if (user.Encrypt == "Y")
    {
        encrypt = true;
    }
    
    // Sent mess
    SentEncrytMsg(client, user.Encrypt, false);
    string s;
    s = to_string(user.Account.length()) + user.Account + user.Password;
    SentEncrytMsg(client, s, encrypt);
    
    /*SentEncrytMsg(client, user.Account, encrypt);
    SentEncrytMsg(client, user.Password, encrypt);*/

}

int Change_Password(SOCKET& client, bool& encrypt,User user, bool &firstTime) {

    string sent_message;

    cout << setw(70) << "====CHANGE_PASSWORD====" << endl;

    //  Input Current Password
    cout << setw(70) << "Input Current Password " << endl;
    cout << setw(60) << "=>>  ";
    // Hint pass
    char ch;
    char buf[100] = { 0 };
    //int count = 1;
    int i;
    for (i = 0; i < 100; i++)
    {
        ch = getch();

        if (ch == 13)break;//13=Enter
        if (ch == 27)exit(0);//27=ESC
        if (ch == 8)//Backspace
        {
            i--;
            if (i < 0)continue;
            buf[i--] = 0;
            cprintf("%c", char(8));
            cprintf("%c", char(32));
            cprintf("%c", char(8));
            continue;
        }
        printf("*");
        buf[i] = ch;
    }
    sent_message.resize(i);
    for (int u = 0; u < i; u++)
    {
        sent_message[u] = buf[u];
    }
    

    ////  Input Password
    //InputPassword(client, encrypt, user);

    Clean(2);
    if (firstTime == true)
    {
        // Encrypt
        EncryptMenu();
        getline(cin, user.Encrypt);
        if (user.Encrypt == "Y")
        {
            encrypt = true;
        }
        SentEncrytMsg(client, user.Encrypt, false);       

    }
    SentEncrytMsg(client, sent_message, encrypt);



    if (sent_message == user.Password)
    {
        return 1;
    }
    return 0;


}
void Correct_Password(SOCKET& client, bool encrypt, User &user) {


    //  Input Password
    InputPassword(client, encrypt, user,2);
    
    SentEncrytMsg(client, user.Password, encrypt);
  
}

void Check_User(SOCKET& client, bool encrypt) {
    cout << "Format For User Declarations:" << endl;
    cout << "[-option] [username]" << endl;
    cout << "CHECK USER: ";
    string sent_message;
    getline(cin, sent_message);
    sent_message = "CHECK_USER " + sent_message;
    SentEncrytMsg(client, sent_message, encrypt);
}

void SetUp_Info(SOCKET& client, bool encrypt) {
    cout << "Format For User Declarations:" << endl;
    cout << "[-option] [information]" << endl;
    cout << "SETUP INFO: ";
    string sent_message;
    getline(cin, sent_message);
    sent_message = "SETUP_INFO " + sent_message;
    SentEncrytMsg(client, sent_message, encrypt);
}
// Game Part


void Add_User_Online_List(string s, vector<string>& user) {

    string tmp = "ONLINE_LIST";
    s = s.erase(0, tmp.size());

    tmp = "ENOUGH_USER";
    if (s.find("ENOUGH_USER") != -1)
    {
        s = s.erase(s.size() - tmp.size(), tmp.size());
    }

    user.push_back(s);
}

void Show_User_Online(vector<string>& user, int IDHOST) {
    Clean(0);
    for (int i = 0; i < user.size(); i++)
    {

        cout << setw(50) << "P" << i << ":" << user[i];
        if (i == IDHOST)
        {
            cout << "(It's you)";
        }
        cout << endl;
    }
    cout << setw(50) << "CREATE_ROOM (room_number) (User_ID)" << endl;
    cout << setw(50) << "=>>";
    user.resize(0);
}

void Choose_user_play_with(SOCKET client, string msg, bool encrypt) {
    // Sent to sever the choosen

    SentEncrytMsg(client, msg, encrypt);

    Clean(1);
    cout << setw(70) << "WATING THE P2 RESPONDING ....." << endl;
}

void P1_INVITE_TO_PLAY(string msg) {

    string s = "INVITE";
    // Cut off the flag
    msg = msg.erase(0, s.size());
    s = msg[0];
    int P1_ID = stoi(s);
    msg = msg.erase(0, 1);

    // the mess will like : "P1_name"

    cout << setw(50) << "==THE P" << P1_ID << " : " << msg << " INVITE YOU TO PLAY==" << endl;
    cout << endl;
    cout << setw(90) << "ACCEPT,P(COMPETITOR_ID) / REJECT,P(COMPETITOR_ID) " << endl;
    cout << setw(50) << "=>>";
}

int Get_P2_ID(string msg) {

    string s = "CREATE_ROOM";
    // Cut off the flag CREATE_ROOM
    msg = msg.erase(0, s.size() + 1);

    // Now the mess like : "(room_number) User_ID"
    s = "";
    for (int i = 0; i < msg.rfind(" "); i++)
    {
        s.resize(i + 1);
        s[i] = msg[i];
    }
    msg = msg.erase(0, msg.rfind(" "));


    // Now the mess like : "(room_number) User2_ID"

    return  stoi(msg);

}

int Get_P1_ID(string msg) {

    string s = "ACCEPT,P";
    // Cut off the flag CREATE_ROOM
    msg.erase(0, s.size());

    // Now the mess like : UserP1_ID"

    return  stoi(msg);

}

void CREATE_ROOM(SOCKET client, string sent_message, bool encrypt, int& P2_ID) {
    Clean(1);
    Choose_user_play_with(client, sent_message, encrypt);
    P2_ID = Get_P2_ID(sent_message);
}

void Upload_Map() {

    cout << setw(72) << "=====UPLOAD_MAP=====" << endl;
    cout << endl;
    cout << setw(72) << "(Namefile.txt)      " << endl;
    cout << endl;
    cout << setw(72) << "=>>";
}
// USER MENU

void Login_Register_Menu() {

    Clean(0);

    cout << setw(70) << "<-       Menu     ->" << endl;
    cout << setw(70) << "====================" << endl;
    cout << setw(70) << "1. Login            " << endl;
    cout << setw(70) << "2. Register         " << endl;
    cout << setw(70) << "100. Logout         " << endl;
    cout << setw(70) << "====================" << endl;
    cout << setw(65) << "=>>  ";
    /*cout << setw(70) << "Input you choose....";*/
}

void OpenMenu() {

    // Init user's Menu
    cout << setw(70) << "====User's_Menu=====" << endl;
    cout << setw(70) << "3. Change_password  " << endl;
    cout << setw(70) << "4. Check_user       " << endl;
    cout << setw(70) << "5. Setup_info       " << endl;
    cout << setw(70) << "20. Start_game      " << endl;
    cout << setw(70) << "100. Logout         " << endl;
    cout << setw(70) << "====================" << endl;
    cout << setw(65) << "=>>  ";

}


// GAME PLAY

int randgen(vector<vector<int>>& Ships, int n, string filename)
{
    string s;
    ifstream in(filename, ios::in);
    if (in.is_open() && !in.eof())
    {
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 20; j++)
            {
                getline(in, s, ' ');
                Ships[i][j] = stoi(s);
            }
        }
    }
    in.close();
    return 12;
}

void draw_gui(vector<vector<char>>& Pseudo_gui, vector<vector<int>>& pos, int n)
{

    Pseudo_gui[0][0] = ' ';
    for (int i = 1; i < (n + 1); i++)
    {
        pos[0][i] = i;
        pos[i][0] = i;
    }

    for (int i = 1; i < (n + 1); i++)
    {
        for (int j = 1; j < (n + 1); j++)
        {
            Pseudo_gui[i][j] = '+';
        }
    }
}
int IscheckRight(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui, int i, int j, int count, int n) {
    if ((i >= n || i < 0 || j >= n || j < 0))
    {
        return count;
    }
    if (Ships[i][j] != 1)
    {
        return count;
    }
    // Right = j+1

    return IscheckRight(Ships, Pseudo_gui, i, j + 1, count + 1, n);
}
int IscheckLeft(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui, int i, int j, int count, int n) {
    if ((i >= n || i < 0 || j >= n || j < 0))
    {
        return count;
    }
    if (Ships[i][j] != 1)
    {
        return count;
    }
    // Left = j-1

    return IscheckLeft(Ships, Pseudo_gui, i, j - 1, count + 1, n);
}
int IscheckUp(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui, int i, int j, int count, int n) {
    if ((i >= n || i < 0 || j >= n || j < 0))
    {
        return count;
    }
    if (Ships[i][j] != 1)
    {
        return count;
    }
    // Up = i-1

    return IscheckUp(Ships, Pseudo_gui, i - 1, j, count + 1, n);
}
int IscheckDown(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui, int i, int j, int count, int n) {
    if ((i >= n || i < 0 || j >= n || j < 0))
    {
        return count;
    }
    if (Ships[i][j] != 1)
    {
        return count;
    }
    // Down = i+1
    return IscheckDown(Ships, Pseudo_gui, i + 1, j, count + 1, n);
}

void Ischeck(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui, int i, int j, int n) {
    int R = IscheckRight(Ships, Pseudo_gui, i, j, 0, n) - 1;
    if (R < 0) R = 0;
    int L = IscheckLeft(Ships, Pseudo_gui, i, j, 0, n) - 1;
    if (L < 0) L = 0;
    int U = IscheckUp(Ships, Pseudo_gui, i, j, 0, n) - 1;
    if (U < 0) U = 0;
    int D = IscheckDown(Ships, Pseudo_gui, i, j, 0, n) - 1;
    if (D < 0) D = 0;
    for (int col = i - U; col <= i + D; col++)
    {
        for (int row = j - L; row <= j + R; row++)
        {
            Pseudo_gui[col + 1][row + 1] = 'O';

        }
    }
    return;
}

void battle(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui, int n, coord x, int* count, int* miss)
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
                    Ischeck(Ships, Pseudo_gui, i, j, n);
                    cout << setw(10) << "Hit!" << endl;

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

void result(vector<vector<char>> Pseudo_gui, vector<vector<int>>& pos, int n)
{
    cout << endl;
    cout << setw(10);
    for (int i = 0; i < (n + 1); i++)
    {
        for (int j = 0; j < (n + 1); j++)
        {
            if (i == 0 && j == 0)
            {
                cout << Pseudo_gui[i][j] << "  ";
            }
            else if (i == 0 || j == 0)
            {
                if (j >= 10)
                {
                    cout << pos[i][j] << " ";
                }
                else {
                    cout << pos[i][j] << "  ";
                }
            }
            else {
                cout << Pseudo_gui[i][j] << "  ";
            }
        }
        cout << endl;
        cout << endl;
        cout << setw(10);
    }
}

// INIT GAME

void Init_Game_Play(vector<vector<int>>& Ships, vector<vector<int>>& pos, vector<vector<char>>& Pseudo_gui, int& n, int& shipcount, string filename) {
    n = 20;

    Ships.resize(n);

    for (int i = 0; i < n; i++)
    {
        Ships[i].resize(n);
    }

    Pseudo_gui.resize(n + 1);
    pos.resize(n + 1);

    for (int i = 0; i < (n + 1); i++)
    {
        Pseudo_gui[i].resize(n + 1);
        pos[i].resize(n + 1);
    }

    shipcount = randgen(Ships, n, filename);

}
void Introduce(vector<vector<int>>& pos, vector<vector<char>>& Pseudo_gui, int& n) {
    cout << endl;
    cout << setw(10) << "Number of ships to be sunk: 12" << endl;
    cout << endl;

    draw_gui(Pseudo_gui, pos, n);
    result(Pseudo_gui, pos, n);
}
void Choose_Attack() {
    cout << endl << "ATTACK_SHIP x y";
    cout << " =>>";
}
void Get_Attack_Coords(string msg, coord& coords) {
    string s = "";
    for (int i = 0; i < msg.rfind(" "); i++)
    {
        s.resize(i + 1);
        s[i] = msg[i];
    }
    coords.x = stoi(s);
    msg = msg.erase(0, msg.rfind(" "));
    coords.y = stoi(msg);
}
void Game_Play(vector<vector<int>>& Ships, vector<vector<int>> pos, vector<vector<char>>& Pseudo_gui, int& n, coord coords, int& shipcount, int& x_count) {

    system("cls");

    battle(Ships, Pseudo_gui, n, coords, &shipcount, &x_count);
    result(Pseudo_gui, pos, n);

    cout << setw(10) << "Number of ships to be sunk:" << shipcount << endl;
    cout << endl;

    //}

}

int ReceviedMessage(client_type& new_client)
{
    vector<string> user_online;
    bool Open = true;
    bool encrypt = false;
    // Game play
    vector<vector<int>> Ships;
    vector<vector<int>> pos;
    vector<vector<char>> Pseudo_gui;
    int n;
    int shipcount = 0;
    string filename;
    coord coords;
    int x_count;
    while (1)
    {
        if (Open == false)
        {
            break;
        }
        string msg = RevMsg(new_client.socket);
        int flag = FlagRev(msg);

        switch (flag)
        {
        case 0: // REV A CONTENT
        {
            cout << msg << endl;
        }
        break;
        case 1: // LOGIN_SUCCESS
        {
            Clean(1);
            cout << setw(70) << "===LOGIN_SUCCESS====" << endl;
            Clean(2);

            OpenMenu();
        }
        break;
        case 2: // LOGIN_FAIL
        {
            Clean(1);
            cout << setw(70) << "=====LOGIN_FAIL=====" << endl;
            cout << setw(70) << "Choose next action :" << endl;
            Login_Register_Menu();
        }
        break;
        case 3: // REGISTER_SUCCESS
        {
            Clean(1);
            cout << setw(71) << "===REGISTER_SUCCESS===" << endl;
            cout << setw(71) << "Choose next action :  " << endl;
            Login_Register_Menu();
        }
        break;
        case 4: // REGISTER_FAIL
        {
            Clean(1);
            cout << setw(71) << "=====REGISTER_FAIL====" << endl;
            cout << setw(71) << "The account is exis   " << endl;
            cout << setw(71) << "Choose next action :  " << endl;
            Login_Register_Menu();
        }
        break;
        case 5: // CHANGE_SUCCESS
        {
            Clean(1);

            cout << setw(71) << "====CHANGE_SUCCESS====" << endl;

            OpenMenu();
        }
        break;
        case 6: // CHANGE_FAIL 
        {
            /*Clean(1);

            cout << setw(70) << "======CHANGE_FAIL======" << endl;
            cout << setw(70) << "CURRENT_PASSWORD_WRONG " << endl;

            Clean(2);*/
        }
        break;
        case 7: // CHECK USER
        {
            Clean(1);
            if (msg.find("online") != -1) {
                msg = msg.substr(strlen("CHECK_USER "));
                cout << msg << endl;
            }
            else if (msg.find("offline") != -1) {
                msg = msg.substr(strlen("CHECK_USER "));
                cout << msg << endl;
            }
            else if (msg.find("show_fullname") != -1) {
                msg = msg.substr(strlen("CHECK_USER show_fullname "));
                cout << msg << endl;
            }
            else if (msg.find("show_date") != -1) {
                msg = msg.substr(strlen("CHECK_USER show_date "));
                cout << msg << endl;
            }
            else if (msg.find("show_note") != -1) {
                msg = msg.substr(strlen("CHECK_USER show_note "));
                cout << msg << endl;
            }
            else if (msg.find("show_all") != -1) {
                msg = msg.substr(strlen("CHECK_USER show_all "));
                cout << msg << endl;
            }
            else {
                cout << setw(70) << "======ACCOUNT_IS_EXIST======" << endl;
                cout << endl << endl << endl;
                cout << setw(65) << "WELCOME TO THE GAME" << endl;
            }
            OpenMenu();
        }
        break;
        case 8: // CHECK FORMAT
        {
            Clean(1);
            cout << "You declare information wrong with format!" << endl;
            cout << "You must declare again:" << endl;
            OpenMenu();
        }
        break;
        case 9: // WHEN ACCOUNT USER IS NOT EXIST
        {
            Clean(1);
            cout << setw(70) << "======ACCOUNT_IS_NOT_EXIST======" << endl;
            cout << endl << endl << endl;
            OpenMenu();
        }
        break;
        case 10:
        {
            Clean(1);
            cout << setw(70) << "======SETUP_INFO_SUCCESS======" << endl;
            cout << endl << endl << endl;
            cout << msg << endl;
            OpenMenu();

        }
        break;
        case 11:
        {
            Clean(1);
            cout << setw(70) << "===USER_FAIL_ONLINE===" << endl;
            Clean(2);
            OpenMenu();

        }
        break;
        case 21: // REV USER ONLINE 
        {
            Clean(1);

            Add_User_Online_List(msg, user_online);
        }
        break;
        case 22: // ENOUGH_USER
        {
            Add_User_Online_List(msg, user_online);

            Clean(1);

            cout << setw(70) << "=======USER_LIST=======" << endl;

            Clean(2);
            // Show user online list for client1 choose
            Show_User_Online(user_online, new_client.id);
        }
        break;
        case 23: // INVITE
        {
            Clean(1);

            cout << setw(70) << "===INVITE_PLAY_GAME====" << endl;

            Clean(2);
            // Show invite offer
            P1_INVITE_TO_PLAY(msg);
        }
        break;
        case 24: // COMPERTITOR_ACCEPT
        {
            Clean(1);

            cout << setw(70) << "===COMPERTITOR_ACCEPT===" << endl;

            Clean(2);

            Upload_Map();

        }
        break;
        case 25: // COMPERTITOR_REJECT
        {
            Clean(1);

            cout << setw(70) << "===COMPERTITOR_REJECT===" << endl;
            user_online.resize(0);
            Clean(2);
            // Return MENU
            OpenMenu();
        }
        break;
        case 26: // P2 REV P1's MAP and send P2's MAP to P1
        {
            string flagname = "REV_SEND_MAP";
            filename = msg.erase(0, flagname.size());
            cout << setw(70) << "=WELCOME_TO_BATTLE_SHIP1=" << endl;
            Clean(2);

            Init_Game_Play(Ships, pos, Pseudo_gui, n, shipcount, filename);
            Introduce(pos, Pseudo_gui, n);
            cout << endl;
            Upload_Map();
        }
        break;
        case 27: // P2 REV P1's MAP and show P1's Map
        {
            Clean(1);

            string flagname = "UPLOAD_MAP";
            filename = msg.erase(0, flagname.size());

            cout << setw(70) << "=WELCOME_TO_BATTLE_SHIP2=" << endl;
            Clean(2);

            Init_Game_Play(Ships, pos, Pseudo_gui, n, shipcount, filename);
            Introduce(pos, Pseudo_gui, n);
            Choose_Attack();

        }
        break;
        case 28: // show map of P2 is attacked
        {
            Clean(1);

            string flagname = "ATTACK_SHIP ";
            msg = msg.erase(0, flagname.size());

            Get_Attack_Coords(msg, coords);
            cout << "Get coords" << endl;
            Game_Play(Ships, pos, Pseudo_gui, n, coords, shipcount, x_count);


        }
        break;
        case 29: // P2 Attack back P1 and so on
        {


            Clean(2);
            // When P1 win the game
            if (shipcount <= 10)
            {
                Clean(1);
                msg = "LOSE_GAME";
                SentEncrytMsg(new_client.socket, msg, encrypt);
                cout << setw(70) << "=====You are Champion!====" << endl;
                Clean(2);
                cout << setw(70) << "Keep playing (YES/NO)" << endl;
                Clean(2);
                cout << setw(70) << " =>>";

            }
            else {
                Choose_Attack();
            }

        }
        break;
        case 30: // P2 LOSE_GAME
        {


            Clean(2);
            Clean(1);
            /*msg = "LOSE_GAME";
            SentEncrytMsg(new_client.socket, msg);*/
            cout << setw(70) << "=====You Losed !=====" << endl;
            Clean(2);
            cout << setw(70) << "Keep playing (YES/NO)" << endl;
            Clean(2);
            cout << setw(70) << " =>>";

        }
        break;
        case 31: // ENCRYPT
        {
            string s = "ENCRYPT_";
            msg = msg.erase(0, s.size());
            if (msg == "1")
            {
                encrypt = true;
            }
            else {

                encrypt = false;
            }
        }
        break;
        case 100: // LOGOUT
        {

            Open = false;
        }
        break;
        }
    }

    if (WSAGetLastError() == WSAECONNRESET)
        cout << "The server has disconnected" << endl;

    return 0;
}

void Running() {
    WSAData wsa_data;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    string sent_message = "";
    client_type client = { INVALID_SOCKET, -1, "" };
    int iResult = 0;
    string message;

    cout << "Starting Client...\n";

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (iResult != 0) {
        cout << "WSAStartup() failed with error: " << iResult << endl;
        return;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    cout << "Connecting...\n";

    // Resolve the server address and port
    iResult = getaddrinfo(IP_ADDRESS, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        cout << "getaddrinfo() failed with error: " << iResult << endl;
        WSACleanup();
        system("pause");
        return;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        client.socket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (client.socket == INVALID_SOCKET) {
            cout << "socket() failed with error: " << WSAGetLastError() << endl;
            WSACleanup();
            system("pause");
            return;
        }

        // Connect to server.
        iResult = connect(client.socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(client.socket);
            client.socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (client.socket == INVALID_SOCKET) {
        cout << "Unable to connect to server!" << endl;
        WSACleanup();
        system("pause");
        return;
    }

    cout << "Successfully Connected" << endl;
    cout << "======================" << endl;
    //Obtain id from server for this client;
    recv(client.socket, client.received_message, DEFAULT_BUFLEN, 0);
    message = client.received_message;


    if (message == "Server is full")
    {
        cout << endl << message << endl;
        Shutdown(client.socket);
        return;
    }

    client.id = atoi(client.received_message);


    // Init Thread
    thread my_thread(&ReceviedMessage, ref(client));
    bool Open = true;
    bool wating_send_map = true;
    bool encrypt = false;
    int P2_ID = -1;
    User user;
    
    // Show Login and Register menu
    Login_Register_Menu();
    while (1) {

        if (Open == false)
        {
            break;
        }
        // Setup the Flag
        getline(cin, sent_message);
        int flag;

        if (FlagGameSend(sent_message) > 0)
        {
            flag = FlagGameSend(sent_message);
        }
        else {
            flag = stoi(sent_message);
        }

        switch (flag)
        {
        case 0: // SENT CONTENT
        {

            SentEncrytMsg(client.socket, sent_message, encrypt);
            break;
        }
        case 1: // LOGIN
        {

            Login(client.socket, encrypt,user);
            break;
        }
        case 2: // REGISTER
        {
            Register(client.socket, encrypt);
            break;
        }
        case 3: // CHANGE_PASSWORD
        {
            Clean(1);
            string sent_message = FlagSend(3);
            // Send Flag CHANGE_PASSWORD
            SentEncrytMsg(client.socket, sent_message, encrypt);

            bool firstTime = true;
            int res = 0;
            while (res == 0)
            {
                res = Change_Password(client.socket, encrypt, user, firstTime);
                firstTime = false;
                if (res == 1)
                {
                    Clean(2);
                }
                else {
                    Clean(1);
                    cout << setw(70) << "======CHANGE_FAIL======" << endl;
                    cout << setw(70) << "CURRENT_PASSWORD_WRONG " << endl;
                    Clean(2);
                }
            }
            Clean(1);
            Correct_Password(client.socket, encrypt, user);
            encrypt = false;
            break;
        }
        case 4: // CHECK USER                                         MADE BY D
        {
            Check_User(client.socket, encrypt);
            break;
        }
        case 5:
        {
            SetUp_Info(client.socket, encrypt);
            break;                                                             //Made By D
        }
        case 20: // START_GAME
        {
            Clean(1);
            SentEncrytMsg(client.socket, FlagSend(20), encrypt);
            break;
        }
        case 21: // CREATE ROOM and  CHOOSE USER
        {
            CREATE_ROOM(client.socket, sent_message, encrypt, P2_ID);

            break;
        }
        case 22: // ACCEPT OFFER:  "ACCEPT,P..."
        {
            Clean(1);
            P2_ID = Get_P1_ID(sent_message);

            // FLAG + P2_ID

            SentEncrytMsg(client.socket, sent_message, encrypt);

            /*Upload_Map();*/
            cout << setw(70) << "WAITING COMPETITOR SEND MAP" << endl;
            wating_send_map = false;
            break;
        }
        case 23: // REJECT OFFER:   "REJECT,P... "
        {
            Clean(1);
            P2_ID = Get_P1_ID(sent_message);

            // FLAG + P2_ID

            SentEncrytMsg(client.socket, sent_message, encrypt);
            Clean(1);
            OpenMenu();
            break;
        }
        case 24: // UPLOAD MAP
        {
            // FLAG + filename.txt

            sent_message = "UPLOAD_MAP" + sent_message;

            SentEncrytMsg(client.socket, sent_message, encrypt);
            if (wating_send_map == true)
            {
                cout << setw(70) << "WAITING COMPETITOR SEND MAP" << endl;
            }
            break;
        }
        case 25: // P1 Attack first time
        {

            SentEncrytMsg(client.socket, sent_message, encrypt);
            break;
        }
        case 26: // Play more
        {
            sent_message = "PLAY_MORE" + sent_message;
            SentEncrytMsg(client.socket, sent_message, encrypt);
            break;
        }
        case 27: // Return menu
        {

            OpenMenu();
            break;
        }
        case 100: // LOGOUT
        {
            SentEncrytMsg(client.socket, FlagSend(flag), encrypt);
            my_thread.join();
            Open = false;
            break;

        }
        }

    }
    if (my_thread.joinable())
    {

        my_thread.detach();
    }
    Shutdown(client.socket);
    return;
}

int main()
{
    Running();

    return 0;
}