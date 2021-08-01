
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstdint>

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
//int ControlFlag() {
//
//}

int FlagRev(string s) {
    if (s == "LOGIN_SUCCESS")
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
string FlagSend(int flag) {

    switch (flag)
    {
    case 1:
        return "LOGIN";
    case 2:
        return "REGISTER";
    case 3:
        return "CHANGE_PASSWORD";
    case 100:
        return "LOGOUT";
    }

    return "";
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

void SentEncrytMsg(SOCKET client,string msg) {

    // Encry before sent
    msg = string_to_hex(msg);
    int iResults = send(client, msg.c_str(), strlen(msg.c_str()),0);
    if (iResults == SOCKET_ERROR)
    {
        cout << "send() failed: " << WSAGetLastError() << endl;
        return ;
    }

    return ;

}

void SentMsg(SOCKET client, string msg) {

    int iResults = send(client, msg.c_str(), strlen(msg.c_str()),0);
    if (iResults == SOCKET_ERROR)
    {
        cout << "send() failed: " << WSAGetLastError() << endl;
        return ;
    }

    return ;

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
        return ;
    }
    cout << setw(70) << "Shutting down socket..." << endl;
    Clean(2);
    cout << setw(69);

    closesocket(client);
    WSACleanup();
    system("pause");
    return ;
}

void Login(SOCKET&client) {

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

void Register(SOCKET &client) {

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
void OpenMenu(SOCKET client) {
    
    // Init user's Menu
    cout << setw(70) << "====User's_Menu=====" << endl;
    cout << setw(70) << "3. Change_password  " << endl;
    cout << setw(70) << "4. Check_user       " << endl;
    cout << setw(70) << "5. Setup_info       " << endl;
    cout << setw(70) << "100. Logout         " << endl;
    cout << setw(70) << "====================" << endl;
    cout << setw(65) << "=>>  ";

}
int ReceviedMessage(client_type& new_client)
{
    bool Open = true;

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
            cout << msg <<endl;
        }
        break;
        case 1: // LOGIN_SUCCESS
        {
            Clean(1);
            cout << setw(70) << "===LOGIN_SUCCESS====" << endl;
            Clean(2);

            OpenMenu(new_client.socket);
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

            cout << setw(70) << "====CHANGE_SUCCESS====" << endl;

            OpenMenu(new_client.socket);
        }
        break;
        case 6: // CHANGE_FAIL 
        {
            Clean(1);

            cout << setw(70) << "======CHANGE_FAIL======" << endl;
            cout << setw(70) << "CURRENT_PASSWORD_WRONG " << endl;

            Clean(2);

            OpenMenu(new_client.socket);
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
        cout<<endl << message << endl;
        Shutdown(client.socket);
        return;
    }

    client.id = atoi(client.received_message);
    
    // Show Login and Register menu
    Login_Register_Menu();

    // Init Thread
    thread my_thread(&ReceviedMessage, ref(client));
    bool Open = true;
    while (1) {

        if (Open == false)
        {
            break;
        }
        // Setup the Flag
        getline(cin, sent_message);
        int flag = stoi(sent_message);

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
