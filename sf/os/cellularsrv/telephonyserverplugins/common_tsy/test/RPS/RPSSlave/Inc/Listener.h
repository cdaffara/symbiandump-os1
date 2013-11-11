// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
*/

#ifndef __SYNC_LISTENER_H__
#define __SYNC_LISTENER_H__

//#include <ctransfer.h>
#include <in_sock.h>
#include <e32cons.h>

#include "RequestHandler.h"
#include "rpsasciirqstdefs.h"

const TUint KQueueListenSize = 5;	// Number of waiting connection to be accepted


//
// class CListenerOptions 
//
class CListenerOptions : public CBase
	{
	public:
		
		CListenerOptions(TUint aAddrFamily, TUint aProtocol, TUint aSockType);
		
		TInetAddr Addr();
		void SetAddress(const TInetAddr& aAddr);
		TUint Family();
		void SetFamily(TUint aFamily);
		TInt Port();
		void SetPort(TInt aPort);
		TName ProgramName();
		void SetProgramName(const TName& aName);
		TName ProgramOptions();
		void SetProgramOptions(const TName& aOptions);
		TUint Protocol();
		void SetProtocol(TUint aProtocol);
		TUint SockType();
		void SetSockType(TUint aSockType);
		TInetAddr KnownAdapterAddr();
		void SetKnownAdapterAddr(const TDesC &aBuf);

	protected:
	//	void ConstructL(TPtrC8 aConfiguration);
		
	private:
		TInetAddr iBindAddr;	// Bind address (Family, ip-addr, port)
		TName iProgramName;		// Program Name
		TName iProgramOptions;	// Program Arguments
		TUint iProtocol;		// Protocol (TCP, UDP, ...)
		TName iService;			// Internet service name
		TUint iSockType;		// Socket type (STREAM, DATAGRAM)
		TInetAddr iKnownAdapterAddr;	// Address of a known adapter
	};


//
// class CListener 
//
class CListener : public CActive
	{
	private:
	enum TListenerState 
		{
		EInitial,
		EWaitingForConnection,
		EWaitingForData
		};
		
	public:
		static CListener* NewL(CRequestHandler& aRqstHndlr);
		
		~CListener();
		void DoCancel();
		void AcceptL();
		CListenerOptions* Options();
		
		
	protected:
		CListener(CRequestHandler& aRqstHndlr);
		void RunL();
		void ConstructL();
		void IssueRead();
		
	private:
	void ArpRegisterL();

	private:
		RSocket iListenSocket;		// Listening socket as specified in iOptions.
		RSocket iBlankSocket;		// Pointer to temporary socket used in Accept.
		RSocketServ iSockServ;		// Socket server
		CListenerOptions* iOptions;	// Listeners options.
		
		TBuf8<KReqstSocketPreReadSize> 	iBufferPreRead;	// Recvs msg data for Msg Header and Msg Len
		RBuf8							iBuffer;		// Recvs remaining msg data
		RBuf8							iBufferAll;		// Holds the entire msg (iBufferPreRead+iBuffer)
		
		
		TListenerState iListenerState;
		
		CRequestHandler& 	iRqstHndlr;
	};

#endif // __SYNC_LISTENER_H__
