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
        // k can dung
        client.id = atoi(client.received_message);
        thread my_thread(process_client, ref(client));
        // tu khuc nay
        string n;
        cout << "Choose the action " << endl;
        cout << "1 - Log in" << endl;
        cout << "2 - Sign up" << endl;
        getline(cin, n);
        sent_message = n;
        sent_message = string_to_hex(sent_message);
        iResult = send(client.socket, sent_message.c_str(), strlen(sent_message.c_str()), 0);
        if (n == "1")
        {
            // Fill account and password
            cout << "Fill your account here" << endl;
            getline(cin, sent_message);
            
            //send data
            sent_message = string_to_hex(sent_message);
            iResult = send(client.socket, sent_message.c_str(), strlen(sent_message.c_str()), 0);
            
            //Encryt message before send
            cout << "Fill your password here" << endl;
            getline(cin, sent_message);

            sent_message = string_to_hex(sent_message);
            iResult = send(client.socket, sent_message.c_str(), strlen(sent_message.c_str()), 0);

            // Encrypt
            cout << "Do you want to encrypt message before sending ?" << endl;
            cout << "Y - Encrypt" << endl;
            cout << "N - Not Encrypt" << endl;
            getline(cin, sent_message);
            sent_message.resize(1);
            //Encryt message before send if Y
            /*if (sent_message == "Y")
            {*/
            sent_message = string_to_hex(sent_message);
            /*}*/
            // Send to the sever know user choose Y
            iResult = send(client.socket, sent_message.c_str(), strlen(sent_message.c_str()), 0);
            /*if (iResult <= 0)
            {
                cout << "send() failed: " << WSAGetLastError() << endl;
                break;
            }*/
        }
        // toi day
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