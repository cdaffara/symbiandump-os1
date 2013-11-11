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
// Name     : CBtClient.cpp
// Part of  : ex_btsocket
// Created  : 17/11/2004 by Shane Kearns
// Client "smart connector" class
// Version  :
// 
//

#include <e32std.h>
#include <bt_sock.h>
#include <btsdp.h>
#include <btextnotifiers.h>
#include "cbtclient.h"

/**
Standard Symbian style 2 phase construction
@param aServiceUUID a universally unique identifier for this application.
@param aSocketServer a constructed socket server session owned by the application
@param aOwner the owning class which should receive notification of new connections
@param aSecurityRequirements optional security settings to use on the socket instead of defaults.  The default is Authorisation only ("Accept connection from xxx" dialog but no pairing)
*/
EXPORT_C CBtClient* CBtClient::NewL(const TUUID& aServiceUUID, RSocketServ& aSocketServer, MConnectionObserver& aOwner, TBTServiceSecurity* aSecurityRequirements)
	{
	LOG_FN_TRACE((_L("+CBtClient::NewL")));
	CBtClient* self = new(ELeave) CBtClient(aServiceUUID, aSocketServer, aOwner);
	CleanupStack::PushL(self);
	self->ConstructL(aSecurityRequirements);
	CleanupStack::Pop(self);
	LOG_FN_TRACE((_L("-CBtClient::NewL")));
	return self;
	}

/**
First phase constructor
initialise member data
*/
CBtClient::CBtClient(const TUUID& aServiceUUID, RSocketServ& aSocketServer, MConnectionObserver& aOwner) :
	CActive(CActive::EPriorityStandard),
	iSocketServer(aSocketServer),
	iServiceUUID(aServiceUUID),
	iOwner(aOwner)
	{
	}

/**
Second phase constructor
Leaving initialisation
*/
void CBtClient::ConstructL(TBTServiceSecurity* aSecurityRequirements)
	{
	LOG_FN_TRACE((_L("+CBtClient::ConstructL")));
	CActiveScheduler::Add(this);
	if(aSecurityRequirements)
		{
		iSecurityRequirements = *aSecurityRequirements;
		}
	else
		{
		//default security settings - authorisation only
		iSecurityRequirements.SetDenied(EFalse);
		iSecurityRequirements.SetAuthorisation(ETrue);
		iSecurityRequirements.SetAuthentication(EMitmNotRequired);
		iSecurityRequirements.SetEncryption(EFalse);
		}
	LOG_FN_TRACE((_L("-CBtClient::ConstructL")));
	}

/**
Destructor
free all resources
*/
EXPORT_C CBtClient::~CBtClient()
	{
	LOG_FN_TRACE((_L("+CBtClient::~CBtClient")));
	Deque();
	delete iConnectionSocket;
	delete iSdpAgent;
	LOG_FN_TRACE((_L("-CBtClient::~CBtClient")));
	}

/**
This function attempts to connect to a remote device
First it does a device discovery using the UI notifier
Second it does an SDP search
Third it will make the socket connection using AttemptConnection()
Failure at any stage is reported via the HandleConnectFailed() callback
Success is reported with the HandleNewConnection() callback
@param aSocketOwner the object which will take ownership of allocated socket.
*/
EXPORT_C void CBtClient::ConnectToRemoteDeviceL(MBluetoothSocketNotifier& aSocketOwner)
	{
	LOG_FN_TRACE((_L("+CBtClient::ConnectToRemoteDeviceL")));
	TBTDevAddr addr;
	TBTDeviceSelectionParamsPckg params;
	TBTDeviceResponseParamsPckg result;
	params().SetUUID(iServiceUUID); //As far as I know, no UI currently implements this yet.

	//use the notifier server to select a device.
	RNotifier notif;
	User::LeaveIfError(notif.Connect());

	TRequestStatus stat;
    notif.StartNotifierAndGetResponse(stat, KDeviceSelectionNotifierUid, params, result);

	User::WaitForRequest(stat);
	notif.Close();
	User::LeaveIfError(stat.Int());

	addr = result().BDAddr();

	ConnectToRemoteDeviceL(addr, aSocketOwner);

	LOG_FN_TRACE((_L("-CBtClient::ConnectToRemoteDeviceL")));
	}

/**
This function attempts to connect to a remote device
First it does an SDP search
Second it will make the socket connection using AttemptConnection()
Failure at any stage is reported via the HandleConnectFailed() callback
Success is reported with the HandleNewConnection() callback
@param aAddr the bluetooth address of remote device
@param aSocketOwner the object which will take ownership of allocated socket.
@note use this version if you already know the device to connect to, e.g. you made your own discovery UI using RHostResolver
*/
EXPORT_C void CBtClient::ConnectToRemoteDeviceL(TBTDevAddr& aAddr, MBluetoothSocketNotifier& aSocketOwner)
	{
	LOG_FN_TRACE((_L("+CBtClient::ConnectToRemoteDeviceL")));
	iConnectingToDevice = aAddr;
	//First, we need to perform an SDP search to discover what port the service is running on
	__ASSERT_ALWAYS(iSdpAgent == NULL && iConnectionSocket == NULL && iSocketOwner == NULL, ExBtSocket::Panic(ExBtSocket::EReEntrant));
	iSocketOwner = &aSocketOwner;
	iSdpAgent = CSdpAgent::NewL(*this, aAddr);

	//Create a search pattern so we only see SDP records for our application
	CSdpSearchPattern *pattern = CSdpSearchPattern::NewL();
	CleanupStack::PushL(pattern);
	pattern->AddL(iServiceUUID);
	iSdpAgent->SetRecordFilterL(*pattern);
	CleanupStack::PopAndDestroy(pattern);

	//Create a search pattern for the SDP attributes we are interested in - only protocol descriptor in this case
	CSdpAttrIdMatchList* attribs = CSdpAttrIdMatchList::NewL();
	CleanupStack::PushL(attribs);
	attribs->AddL(TAttrRange(KSdpAttrIdProtocolDescriptorList));
	iSdpAgent->SetAttributePredictorListL(*attribs);
	CleanupStack::PopAndDestroy(attribs);
	
	//Start the search...
	iSdpAgent->NextRecordRequestL();
	//Results will be in NextRecordRequestComplete()
	LOG_FN_TRACE((_L("-CBtClient::ConnectToRemoteDeviceL")));
	}

/**
This function is called after SDP search is completed.
It is used to allocate an appropriate socket and start the connection procedure
*/
void CBtClient::AttemptConnection(TUUID aProtocol, TUint16 aPort)
	{
	LOG_FN_TRACE((_L("+CBtClient::AttemptConnection")));
	//Convert protocol from a UUID (SDP format) to a TUint (ESOCK format)
	TUint protocol;
	TUint socktype;
	TBTSockAddr* addrCommon;
	//protocol specific initialisation
	if(aProtocol == TUUID(KL2CAP))
		{
		protocol = KL2CAP;
		socktype = KSockSeqPacket;
		LOG_INFO((_L("Creating an L2CAP socket")));
		iAddrL2CAP.SetPort(aPort);
		addrCommon = &iAddrL2CAP;
		}
	else if(aProtocol == TUUID(KRFCOMM))
		{
		protocol = KRFCOMM;
		socktype = KSockStream;
		LOG_INFO((_L("Creating an RFCOMM socket")));
		iAddrRFCOMM.SetPort(aPort);
		addrCommon = &iAddrRFCOMM;
		}
	else
		{
		CallOut(KErrNotSupported);
		return;
		}
	//Common initialisation
	addrCommon->SetBTAddr(iConnectingToDevice);
	addrCommon->SetSecurity(iSecurityRequirements);

	//Create the socket and make an outgoing connection
	TRAPD(err, iConnectionSocket = CBluetoothSocket::NewL(*iSocketOwner, iSocketServer, socktype, protocol));
	if(err != KErrNone)
		{
		CallOut(err);
		return;
		}
	iConnectionSocket->Connect(*addrCommon);

	//The current API doesnt allow the MBluetoothSocketNotifier to be changed after construction.
	//Therefore, the client must deal with connection success or failure
	NewConnection();
	LOG_FN_TRACE((_L("-CBtClient::AttemptConnection")));
	}

/**
Called to clean up after we failed to make a connection for some reason
e.g. missing SDP record.
Reset the object to initial state, then callback the owner
*/
void CBtClient::ConnectFailed(TInt aError)
	{
	LOG_FN_TRACE((_L("+CBtClient::ConnectFailed")));
	//Connection failed, attempt to clean up.
	delete iConnectionSocket;
	iConnectionSocket = NULL;
	delete iSdpAgent;
	iSdpAgent = NULL;
	iSocketOwner = NULL;
	//and inform owner
	iOwner.HandleConnectFailed(aError);
	LOG_FN_TRACE((_L("-CBtClient::ConnectFailed")));
	}

/**
Called when we made a connection successfully.
The HandleConnectComplete() from CBluetoothSocket will be sent to the owning object.
In this function, we transfer the CBluetoothSocket pointer to our owner, and perform clean up.
*/
void CBtClient::NewConnection()
	{
	LOG_FN_TRACE((_L("+CBtClient::NewConnection")));
	delete iSdpAgent;
	iSdpAgent = NULL;
	//Connection complete
	CBluetoothSocket *socket = iConnectionSocket;
	iConnectionSocket = NULL; //ownership transferred	
	iOwner.HandleNewConnection(socket);
	iSocketOwner = NULL;
	LOG_FN_TRACE((_L("-CBtClient::NewConnection")));
	}

/**
Asynchronously call back this object.
This is done to avoid deleting the SDP agent from within one of its callbacks.

Called with KErrNone, it will schedule AttemptConnection()
Called with an error, it will schedule ConnectFailed()
*/
void CBtClient::CallOut(TInt aErr)
	{
	LOG_FN_TRACE((_L("+CBtClient::CallOut")));
	SetActive();
	TRequestStatus* stat = &iStatus;
	User::RequestComplete(stat, aErr);
	LOG_FN_TRACE((_L("-CBtClient::CallOut")));
	}

//virtual functions from CActive

/**
This is the callback from CallOut()
Called with KErrNone, it will run AttemptConnection()
Called with an error, it will run ConnectFailed()
*/
void CBtClient::RunL()
	{
	LOG_FN_TRACE((_L("+CBtClient::RunL")));
	delete iSdpAgent; // done with this
	iSdpAgent = NULL;
	if(iStatus==KErrNone)
		{
		AttemptConnection(iParser.iProtocol, static_cast<TUint16>(iParser.iPort));
		iParser.iFoundFlags = 0;
		}
	else
		{
		ConnectFailed(iStatus.Int());
		}
	LOG_FN_TRACE((_L("-CBtClient::RunL")));
	}

/**
pure virtual function of CActive
This function should cancel any outstanding asynchronous requests.
However, this class doesn't make any asynchronous requests, it only completes itself.
So there is guarunteed not to be a request outstanding when this is called.
*/
void CBtClient::DoCancel()
	{
	ASSERT(iStatus != KRequestPending);
	}

//Virtual functions from MSdpAgentNotifier

/**
This function is a callback from SDP when it has found or failed to find a matching service record
In this example, we assume there will be only one good matching record (valid for most applications)
*/
#ifdef _DEBUG
void CBtClient::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt aTotalRecordsCount)
#else
void CBtClient::NextRecordRequestComplete(TInt aError, TSdpServRecordHandle aHandle, TInt /*aTotalRecordsCount*/)
#endif
	{
	LOG_FN_TRACE((_L("+CBtClient::NextRecordRequestComplete")));
#ifdef _DEBUG
	LOG_INFO((_L("SDP record found, err=%d, handle=%d, count=%d"), aError, aHandle, aTotalRecordsCount));
#endif
	if(aError == KErrNone)
		{
		//We have found a service, query its port
		TRAP(aError, iSdpAgent->AttributeRequestL(aHandle, KSdpAttrIdProtocolDescriptorList));
		}
	else if(aError == KErrEof)
		{
		if(iParser.iFoundFlags != (TBtClientSdpProtocolListParser::EFoundPort | TBtClientSdpProtocolListParser::EFoundProtocol))
			{
			aError = KErrNotFound;
			}
		else
			{
			aError = KErrNone;
			CallOut(KErrNone);
			}
		}
	if(aError != KErrNone)
		{
		CallOut(aError);
		}
	LOG_FN_TRACE((_L("-CBtClient::NextRecordRequestComplete")));
	}

/**
This function is a callback from SDP when it finds a matching service attribute
The attribute value is transferred to us so we must delete it when finished with it
*/
#ifdef _DEBUG
void CBtClient::AttributeRequestResult(TSdpServRecordHandle aHandle, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
#else
void CBtClient::AttributeRequestResult(TSdpServRecordHandle /*aHandle*/, TSdpAttributeID aAttrID, CSdpAttrValue* aAttrValue)
#endif
	{
	LOG_FN_TRACE((_L("+CBtClient::AttributeRequestResult")));
#ifdef _DEBUG
	LOG_INFO((_L("SDP attribute found, handle=%d, id=0x%x"), aHandle, aAttrID));
#endif

	if(aAttrID == KSdpAttrIdProtocolDescriptorList)
		{
		//Decode the SDP attribute to find protocol and port
		TRAPD(err, aAttrValue->AcceptVisitorL(iParser));
		if(err != KErrNone)
			{
			CallOut(err);
			}
		}
	delete aAttrValue;
	LOG_FN_TRACE((_L("-CBtClient::AttributeRequestResult")));
	}

/**
The function is a callback from SDP after fully processing a service attribute
We schedule a search for the next matching service record.
*/
void CBtClient::AttributeRequestComplete(TSdpServRecordHandle, TInt /*aError*/)
	{
	LOG_FN_TRACE((_L("+CBtClient::AttributeRequestComplete")));
	TRAPD(err, iSdpAgent->NextRecordRequestL());
	if(err != KErrNone)
		{
		CallOut(err);
		}
	LOG_FN_TRACE((_L("-CBtClient::AttributeRequestComplete")));
	}

/**
Constructor
*/
TBtClientSdpProtocolListParser::TBtClientSdpProtocolListParser()
	{
	iFoundFlags = 0;
	}

/**
This function is a callback from SDP which is called with each element of the protocol descriptor list
we are parsing.
There will be one or more protocol UUIDs, received in lower -> higher layer order.
We are interested only in the highest layer protocol, so overwrite each time.
For the highest layer protocol, there will be connection parameters (a port number).
This implementation only supports L2CAP and RFCOMM, so other protocols will cause a KErrNotSupported exception.
*/
void TBtClientSdpProtocolListParser::VisitAttributeValueL(CSdpAttrValue &aValue, TSdpElementType aType)
	{
	LOG_FN_TRACE((_L("+TBtClientSdpProtocolListParser::VisitAttributeValueL")));
	switch(aType)
		{
		case ETypeDES:
		case ETypeDEA:
			break;
		case ETypeUUID:
			iProtocol = aValue.UUID();
			iFoundFlags |= EFoundProtocol;
			if(iProtocol == TUUID(KL2CAP))
				{
				LOG_INFO((_L("Found L2CAP protocol")));
				}
			else if(iProtocol == TUUID(KRFCOMM))
				{
				LOG_INFO((_L("Found RFCOMM protocl")));
				}
			else
				{
				LOG_ERROR((_L("Found unsupported protocol")));
				User::Leave(KErrNotSupported);
				}
			break;
		case ETypeUint:
			if(aValue.DoesIntFit())
				{
				iPort = aValue.Uint();
				iFoundFlags |= EFoundPort;
				LOG_INFO((_L("Found port %d"), iPort));
				}
			else
				{
				LOG_ERROR((_L("Port won't fit in 32 bits")));
				User::Leave(KErrNotSupported);
				}
			break;
		default:
			LOG_ERROR((_L("Found unexpected attribute type %d"), aType));
			User::Leave(KErrNotSupported);
			break;
		}
	LOG_FN_TRACE((_L("+TBtClientSdpProtocolListParser::VisitAttributeValueL")));
	}

/**
Callback from SDP at the start of each DEA or DES in the protocol descriptor list.
In this implementation we don't care about structure so it is ignored
*/
void TBtClientSdpProtocolListParser::StartListL(CSdpAttrValueList& /*aList*/)
	{
	//do nothing
	}

/**
Callback from SDP at the end of each DEA or DES in the protocol descriptor list.
In this implementation we don't care about structure so it is ignored
*/
void TBtClientSdpProtocolListParser::EndListL()
	{
	//do nothing
	}

