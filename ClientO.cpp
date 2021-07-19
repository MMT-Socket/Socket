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

using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define IP_ADDRESS "192.168.1.6"
#define DEFAULT_PORT "3504"
struct user {
    string account;
    string password;
    string birthdate;
    string fullname;
    string note;
    string encry; //Y or N
};
struct client_type
{
    SOCKET socket;
    int id;
    char received_message[DEFAULT_BUFLEN];
    bool flag = true;
};

int process_client(client_type& new_client);
string string_to_hex(const string& in);
string hex_to_string(const string& in);
int main();

int process_client(client_type& new_client)
{

    while (1)
    {
        memset(new_client.received_message, 0, DEFAULT_BUFLEN);

        if (new_client.socket != 0)
        {
            int iResult = recv(new_client.socket, new_client.received_message, DEFAULT_BUFLEN, 0);
            if (iResult == 0)
            {
                return 0;
            }
            else if (iResult != SOCKET_ERROR)
                cout << new_client.received_message << endl;
            else
            {
                cout << "recv() failed: " << WSAGetLastError() << endl;
                break;
            }
        }
    }

    if (WSAGetLastError() == WSAECONNRESET)
        cout << "The server has disconnected" << endl;

    return 0;
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

int main()
{
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
        return 1;
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
        return 1;
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
            return 1;
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
        return 1;
    }

    cout << "Successfully Connected" << endl;

    //Obtain id from server for this client;
    recv(client.socket, client.received_message, DEFAULT_BUFLEN, 0);
    // message is char following interger
    message = client.received_message;

    if (message != "Server is full")
    {
        client.id = atoi(client.received_message);
        thread my_thread(process_client, ref(client));
        // tu khuc nay
        user info;
        string n;
        cout << "Choose the action " << endl;
        cout << "1 - Log in" << endl;
        cout << "2 - Sign up" << endl;
        getline(cin, n);
        sent_message = n;
        cout << n << endl;
        //send message to server
        sent_message = string_to_hex(sent_message);
        iResult = send(client.socket, sent_message.c_str(), strlen(sent_message.c_str()), 0);
        if (n == "1")
        {
            // Encrypt
            cout << "Do you want to encrypt message before sending ?" << endl;
            cout << "Y - Encrypt" << endl;
            cout << "N - Not Encrypt" << endl;
            getline(cin, info.encry);
            cout << info.encry << endl;
            info.encry.resize(1);
            info.encry = string_to_hex(info.encry);
            iResult = send(client.socket, info.encry.c_str(), strlen(info.encry.c_str()), 0);
            while (1) {
                // Fill account and password
                cout << "Fill your account here" << endl;
                getline(cin, info.account);
                cout << info.account << endl;
                //send message to server
                info.account = string_to_hex(info.account);
                iResult = send(client.socket, info.account.c_str(), strlen(info.account.c_str()), 0);

                //Encryt message before send
                cout << "Fill your password here" << endl;
                getline(cin, info.password);
                cout << info.password << endl;
                //send message to server
                info.password = string_to_hex(info.password);
                iResult = send(client.socket, info.password.c_str(), strlen(info.password.c_str()), 0);

                // Received message from sever to check log in success or not
                iResult = recv(client.socket, client.received_message, DEFAULT_BUFLEN, 0);
                
                if (client.received_message == "Success")
                {
                    cout << client.received_message << endl;
                    cout << "Login successfully or Wellcome legon login to server and Message was encrypted" << endl;
                    break;
                }
                else {
                    cout << client.received_message << endl;
                    break;
                }
                /*iResult = recv(client.socket, client.received_message, DEFAULT_BUFLEN, 0);
                cout << client.received_message << endl;*/
            }

            //Encryt message before send if Y
            /*if (sent_message == "Y")
            {*/
            /*}*/
            //Send to the sever know user choose Y
            /*if (iResult <= 0)
            {
                cout << "send() failed: " << WSAGetLastError() << endl;
                break;
            }*/
        }
        // toi day
        my_thread.detach();
    }
    else
        cout << client.received_message << endl;

    cout << "Shutting down socket..." << endl;
    iResult = shutdown(client.socket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        cout << "shutdown() failed with error: " << WSAGetLastError() << endl;
        closesocket(client.socket);
        WSACleanup();
        system("pause");
        return 1;
    }

    closesocket(client.socket);
    WSACleanup();
    system("pause");
    return 0;
}