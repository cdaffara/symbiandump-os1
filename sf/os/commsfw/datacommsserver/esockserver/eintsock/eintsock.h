// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalAll
*/

#ifndef __EINTSOCK_H__
#define __EINTSOCK_H__

#include <e32std.h>
#include <es_sock.h>
#include <es_panic.h>


GLREF_C void InternalSocketPanic(TESockPanic aPanic);

class RInternalHostResolver; 	// no implementation yet - not need for PAN profile
class RInternalServiceResolver; // no implementation yet - not need for PAN profile
class RInternalConnection;		// no implementation yet - not need for PAN profile
class RInternalNetDatabase;		// no implementation yet - not need for PAN profile

class RMBufChain;
class CInternalSocketImpl;
class RInternalSocket
/**
A socket intended for use by components running inside the ESOCKV3 server, either within one
thread or between threads.
Currently, rather than interfacing with esock after the client-server interface (esock 
wouldn't support this without modification), we just pretend to be esock from the protocol 
below's point of view.

@note This implementation *does not* support the use of RMBufChains by clients if the 
protocol only provides a descriptor interface (and because of the mbuf<->des shim 
provided by esock, we can never tell that it won't work, but no data will be returned 
to the client)

@internalAll
@released Complete implementation as like RSocket
*/
	{
	friend class CInternalSocketImpl;
	
public:
	//
	// RSocket methods
	//
	// All methods copied from RSocket API, methods commented rather than
    // removed to allow incremental implementation, and easy reference to the
    // original API

    IMPORT_C RInternalSocket();
    IMPORT_C ~RInternalSocket();

	// These methods replace the normal Open() methods, as we just need to create
    // an object here, rather than the three stage, connect, create & open process
    // used outside the esock thread.  As such the Socket Server is not required.
	// The below 2 Open APIs are deprecated. With the asynchronous nature of COMMS FW and the 
	// recent changing the RInternalSocket implementation to use ASocket the synchronous API need 
	// special handling to make it work. It is better to use the asynchronous version of Open API and t
	// this works inside the COMMS FW. Currently Bluetooth uses these 2 synchronous APIs. Future use of the
	// Open API should use the asynchronous versions.
	// 	@deprecated
	IMPORT_C TInt Open(const TDesC& aName);		
	//@deprecated
	IMPORT_C TInt Open(TUint aAddrFamily,  TUint aSockType, TUint aProtocol);		
	// 	@internalAll
	IMPORT_C TInt Open();		
	IMPORT_C void Open(const TDesC& aName, TRequestStatus& aStatus);		
	IMPORT_C void Open(TUint aAddrFamily,  TUint aSockType, TUint aProtocol, TRequestStatus& aStatus);		
	
	IMPORT_C void Send(const TDesC8& aDesc,TUint aFlags,TRequestStatus& aStatus);
	IMPORT_C void Send(RMBufChain& aData,TUint aFlags,TRequestStatus& aStatus);
	IMPORT_C void Send(const TDesC8& aDesc,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen);
	IMPORT_C void Send(RMBufChain& aData,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen);
	IMPORT_C void CancelSend();

	IMPORT_C void Recv(TDes8& aDesc, TUint aFlags, TRequestStatus& aStatus);
	IMPORT_C void Recv(RMBufChain& aData, TUint aFlags, TRequestStatus& aStatus);
	IMPORT_C void Recv(TDes8& aDesc, TUint aFlags, TRequestStatus& aStatus, TSockXfrLength& aLen);
	IMPORT_C void Recv(RMBufChain& aData, TUint aFlags, TRequestStatus& aStatus, TSockXfrLength& aLen);
	IMPORT_C void RecvOneOrMore(TDes8& aDesc, TUint aFlags, TRequestStatus& aStatus, TSockXfrLength& aLen);
	IMPORT_C void CancelRecv();
	
	IMPORT_C void Read(TDes8& aDesc, TRequestStatus& aStatus);
	IMPORT_C void Read(RMBufChain& aData, TRequestStatus& aStatus);
	IMPORT_C void CancelRead();
	
	IMPORT_C void Write(const TDesC8& aDesc, TRequestStatus& aStatus);
	IMPORT_C void Write(RMBufChain& aData, TRequestStatus& aStatus);
	IMPORT_C void CancelWrite();
	
	IMPORT_C void SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus);
	IMPORT_C void SendTo(RMBufChain& aData,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus);
	IMPORT_C void SendTo(const TDesC8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen);
	IMPORT_C void SendTo(RMBufChain& aData, TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen);

	IMPORT_C void RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus);
	IMPORT_C void RecvFrom(RMBufChain& aData,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus);
	IMPORT_C void RecvFrom(TDes8& aDesc,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen);
	IMPORT_C void RecvFrom(RMBufChain& aData,TSockAddr& aAddr,TUint aFlags,TRequestStatus& aStatus,TSockXfrLength& aLen);

	IMPORT_C void Connect(TSockAddr& aAddr,TRequestStatus& aStatus);
	IMPORT_C void Connect(TSockAddr& aAddr,const TDesC8& aConnectDataOut,TDes8& aConnectDataIn,TRequestStatus& aStatus);
	IMPORT_C void CancelConnect();
	
	IMPORT_C TInt Bind(TSockAddr& aAddr);								
	IMPORT_C TInt SetLocalPort(TInt aPort);								
	
	IMPORT_C void Accept(RInternalSocket& aBlankSocket,TRequestStatus& aStatus);
	IMPORT_C void Accept(RInternalSocket& aBlankSocket,TDes8& aConnectData,TRequestStatus& aStatus);
	IMPORT_C void CancelAccept();
	
	IMPORT_C TInt Listen(TUint aQSize);									
	IMPORT_C TInt Listen(TUint aQSize,const TDesC8& aConnectData);		
	
	IMPORT_C TInt SetOpt(TUint aOptionName,TUint aOptionLevel,const TDesC8& aOption=TPtrC8(NULL,0));	
	IMPORT_C TInt SetOpt(TUint aOptionName,TUint aOptionLevel,TInt aOption);							
	IMPORT_C TInt GetOpt(TUint aOptionName,TUint aOptionLevel,TDes8& aOption);							
	IMPORT_C TInt GetOpt(TUint aOptionName,TUint aOptionLevel,TInt& aOption);							

	IMPORT_C void Ioctl(TUint aCommand,TRequestStatus& aStatus,TDes8* aDesc=NULL,TUint aLevel=KLevelUnspecified);
	IMPORT_C void CancelIoctl();
	
	IMPORT_C TInt GetDisconnectData(TDes8& aDesc);		
	
	IMPORT_C void LocalName(TSockAddr& aAddr);			
	IMPORT_C TUint LocalPort();							
	IMPORT_C void RemoteName(TSockAddr& aAddr);			
	
	IMPORT_C void Close();
	
	IMPORT_C void Shutdown(RSocket::TShutdown aHow,TRequestStatus& aStatus);
	IMPORT_C void Shutdown(RSocket::TShutdown aHow,const TDesC8& aDisconnectDataOut,TDes8& aDisconnectDataIn,TRequestStatus& aStatus);
	IMPORT_C void CancelAll();

	IMPORT_C TInt Info(TProtocolDesc& aProtocol);		

	// don't support socket ::Name() with internal sockets, it's not needed for xfer here	- IMPORT_C TInt Name(TName& aName);
	IMPORT_C TInt Transfer(RInternalSocket& aSource); // special version for internal sockets

private:
	// for socket xfer and listen/accept
	inline CInternalSocketImpl* Implementation()
		{
		return(iImplementation);
		}
	inline void SetImplementation(CInternalSocketImpl* aImplementation)
		{
		iImplementation = aImplementation;
		}
	TInt AdoptBlankSocketToCurrentThread ( RInternalSocket& aBlankSocket );
private:
	CInternalSocketImpl* iImplementation; //< ptr to implementation, created on Open()
	};
	
#endif // __EINTSOCK_H__

