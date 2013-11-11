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
// Name     : CBtService.cpp
// Part of  : ex_btsocket
// Created  : 17/11/2004 by Shane Kearns
// Server "smart connector" class
// Version  :
// 
//

#include "cbtservice.h"

/**
Standard Symbian style 2 phase construction
@param aServiceUUID a universally unique identifier for this application.
@param aSdpSession a constructed sdp session owned by the application
@param aSocketServer a constructed socket server session owned by the application
@param aOwner the owning class which should receive notification of new connections
@param aProtocol the protocol (RFCOMM or L2CAP) to register
@param aSecurity optional security settings to use on the socket instead of system defaults
*/
EXPORT_C CBtService* CBtService::NewL(const TUUID& aServiceUUID, 
							 RSdp& aSdpSession, 
							 RSocketServ& aSocketServer, 
							 MConnectionObserver& aOwner,
							 TUint aProtocol,
							 const TBTServiceSecurity* aSecurity=NULL)
	{
	LOG_FN_TRACE((_L("+CBtService::NewL")));
	__ASSERT_ALWAYS(aProtocol == KL2CAP || aProtocol == KRFCOMM, ExBtSocket::Panic(ExBtSocket::EUnsupportedProtocol));
	CBtService* self = new(ELeave) CBtService(aSocketServer, aOwner, aProtocol);
	CleanupStack::PushL(self);
	self->ConstructL(aServiceUUID, aSdpSession, aSecurity);
	CleanupStack::Pop(self);
	LOG_FN_TRACE((_L("-CBtService::NewL")));
	return self;
	}

/**
Standard 1st phase constructor, inialise member variables
*/
CBtService::CBtService(RSocketServ& aSocketServer, MConnectionObserver& aOwner, TUint aProtocol) :
	iSocketServer(aSocketServer),
	iOwner(aOwner),
	iProtocol(aProtocol)
	{
	}

/**
Standard 2nd phase constructor, perform initialisation which could leave
*/
void CBtService::ConstructL(const TUUID& aServiceUUID, RSdp& aSdpSession, const TBTServiceSecurity* aSecurity)
	{
	LOG_FN_TRACE((_L("+CBtService::ConstructL")));

	//open handle so it is certain to be valid in the destructor
	iServiceRecord.Open(aSdpSession);

	//First, create a listening socket
	if(iProtocol == KL2CAP)
		{
		iAcceptorSocket = CBluetoothSocket::NewL(*this, iSocketServer, KSockSeqPacket, KL2CAP);
		//bind the socket to a port
		TL2CAPSockAddr addr;
		addr.SetPort(KL2CAPPassiveAutoBind); //allow the stack to choose a free port automatically
		if(aSecurity)
			{
			addr.SetSecurity(*aSecurity); //set security requirements, if present
			}
		User::LeaveIfError(iAcceptorSocket->Bind(addr));
		}
	else if(iProtocol == KRFCOMM)
		{
		iAcceptorSocket = CBluetoothSocket::NewL(*this, iSocketServer, KSockStream, KRFCOMM);
		//bind the socket to a port
		TRfcommSockAddr addr;
		addr.SetPort(KRfcommPassiveAutoBind); //allow the stack to choose a free port automatically
		if(aSecurity)
			{
			addr.SetSecurity(*aSecurity); //set security requirements, if present
			}
		User::LeaveIfError(iAcceptorSocket->Bind(addr));
		}
	User::LeaveIfError(iAcceptorSocket->Listen(1)); 

	//Now, register in the SDP database
	TUint port = iAcceptorSocket->LocalPort(); //get the port chosen by the stack
	LOG_INFO((_L("Listening for connections on port %d"), port));
	iServiceRecord.CreateServiceRecordL(aServiceUUID, iServiceRecordHandle);
	
	// Set attr 4 (protocol list) to L2CAP, and RFCOMM
	CSdpAttrValueDES *attrValDES;

	//This following code is doing these steps
	//1. create a DES (data element set) attribute
	//2. within that list, create a DES for each protocol, L2CAP and optionally RFCOMM
	//3. each protocol is a list of elements, the UUID of the protocol, and the port to connect to.
	//4. the service record is updated to include the new attribute
	//5. discard the attribute we built, as it has been copied to the SDP server
	attrValDES = CSdpAttrValueDES::NewDESL(NULL);
	CleanupStack::PushL(attrValDES);
	if(iProtocol == KL2CAP)
		{
		LOG_INFO((_L("Registering SDP PDL for L2CAP")));
		//This code will register an L2CAP (datagram) service
		attrValDES
			->StartListL()
				->BuildDESL()
					->StartListL()
						->BuildUUIDL(TUUID(TUint16(KL2CAP)))	// L2CAP
						->BuildUintL(TSdpIntBuf<TUint16>(TUint16(port)))	// The Protocol Service Multiplexor (port) for our service
					->EndListL()
			->EndListL();
		}
	else if(iProtocol == KRFCOMM)
		{
		LOG_INFO((_L("Registering SDP PDL for RFCOMM")));
		//This code will register an RFCOMM (stream) service
		attrValDES
			->StartListL()
				->BuildDESL()
					->StartListL()
						->BuildUUIDL(TUUID(TUint16(KL2CAP))) // L2CAP
					->EndListL()
				->BuildDESL()
					->StartListL()
						->BuildUUIDL(TUUID(TUint16(KRFCOMM))) // RFCOMM
						->BuildUintL(TSdpIntBuf<TUint8>(TUint8(port)))
					->EndListL()
			->EndListL();
		}
	iServiceRecord.UpdateAttributeL(iServiceRecordHandle, KSdpAttrIdProtocolDescriptorList, *attrValDES);
	CleanupStack::PopAndDestroy(attrValDES);
	attrValDES = NULL;
	LOG_FN_TRACE((_L("-CBtService::ConstructL")));
	}

EXPORT_C CBtService::~CBtService()
	{
	LOG_FN_TRACE((_L("+CBtService::~CBtService")));
	//attempt to remove the SDP record
	TRAP_IGNORE(iServiceRecord.DeleteRecordL(iServiceRecordHandle));
	iServiceRecord.Close();
	delete iAcceptorSocket;
	LOG_FN_TRACE((_L("-CBtService::~CBtService")));
	}

/**
Accept a new connection.
When the incoming connection has been made, HandleAcceptCompleteL() will be called
in this class.
That function informs the owner by calling HandleNewConnection() or HandleConnectFailed()
@param aBlankSocket an empty socket to receive the incoming connection. 
*/
EXPORT_C void CBtService::AcceptConnection(CBluetoothSocket& aBlankSocket)
	{
	LOG_FN_TRACE((_L("+CBtService::AcceptConnection")));
	__ASSERT_ALWAYS(iConnectionSocket == NULL, ExBtSocket::Panic(ExBtSocket::EReEntrant));
	iConnectionSocket = &aBlankSocket;
	iAcceptorSocket->Accept(*iConnectionSocket);
	LOG_FN_TRACE((_L("-CBtService::AcceptConnection")));
	}

/**
Callback from CBluetoothSocket when a new incoming connection is complete.
Inform the owner 
*/
void CBtService::HandleAcceptCompleteL(TInt aErr)
	{
	LOG_FN_TRACE((_L("+CBtService::HandleAcceptCompleteL")));
	if(aErr == KErrNone)
		{
		iOwner.HandleNewConnection(iConnectionSocket); 
		}
	else
		{
		iOwner.HandleConnectFailed(aErr);
		}
	iConnectionSocket = NULL;
	LOG_FN_TRACE((_L("-CBtService::HandleAcceptCompleteL")));
	}

/**
This function is not used but must be implemented as it is pure virtual
*/
void CBtService::HandleConnectCompleteL(TInt /*aErr*/)
	{
	LOG_FN_TRACE((_L("+CBtService::HandleConnectCompleteL")));
	ExBtSocket::Panic(ExBtSocket::EBadCallback);
	}

/**
This function is not used but must be implemented as it is pure virtual
*/
void CBtService::HandleShutdownCompleteL(TInt /*aErr*/)
	{
	LOG_FN_TRACE((_L("+CBtService::HandleShutdownCompleteL")));
	ExBtSocket::Panic(ExBtSocket::EBadCallback);
	}

/**
This function is not used but must be implemented as it is pure virtual
*/
void CBtService::HandleSendCompleteL(TInt /*aErr*/)
	{
	LOG_FN_TRACE((_L("+CBtService::HandleSendCompleteL")));
	ExBtSocket::Panic(ExBtSocket::EBadCallback);
	}

/**
This function is not used but must be implemented as it is pure virtual
*/
void CBtService::HandleReceiveCompleteL(TInt /*aErr*/)
	{
	LOG_FN_TRACE((_L("+CBtService::HandleReceiveCompleteL")));
	ExBtSocket::Panic(ExBtSocket::EBadCallback);
	}

/**
This function is not used but must be implemented as it is pure virtual
*/
void CBtService::HandleIoctlCompleteL(TInt /*aErr*/)
	{
	LOG_FN_TRACE((_L("+CBtService::HandleIoctlCompleteL")));
	ExBtSocket::Panic(ExBtSocket::EBadCallback);
	}

/**
This function is not used but must be implemented as it is pure virtual
*/
void CBtService::HandleActivateBasebandEventNotifierCompleteL(TInt /*aErr*/, TBTBasebandEventNotification& /*aEventNotification*/)
	{
	LOG_FN_TRACE((_L("+CBtService::HandleActivateBasebandEventNotifierCompleteL")));
	ExBtSocket::Panic(ExBtSocket::EBadCallback);
	}

