
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
#include <string>
#include <fstream>


using namespace std;
#pragma comment (lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

#define IP_ADDRESS "192.168.1.7"
#define DEFAULT_PORT "3504"
#define DEFAULT_BUFLEN 512

#define USER_INFO "User_Info.txt"
#define USER_ONLINE "User_Online.txt"


struct client_type
{
    int id;
    SOCKET socket;
};


struct User {

    string Account;
    string Password;
    string Fullname = "";
    string Date = "";
    string Point = "";
    int Online_ID = -1;
    int P2_ID = -1;
};

int FlagRev(string s) {

    if (s == "LOGIN")
    {
        return 1;
    }
    else if (s == "REGISTER")
    {
        return 2;
    }
    else if (s == "CHANGE_PASSWORD")
    {
        return 3;
    }
    else if (s == "START_GAME")
    {
        return 20;
    }
    else if (s.find("CREATE_ROOM") != -1)
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
    else if (s.find("REJECT,P") != -1)
    {
        return 23;
    }
    else if (s.find("UPLOAD_MAP") != -1)
    {
        return 24; 
    }
    else if (s.find("REV_SEND_MAP") != -1)
    {
        return 25; 
    }
    //25
    else if (s == "LOGOUT")
    {
        return 100;
    }
    return 0;

}

string FlagSend(int flag) {
    // Sign and user's info part
    switch (flag)
    {
    case 1:
        return "LOGIN_SUCCESS";
    case 2:
        return "LOGIN_FAIL";
    case 3:
        return "REGISTER_SUCCESS";
    case 4:
        return "REGISTER_FAIL";
    case 5:
        return "CHANGE_SUCCESS";
    case 6:
        return "CHANGE_FAIL";


    // Game part
    case 7:
        return "ENOUGH_USER";
    case 100:
        return "LOGOUT";
    }
    return "";
}

string FLag_Game_Sent(int flag) {
    switch (flag)
    {
    case 1: // Online user
        return "ONLINE_LIST";
    case 2: // Invite
        return "INVITE";
    case 3: // Accept play game
        return "COMPERTITOR_ACCEPT";
    case 4: // Reject play game
        return "COMPERTITOR_REJECT";
    case 5: // Send map to compertitor and request to get back map
        return "REV_SEND_MAP";
    case 6: // Send map to compertitor
        return "UPLOAD_MAP";
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


const char OPTION_VALUE = 1;
const int MAX_CLIENTS = 5;

int SentEncrytMsg(SOCKET client, string msg) {

    // Encry before sent
    msg = string_to_hex(msg);
    int iResults = send(client, msg.c_str(), strlen(msg.c_str()), 0);
    if (iResults == SOCKET_ERROR)
    {
        cout << "send() failed: " << WSAGetLastError() << endl;
        return -1;
    }

    return 1;

}

int SentMsg(SOCKET client, string msg) {

    int iResults = send(client, msg.c_str(), strlen(msg.c_str()), 0);
    if (iResults == SOCKET_ERROR)
    {
        cout << "send() failed: " << WSAGetLastError() << endl;
        return -1;
    }

    return 1;

}
string RevEncrytMsg(SOCKET client) {

    char tempmsg[DEFAULT_BUFLEN] = "";
    memset(tempmsg, 0, DEFAULT_BUFLEN);
    int iResults = recv(client, tempmsg, DEFAULT_BUFLEN, 0);

    return hex_to_string(tempmsg);

}
void Write_User_Info_To_Database(vector<User> Database) {

    // Write down file with syntax:     Account-Password-Fullname-Date-Point.
    ofstream out(USER_INFO, ios::out);
    if (out.is_open())
    {
        out._Seekbeg();
        for (int i = 0; i < Database.size(); i++)
        {
            out << Database[i].Account << "-" << Database[i].Password << "-" << Database[i].Fullname << "-" << Database[i].Date << "-" << Database[i].Point;
            if (i!=Database.size()-1)
            {
                out << endl;
            }
        }
        out._Seekbeg();
    }
    out.close();

}

// SIGN AND INFO PART
void Upload_Database(vector<User>& Database) {

    string s;
    User user;
    ifstream in(USER_INFO, ios::in);
    while (in.is_open() && !in.eof())
    {
        getline(in, s, '-');
        user.Account = s;

        getline(in, s, '-');
        user.Password = s;

        getline(in, s, '-');
        user.Fullname = s;

        getline(in, s, '-');
        user.Date = s;

        getline(in, s);
        user.Point = s;
        if (user.Account != "" && user.Password != "")
        {

            Database.push_back(user);
        }
    }
    in.close();

}

int Verify_Login(vector<User> Database,User user, bool flag) {

    string s;
    int countRegister = 0;
    int i = 0;
    
    while (  i <= (Database.size()-1) && Database.size() != 0)
    {

        int countLogin = 0;

        if (user.Account == Database[i].Account)
        {
            ++countRegister;
            ++countLogin;
        }
        
        if (user.Password == Database[i].Password) 
            ++countLogin;

        // LOGIN SUCCESS
        if (countLogin == 2 && flag == true ) {
            return 1;
        }
        //RIGISTER_FAIL
        else if (countRegister > 0 && flag == false) {

            return 4;
        }
        i++;
    }
    //LOGIN_FAIL
    if (flag == true)
    {
        return 2;
    }
    //RIGISTER_SUCCESS
    return 3;
}

int Change_Info(vector<User>&  Database,User user, int flag) {

    string s;
    
    switch (flag) {
    case 1: // CHANGE_PASSWORD
    {
        int i = 0;
        while (i < Database.size() - 1)
        {
            if (user.Account == Database[i].Account)
            {

                Database[i].Password = user.Password;
            }
            i++;
        }
        break;
    }

    }

    //RIGISTER_SUCCESS
    return 1;
}
void Login(SOCKET client,vector<User>& Database,User& user, bool& success) {

    
    string msg;

    // Rev Account
    user.Account = RevEncrytMsg(client);
    cout << user.Account<<endl;

    // Rev Password
    user.Password = RevEncrytMsg(client);
    cout << user.Password << endl;

    // Get Flag to respond to sever
    int flag = Verify_Login(Database,user,true);
    
    switch (flag)
    {
    case 1: // msg = "LOGIN_SUCCESS"
    {
        msg = FlagSend(flag);
        int Result = SentMsg(client, msg);
        
    }
        break;
    case 2: // msg = "LOGIN_FAIL"
    {
        msg = FlagSend(flag);
        int Result = SentMsg(client, msg);
        success == false;
    }
        break;
    }
}

void Register(SOCKET client, vector<User>& Database) {

    string msg;
    User user;

    // Rev Account
    user.Account = RevEncrytMsg(client);
    cout << user.Account << endl;

    // Rev Password
    user.Password = RevEncrytMsg(client);
    cout << user.Password << endl;

    // Get Flag to respond to sever
    int flag = Verify_Login(Database,user, false);
    cout << flag << endl;
    switch (flag)
    {
    case 3: // msg = "REGISTER_SUCCESS"
    {
        // Update user into database
        cout << "Up_Load_Database" << endl;
        Database.push_back(user);
        msg = FlagSend(flag);
        int Result = SentMsg(client, msg);
    }
    break;
    case 4: // msg = "REGISTER_FAIL"
    {
        msg = FlagSend(flag);
        int Result = SentMsg(client, msg);
    }
    break;
    }

}

void Change_Password(SOCKET client,vector<User>& Database, User& user) {

    string msg;
    bool change = true;
    // Rev current Password 
    msg = RevEncrytMsg(client);
    if (msg != user.Password)
    {
        change = false;
    }

    // Rev Password
    user.Password = RevEncrytMsg(client);
    cout << user.Password << endl;

    // Update Database
    if (change == true)
    {
        // Update Database
        int flag = Change_Info(Database, user, 1);

        // Sent Flag CHANE_SUCCESSS to sever
        msg = FlagSend(5);
        int Result = SentMsg(client, msg);
    }
    else {

        // Sent Flag CHANE_FAIL to sever
        msg = FlagSend(6);
        int Result = SentMsg(client, msg);
    }

      
}

// ONLINE USER PART
// Write online user to file to manager
void Add_User_Online_File(vector<User>& User_Online, User user,int id) {
    
    User_Online.push_back(user);

    ofstream out(USER_ONLINE, ios::out);
    
    if (out.is_open() )
    {
        out._Seekbeg();
        // Format file USER_ONLINE.txt : Username-ONLINE_ID
        for (int i = 0; i < User_Online.size(); i++)
        {
            if (i == User_Online.size() - 1)
            {
                User_Online[i].Online_ID = id;
            }

            out << User_Online[i].Account << "/" << User_Online[i].Online_ID << "/" << User_Online[i].P2_ID;

            if (i!= User_Online.size()-1)
            {
                out << endl;
            }
        }
        out._Seekbeg();
        
    }
    out.close();
}
// Remove offline user and Update file
void Remove_Update_Online(vector<User>& User_Online, User user) {
    
    int i = 0;
    while (i < User_Online.size())
    {
        if (User_Online[i].Account == user.Account)
        {
            User_Online.erase(User_Online.begin() + i);
        }
    }

    ofstream out(USER_ONLINE, ios::out);

    if (out.is_open())
    {
        out._Seekbeg();
        for (int i = 0; i < User_Online.size(); i++)
        {
            out << User_Online[i].Account << "/" << User_Online[i].Online_ID << "/" << User_Online[i].P2_ID;
            if (i != User_Online.size() - 1)
            {
                out << endl;
            }
        }
        out._Seekbeg();

    }
    out.close();

}

// Read online user from file 
void Collect_Online_List(vector<User>& User_Online) {
    
    string s;
    User user;
    vector<User> tmp;

    ifstream in(USER_ONLINE, ios::in);

    while (in.is_open() && !in.eof())
    {

        getline(in, s,'/');
        user.Account = s;
        cout <<"user: "<< s << endl;

        getline(in, s,'/');
        user.Online_ID = stoi(s);
        cout << "id: " << stoi(s) << endl;

        getline(in, s);
        user.P2_ID = stoi(s);
        tmp.push_back(user);

    }
    in.close();
    User_Online = tmp;

}

// Send User online list to Client order 
void Send_Online_User(SOCKET client,vector<User> user) {


    for (int i = 0; i < user.size() ; i++)
    {
        // Sent all user online list 
        string s = FLag_Game_Sent(1);
        s = s + user[i].Account;
        cout << s << endl;

        if (i ==  user.size()-1)
        {
            cout << "send flag enough" << endl;
            s = s + FlagSend(7);
            int res = SentMsg(client, s);
        }
        else {
            int res = SentMsg(client, s);
        }
    }

}
// Send invite to second player 
void Rev_Responding_to_user_is_choosen(SOCKET client, string msg,vector<client_type>& client_array,int& P2_ID,int P1_ID, User user ) {

    string s = "CREATE_ROOM";
    cout <<"nguyen van :" << msg << endl;
    // Cut off the flag CREATE_ROOM
    msg.erase(0, s.size() + 1);
    cout << "sau khi xoa flag :" << msg << endl;
    // Now the mess like : "(room_number) User_ID"
    s = "";
    for (int i = 0; i < msg.rfind(" "); i++)
    {
        s.resize(i + 1);
        s[i] = msg[i];
    }
    int room_number = stoi(s);

    cout << "so phong" <<room_number<< endl;
    msg.erase(0, msg.rfind(" "));
    cout << "sau khi lay so phong" << msg << endl;

    // Now the mess like : "(room_number) User_ID"
    cout << "lay so phong :" << msg << endl;
    P2_ID = stoi(msg);

    // INVITE P2 to play game with P1
    msg = FLag_Game_Sent(2) + to_string(P1_ID) + user.Account;
    int RES = SentMsg(client_array[P2_ID].socket, msg);
}

void Update_UserOnlineList(vector<User>& OnlineUser,int P2_ID,int P1_ID) {
    for (int i = 0; i < OnlineUser.size(); i++)
    {
        if (OnlineUser[i].Online_ID == P1_ID)
        {
            OnlineUser[i].P2_ID = P2_ID;
        }
        else if(OnlineUser[i].Online_ID == P2_ID)
        {
            OnlineUser[i].P2_ID = P1_ID;
        }
    }
}

void Get_P2_ID(string msg,int& P2_ID,string flag) {
    // FLAG + P2_ID
    string s;
    msg = msg.erase(0, flag.size() );
    P2_ID = stoi(msg);
}

void ShutdownClient(SOCKET& client, vector<client_type>& client_array,int id) {

    // Send Flag LOG
    int Result = SentMsg(client, FlagSend(100));

    // shut down client
    cout << "Shut down client" << endl;
    closesocket(client);
    closesocket(client_array[id].socket);
    client_array[id].socket = INVALID_SOCKET;

}
    
int process_client(client_type& new_client, std::vector<client_type>& client_array, std::thread& thread)
{
    std::string msg = "";
    bool Open = true;
    User user;
    cout << "Up_Load_Database" << endl;
    // Upload_Database 
    vector<User> Database;
    vector<User> UserOnline;
    Upload_Database(Database);

    // Game Part
    int P1_ID = new_client.id;
    int P2_ID = -1;

    //Session 
    cout << "client ID" << new_client.id << endl;
    while (1)
    {
        if (Open == false)
        {
            break;
        }
        // Have Encry
        msg = RevEncrytMsg(new_client.socket);
        cout <<"Client sent :"<< msg << endl;
        
        int flag = FlagRev(msg);

        /*if (flag == 24)
        {
            if (UserOnline[P1_ID].P2_ID != -1 && UserOnline[P2_ID].P2_ID != -1)
            {
                flag = 25;
            }
        }*/
        cout << flag << endl;
        switch (flag)
        {
 
        case 1: //Login
        {
            bool success = true;
            Login(new_client.socket,Database,user,success);

            if (success)
            {
                cout << "Prepare Userlist" << endl;
                Collect_Online_List(UserOnline);

                cout << "Add new ogin user" << endl;
                Add_User_Online_File(UserOnline,user,new_client.id);

            }
            break;
        }
        case 2: // REGISTER
        {
            Register(new_client.socket,Database);
            break;
        }
        case 3: // CHANGE_PASSWORD
            Change_Password(new_client.socket, Database, user);
            break;
        case 20: // START_GAME
        {

            cout << "Prepare Userlist again" << endl;
            Collect_Online_List(UserOnline);

            cout << "Starting send messs" << endl;
            Send_Online_User(new_client.socket, UserOnline);

            break;
        }
        case 21: // CREATE_ROOM
        {
            cout << "Starting send messs" << endl;
            Rev_Responding_to_user_is_choosen(new_client.socket, msg, client_array, P2_ID,P1_ID,user);


            break;
        }
        case 22: // P2 accept P1
        {
            // CREATE CONNECTION BETWEEN P1 AND P2
            /*cout << "P1 Accesss P2" << endl;
            Update_UserOnlineList(UserOnline, P2_ID, P1_ID);*/

            Get_P2_ID(msg, P2_ID, "ACCEPT,P");
            msg = "COMPERTITOR_ACCEPT";

            int Result = SentMsg(client_array[P2_ID].socket, msg);

            break;
        }
        case 23: // P2 reject P1
        {
            Get_P2_ID(msg,P2_ID,"REJECT,P");
            msg = "COMPERTITOR_REJECT";

            cout << "P2 Reject to play" << endl;

            int Result = SentMsg(client_array[P2_ID].socket, msg);
            break;
        }
        case 24: // Send Map and want P2 take back Map : REV_SEND_MAP
        {
            // FLAG + filename.txt

            string flag_now = "UPLOAD_MAP";

            string flag_send = "REV_SEND_MAP";
            
            msg = msg.erase(0, flag_now.size());

            msg = flag_send + msg;

            cout << "Send Map and want P2 take back Map" << endl;

            int Result = SentMsg(client_array[P2_ID].socket, msg);

            Update_UserOnlineList(UserOnline, P2_ID, P1_ID);
            break;
        }
        case 25: // Send Map and start game : UPLOAD_MAP
        {
            // FLAG + filename.txt

            cout << "Send Map and start game" << endl;

            int Result = SentMsg(client_array[P2_ID].socket, msg);
            break;
        }
        case 100://  LOGOUT  : Shut down client
        {
            // Updata Database before shutdow
            Write_User_Info_To_Database(Database);
            
            // Remove User online before shutdown
            Remove_Update_Online(UserOnline, user);

            int Result = SentMsg(new_client.socket, FlagSend(100));

            msg = "Client #" + std::to_string(new_client.id) + " Disconnected";

            std::cout << msg << std::endl;

            closesocket(new_client.socket);
            closesocket(client_array[new_client.id].socket);
            client_array[new_client.id].socket = INVALID_SOCKET;

            //Broadcast the disconnection message to the other clients
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_array[i].socket != INVALID_SOCKET)
                    int iResult = send(client_array[i].socket, msg.c_str(), strlen(msg.c_str()), 0);
            }
            Open = false;
            break;
        }   
        }
        
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
    int u = WSAStartup(MAKEWORD(2, 2), &wsaData);

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