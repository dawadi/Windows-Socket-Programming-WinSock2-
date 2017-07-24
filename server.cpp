/*
Windows Socket proframming - Server application

Author: Deepen Dawadi , Mukesh Maharjan
*/

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <WinSock2.h>                //Use the Winsock API 
#include <cassert>

#define PORT 5990                   //port defined
#define SERVER_IP "127.0.0.1"       //server ip defined
#define BUFFER_SIZE 1024            
#define FILE_NAME_MAX_SIZE 512
#pragma comment(lib, "WS2_32") //Winsock Library

//If expression evaluates to TRUE, assert() does nothing. 
//If expression evaluates to FALSE, assert() displays an error message and aborts program execution
SOCKET Server_Start()
{
	sockaddr_in server_addr;                   /* Address Family */
	server_addr.sin_family = AF_INET;          /* set the type of connection to TCP/IP */
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);           /* set the server port number */   

    /* socket dll */
	WSADATA wsaData; //WSADATA structure contains information about the Windows Sockets implementation

	assert(WSAStartup(MAKEWORD(2, 0), &wsaData) == 0);
	std::cout <<"-->winsock initialization done.\n";

    // Create a SOCKET for the server to listen for client connections
    //The socket() function tells our OS that we want a file descriptor for a socket which we can use for a network stream connection
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);  
	//AF_INET is used to specify the IPv4 address family.
    //SOCK_STREAM is used to specify a stream socket

    assert(SOCKET_ERROR != listen_socket);              	 //Check for errors to ensure that the socket is a valid socket.
	std::cout <<"-->Socket created.\n";
    
    // bind server information to listen_socket
	assert(SOCKET_ERROR != bind(listen_socket, (LPSOCKADDR)&server_addr, sizeof(server_addr)));
	std::cout <<"-->Socket binding completed.\n";
	
	// start listening, allowing a queue of up to 5 pending connection
	assert(SOCKET_ERROR != listen(listen_socket, 5));

	return listen_socket;
}

int main()
{

	SOCKET listen_socket = Server_Start();

	while (1) //infinite loop to keep the server active
	{
		std::cout << "-->Listening To Client ..." << std::endl;
		sockaddr_in client_addr;
		int client_addr_len = sizeof(client_addr);
		//To accept a connection on a socket
		SOCKET m_new_socket = accept(listen_socket, (sockaddr *)&client_addr, &client_addr_len);
		assert(SOCKET_ERROR != m_new_socket);

		char buffer[BUFFER_SIZE];
		memset(buffer, 0, BUFFER_SIZE);
		assert(recv(m_new_socket, buffer, BUFFER_SIZE, 0) >= 0);
        
        //store the filename of client requested file in "file_name"
		char file_name[FILE_NAME_MAX_SIZE + 1];
		memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);
		strncpy(file_name, buffer, strlen(buffer) > FILE_NAME_MAX_SIZE ? FILE_NAME_MAX_SIZE : strlen(buffer));
		std::cout << "File to transfer: " << file_name << std::endl;
        
        //open file in 'rb' mode: "Open file as binary for reading"
        //The fopen() function opens a file indicated by fname and returns a stream associated with that file
        FILE *fp = fopen(file_name, "rb");
        assert(NULL != fp); //if NULL exit
		memset(buffer, 0, BUFFER_SIZE);
		int length = 0;
		int size=0;
        //send the content of the file just read to the client
		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
		{
			assert(send(m_new_socket, buffer, length, 0) >= 0); //sending until end of file 
			memset(buffer, 0, BUFFER_SIZE);
			size += length;
		}
		fclose(fp);        //close the file

        std::cout << "Transfer Successful !\n";
        std::cout << "Bytes transferred: "<<size<<" bytes \n\n";
		closesocket(m_new_socket);
	}
	closesocket(listen_socket);
    
    //The WSACleanup function terminates use of the Winsock 2 DLL (Ws2_32.dll).
	WSACleanup();
	return 0;
}
