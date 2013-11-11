/**
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* CTcpServer
* 
*
*/



#ifndef __CTCPSERVER_H__
#define __CTCPSERVER_H__


/***************************************************************************************
 *
 * System Includes
 *
 **************************************************************************************/
#include <winsock2.h>


/***************************************************************************************
 *
 * Definitions
 *
 **************************************************************************************/
#define ITS_OK					0
#define E_SOCKETSTARTUP			1								
#define E_SOCKETBIND			2						
#define E_SOCKETSEND			3			
#define E_SOCKETRECV			4							
#define E_SOCKETCLOSE			5			
#define E_SOCKETSHUTDOWN		6
#define E_SENDING				7
#define E_RECEIVING				8
#define E_LISTENING				9
#define	E_ACCEPT				10
#define E_INCORRECTARG			11

#define RECVBUFFSIZE	1024
		

/***************************************************************************************
 *
 * CTcpServer
 *
 **************************************************************************************/
class CTcpServer
{
public:
	CTcpServer();	
	~CTcpServer();	
	int StartServer( char* aLocalInterface, char* aLocalPort );	

private:
	void ProcessBuffer( char* aBuffer, int aLen );
	bool IsBufferComplete( char* aBuffer, int aLen );

	int initialise(void);
	int disconnect(void);
	int release(void);
	int setArguments(char* li, char* lp);
	int connect( SOCKET &aSocket, SOCKADDR_IN &aAddr );
	int makeConnection(SOCKET &aLocalSocket, SOCKET &aRemoteSocket, SOCKADDR_IN &aRemoteAddr);
	SOCKET getSocket( void );
	SOCKADDR_IN getSockAddr( const char* aIp, const char* aPort );
	bool isIPAdress( const char* aAddr );

private:
	SOCKET iLocal_Sock;
	SOCKADDR_IN iLocal_Addr;
	SOCKET iRemote_Sock;
	SOCKADDR_IN iRemote_Addr;
	char iPacket[RECVBUFFSIZE];
};

#endif //__CTCPSERVER_H__
