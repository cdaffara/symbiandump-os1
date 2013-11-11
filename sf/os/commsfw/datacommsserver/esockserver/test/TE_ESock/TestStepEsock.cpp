// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This contains CTestCase which is the base class for all the TestCase DLLs
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
#include "TestStepEsock.h"

TVerdict CTestStepEsock::easyTestStepPostambleL()
	{
	// For a failed OOM loop, all sockets that have been added to the socket container
	// will be closed here.  Otherwise, let Test1.15 handle closures.
	
	if (TestStepResult() == EFail)
		{
		CloseSockets();
		}
	
	return EPass;
	}

/**
Gets the (INI file) section name
@param aSectName	section name to use if none specified in script
@return section name
 */
const TDesC& CTestStepEsock::SectionName(const TDesC& aSectName)
	{
	const TDesC &sectName = ConfigSection();
	if (sectName.Length() > 0)
		return sectName;
	else
		return aSectName;
	}

TBool CTestStepEsock::GetIpAddressFromConfig(const TDesC &aSectName,const TDesC &aKeyName,TInetAddr &anAddr)
	{
	TPtrC result;
	TBool bRet;
	TInt nRet;
	
	// get string from config file
	bRet = GetStringFromConfig(aSectName, aKeyName, result);
	
	if (bRet) // string was retrieved successfully
		{
		// convert to IP address
		nRet = anAddr.Input(result);
		
		// if IP address is invalid
		if (nRet != KErrNone)
			{
			// display error message
			Logger().WriteFormat(_L("Invalid IP address, section:%S key:%S "), &aSectName, &aKeyName );
			bRet = EFalse;
			}
		}
	
	return bRet;
	}

void CTestStepEsock::StripeMem32(TUint *aBuf, TInt aStartPos, TInt aEndPos)
//
// Mark a buffer with repeating byte pattern
//
	{
	aStartPos >>= 2;
	aEndPos >>= 2;
	
	for (TInt i=aStartPos;i<aEndPos;i++)
		{
		aBuf[i]=i<<2;
		}
	}

void CTestStepEsock::StripeDes32(TDes8 &aBuf, TInt aStartPos, TInt anEndPos)
	{
	StripeMem32((TUint*)aBuf.Ptr(), aStartPos, anEndPos);
	}

void CTestStepEsock::StripeMem(TUint8 *aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset)
//
// Mark a buffer with repeating byte pattern
//
	{
	TUint character=aStartChar+(aOffset%((anEndChar+1)-aStartChar));
	
	for (TInt i=aStartPos;i<anEndPos;i++)
		{
		aBuf[i]=(TText8)character;
		if(++character>anEndChar)
			character=aStartChar;
		}
	}

void CTestStepEsock::StripeDes(TDes8 &aBuf, TInt aStartPos, TInt anEndPos, TUint aStartChar, TUint anEndChar, TInt aOffset)
	{
	StripeMem((TUint8 *)aBuf.Ptr(), aStartPos, anEndPos, aStartChar, anEndChar, aOffset);
	}



TInt CTestStepEsock::OpenListeningSocketL(TInetAddr anAddr, TInt &aSockIndex, 
										   TInt aPort, TInt aSize)
	{
	TInt nRet;
	
	RSocket sock;
	CleanupClosePushL(sock);
	
	// open a TCP socket
	nRet = sock.Open(iEsockSuite->iSocketServer,
		KAfInet,KSockStream, KProtocolInetTcp);
	
	if (nRet != KErrNone)
		{
		Logger().WriteFormat(_L("Failed to open listening socket: return value = %d"), nRet);
		CleanupStack::PopAndDestroy();
		return nRet;
		}
	
	// bind the socket
	anAddr.SetPort(aPort);
	nRet = sock.Bind(anAddr);
	
	if (nRet != KErrNone)
		{
		Logger().WriteFormat(_L("Failed to bind socket: return value = %d"), nRet);
		CleanupStack::PopAndDestroy();
		return nRet;
		}
	
	// listen on the socket
	nRet = sock.Listen(aSize);
	
	if (nRet != KErrNone)
		{
		Logger().WriteFormat(_L("Failed to listen on socket: return value = %d"), nRet);
		CleanupStack::PopAndDestroy();
		return nRet;
		}
	
	// store socket handle
	aSockIndex = iEsockSuite->AddSocketToListL(sock);
	CleanupStack::Pop();
	Logger().WriteFormat(_L("Socket %d listening..."), aSockIndex);
	return nRet;
	}



TInt CTestStepEsock::OpenActiveSocketL(TInetAddr anAddr, TInt &aSockIndex,
										TInt aPort)
	{
	TInt nRet;
	TRequestStatus stat;
	
	RSocket sock;
	CleanupClosePushL(sock);
	
	// open a TCP socket
	nRet = sock.Open(iEsockSuite->iSocketServer,
		KAfInet,KSockStream, KProtocolInetTcp);
	
	if (nRet != KErrNone)
		{
		Logger().WriteFormat(_L("Failed to open active socket: return value = %d"), nRet);
		CleanupStack::PopAndDestroy();
		return nRet;
		}
	
	// set port number
	anAddr.SetPort(aPort);
	
	// make connect request
	sock.Connect(anAddr, stat);
	User::WaitForRequest(stat);

	nRet = stat.Int();
	if (nRet != KErrNone)
		{
		Logger().WriteFormat(_L("Failed to connect: return value = %d"), nRet);
		CleanupStack::PopAndDestroy();
		return nRet;
		}
	
	// store socket handle
	aSockIndex = iEsockSuite->AddSocketToListL(sock);
	CleanupStack::Pop();
	Logger().WriteFormat(_L("Socket %d making connection..."), aSockIndex);
	return nRet;
	}


TBool CTestStepEsock::AcceptConnectionL(TInt &aSockIndex, TInt aListenIndex)
	{
	TInt nRet;
	TRequestStatus stat;
	
	RSocket sock;
	CleanupClosePushL(sock);
	
	// open a blank socket
	nRet = sock.Open(iEsockSuite->iSocketServer);
	
	if (nRet != KErrNone)
		{
		Logger().WriteFormat(_L("Failed to open blank socket: return value = %d"), nRet);
		CleanupStack::PopAndDestroy();
		return nRet;
		}
	
	// accept the connection on the listening socket, pass
	// the blank socket as parameter
	(iEsockSuite->GetSocketHandle(aListenIndex)).Accept(sock, stat);
	
	User::WaitForRequest(stat);
	nRet = stat.Int();
	
	if (nRet != KErrNone)
		{
		Logger().WriteFormat(_L("Failed to accept connection: return value = %d"), nRet);
		CleanupStack::PopAndDestroy();
		return nRet;
		}
	
	// store socket handle
	aSockIndex = iEsockSuite->AddSocketToListL(sock);
	CleanupStack::Pop();
	Logger().WriteFormat(_L("Socket %d accepting..."), aSockIndex);
	return nRet;
	}

/**
Closes all sockets in the socket container.
@param aLoglevel	Specifies level of detail for logging of socket closures;
					0 => no logging
 */ 
void CTestStepEsock::CloseSockets(TUint aLoglevel)
	{
	TInt nSockets = iEsockSuite->GetSocketListCount();
	
	for(TInt i=nSockets; i>0; i--)
		{
		if (aLoglevel > 1)
			Logger().WriteFormat(_L("Closing socket #%d"), i);
		iEsockSuite->CloseSocket(i);
		}
	
	if ((aLoglevel > 1) || (aLoglevel > 0 && nSockets > 0))
		Logger().WriteFormat(_L("Closed %d socket(s)"), nSockets);
	}

/**
Attempts to open aNumSockets sockets with the supplied parameters and
stores the successfully opened sockets in the socket container.
@param addrFamily	a valid address constant for a protocol family
@param aSockType	a valid socket type for the protocol
@param aProtocol	a constant which identifies a specific protocol
@param aNumSockets	the integral quantity of sockets to open
@return Error code of last attempted socket open: KErrNone if open succeeded, 
		otherwise one of the other system wide error codes.
 */ 
TInt CTestStepEsock::OpenSockets(TUint addrFamily, TUint aSockType, TUint aProtocol, TUint aNumSockets)
	{
	TInt nRet;
	TInt i;
	for (i = 0; i < aNumSockets; i++)
		{
		// open a TCP socket
		RSocket sock;
		nRet = sock.Open(iEsockSuite->iSocketServer, addrFamily, aSockType, aProtocol);
		if (KErrNone == nRet)
			{
			// store socket handle
			nRet = iEsockSuite->AddSocketToList(sock);
			}
		if (nRet != KErrNone)
			{
			sock.Close();
			break;
			}
		}
	
	return nRet;
	}

/**
Attempts to open aNumSockets sockets with the supplied protocol name and
stores the successfully opened sockets in the socket container.
@param aName		Name of a protocol
@param aNumSockets	the integral quantity of sockets to open
@return Error code of last attempted socket open: KErrNone if open succeeded, 
		otherwise one of the other system wide error codes.
 */ 
TInt CTestStepEsock::OpenSockets(const TDesC &aName, TUint aNumSockets)
	{
	TInt nRet;
	TInt i;
	for (i = 0; i < aNumSockets; i++)
		{
		// open a TCP socket
		RSocket sock;
		nRet = sock.Open(iEsockSuite->iSocketServer, aName);
		if (KErrNone == nRet)
			{
			// store socket handle
			nRet = iEsockSuite->AddSocketToList(sock);
			}
		if (nRet != KErrNone)
			{
			sock.Close();
			break;
			}
		}
	
	return nRet;
	}

/**
Attempts to open aNumSockets null sockets and stores the successfully opened sockets
in the socket container.
@param aNumSockets	the integral quantity of sockets to open
@return Error code of last attempted socket open: KErrNone if open succeeded, 
		otherwise one of the other system wide error codes.
 */ 
TInt CTestStepEsock::OpenSockets(TUint aNumSockets)
	{
	TInt nRet;
	TInt i;
	for (i = 0; i < aNumSockets; i++)
		{
		// open a TCP socket
		RSocket sock;
		nRet = sock.Open(iEsockSuite->iSocketServer);
		if (KErrNone == nRet)
			{
			// store socket handle
			nRet = iEsockSuite->AddSocketToList(sock);
			}
		if (nRet != KErrNone)
			{
			sock.Close();
			break;
			}
		}
	
	return nRet;
	}

/**
Attempts to open a minimum number of sockets with the supplied parameters and
stores the successfully opened sockets in the socket container.
@param aSectName	an INI file section name
@param addrFamily	a valid address constant for a protocol family
@param aSockType	a valid socket type for the protocol
@param aProtocol	a constant which identifies a specific protocol
@param aNumSockets	the integral quantity of sockets to open
@return Number of sockets opened; 
		otherwise one of the other system wide error codes.
 */ 
TInt CTestStepEsock::OpenMinSockets(const TDesC &aSectName, TUint addrFamily, TUint aSockType, TUint aProtocol, TUint aMinSockets)
	{
	TInt ret = KErrNone;
    TInt nSockets;
	if (!GetIntFromConfig(SectionName(aSectName), _L("numSockets"), nSockets))
		{
    	nSockets = aMinSockets;
    	}
    nSockets = (nSockets > aMinSockets) ? nSockets : aMinSockets;

    nSockets -= iEsockSuite->GetSocketListCount();
    if (nSockets > 0)
    	{
    	ret = OpenSockets(addrFamily, aSockType, aProtocol, nSockets);
    	}

    return ret;
	}

/**
Attempts to open the required number of connections.
@param aSectName	an INI file section name, that may have an entry for the
					number of client sockets to use.
					If there is no entry, then one client socket is assumed.
@param iSockIndex2	index to 1st client connection end-point
@param iSockIndex3	index to 1st server connection end-point
@return Number of connections opened;
		otherwise one of the other system wide error codes.
 */
TInt CTestStepEsock::OpenConnectionsL(const TDesC &aSectName, TInt &aSockIndex2, TInt &aSockIndex3)
	{
	TInt nConnections;
	const TDesC &sectName = SectionName(aSectName);
	// get number of client sockets to open
	if (!GetIntFromConfig(sectName, _L("numSockets"), nConnections))
		{
		nConnections = 1;
		}
	
	if (iEsockSuite->GetSocketListCount() != 2 * nConnections + 1)
		{
		CloseSockets();
		
		TInetAddr addrLocal, addrRemote;
		// get local ip address
		if (!GetIpAddressFromConfig(sectName, _L("ipAddressLocal"), addrLocal))
			{
			return KErrNotFound;
			}
		
		// get ip address to connect to (usually loopback)
		if (!GetIpAddressFromConfig(sectName, _L("ipAddressRemote"), addrRemote))
			{
			return KErrNotFound;
			}
		
		// get port number
		TInt port;
		if (!GetIntFromConfig(sectName, _L("port"), port))
			{
			port = 1;
			}
		
		User::After(1000000);
		// open socket and listen for connect requests
		TInt sockIndex1;
		TInt nRet = OpenListeningSocketL(addrLocal, sockIndex1, port);
		if (KErrNone != nRet)
			{
			return nRet;
			}
		
		// save indices for client and server sockets
		aSockIndex2 = sockIndex1 + 1;
		aSockIndex3 = aSockIndex2 + 1;
		
		// for each client socket
		TInt sockIndexIgnore;
		for (TInt i = 0; i < nConnections; i++)
			{
			// open active socket and make connect request
			nRet = OpenActiveSocketL(addrRemote, sockIndexIgnore, port);
			if (KErrNone != nRet)
				{
				return nRet;
				}
			
			// accept connect request
			nRet = AcceptConnectionL(sockIndexIgnore, sockIndex1);
			if (KErrNone != nRet)
				{
				return nRet;
				}
			}
		}
	
	return nConnections;
	}

TInt CTestStepEsock::SetRoute(TInt aOp, TUint32 aDest, TUint32 aNetMask, TUint32 aGateway, TUint32 aInterface, TInt aMetric)
	{
	
	TPckgBuf<TSoInetRouteInfo> info;
	info().iType = ERtUser;
	info().iState = ERtReady;
	info().iMetric = aMetric;
	info().iDstAddr.SetAddress(aDest);
	info().iNetMask.SetAddress(aNetMask);
	info().iGateway.SetAddress(aGateway);
	info().iIfAddr.SetAddress(aInterface);
	
	return iEsockSuite->GetSocketHandle(1).SetOpt(aOp, KSolInetRtCtrl, info);
	}

TInt CTestStepEsock::ListRoutes()
	{
	TPckgBuf<TSoInetRouteInfo> opt;
	TInt ret;
	TBuf<39> ia, ga, da, nm;// changed from 16 AJH
	
	ret = iEsockSuite->GetSocketHandle(1).SetOpt(KSoInetEnumRoutes, KSolInetRtCtrl);
	if (ret!=KErrNone)
		return ret;
	
	Logger().WriteFormat(_L("  Destination      NetMask        Gateway      Interface"));
	while (ret==KErrNone)
		{
		ret = iEsockSuite->GetSocketHandle(1).GetOpt(KSoInetNextRoute, KSolInetRtCtrl, opt);
		if (ret!=KErrNone)
			continue;
		opt().iDstAddr.OutputWithScope(da);
		opt().iNetMask.OutputWithScope(nm);
		opt().iGateway.OutputWithScope(ga);
		opt().iIfAddr.OutputWithScope(ia);
		
		Logger().WriteFormat(_L("%15S %15S %15S %15S\n"), &da, &nm, &ga, &ia);
		}
	return ret==KErrEof ? KErrNone : ret;
	}


TBool CTestStepEsock::GetAppendixB1()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixInArray.Reset();

	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress1"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Set IP to Class A Network
	appendixInArray.Insert(addrLocal, 0);
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress2"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Set IP to Class B Network
	appendixInArray.Insert(addrLocal, 1);
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress3"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Set IP to Class C Network
	appendixInArray.Insert(addrLocal, 2);
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress4"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Set IP to Class D Network
	appendixInArray.Insert(addrLocal, 3);
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress5"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Set IP to Class E Network
	appendixInArray.Insert(addrLocal, 4);
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress6"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Set IP to Loopback
	appendixInArray.Insert(addrLocal, 5);
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress7"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Set IP to IsUnspecified - 0.0.0.0
	appendixInArray.Insert(addrLocal, 6);
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixB1"), _L("ipAddress8"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// Broadcast Address
	// Set IP to Broadcast - 255.255.255.255
	appendixInArray.Insert(addrLocal, 7);
	
	return ETrue;
	}

TBool CTestStepEsock::GetAppendixB2()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixOutArray.Reset();
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet1"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 0);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet2"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 1);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet3"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 2);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet4"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 3);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet5"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 4);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet6"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 5);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet7"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 6);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB2"), _L("subnet8"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 7);
	
	return ETrue;
	}

TBool CTestStepEsock::GetAppendixB3()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixOutArray.Reset();
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId1"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 0);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId2"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 1);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId3"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 2);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId4"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 3);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId5"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 4);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId6"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 5);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId7"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 6);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB3"), _L("networkId8"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 7);
	
	return ETrue;
	}

TBool CTestStepEsock::GetAppendixB4()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixOutArray.Reset();
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId1"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 0);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId2"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 1);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId3"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 2);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId4"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 3);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId5"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 4);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId6"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 5);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId7"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 6);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB4"), _L("networkIdHostId8"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 7);
	
	return ETrue;
	}

TBool CTestStepEsock::GetAppendixB5()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixOutArray.Reset();
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast1"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 0);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast2"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 1);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast3"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 2);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast4"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 3);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast5"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 4);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast6"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 5);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast7"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 6);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB5"), _L("networkIdHostBroadcast8"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 7);
	
	return ETrue;
	}

TBool CTestStepEsock::GetAppendixB6()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixOutArray.Reset();
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast1"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 0);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast2"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 1);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast3"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 2);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast4"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 3);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast5"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 4);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast6"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 5);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast7"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 6);
	
	// get address
	bRet = GetIpAddressFromConfig(_L("AppendixB6"), _L("networkIdHostIdSubnetBroadcast8"), addrLocal);
	if (!bRet)
		{
		// failed to get Address
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, 7);
	
	return ETrue;
	}

TBool CTestStepEsock::GetAppendixC1()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixInArray.Reset();
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress1"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// add to array
	appendixInArray.Insert(addrLocal, ipAddress1);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress2"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixInArray.Insert(addrLocal, ipAddress2);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress3"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixInArray.Insert(addrLocal, ipAddress3);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress4"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixInArray.Insert(addrLocal, ipAddress4);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress5"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixInArray.Insert(addrLocal, ipAddress5);
	
	
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress7"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixInArray.Insert(addrLocal, ipAddress7);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress8"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixInArray.Insert(addrLocal, ipAddress8);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC1"), _L("ipAddress9"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixInArray.Insert(addrLocal, ipAddress9);
	
	return ETrue;
	}

void CTestStepEsock::GetAppendixC2()
	{
	
	}

TBool CTestStepEsock::GetAppendixC3()
	{
	TBool bRet;
	TInetAddr addrLocal;
	
	appendixOutArray.Reset();
	
	// get ip address
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress1"), addrLocal);
	if (!bRet)
		{
		// failed to get ipAddress
		return EFalse;
		}
	// add to array
	appendixOutArray.Insert(addrLocal, ipAddress1);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress2"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, ipAddress2);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress3"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, ipAddress3);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress4"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, ipAddress4);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress5"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, ipAddress5);
	
	
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress7"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, ipAddress7);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress8"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, ipAddress8);
	
	bRet = GetIpAddressFromConfig(_L("AppendixC3"), _L("ipAddress9"), addrLocal);
	if (!bRet)
		{
		return EFalse;
		}
	appendixOutArray.Insert(addrLocal, ipAddress9);
	
	return ETrue;
	}

void CTestStepEsock::GetHostByNameL(TNameRecord& aRecord, const TDesC& aHost)
	{
	RHostResolver hr;
	CleanupClosePushL(hr);
	TInt ret = hr.Open(iEsockSuite->iSocketServer, KAfInet, KProtocolInetUdp);
	TESTEL(KErrNone == ret, ret);
	
	aRecord.iName = _L("");
	TInetAddr::Cast(aRecord.iAddr) = TInetAddr(0);
	aRecord.iFlags = 0;
	TNameEntry hostent(aRecord);
	
	TRequestStatus stat;
	hr.GetByName(aHost, hostent, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Lookup returned %d"), stat.Int());
	TESTL(stat==KErrNone);
	
	// wait for 0.1 seconds
	User::After(100000);
	
	aRecord = hostent();
	do
		{
		if(stat==KErrNone)
			{
			THostName name;
			TInetAddr::Cast(hostent().iAddr).OutputWithScope(name);
			Logger().WriteFormat(_L(" Name= \"%S\", Addr= %S Flags "), &hostent().iName, &name);
			}
		else
			{
			Logger().WriteFormat(_L("Lookup failed %d\n"), stat.Int());
			break;
			}
		} while((stat=hr.Next(hostent))!=KErrEof);
		
	CleanupStack::PopAndDestroy(1, &hr);	
	}


void CTestStepEsock::GetHostByAddrL(TNameRecord& aRecord, const TInetAddr& aAddr)
	{
	THostName addr;
	aAddr.OutputWithScope(addr);
	
	RHostResolver hr;
	CleanupClosePushL(hr);
	TInt ret = hr.Open(iEsockSuite->iSocketServer, KAfInet, KProtocolInetUdp);
	TESTEL(KErrNone == ret, ret);
	
	TNameEntry hostent(aRecord);
	
	TRequestStatus stat;
	hr.GetByAddress(aAddr, hostent, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Lookup returned %d"), stat.Int());
	TESTL(stat==KErrNone);
	
	// wait for 0.1 seconds
	User::After(100000);
	
	aRecord = hostent();
	do
		{
		if(stat==KErrNone)
			{
			THostName name;
			TInetAddr::Cast(hostent().iAddr).OutputWithScope(name);
			Logger().WriteFormat(_L(" Name= \"%S\", Addr= %S Flags "), &hostent().iName, &name);
			}
		else
			{
			Logger().WriteFormat(_L("Lookup failed %d\n"), stat.Int());
			break;
			} 
		} while((stat=hr.Next(hostent))!=KErrEof);
		
	CleanupStack::PopAndDestroy(1, &hr);	
	}


void CTestStepEsock::GetHostNameL()
//
// Return this host's machine name
//
	{
	RHostResolver hr;
	CleanupClosePushL(hr);
	TInt ret = hr.Open(iEsockSuite->iSocketServer, KAfInet, KProtocolInetUdp);
	TESTEL(KErrNone == ret, ret);
	
	TRequestStatus stat;
	THostName name;
	hr.GetHostName(name, stat);
	User::WaitForRequest(stat);
	Logger().WriteFormat(_L("Lookup returned %d"), stat.Int());
	TESTL(stat==KErrNone);
	
	Logger().WriteFormat(_L(" Name= \"%S\"\r"), &name);
	
	CleanupStack::PopAndDestroy(1, &hr);	
	}

void CTestStepEsock::SetHostNameL(const TDesC& aHost)
	{
	RHostResolver hr;
	CleanupClosePushL(hr);
	TInt ret = hr.Open(iEsockSuite->iSocketServer, KAfInet, KProtocolInetUdp);
	TESTEL(KErrNone == ret, ret);

	ret = hr.SetHostName(aHost);
	TESTEL(KErrNone == ret, ret);
	
	CleanupStack::PopAndDestroy(1, &hr);	
	}

void CTestStepEsock::GetHostByNameCancelL(TNameRecord& aRecord, const TDesC& aHost)
	{
	RHostResolver hr;
	CleanupClosePushL(hr);
	TInt ret = hr.Open(iEsockSuite->iSocketServer, KAfInet, KProtocolInetUdp);
	TESTL(ret==KErrNone);
	
	aRecord.iName = _L("");
	TInetAddr::Cast(aRecord.iAddr) = TInetAddr(0);
	aRecord.iFlags = 0;
	TNameEntry hostent(aRecord);
	
	TRequestStatus stat;
	hr.GetByName(aHost, hostent, stat);
	hr.Cancel();
	User::WaitForRequest(stat);
	
	//???? Check 
	//TESTL(ret==KErrCancel);
	
	
	Logger().WriteFormat(_L("Status of Cancel lookup return address = %d"), stat.Int());
	
	CleanupStack::PopAndDestroy(1, &hr);	
	}

