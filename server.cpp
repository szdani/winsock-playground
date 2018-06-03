#include "stdafx.h"
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <ctime>
#include <afunix.h>


#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512

using namespace std;


// 1. Initialize Winsock
// 2. Socket
//		2.1 Settings for socket
//		2.2 Socket creation
// 3. Bind Socket
//		3.1 free getaddr result
// 4. Listening on Socket
// 5. Accept incoming connection
// 6. Receive data
// 7. Close socket
// 8. Shutdown Winsock

int main() {
	int iResult;

	// Initialize Winsock
	cout << "Initializing Winsock..." << endl;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) { // Error handling
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	// Socket creation for Server
	cout << "Creating Socket for the Server..." << endl;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	// Settings for the Socket
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET; // used to specify the IPv4 address family. ( after vista you can specify dual-socket for IPv4 an IPv6 )
	hints.ai_socktype = SOCK_STREAM; // used to specify a stream socket.
	hints.ai_protocol = IPPROTO_TCP; // used to specify the TCP protocol .
	hints.ai_flags = AI_PASSIVE; // caller intends to use the returned socket address structure in a call to the bind function.

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
	if (iResult != 0) { // Error handling
		printf("getaddrinfo failed: %d\n", iResult);
		WSACleanup();
		return 1;
	}

	// Create Socket for clients
	SOCKET ListenSocket = INVALID_SOCKET;
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) { // Error handling
		printf("Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return 1;
	}

	cout << "Socket created, now binding the Socket..." << endl;
	// Socket Binding
	iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		printf("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	freeaddrinfo(result); // after bind the information returned by the getaddrinfo function is no longer needed

	// Lsitening on Socket
	cout << "Start listening on Socket..." << endl;
	if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) { // SOMAXCONN = max size of pending connection queue
		printf("Listen failed with error: %ld\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}

	// Accept a client socket
	cout << "Waiting for new incoming connection!" << endl;
	clock_t begin = clock();
	SOCKET ClientSocket;
	ClientSocket = INVALID_SOCKET;
	ClientSocket = accept(ListenSocket, NULL, NULL);
	if (ClientSocket == INVALID_SOCKET) {
		printf("accept failed: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return 1;
	}
	cout << "Rexeive data!" << endl;
	// Receiving Data
	char recvbuf[DEFAULT_BUFLEN];
	int iSendResult;
	int recvbuflen = DEFAULT_BUFLEN;

	// Receive until the peer shuts down the connection
	do {

		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			printf("Bytes received: %d\n", iResult);

			// Echo the buffer back to the sender
			iSendResult = send(ClientSocket, recvbuf, iResult, 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(ClientSocket);
				WSACleanup();
				return 1;
			}
			printf("Bytes sent: %d\n", iSendResult);
		}
		else if (iResult == 0)
			printf("Connection closing...\n");
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return 1;
		}

	} while (iResult > 0);
	clock_t end = clock();
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
	cout << "Elapsed time: " << elapsed_secs << endl;

	// Shutdown
	cout << "Shutdown the Server!" << endl;
	iResult = shutdown(ClientSocket, SD_BOTH);
	if (iResult == SOCKET_ERROR) {
		printf("shutdown failed: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return 1;
	}
	// cleanup
	closesocket(ClientSocket);
	WSACleanup();

	return 0;
}
