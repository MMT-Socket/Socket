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
#include "vector"
#include <fstream>

using namespace std;

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

struct coord
{
    int y;
    int x;

}coords;

int FlagRev(string s) {
    // Game part
    // Take User online list 
    if (s.find("ENOUGH_USER") != -1 )
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
    if (s.find( "CREATE_ROOM") != -1)
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

void SentEncrytMsg(SOCKET client, string msg) {

    // Encry before sent
    msg = string_to_hex(msg);
    int iResults = send(client, msg.c_str(), strlen(msg.c_str()), 0);
    if (iResults == SOCKET_ERROR)
    {
        cout << "send() failed: " << WSAGetLastError() << endl;
        return;
    }

    return;

}

void SentMsg(SOCKET client, string msg) {

    int iResults = send(client, msg.c_str(), strlen(msg.c_str()), 0);
    if (iResults == SOCKET_ERROR)
    {
        cout << "send() failed: " << WSAGetLastError() << endl;
        return;
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

void Login(SOCKET& client) {

    Clean(1);

    string sent_message = FlagSend(1);
    // Send Flag LOGIN 
    SentEncrytMsg(client, sent_message);
    cout << setw(70) << "=====LOGIN=====" << endl;

    //  Input Account
    cout << setw(70) << "Input Account :" << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, sent_message);
    SentEncrytMsg(client, sent_message);

    //  Input Password
    cout << setw(70) << "Input Password:" << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, sent_message);
    SentEncrytMsg(client, sent_message);

}

void Register(SOCKET& client) {

    Clean(1);

    string sent_message = FlagSend(2);
    // Send Flag REGISTER 
    SentEncrytMsg(client, sent_message);
    cout << setw(70) << "====REGISTER====" << endl;

    //  Input Account
    cout << setw(70) << "Input Account : " << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, sent_message);
    SentEncrytMsg(client, sent_message);

    //  Input Password
    cout << setw(70) << "Input Password :" << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, sent_message);
    SentEncrytMsg(client, sent_message);

}

void Change_Password(SOCKET& client) {

    string sent_message = FlagSend(3);
    // Send Flag CHANGE_PASSWORD
    SentEncrytMsg(client, sent_message);
    cout << setw(70) << "====CHANGE_PASSWORD====" << endl;

    //  Input Current Password
    cout << setw(70) << "Input Current Password " << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, sent_message);
    SentEncrytMsg(client, sent_message);

    //  Input Password
    cout << setw(70) << "Input New Password     " << endl;
    cout << setw(60) << "=>>  ";
    getline(cin, sent_message);
    SentEncrytMsg(client, sent_message);
}

// Game Part
void Add_User_Online_List(string s, vector<string>& user) {

    string tmp = "ONLINE_LIST";
    s = s.erase(0,tmp.size());

    tmp = "ENOUGH_USER";
    if (s.find("ENOUGH_USER") != -1)
    {
        s = s.erase(s.size() - tmp.size(), tmp.size());
    }
    
    user.push_back(s);   
}

void Show_User_Online(vector<string>& user,int IDHOST) {
    Clean(0);
    for (int i = 0; i < user.size(); i++)
    {
        
        cout << setw(50) << "P" << i  << ":" << user[i];
        if (i == IDHOST)
        {
            cout << "(It's you)";
        }
        cout<< endl;
    }
    cout << setw(50) << "CREATE_ROOM (room_number) (User_ID)" << endl;
    cout << setw(50) <<"=>>";
}

void Choose_user_play_with(SOCKET client,string msg) {
    // Sent to sever the choosen
    
    SentEncrytMsg(client, msg);

    Clean(1);
    cout << setw(70) << "WATING THE P2 RESPONDING ....." << endl;
}

void P1_INVITE_TO_PLAY( string msg) {

    string s = "INVITE";
    // Cut off the flag
    msg = msg.erase(0,s.size());
    s = msg[0];
    int P1_ID = stoi(s);
    msg = msg.erase(0,1);

    // the mess will like : "P1_name"

    cout << setw(50) << "==THE P"<<P1_ID<<" : " << msg << " INVITE YOU TO PLAY==" << endl;
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
    msg.erase(0, s.size() );

    // Now the mess like : UserP1_ID"

    return  stoi(msg);

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
    cout << "R: " << R;
    int L = IscheckLeft(Ships, Pseudo_gui, i, j, 0, n) - 1;
    if (L < 0) L = 0;
    cout << "L: " << L;
    int U = IscheckUp(Ships, Pseudo_gui, i, j, 0, n) - 1;
    if (U < 0) U = 0;
    cout << "U: " << U;
    int D = IscheckDown(Ships, Pseudo_gui, i, j, 0, n) - 1;
    if (D < 0) D = 0;
    cout << "D: " << D;
    for (int col = i - U; col <= i + D; col++)
    {
        for (int row = j - L; row <= j + R; row++)
        {
            Pseudo_gui[col + 1][row + 1] = 'O';

        }
    }
    cout << "===========" << endl;
    for (int i = 0; i < 21; i++)
    {
        for (int j = 0; j < 21; j++)
        {
            cout << Pseudo_gui[i][j] << " ";
        }
        cout << endl;
    }
    return;
}

void battle(vector<vector<int>>& Ships, vector<vector<char>>& Pseudo_gui, int n, struct coord x, int* count, int* miss)
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
void Introduce(vector<vector<int>> pos, vector<vector<char>>& Pseudo_gui, int& n) {
    cout << endl;
    cout << setw(10) << "Number of ships to be sunk: 12"  << endl;
    cout << endl;

    draw_gui(Pseudo_gui, pos, n);
    result(Pseudo_gui, pos, n);
}

void Game_Play(vector<vector<int>>& Ships, vector<vector<int>> pos, vector<vector<char>>& Pseudo_gui, int& n, int& shipcount, int& x_count) {


    while (shipcount != 0)
    {

        cout << endl << "Attack at (x,y):";
        cout << " =>>";
        cin >> coords.x;
        cin >> coords.y;

        system("cls");

        battle(Ships, Pseudo_gui, n, coords, &shipcount, &x_count);
        result(Pseudo_gui, pos, n);

        cout << setw(10) << "Number of ships to be sunk:" << shipcount << endl;
        cout << endl;

    }
    if (shipcount == 0)
    {
        cout << endl << "Winner!";
        system("pause");
    }

}

int ReceviedMessage(client_type& new_client)
{
    vector<string> user_online;
    bool Open = true;
    
    // Game play
    vector<vector<int>> Ships;
    vector<vector<int>> pos;
    vector<vector<char>> Pseudo_gui; 
    int n; 
    int shipcount; 
    string filename;


    while (1)
    {
        if (Open == false)
        {
            break;
        }
        string msg = RevMsg(new_client.socket);
        int flag = FlagRev(msg);
        cout << msg << endl;
        cout << flag << endl;
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
            Clean(1);

            cout << setw(70) << "======CHANGE_FAIL======" << endl;
            cout << setw(70) << "CURRENT_PASSWORD_WRONG " << endl;

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
            filename = msg.erase(flagname.size());

            
            Upload_Map();
        }
        break;
        case 27: // P2 REV P1's MAP and show P1's MAp
        {
            Clean(1);

            string flagname = "UPLOAD_MAP";
            filename = msg.erase(flagname.size());

            cout << setw(70) << "=WELCOME_TO_BATTLE_SHIP=" << endl;
            Clean(2);

            Init_Game_Play(Ships, pos, Pseudo_gui, n, shipcount, filename);
            Introduce(pos, Pseudo_gui, n);


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

    // Show Login and Register menu
    Login_Register_Menu();

    // Init Thread
    thread my_thread(&ReceviedMessage, ref(client));
    bool Open = true;
    bool second_send_map = false;

    int P2_ID = -1;
    while (1) {

        if (Open == false)
        {
            break;
        }
        // Setup the Flag
        getline(cin, sent_message);
        int flag ;
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

            SentEncrytMsg(client.socket, sent_message);
            break;
        }
        case 1: // LOGIN
        {

            Login(client.socket);
            break;
        }
        case 2: // REGISTER
        {
            Register(client.socket);
            break;
        }
        case 3: // CHANGE_PASSWORD
        {
            Clean(1);
            Change_Password(client.socket);
            break;
        }
        case 20: // START_GAME
        {
            Clean(1);
            SentEncrytMsg(client.socket, FlagSend(20));
            break;
        }
        case 21: // CREATE ROOM and  CHOOSE USER
        {
            Clean(1);
            Choose_user_play_with(client.socket, sent_message);
            P2_ID = Get_P2_ID(sent_message); 

            break;
        }
        case 22: // ACCEPT OFFER:  "ACCEPT,P..."
        { 
            Clean(1);
            P2_ID = Get_P1_ID(sent_message);
            
            // FLAG + P2_ID
            
            SentEncrytMsg(client.socket, sent_message);

            /*Upload_Map();*/
            cout << setw(70) << "WAITING COMPETITOR SEND MAP" << endl;
            break;
        }
        case 23: // REJECT OFFER:   "REJECT,P... "
        {
            Clean(1);
            P2_ID = Get_P1_ID(sent_message);

            // FLAG + P2_ID
            
            SentEncrytMsg(client.socket, sent_message);
            Clean(1);
            OpenMenu();
            break;
        }
        case 24: // UPLOAD MAP
        {
            // FLAG + filename.txt

            sent_message =  "UPLOAD_MAP" + sent_message;

            SentEncrytMsg(client.socket, sent_message);

            if (second_send_map == false)
            {
                cout << setw(70) << "WAITING COMPETITOR SEND MAP" << endl;
                second_send_map = true;
            }
            break;
        }
        
        case 100: // LOGOUT
        {
            SentEncrytMsg(client.socket, FlagSend(flag));
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
