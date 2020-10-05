// client.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define _CRT_SECURE_NO_WARNINGS
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#pragma comment (lib,"ws2_32.lib")
#define SCK_VERSION2 0x0202
#define MYPORT "3490"

std::mutex l;
std::string myname;
std::string addr;
int get_ai_family(const addrinfo* ai) {
	return ai->ai_family;
}

int get_ai_type(const addrinfo* ai) {
	return ai->ai_socktype;
}

int get_ai_protocol(const addrinfo* ai) {
	return ai->ai_protocol;
}

void Receive(SOCKET fd, int& r) {
	std::string name;
	do {
		
		char buffer[512];
		std::string new_buffer;
		

		r = recv(fd, buffer, sizeof(buffer), 0);

		if(r > 0)
		{ 		
			name.erase(0, name.length());
			name.append(buffer, 7);
			new_buffer.append(buffer, r);
			new_buffer.erase(0, 7);
			
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			std::cout << name;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			std::cout << new_buffer<<std::endl;
		}
		else {
			std::cout << "connection closed!"<<std::endl;
			break;
		}
	} while (r > 0);

}

void Send(SOCKET fd, int& r) {
	
	
	while (r > 0)
	{		
		std::string send_buffer;
		
		std::getline(std::cin, send_buffer);
		//i want to send the name
		send_buffer = std::to_string(myname.length())+myname + send_buffer;
		send(fd, send_buffer.c_str(), strlen(send_buffer.c_str()), 0);
	}
}

sockaddr* get_sockadrr(const addrinfo* ai) {
	return ai->ai_addr;
}

int main()
{
	
	SOCKET sock;
	WSADATA wsdata;
	WORD ver = MAKEWORD(2, 2);
	
	addrinfo hints;
	addrinfo *servinfo;
	
	int status = 1;

	if (WSAStartup(ver, &wsdata) != 0)
	{
		std::cerr << "WSAStartup fail!" << std::endl;
	}
	
	std::cout << "enter ip adresse";
	std::getline(std::cin, addr);

	std::cout << "ur name?";
	std::getline(std::cin, myname);
	
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
		
	if ((getaddrinfo(addr.data(), "8090", &hints, &servinfo)) != 0) {
		std::cerr << WSAGetLastError() << std::endl;
		system("pause");
		exit(1);
	}

	sock = socket(get_ai_family(servinfo), get_ai_type(servinfo), get_ai_protocol(servinfo));
	std::cout << "connecting" << std::endl;
	connect(sock, get_sockadrr(servinfo), servinfo->ai_addrlen);
	std::cout << "connected" << std::endl;
	freeaddrinfo(servinfo);
	
	std::thread recv_thread(Receive, sock, std::ref<int>(status));
	std::thread send_thread(Send, sock, std::ref<int>(status));

	send_thread.join();
	recv_thread.join();

	std::cout << "exited" << std::endl;
	system("PAUSE");
}
