#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>

#pragma comment (lib,"ws2_32.lib")

#define SCK_VERSION2 0x0202
#define MYPORT "8090"
#define BACKLOG 5

void Receive(SOCKET fd,int& r) {
	std::string name;
	do{
		char buffer[512];
		std::string new_buffer;
		r = recv(fd, buffer, sizeof(buffer), 0);
		if (r > 0)
		{
			//i want to receive the name
			new_buffer = new_buffer.append(buffer, r);

			int nameSize = new_buffer[0] - '0';

			name.erase();
			name.append(new_buffer, 1, nameSize);

			new_buffer = new_buffer.erase(0, nameSize+1);
			
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
			std::cout << name + ": ";
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
			std::cout << new_buffer << std::endl;
		}
		else {
			std::cout << "connection closed!" << std::endl;
			break;
		}
	} while (r > 0);
	
}
void Send(SOCKET fd,const int& r) {
	
	while (r > 0)
	{
		std::string send_buffer;
		
		std::getline(std::cin, send_buffer);
		
		send_buffer = "server:" + send_buffer;
		send(fd, send_buffer.c_str(), strlen(send_buffer.c_str()), 0);
	}	
}
int main()
{		
	SOCKET listen_s;
	int status = 1;
	addrinfo *servinfo;
	WSADATA wsdata;
	addrinfo hints;
	sockaddr_storage their_addr;
	
	socklen_t slenth = sizeof(sockaddr_storage);

	WORD ver = MAKEWORD(2, 2);
	
	if (WSAStartup(ver, &wsdata) != 0 )
	{
		std::cerr << "WSAStartup fail!" << std::endl;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	/*hints.ai_flags = AI_PASSIVE;*/
	if ((getaddrinfo("192.168.1.6", MYPORT, &hints, &servinfo)) != 0) {
		std::cerr << WSAGetLastError()<< std::endl;
		system("pause");
		exit(1);
	}
	
	listen_s = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
	
	bind(listen_s, servinfo->ai_addr, servinfo->ai_addrlen);

	listen(listen_s, BACKLOG);
	std::cout << "Waiting.." << std::endl;
	SOCKET new_fd = accept(listen_s, (sockaddr *)&their_addr, &slenth);	

	std::cout << "connected to a client" << std::endl;
	
	std::thread recv_thread(Receive,new_fd,std::ref<int>(status));
	std::thread send_thread(Send,new_fd, std::ref<int>(status));
	recv_thread.join();
	send_thread.join();	

	system("pause");
}
