/*
Client Application
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <WinSock2.h>

#define PORT 5990
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024
#define FILE_NAME_MAX_SIZE 512
#pragma comment(lib, "WS2_32")

SOCKET Client_Startup()
{
	// socket dll
	WSADATA wsaData; //initialize the winsock
	WORD socketVersion = MAKEWORD(2, 0); //makes a request for version 2.0 of Winsock on the system
	assert(WSAClient_Startup(socketVersion, &wsaData) == 0); //WSAClient_Startup function is called to initiate use of WS2_32.dll.

	//socket
	SOCKET c_socket = socket(AF_INET, SOCK_STREAM, 0);
	assert(SOCKET_ERROR != c_socket);

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);
	//connect() tells our OS to use the socket c_socket to create a connection to the machine specified.
	assert(SOCKET_ERROR != connect(c_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)));
	return c_socket;
}

int main()
{
    char ans = 'N';
    
    do
    {
	//Client
	SOCKET c_socket = Client_Startup();

	char file_name[FILE_NAME_MAX_SIZE + 1];
	memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
	std::cout << "Input File Name On Server :" << std::endl;
	std::cin >> file_name;   //filename

	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name)); 
    //sends the filename to the server
	assert( send(c_socket, buffer, BUFFER_SIZE, 0) >= 0);
    
    //open the file 
	FILE *fp = fopen(file_name, "wb");  
	assert(NULL != fp);
	memset(buffer, 0, BUFFER_SIZE);
	int length = 0;
	int size=0;
	
	//read the stream from the server
	while ((length = recv(c_socket, buffer, BUFFER_SIZE, 0)) > 0)
	{
		//write the read streams into the file
        assert(fwrite(buffer, sizeof(char), length, fp) >= (size_t)length);
		memset(buffer, 0, BUFFER_SIZE);
		size += length;
	}
	
	std::cout << "Receive File : " << file_name;
	std::cout << "File Transfer from Server Successfull!!!\n" << std::endl;
	std::cout << "Bytes received: "<<size<<" bytes \n\n";
	
	std::cout << "Do you want to download another file (y/n)?\n\n"<< std::endl;
	std::cin >> ans;
	
	fclose(fp);
	closesocket(c_socket);
	//winsock	
	WSACleanup();
    }while ((ans == 'Y') || (ans == 'y'));        //loop till the user wants


}
