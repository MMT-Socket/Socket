
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
	string Note = "";
	string Point = "";
	int online = 0;
	int Online_ID = -1;
	int Map_Status = 0;
};

const char OPTION_VALUE = 1;
const int MAX_CLIENTS = 5;

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
	else if (s.find("CHECK_USER") != -1) { // Fix by D
		return 4;
	}
	else if (s.find("SETUP_INFO") != -1) { // Fix by D
		return 5;
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
	else if (s.find("ATTACK_SHIP ") != -1)
	{
		return 26;
	}
	else if (s.find("PLAY_MORE") != -1)
	{
		return 27;
	}
	else if (s.find("LOSE_GAME") != -1)
	{
		return 28;
	}
	else if (s.find("ENCRYPT_") != -1)
	{
		return 29;
	}

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
	case 0:
		return "USER_FAIL_ONLINE";
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
	case 7: // Notify to P is P's turn
		return "MY_TURN";
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

string RevEncrytMsg(SOCKET client, bool encrypt) {
	if (encrypt == true)
	{
		char tempmsg[DEFAULT_BUFLEN] = "";
		memset(tempmsg, 0, DEFAULT_BUFLEN);
		int iResults = recv(client, tempmsg, DEFAULT_BUFLEN, 0);
		/*cout << endl;
		cout << "===" << endl;
		cout << tempmsg << endl;
		cout << "===" << endl;*/
		return hex_to_string(tempmsg);
	}
	else {
		char tempmsg[DEFAULT_BUFLEN] = "";
		memset(tempmsg, 0, DEFAULT_BUFLEN);
		int iResults = recv(client, tempmsg, DEFAULT_BUFLEN, 0);
		/*cout << endl;
		cout << "===" << endl;
		cout << tempmsg << endl;
		cout << "===" << endl;*/
		return tempmsg;
	}

}
void Write_User_Info_To_Database(vector<User> Database) {

	// Write down file with syntax:     Account-Password-Fullname-Date-Point.
	ofstream out(USER_INFO, ios::out);
	if (out.is_open())
	{
		/* out._Seekbeg;*/
		for (int i = 0; i < Database.size(); i++)
		{
			out << Database[i].Account << "-" << Database[i].Password << "-" << Database[i].Fullname << "-" << Database[i].Date << "-" << Database[i].Point << "-" << Database[i].Note;
			/*cout << Database[i].Account << "-" << Database[i].Password << "-" << Database[i].Fullname << "-" << Database[i].Date << "-" << Database[i].Point << "-" << Database[i].Note << "." << endl;*/
			if (i != Database.size() - 1)
			{
				out << endl;
			}
		}
		/*out._Seekbeg;*/
	}
	out.close();

}

// SIGN AND INFO PART
void Upload_Database(vector<User>& Database) {
	Database.resize(0);
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

		getline(in, s, '-');
		user.Point = s;

		getline(in, s);
		user.Note = s;
		if (user.Account != "" && user.Password != "")
		{

			Database.push_back(user);
		}
	}
	in.close();

}

int Verify_Login(vector<User> Database, User user, bool flag) {

	string s;
	int countRegister = 0;
	int i = 0;

	while (i <= (Database.size() - 1) && Database.size() != 0)
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
		if (countLogin == 2 && flag == true) {
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
int Verify_Online(vector<User> Online, User user) {

	for (int i = 0; i < Online.size(); i++)
	{
		if (Online[i].Account == user.Account)
		{
			return 0;
		}
	}
	return 1;
};
int Change_Info(vector<User>& Database, User user, int flag) {

	Upload_Database(Database);

	string s;

	switch (flag) {
	case 1: // CHANGE_PASSWORD
	{
		int i = 0;
		while (i < Database.size())
		{
			if (user.Account == Database[i].Account)
			{

				Database[i].Password = user.Password;
				/*cout << "Da doi " << endl;
				cout << Database[i].Password << endl;*/
			}

			i++;
		}
		break;
	}
	case 2:     // Fix By D
	{
		int i = 0;
		while (i < Database.size()) {

			if (user.Account == Database[i].Account) {
				Database[i].Fullname = user.Fullname;
				Database[i].Date = user.Date;
				Database[i].Note = user.Note;
				Database[i].online = user.online;

			}
			i++;
		}
		break;
	}

	}
	for (int i = 0; i < Database.size(); i++)
	{
		/*cout << Database[i].Account << "/" << Database[i].Password << endl;*/
	}
	Write_User_Info_To_Database(Database);
	/*cout << "da viet lai " << endl;*/
	for (int i = 0; i < Database.size(); i++)
	{
		/*cout << Database[i].Account << "/" << Database[i].Password << endl;*/
	}
	Upload_Database(Database);
	//RIGISTER_SUCCESS
	return 1;
}
void Login(SOCKET client, vector<User>& Database, vector<User> Online, User& user, bool& success, bool encrypt) {


	string msg;

	//Rev En
	char Encrypt[DEFAULT_BUFLEN] = "";
	memset(Encrypt, 0, DEFAULT_BUFLEN);
	int iResults = recv(client, Encrypt, DEFAULT_BUFLEN, 0);
	/*cout << "En" << endl;
	cout << Encrypt << endl;*/

	//// Rev Account
	char Account[DEFAULT_BUFLEN] = "";
	memset(Account, 0, DEFAULT_BUFLEN);
	iResults = recv(client, Account, DEFAULT_BUFLEN, 0);
	/*cout << "size + Account + mk" << endl;
	cout << Account << endl;*/

	string s;
	// IF have Encrypt
	if (Encrypt[0] == 'Y')
	{
		s = hex_to_string(Account);
		string u;
		u = s[0];
		int size_acc = stoi(u);
		/*cout << "length" << endl;
		cout << size_acc << endl;*/
		s = s.erase(0, 1);
		for (int i = 0; i < size_acc; i++)
		{
			user.Account.resize(i + 1);
			user.Account[i] = s[i];
		}

		s = s.erase(0, size_acc);

		user.Password = s;

	}
	else {
		s = Account;
		string u;
		u = s[0];

		int size_acc = stoi(u);
		/*cout << "length" << endl;
		cout << size_acc << endl;*/
		s = s.erase(0, 1);
		for (int i = 0; i < size_acc; i++)
		{
			user.Account.resize(i + 1);
			user.Account[i] = s[i];
		}

		s = s.erase(0, size_acc);

		user.Password = s;
		/*user.Password = hex_to_string(Password);*/
	}
	//show

	/*cout << "Acc" << endl;
	cout << user.Account << endl;


	cout << "Pass" << endl;
	cout << user.Password << endl;*/

	// Get Flag to respond to sever
	int flag = Verify_Login(Database, user, true);
	if (flag == 1 && Verify_Online(Online, user) == 0)
	{
		flag = 3;
	}
	switch (flag)
	{
	case 1: // msg = "LOGIN_SUCCESS"
	{
		msg = FlagSend(flag);
		int Result = SentMsg(client, msg);
		success = true;
	}
	break;
	case 2: // msg = "LOGIN_FAIL"
	{
		msg = FlagSend(flag);
		int Result = SentMsg(client, msg);
		success = false;
	}
	break;
	case 3: // msg = "USER_FAIL_ONLINE"
	{
		msg = FlagSend(0);
		int Result = SentMsg(client, msg);
		success = false;
	}
	break;
	}
}

void Register(SOCKET client, vector<User>& Database, bool encrypt) {

	string msg;
	User user;

	//Rev En
	char Encrypt[DEFAULT_BUFLEN] = "";
	memset(Encrypt, 0, DEFAULT_BUFLEN);
	int iResults = recv(client, Encrypt, DEFAULT_BUFLEN, 0);
	cout << "En" << endl;
	cout << Encrypt << endl;

	//// Rev Account
	char Account[DEFAULT_BUFLEN] = "";
	memset(Account, 0, DEFAULT_BUFLEN);
	iResults = recv(client, Account, DEFAULT_BUFLEN, 0);
	cout << "size + Account + mk" << endl;
	cout << Account << endl;

	string s;
	// IF have Encrypt
	if (Encrypt[0] == 'Y')
	{
		s = hex_to_string(Account);
		string u;
		u = s[0];
		int size_acc = stoi(u);
		cout << "length" << endl;
		cout << size_acc << endl;
		s = s.erase(0, 1);
		for (int i = 0; i < size_acc; i++)
		{
			user.Account.resize(i + 1);
			user.Account[i] = s[i];
		}

		s = s.erase(0, size_acc);

		user.Password = s;
		/*user.Password = hex_to_string(Password);*/

	}
	else {
		s = Account;
		string u;
		u = s[0];

		int size_acc = stoi(u);
		cout << "length" << endl;
		cout << size_acc << endl;
		s = s.erase(0, 1);
		for (int i = 0; i < size_acc; i++)
		{
			user.Account.resize(i + 1);
			user.Account[i] = s[i];
		}

		s = s.erase(0, size_acc);

		user.Password = s;
		/*user.Password = hex_to_string(Password);*/
	}
	// show
	cout << "Acc" << endl;
	cout << user.Account << endl;


	cout << "Pass" << endl;
	cout << user.Password << endl;

	// Get Flag to respond to sever
	int flag = Verify_Login(Database, user, false);
	cout << flag << endl;
	switch (flag)
	{
	case 3: // msg = "REGISTER_SUCCESS"
	{
		// Update user into database
		cout << "Up_Load_Database" << endl;
		Database.push_back(user);
		Write_User_Info_To_Database(Database);
		Upload_Database(Database);

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

void Change_Password(SOCKET client, vector<User>& Database, User& user, bool encrypt) {

	string msg;

	bool change = true;
	// Rev Encrpt  
	/*cout << "En" << endl;*/
	msg = RevEncrytMsg(client, false);


	//// Rev Password
	char Password[DEFAULT_BUFLEN] = "";
	memset(Password, 0, DEFAULT_BUFLEN);
	int iResults = recv(client, Password, DEFAULT_BUFLEN, 0);

	if (msg == "Y")
	{
		encrypt = true;
		msg = hex_to_string(Password);
	}
	else {
		msg = Password;
	}

	//Rev current pass
	while (msg != user.Password)
	{
		change = false;
		msg = FlagSend(6);
		int res = SentMsg(client, msg);
		msg = RevEncrytMsg(client, encrypt);
	}

	// Rev Changed Password
	user.Password = RevEncrytMsg(client, encrypt);
	/*cout << "Pass" << endl;
	cout << user.Password << endl;*/
	change = true;


	// Update Database
	if (change == true)
	{
		// Update Database
		int flag = Change_Info(Database, user, 1);
		Upload_Database(Database);
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

void Check_User(SOCKET client, vector<User> Database, User user, string message) {
	string option, username;
	message = message.substr(strlen("CHECK_USER "));
	/*cout << message << endl;*/
	int pos = message.find(" ");
	option = message.substr(0, pos);
	username = message.substr(pos + 1);
	string msg = "NotFound";
	string sendmsg;
	pos = -1;
	for (int i = 0; i < Database.size(); ++i) {
		if (Database[i].Account == username) {
			pos = i;
			break;
		}
	}
	if (pos > -1) {
		if (option.find("find") != -1) {
			int count = 0;
			for (int i = 0; i < Database.size(); ++i) {
				if (Database[i].Account == username) {
					/*cout << "Exist The Account " << endl;*/
					int results = SentMsg(client, "CHECK_USER Exist");
					count++;
					break;
					return;
				}
			}
			if (count != 1) {
				/*cout << "Not have exist account" << endl;*/
				int results = SentMsg(client, "NotExist");
			}
		}
		else if (option.find("online") != -1) {
			if (Database[pos].online == 1) {
				int results = SentMsg(client, "CHECK_USER online");
			}
			else {
				int results = SentMsg(client, "CHECK_USER offline");
			}
		}
		else if (option.find("show_date") != -1) {
			sendmsg = "CHECK_USER show_date " + Database[pos].Date;
			int results = SentMsg(client, sendmsg);

		}
		else if (option.find("show_fullname") != -1) {
			sendmsg = "CHECK_USER show_fullname " + Database[pos].Fullname;
			int results = SentMsg(client, sendmsg);
		}
		else if (option.find("show_note") != -1) {
			sendmsg = "CHECK_USER show_note " + Database[pos].Note;
			int results = SentMsg(client, sendmsg);
		}
		else if (option.find("show_all") != -1) {
			sendmsg = "CHECK_USER  show_all" + Database[pos].Date + " " + Database[pos].Fullname + " " + Database[pos].Note;
			int results = SentMsg(client, sendmsg);
		}
		else if (option.find("show_point") != -1) {
			sendmsg = "CHECK_USER show_point " + Database[pos].Point;
			int results = SentMsg(client, sendmsg);
		}
		else {
			/*cout << msg << endl;*/
			int results = SentMsg(client, msg);
		}
	}
	else {
		int results = SentMsg(client, "NotExist");
	}
}
void Setup_Info(SOCKET client, vector<User>& Database, string message, User& user) {  // Fix By D
	string option, information;
	message = message.substr(strlen("SETUP_INFO "));
	/*cout << message << endl;*/
	int pos = message.find(" ");
	option = message.substr(0, pos);
	information = message.substr(pos + 1);
	string msg = "NotFound";
	if (option.find("fullname") != -1) {
		user.Fullname = information;
		int results = SentMsg(client, "SETUP_INFO ");
	}
	else if (option.find("date") != -1) {
		user.Date = information;
		int results = SentMsg(client, "SETUP_INFO ");
	}
	else if (option.find("note") != -1) {
		user.Note = information;
		int results = SentMsg(client, "SETUP_INFO ");
	}
	else {
		/*cout << msg << endl;*/
		int results = SentMsg(client, msg);
	}

	Change_Info(Database, user, 2);
}

void Point(SOCKET client, vector<User>& Database, string message, User& user) {
	string point;
	point = message.substr(strlen("POINT "));
	cout << message << endl;
	string sendmsg;
	for (int i = 0; i < Database.size(); ++i) {
		if (Database[i].Account == user.Account) {
			Database[i].Point = point;
		}
	}
	Change_Info(Database, user, 2);
}

// ONLINE USER PART
// Write online user to file to manager
void Add_User_Online_File(vector<User>& User_Online, User user, int id) {

	User_Online.push_back(user);

	ofstream out(USER_ONLINE, ios::out);

	if (out.is_open())
	{
		/*out._Seekbeg();*/
		// Format file USER_ONLINE.txt : Username/ONLINE_ID/Map
		for (int i = 0; i < User_Online.size(); i++)
		{
			/*if (i == User_Online.size() - 1)
			{
				User_Online[i].Online_ID = id;
			}*/
			if (User_Online[i].Account == user.Account)      //Fix
			{
				User_Online[i].Online_ID = id;
			}

			out << User_Online[i].Account << "/" << User_Online[i].Online_ID << "/" << User_Online[i].Map_Status;

			if (i != User_Online.size() - 1)
			{
				out << endl;
			}
		}
		/*out._Seekbeg();*/

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
		i++;
	}

	ofstream out(USER_ONLINE, ios::out);

	if (out.is_open())
	{
		/*out._Seekbeg();*/
		for (int i = 0; i < User_Online.size(); i++)
		{
			out << User_Online[i].Account << "/" << User_Online[i].Online_ID << "/" << User_Online[i].Map_Status;
			if (i != User_Online.size() - 1)
			{
				out << endl;
			}
		}
		/*out._Seekbeg();*/

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

		getline(in, s, '/');
		user.Account = s;
		if (s == "")
		{
			return;
		}
		cout << "user: " << s << endl;

		getline(in, s, '/');
		user.Online_ID = stoi(s);
		cout << "id: " << stoi(s) << endl;

		getline(in, s);
		user.Map_Status = stoi(s);


		/*for (int i = 0; i < User_Online.size(); ++i) {
			if (User_Online[i].Account == user.Account) {
				User_Online[i].Online_ID = user.Online_ID;
				User_Online[i].Map_Status = user.Map_Status;
			}
		}*/
		tmp.push_back(user);
	}
	User_Online = tmp;
	in.close();

}

// Send User online list to Client order 
void Send_Online_User(SOCKET client, vector<User> user) {


	for (int i = 0; i < user.size(); i++)
	{
		// Sent all user online list 
		string s = FLag_Game_Sent(1);
		s = s + user[i].Account;
		/*cout << s << endl;*/

		if (i == user.size() - 1)
		{
			/*cout << "send flag enough" << endl;*/
			s = s + FlagSend(7);
			int res = SentMsg(client, s);
		}
		else {
			int res = SentMsg(client, s);
		}
	}

}
// Send invite to second player 
void Rev_Responding_to_user_is_choosen(SOCKET client, string msg, vector<client_type>& client_array, int& P2_ID, int P1_ID, User user) {

	string s = "CREATE_ROOM";
	/*cout << "nguyen van :" << msg << endl;*/
	// Cut off the flag CREATE_ROOM
	msg.erase(0, s.size() + 1);
	/*cout << "sau khi xoa flag :" << msg << endl;*/
	// Now the mess like : "(room_number) User_ID"
	s = "";
	for (int i = 0; i < msg.rfind(" "); i++)
	{
		s.resize(i + 1);
		s[i] = msg[i];
	}
	int room_number = stoi(s);

	/*cout << "so phong" << room_number << endl;*/
	msg.erase(0, msg.rfind(" "));
	/*cout << "sau khi lay so phong" << msg << endl;*/

	// Now the mess like : "(room_number) User_ID"
	/*cout << "lay so phong :" << msg << endl;*/
	P2_ID = stoi(msg);

	// INVITE P2 to play game with P1
	msg = FLag_Game_Sent(2) + to_string(P1_ID) + user.Account;
	int RES = SentMsg(client_array[P2_ID].socket, msg);
}

void Update_Map_Status(vector<User>& User_Online, int P1_ID) {
	User_Online[P1_ID].Map_Status = 1;

	ofstream out(USER_ONLINE, ios::out);

	if (out.is_open())
	{
		/*out._Seekbeg();*/
		for (int i = 0; i < User_Online.size(); i++)
		{
			out << User_Online[i].Account << "/" << User_Online[i].Online_ID << "/" << User_Online[i].Map_Status;
			if (i != User_Online.size() - 1)
			{
				out << endl;
			}
		}
		/*out._Seekbeg();*/

	}
	out.close();
}

void Get_P2_ID(string msg, int& P2_ID, string flag) {
	// FLAG + P2_ID
	string s;
	msg = msg.erase(0, flag.size());
	P2_ID = stoi(msg);
}

void ServerShow(vector<User> Database, vector<User>& User_Online) {
	system("cls");
	for (int i = 0; i < 4; i++)
	{
		cout << endl;
	}
	cout << setw(20) << "=====DATABASE====" << endl << endl;
	cout << setw(38) << "<Name> // Fullname // Note // Point" << endl;
	for (int i = 0; i < Database.size(); i++)
	{
		cout << setw(10) << i + 1 << ": <" << Database[i].Account << "> // ";

		if (Database[i].Fullname == "") cout << "None";
		else cout << Database[i].Fullname;
		cout << " // ";

		if (Database[i].Note == "") cout << "None";
		else cout << Database[i].Note;
		cout << " // ";

		if (Database[i].Point == "") cout << "None";
		else cout << Database[i].Point;

		cout << endl;
	}
	for (int i = 0; i < 4; i++)
	{
		cout << endl;
	}
	cout << setw(20) << "===USER_ONLINE===" << endl << endl;
	cout << setw(31) << "<Name> // Online_ID // Point" << endl;
	for (int i = 0; i < User_Online.size(); i++)
	{

		cout << setw(10) << i + 1 << ": <" << Database[i].Account << "> // ";

		if (User_Online[i].Account == "") cout << "None";
		else cout << User_Online[i].Account;
		cout << " // ";

		cout << User_Online[i].Online_ID;
		cout << " // ";

		if (Database[i].Point == "") cout << "None";
		else cout << Database[i].Point;

		cout << endl;
	}
	for (int i = 0; i < 4; i++)
	{
		cout << endl;
	}
	cout << "======================================================" << endl;
}
void ShutdownClient(SOCKET& client, vector<client_type>& client_array, int id) {

	// Send Flag LOG
	int Result = SentMsg(client, FlagSend(100));

	// shut down client
	/*cout << "Shut down client" << endl;*/
	closesocket(client);
	closesocket(client_array[id].socket);
	client_array[id].socket = INVALID_SOCKET;

}

int process_client(client_type& new_client, std::vector<client_type>& client_array, std::thread& thread)
{
	std::string msg = "";
	bool Open = true;
	User user;
	bool encrypt = false;
	cout << "Up_Load_Database" << endl;
	// Upload_Database 
	vector<User> Database;
	vector<User> UserOnline;
	Upload_Database(Database);
	Collect_Online_List(UserOnline);
	// Game Part
	int P1_ID = new_client.id;
	int P2_ID = -1;
	ServerShow(Database, UserOnline);
	//Session 
	/*cout << "client ID" << new_client.id << endl;*/
	while (1)
	{
		if (Open == false)
		{
			break;
		}
		// Have Encry
		msg = RevEncrytMsg(new_client.socket, encrypt);
		/*cout << "Client sent :" << msg << endl;*/

		int flag = FlagRev(msg);


		/*cout << flag << endl;*/
		switch (flag)
		{

		case 1: //Login
		{
			Upload_Database(Database);
			Collect_Online_List(UserOnline);

			bool success = true;
			Login(new_client.socket, Database, UserOnline, user, success, encrypt);

			if (success)
			{
				/*cout << "Prepare Userlist" << endl;*/
				Collect_Online_List(UserOnline);

				/*cout << "Add new ogin user" << endl;*/
				Add_User_Online_File(UserOnline, user, new_client.id);
				//	for (int i = 0; i < UserOnline.size(); i++)
				//	{
				//		cout << "User online now" << endl;
				//		cout << UserOnline[i].Account << "/" << UserOnline[i].Password << endl;
				//	}
				//}
			}
			break;
		}
		case 2: // REGISTER
		{
			Upload_Database(Database);
			Register(new_client.socket, Database, encrypt);
			break;
		}
		case 3: // CHANGE_PASSWORD
		{
			Upload_Database(Database);
			Change_Password(new_client.socket, Database, user, encrypt);
			break;
		}
		case 4: {   // Fix By D

			Check_User(new_client.socket, Database, user, msg);
			break;
		}
		case 5: {   // Fix By D

			Setup_Info(new_client.socket, Database, msg, user);
			break;
		}
		case 6: {
			// FIX
			Point(new_client.socket, Database, msg, user);
			break;
		}
		case 20: // START_GAME
		{

			/*cout << "Prepare Userlist again" << endl;*/
			Collect_Online_List(UserOnline);

			/*cout << "Starting send messs" << endl;*/
			Send_Online_User(new_client.socket, UserOnline);

			break;
		}
		case 21: // CREATE_ROOM
		{
			/*cout << "Starting send messs" << endl;*/
			Rev_Responding_to_user_is_choosen(new_client.socket, msg, client_array, P2_ID, P1_ID, user);


			break;
		}
		case 22: // P2 accept P1
		{
			// CREATE CONNECTION BETWEEN P1 AND P2
			/*cout << "P1 Accesss P2" << endl;*/

			Get_P2_ID(msg, P2_ID, "ACCEPT,P");
			msg = "COMPERTITOR_ACCEPT";

			int Result = SentMsg(client_array[P2_ID].socket, msg);

			break;
		}
		case 23: // P2 reject P1
		{
			Get_P2_ID(msg, P2_ID, "REJECT,P");
			msg = "COMPERTITOR_REJECT";

			/*cout << "P2 Reject to play" << endl;*/

			int Result = SentMsg(client_array[P2_ID].socket, msg);
			break;
		}
		case 24: // Send Map and want P2 take back Map : REV_SEND_MAP
		{
			// FLAG + filename.txt
			// Update P1 has Sent the Map
			Collect_Online_List(UserOnline);
			Update_Map_Status(UserOnline, P1_ID);

			if (UserOnline[P1_ID].Map_Status == UserOnline[P2_ID].Map_Status)
			{
				// Flag 25: Send Map and start game : UPLOAD_MAP

				// FLAG + filename.txt

				/*cout << "Send Map and start game" << endl;*/

				int Result = SentMsg(client_array[P2_ID].socket, msg);
			}
			else
			{
				string flag_now = "UPLOAD_MAP";

				string flag_send = "REV_SEND_MAP";

				msg = msg.erase(0, flag_now.size());

				msg = flag_send + msg;

				/*cout << "Send Map and want P2 take back Map" << endl;*/

				int Result = SentMsg(client_array[P2_ID].socket, msg);

			}

			break;
		}
		case 26: // Seen P1 Attack back to P1 client then  Notify to P2's turn
		{
			// Sent P1 Attack back to P1 client
			int Result = SentMsg(client_array[P1_ID].socket, msg);
			/*cout << "Sent P1 Attack back to P1 client" << endl;*/
			// Notify to P2's turn
			msg = FLag_Game_Sent(7);
			Result = SentMsg(client_array[P2_ID].socket, msg);
			/*cout << "Notify to P2's turn" << endl;*/

			break;
		}
		case 27: // START_GAME _ PLAY_MORE
		{


			/*cout << "Prepare Userlist again" << endl;*/
			Collect_Online_List(UserOnline);

			/*cout << "Starting send messs" << endl;*/
			Send_Online_User(new_client.socket, UserOnline);

			break;
		}
		case 28: // Notifi P1 WIN to P2 and count point for P1
		{
			Upload_Database(Database);
			int num;
			if (Database[P1_ID].Point == "")
			{
				num = 0;
			}
			else {
				num = stoi(Database[P1_ID].Point);
			}

			Database[P1_ID].Point = to_string(num + 1);
			//Update point to database
			Write_User_Info_To_Database(Database);
			Upload_Database(Database);

			msg = "LOSE_GAME";
			int Result = SentMsg(client_array[P2_ID].socket, msg);

			break;
		}
		case 29: // Encrypt
		{
			string s = "ENCRYPT_";
			msg = msg.erase(0, s.size());
			if (msg == "Y")
			{
				encrypt = true;
			}
			else
			{
				encrypt = false;
			}

			break;

		}
		case 100://  LOGOUT  : Shut down client
		{
			Upload_Database(Database);

			user.online = 0;
			Change_Info(Database, user, 2);

			// Updata Database before shutdow

			Write_User_Info_To_Database(Database);


			// Remove User online before shutdown
			Collect_Online_List(UserOnline);
			Remove_Update_Online(UserOnline, user);

			int Result = SentMsg(new_client.socket, FlagSend(100));
			/*cout << "sent flag log out to p1" << endl;*/
			msg = "Client #" + std::to_string(new_client.id) + " Disconnected";

			/*std::cout << msg << std::endl;*/

			closesocket(new_client.socket);
			closesocket(client_array[new_client.id].socket);
			client_array[new_client.id].socket = INVALID_SOCKET;

			Open = false;
			break;
		}
		}
		ServerShow(Database, UserOnline);

	} //end while

	thread.detach();
	return 0;
}

void Run() {

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
}

int main()
{
	Run();

	return 0;
}