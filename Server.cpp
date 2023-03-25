#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

int main()
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		cout << "WSAStartup failed: " << iResult << endl;
		return 1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSocket == INVALID_SOCKET) {
		cout << "socket failed: " << WSAGetLastError() << endl;
		WSACleanup();
		return 1;
	}

	sockaddr_in localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = INADDR_ANY;
	localAddr.sin_port = htons(27015);
	iResult = bind(listenSocket, (sockaddr*)&localAddr, sizeof(localAddr));
	if (iResult == SOCKET_ERROR) {
		cout << "bind failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	iResult = listen(listenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		cout << "listen failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Waiting for client connection..." << endl;
	sockaddr_in clientAddr;
	int clientAddrSize = sizeof(clientAddr);
	SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrSize);
	if (clientSocket == INVALID_SOCKET) {
		cout << "accept failed: " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	char ipAddress[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(clientAddr.sin_addr), ipAddress, INET_ADDRSTRLEN);
	cout << "Client connected from " << ipAddress << endl;

	char recvbuf[1024];
	int recvbuflen = sizeof(recvbuf);
	iResult = recv(clientSocket, recvbuf, recvbuflen - 1, 0);
	if (iResult > 0) {
		recvbuf[iResult] = '\0'; // add null terminator
		cout << "Received: " << recvbuf << endl;
	}
	else if (iResult == 0) {
		cout << "Connection closed" << endl;
	}
	else {
		cout << "recv failed: " << WSAGetLastError() << endl;
	}

	closesocket(clientSocket);
	closesocket(listenSocket);
	WSACleanup();
	return 0;
}