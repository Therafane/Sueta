#include "pch.h"
#include <iostream>
#include <WS2tcpip.h>
#include <vector>
#include <string>

#pragma comment (lib, "ws2_32.lib")

// Комментарий для dev ветки
using namespace std;

int result;
int reload = 1;

SOCKET in;
string message;
sockaddr_in receiver, sender;
vector<sockaddr_in> addreses;


void DrawList() {
	system("cls");
	cout << "Servers:\n";
	int i = 0;
	for (auto const& addr : addreses) {
		char ip[256];
		inet_ntop(AF_INET, &addr.sin_addr, ip, 256);
		cout << i << ") " << ip << endl;
		i++;
	}
}

void RefreshHandler() {
	char buf[1024];
	message = "Broadcast work!";
	int senderLength = sizeof(sender);
	int sizeSender = sizeof(sender);
	ZeroMemory(&sender, sizeSender);
	int run = 1;

	while (run) {
		//if (!reload) continue;
		ZeroMemory(buf, 1024);
		ZeroMemory(&sender, senderLength);
		int recvRes = recvfrom(in, buf, 1024, 0, (sockaddr*)&sender, &senderLength);
		if (recvRes != SOCKET_ERROR) {
			if (buf[0] == 'c') {
				SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
				if (s == INVALID_SOCKET)
				{
					return;
				}

				if (connect(s, (sockaddr*)&sender, sizeSender) == SOCKET_ERROR) {
					cout << "error";
					return;
				}

				run = 0;
				system("cls");
				char msg[20];
				string hui = "Lupa";
				recv(s, msg, 20, NULL);
				cout << msg;
				send(s, hui.c_str(), hui.size(), NULL);
				return;
			}
			else {
				char ip[256];
				inet_ntop(AF_INET, &sender.sin_addr, ip, 256);
				addreses.push_back(sender);
			
				DrawList();
			}
		}
	}
}


void RefreshServers() {
	addreses.clear();
	sendto(in, "r", sizeof(char), 0, (sockaddr*)&receiver, sizeof(receiver));
}

void main()
{
	setlocale(LC_CTYPE, "rus");
	HANDLE sendIp;
	WSADATA data;
	WORD version = MAKEWORD(2, 2);

	int wsOk = WSAStartup(version, &data);
	if (wsOk != 0)
	{
		cout << "Can't start Winsock! " << wsOk;
		return;
	}

	in = socket(AF_INET, SOCK_DGRAM, 0);

	char broadcast = '1';
	int broadOk = setsockopt(in, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
	if (broadOk != 0)
	{
		cout << "Error in setting Broadcast option";
		closesocket(in);
		return;
	}

	receiver.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	receiver.sin_family = AF_INET;
	receiver.sin_port = htons(2620);

	system("cls");
	char choice;

	sendIp = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)RefreshHandler, NULL, NULL, NULL);
	//HANDLE input = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)InputHandler, NULL, NULL, NULL);

	//RefreshServers();
	cout << "Для обновления введите (r)\n";

	while (true)
	{
		char choice;
		cin >> choice;
		if (choice == 'r') {
			RefreshServers();
		}
		else {
			
			int a = (int)(choice - '0');
			if (0 <= a && a <= 9 && a <= addreses.size() && addreses.size() > 0) {
				sendto(in, "1", sizeof(char), 0, (sockaddr*)&addreses[a], sizeof(sockaddr_in));
			}
			else {
				cout << "Иди наводи суету в другой район";
			}
		}
	}

	// Close socket
	closesocket(in);

	// Shutdown winsock
	//CloseHandle(sendIp);
	WSACleanup();
}
