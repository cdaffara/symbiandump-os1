// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// CTcpServer
// System Include
// 
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


/***************************************************************************************
 *
 * Local Include
 *
 **************************************************************************************/
#include "CTcpServer.h"


/***************************************************************************************
 *
 * PUBLIC METHOD: Constructor
 *
 **************************************************************************************/
CTcpServer::CTcpServer()
{
	
}


/***************************************************************************************
 *
` * PUBLIC METHOD: Destructor
 *
 **************************************************************************************/
CTcpServer::~CTcpServer()
{
	disconnect();
	release();
}	


/***************************************************************************************
 *
 * PUBLIC METHOD: StartServer
 *
 **************************************************************************************/
int CTcpServer::StartServer( char* aLocalInterface, char* aLocalPort )
{	
	int r;
	r =initialise();
 
	if(r != ITS_OK)
		return r;

	r = setArguments( aLocalInterface, aLocalPort );
	if(r != ITS_OK)
		return r;
	
	r = makeConnection(iLocal_Sock, iRemote_Sock, iRemote_Addr);
	return r;
	
}


/***************************************************************************************
 *
 * SECTION: CUSTOMISATION METHODS
 *
 **************************************************************************************/
/***************************************************************************************
 *
 * PRIVATE METHOD: ProcessBuffer
 *
 **************************************************************************************/
void CTcpServer::ProcessBuffer( char* aBuffer, int aLen )
{
	int i;
	for( i = 0; i < aLen; i++ ) {
	//	fprintf( stderr, "%c (0x%x)\n", aBuffer[i], aBuffer[i] );
		fprintf( stderr, "%c", aBuffer[i] );
	}
}

/***************************************************************************************
 *
 * PRIVATE METHOD: IsBufferComplete
 *
 **************************************************************************************/
bool CTcpServer::IsBufferComplete(char* aBuffer,int aLen)
{
	return true;
}


/***************************************************************************************
 *
 * SECTION: PRIVATE METHODS
 *
 **************************************************************************************/
//
//	Method binds the sock to the address. 
//
int CTcpServer::connect(SOCKET &aSocket, SOCKADDR_IN &aAddr)
{
	int err;
	
	//Bind the local address with a socket, so it listens to that port.
	if (bind(aSocket,(struct sockaddr*)&aAddr, sizeof(SOCKADDR_IN)) != 0)
	{	
		//return, error has occured
		err = WSAGetLastError();
		printf ("Error binding socket: %d", err);
		return E_SOCKETBIND;
	}

	return ITS_OK;
}

//
//  Disoconnect
//
int CTcpServer::disconnect()
{
	shutdown(iLocal_Sock, SD_SEND);
	shutdown(iLocal_Sock, SD_RECEIVE);

	//do we close remote sock?

	if (closesocket(iLocal_Sock) == SOCKET_ERROR)
		return E_SOCKETCLOSE;

	return ITS_OK;
}

//
// Method to check if we have IP adress or hostname, and return IP address
//
SOCKADDR_IN CTcpServer::getSockAddr( const char* aIp, const char* aPort)
{
	SOCKADDR_IN sockAddr;

	// check params
	assert( aIp != NULL );

	// check for null port
	if( aPort == NULL ) {
		aPort = "0";
	}

	// set the socket
	sockAddr.sin_family = AF_INET;		
	sockAddr.sin_port = htons( atoi(aPort) );
	sockAddr.sin_addr.S_un.S_addr = inet_addr( aIp );
	return sockAddr;
}


//
// Method to check if the adress is IP format or is a hostname string.
//
bool CTcpServer::isIPAdress(const char* aAddr)
{
	int c =0;

	char* q=(char *)aAddr;
	char* tokenPtr = strtok(q,".");
	
	while(tokenPtr != NULL)
	{
		tokenPtr = strtok(NULL,".");	
		c++;
	}

	if (c==4)
		return true; //We have IP adrress, nothing to do
	else 
		return false;
}

//
// Makes a connection, listens, and prints any incoming data.
//
int CTcpServer::makeConnection(SOCKET &aLocalSocket, SOCKET &aRemoteSocket, SOCKADDR_IN & aRemoteAddr)
{
	int remote_addr_len;

	int err = listen( aLocalSocket, 1 );
	if( err == SOCKET_ERROR ) 
	{
		err = WSAGetLastError ();
		printf ("Error listening to socket: %d", err);
		return E_LISTENING;
	}

	//wait for connection 
	while( 1 ) 
		{
			// Wait for the next connection
			printf( "\nWaiting for connection...\n" ); 
			remote_addr_len = sizeof(aRemoteAddr);
			aRemoteSocket = accept( aLocalSocket, (struct sockaddr*)&aRemoteAddr, &remote_addr_len );
			if( aRemoteSocket == INVALID_SOCKET ) 
				{
				err = WSAGetLastError();
				printf ("Accept failed. Error: %d",err);
				return E_ACCEPT;
				}

			// Connection received 
			printf( "Connection received from %d.%d.%d.%d : %d\n",
				aRemoteAddr.sin_addr.S_un.S_un_b.s_b1,
				aRemoteAddr.sin_addr.S_un.S_un_b.s_b2,
				aRemoteAddr.sin_addr.S_un.S_un_b.s_b3,
				aRemoteAddr.sin_addr.S_un.S_un_b.s_b4,
				aRemoteAddr.sin_port );

			// Read until the socket is closed and print out the info received.
			err = 1;
			while( err > 0 ) 
				{
				err = recv(aRemoteSocket, iPacket, RECVBUFFSIZE, 0 );
				if (err == SOCKET_ERROR)
					 return E_RECEIVING;
				ProcessBuffer(iPacket, err);
				err = send(aRemoteSocket, iPacket, err, 0 );
				if (err == SOCKET_ERROR)
					return E_SENDING;
				}

			// Close the socket and go again
			closesocket(aRemoteSocket);
	}
	return ITS_OK;
}

//
// Returns a a TCP/IP socket
//
SOCKET CTcpServer::getSocket()
{
	SOCKET sock;
	int err;

	// create a socket 
	sock = socket( AF_INET, SOCK_STREAM, 0 ) ;
	if( sock == INVALID_SOCKET)
	{
		//return, error has occured
		err = WSAGetLastError ();
		printf ("Allocating socket failed. Error: %d",err);
		return NULL;
	}
	return sock;
}

//
// Initialise use of the dll.
//
int CTcpServer::initialise()
{
	WORD version;
	WSADATA wsaData;

	// 1. start the windows iSocket server
	version = MAKEWORD( 2, 2 );
	if (WSAStartup( version, &wsaData ))
		return E_SOCKETSTARTUP;		

	return ITS_OK;		
}


//
// Release.
//
int CTcpServer::release()
{
	int err =WSACleanup();

	if ( err == SOCKET_ERROR)
		return E_SOCKETSHUTDOWN;

	return ITS_OK;
}

//
// Set the arguments
//
int CTcpServer::setArguments(char* li, char* lp)
{
	if(!lp)
	{
		printf("Error: Missing argument - lp argument required.\n");
		return E_INCORRECTARG;
	}
	
	iLocal_Sock = getSocket();
	iLocal_Addr = getSockAddr(li,lp );	
	connect(iLocal_Sock,iLocal_Addr);	//bind

	return ITS_OK;

}

