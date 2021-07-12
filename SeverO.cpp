
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cstdint>

using namespace std;
#pragma comment (lib, "Ws2_32.lib")

#define IP_ADDRESS "192.168.1.6"
#define DEFAULT_PORT "3504"
#define DEFAULT_BUFLEN 512
struct user {
    string account;
    string password;
    string birthdate;
    string fullname;
    string note;
};
struct client_type
{
    int id;
    SOCKET socket;
};

const char OPTION_VALUE = 1;
const int MAX_CLIENTS = 5;

//Function Prototypes
int process_client(client_type& new_client, std::vector<client_type>& client_array, std::thread& thread);
string string_to_hex(const string& in);
string hex_to_string(const string& in);
int main();

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

int process_client(client_type& new_client, std::vector<client_type>& client_array, std::thread& thread)
{
    std::string msg = "";
    char tempmsg[DEFAULT_BUFLEN] = "";

    //Session
    int i = 0;
    while (i<1)
    {
        memset(tempmsg, 0, DEFAULT_BUFLEN);
        
        if (new_client.socket != 0)
        {
            // tu day
            user info;
            int iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);
            
            if (hex_to_string(tempmsg) == "1")
            {
                iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);             
                info.account = hex_to_string(tempmsg);
                

                iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);
                info.password = hex_to_string(tempmsg);
                cout << info.password << endl;
                /*int iResult = recv(new_client.socket, tempmsg, DEFAULT_BUFLEN, 0);*/
                
            }
            /*if (hex_to_string(tempmsg)=="end")
            {
                cout << "client want to dis" << endl;
                iResult = 0;
            }*/
            /*cout << iResult << endl;*/
            //if (iResult == 0 || hex_to_string(tempmsg) == "end" || hex_to_string(tempmsg) == "close")
            //{
            //    msg = "Client #" + std::to_string(new_client.id) + " Disconnected";

            //    std::cout << msg << std::endl;

            //    closesocket(new_client.socket);
            //    closesocket(client_array[new_client.id].socket);
            //    client_array[new_client.id].socket = INVALID_SOCKET;

            //    //Broadcast the disconnection message to the other clients
            //    for (int i = 0; i < MAX_CLIENTS; i++)
            //    {
            //        if (client_array[i].socket != INVALID_SOCKET)
            //            iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
            //    }

            //    break;
            //}
            //else if (iResult != SOCKET_ERROR)
            //{
            //    if (strcmp("", tempmsg))
            //        msg = "Client #" + std::to_string(new_client.id) + ": " + hex_to_string(tempmsg);

            //    std::cout << msg.c_str() << std::endl; 
            //   
            //    //Broadcast that message to the other clients
            //    for (int i = 0; i < MAX_CLIENTS; i++)
            //    {
            //        if (client_array[i].socket != INVALID_SOCKET)
            //            if (new_client.id != i)
            //                iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
            //    }
            //}
            //else
            //{
            //    msg = "Client #" + std::to_string(new_client.id) + " Disconnected";

            //    std::cout << msg << std::endl;

            //    closesocket(new_client.socket);
            //    closesocket(client_array[new_client.id].socket);
            //    client_array[new_client.id].socket = INVALID_SOCKET;

            //    //Broadcast the disconnection message to the other clients
            //    for (int i = 0; i < MAX_CLIENTS; i++)
            //    {
            //        if (client_array[i].socket != INVALID_SOCKET)
            //            iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
            //    }

            //    break;
            //}
            // chuyen no het vo file
            //toi day
        }
        i++;
    } //end while

    thread.detach();

    return 0;
}

int main()
{
    WSADATA wsaData;
    struct addrinfo hints;
    struct addrinfo* server = NULL;
    SOCKET server_socket = INVALID_SOCKET;
    std::string msg = "";
    std::vector<client_type> client(MAX_CLIENTS);
    int num_clients = 0;
    int temp_id = -1;
    std::thread my_thread[MAX_CLIENTS];

    //Initialize Winsock
    std::cout << "Intializing Winsock..." << std::endl;
    int value = WSAStartup(MAKEWORD(2, 2), &wsaData);

    //Setup hints
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    //Setup Server
    std::cout << "Setting up server..." << std::endl;
    getaddrinfo(IP_ADDRESS, DEFAULT_PORT, &hints, &server);

    //Create a listening socket for connecting to server
    std::cout << "Creating server socket..." << std::endl;
    server_socket = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

    //Setup socket options
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &OPTION_VALUE, sizeof(int)); //Make it possible to re-bind to a port that was used within the last 2 minutes
    setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, &OPTION_VALUE, sizeof(int)); //Used for interactive programs

    //Assign an address to the server socket.
    std::cout << "Binding socket..." << std::endl;
    bind(server_socket, server->ai_addr, (int)server->ai_addrlen);

    //Listen for incoming connections.
    std::cout << "Listening..." << std::endl;
    listen(server_socket, SOMAXCONN);

    //Initialize the client list
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        client[i] = { -1, INVALID_SOCKET };
    }

    while (1)
    {
        SOCKET incoming = INVALID_SOCKET;
        incoming = accept(server_socket, NULL, NULL);

        if (incoming == INVALID_SOCKET) continue;

        //Reset the number of clients
        num_clients = -1;

        //Create a temporary id for the next client
        temp_id = -1;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (client[i].socket == INVALID_SOCKET && temp_id == -1)
            {
                client[i].socket = incoming;
                client[i].id = i;
                temp_id = i;
            }

            if (client[i].socket != INVALID_SOCKET)
                num_clients++;

            //std::cout << client[i].socket << std::endl;
        }

        if (temp_id != -1)
        {
            
            //Send the id to that client
            std::cout << "Client #" << client[temp_id].id << " Accepted" << std::endl;
            msg = std::to_string(client[temp_id].id);
            
            send(client[temp_id].socket, msg.c_str(), strlen(msg.c_str()), 0);

            //Create a thread process for that client
            my_thread[temp_id] = std::thread(process_client, std::ref(client[temp_id]), std::ref(client), std::ref(my_thread[temp_id]));
            /*int running = process_client(client[temp_id], client);*/
        }
        else
        {
            msg = "Server is full";
            send(incoming, msg.c_str(), strlen(msg.c_str()), 0);
            std::cout << msg << std::endl;
        }
    } //end while


    //Close listening socket
    closesocket(server_socket);

    //Close client socket
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        my_thread[i].detach();
        closesocket(client[i].socket);
    }

    //Clean up Winsock
    WSACleanup();
    std::cout << "Program has ended successfully" << std::endl;

    system("pause");
    return 0;
}