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
// Generic functions associated with all Bluetooth socket addresses
// 
//

#include <bt_sock.h>
#include "btsocketpanic.h"
#include "btphysicallinkshelpers.h"

//----
EXPORT_C CBluetoothPhysicalLinks* CBluetoothPhysicalLinks::NewL(MBluetoothPhysicalLinksNotifier& aNotifier,
														        RSocketServ& aServer)
/** Constructor

Opens a Bluetooth socket.
@pre A session on the socket server has been started.
@leave No memory, or error on opening socket
@param aNotifier The object which will receive asynchronous events.
@param aServer The socket server.
@return An object of type CBluetoothPhysicalLinks
*/
	{
	CBluetoothPhysicalLinks* self = NewLC(aNotifier, aServer);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CBluetoothPhysicalLinks* CBluetoothPhysicalLinks::NewLC(MBluetoothPhysicalLinksNotifier& aNotifier,
														         RSocketServ& aServer)
/** Constructor

Opens a Bluetooth socket.
@pre A session on the socket server has been started.
@leave No memory, or error on opening socket
@param aNotifier The object which will receive asynchronous events.
@param aServer The socket server.
@return An object of type CBluetoothPhysicalLinks
*/
	{
	CBluetoothPhysicalLinks* self=new (ELeave) CBluetoothPhysicalLinks(aNotifier, aServer);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CBluetoothPhysicalLinks::~CBluetoothPhysicalLinks()
/**
Destructor
*/
	{
	delete iBTBasebandConnecter;
	delete iBTDisconnector;
	BTBaseband().Close();
	}

CBluetoothPhysicalLinks::CBluetoothPhysicalLinks(MBluetoothPhysicalLinksNotifier& aNotifier,
												 RSocketServ& aServer)
: iNotifier(aNotifier), iSockServer(aServer)
	{
	}

void CBluetoothPhysicalLinks::ConstructL()
	{
	User::LeaveIfError(BTBaseband().Open(SockServer()));
	}

EXPORT_C TInt CBluetoothPhysicalLinks::CreateConnection(const TBTDevAddr& aBDAddr)
/**
Bring in a new member to the piconet

This can be useful for preparing for a logical link (service socket). 
Firstly if the local device has not bonded with the specified remote device, 
then this function can be used to allow the bonding procedure can take place in advance. 
Secondly if a CBluetoothSocket call is made to create a logical link within an internally 
specified time of this CreateConnection function being called, then that link will come 
up much faster since the physical link will already be in place.
Unless this connection is taken up by a logical link (or a synchronous link) within that
internally specified time then it will be closed.

This function can also be used to create a physical link ready for the setting up
of a synchronous link (e.g. voice link).

@param aBDAddr Bluetooth address specifying remote device (new member of piconet)
@return Error code
*/
	{
	if(iBTBasebandConnecter)
		{
		return KErrInUse;
		}
	
	//Close and re-open socket subsession
	//Allow ESock to do multiple connects	
	BTBaseband().Close(); 
	TInt err = BTBaseband().Open(SockServer());
	if(err != KErrNone)
		{
		return err;
		}

	//Create Connecting AO
	TRAP(err, iBTBasebandConnecter = CBTBasebandConnecter::NewL(*this));
	if(err != KErrNone)
		{
		//BTBaseband().Close();//no leave it open! 
		return err;
		}

	//Try to make BT connection
	iBTBasebandConnecter->BasebandConnect(aBDAddr);
	return err;
	}

EXPORT_C void CBluetoothPhysicalLinks::CancelCreateConnection()
/** Cancel bringing in a new member to the piconet
*/
	{
	if(iBTBasebandConnecter)
		{
		delete iBTBasebandConnecter;
		iBTBasebandConnecter = 0;
		}
	}

EXPORT_C TInt CBluetoothPhysicalLinks::Disconnect(const TBTDevAddr& aBDAddr)
/** Disconnect a single member of piconet

@param aBDAddr Bluetooth address specifying remote device (member of piconet)
@return Error code
@capability NetworkControl
*/
	{
	if(iBTDisconnector)
		{
		return KErrInUse;
		}

	//Allow ESock to do multiple disconnects	
	BTBaseband().Close(); 
	TInt ESockErr = BTBaseband().Open(SockServer());
	if(ESockErr != KErrNone)
		{
		return ESockErr;
		}
	
	TRAPD(err, iBTDisconnector = CBTDisconnector::NewL(*this));
	if(err == KErrNone)
		{
		iBTDisconnector->Disconnect(aBDAddr);
		}
	return err;
	}

EXPORT_C TInt CBluetoothPhysicalLinks::DisconnectAll()
/** Disconnect all members of piconet with the reason code "Remote User Terminated Connection"
@return Error code
@capability NetworkControl
*/
	{
	if(iBTDisconnector)
		{
		return KErrInUse;
		}

	//Allow ESock to do multiple disconnects	
	BTBaseband().Close(); 
	TInt openErr = BTBaseband().Open(SockServer());
	if(openErr != KErrNone)
		{
		return openErr;
		}
		
	TRAPD(err, iBTDisconnector = CBTDisconnector::NewL(*this));
	if(err == KErrNone)
		{
		iBTDisconnector->DisconnectAll();
		}
	return err;
	}

EXPORT_C TInt CBluetoothPhysicalLinks::DisconnectAllForPowerOff()
/** Disconnect all members of piconet with the reason code "Remote Device Terminated Connection due to Power Off"
@return Error code
@capability NetworkControl
*/
	{
	if(iBTDisconnector)
		{
		return KErrInUse;
		}

	//Allow ESock to do multiple disconnects	
	BTBaseband().Close(); 
	TInt openErr = BTBaseband().Open(SockServer());
	if(openErr != KErrNone)
		{
		return openErr;
		}
		
	TRAPD(err, iBTDisconnector = CBTDisconnector::NewL(*this));
	if(err == KErrNone)
		{
		iBTDisconnector->DisconnectAllForPowerOff();
		}
	return err;
	}

EXPORT_C TInt CBluetoothPhysicalLinks::Broadcast(const TDesC8& aData)
/** Write (raw) broadcast data

Raw broadcast data is data that is not directed to any specified service.
@param aData Contains data to be broadcast.
@test
@return Error code
@capability NetworkControl
*/
	{
	return BTBaseband().Broadcast(aData);
	}

EXPORT_C TInt CBluetoothPhysicalLinks::ReadRaw(TDes8& aData)
/** Receive (raw) broadcast data

Raw broadcast data is data that is not directed to any specified service.
@param aData Contains data received.
@test
@return Error code
@capability NetworkControl
*/
	{
	return BTBaseband().ReadRaw(aData);
	}

EXPORT_C TInt CBluetoothPhysicalLinks::Enumerate(RBTDevAddrArray& aBTDevAddrArray, TUint aMaxNumber)
/** Enumerate connected members of the piconet
@param aBTDevAddrArray Bluetooth device address array to be filled with bluetooth addresses of connected members of 
the piconet.
@param aMaxNumber Upper limit on number of members to be returned.
@return Error code
*/
	{
	return BTBaseband().Enumerate(aBTDevAddrArray, aMaxNumber);
	}



void CBluetoothPhysicalLinks::HandleCreateConnectionCompleteL(TInt aErr)
	{
	delete iBTBasebandConnecter;
	iBTBasebandConnecter = 0;
	iNotifier.HandleCreateConnectionCompleteL(aErr);
	}

void CBluetoothPhysicalLinks::HandleDisconnectCompleteL(TInt aErr)
	{
	delete iBTDisconnector;
	iBTDisconnector = 0;
	iNotifier.HandleDisconnectCompleteL(aErr);
	}

void CBluetoothPhysicalLinks::HandleDisconnectAllCompleteL(TInt aErr)
	{
	delete iBTDisconnector;
	iBTDisconnector = 0;
	Notifier().HandleDisconnectAllCompleteL(aErr);
	}

RSocketServ& CBluetoothPhysicalLinks::SockServer()
	{
	return iSockServer;
	}

RBTBaseband& CBluetoothPhysicalLinks::BTBaseband()
	{
	return iBTBaseband;
	}

MBluetoothPhysicalLinksNotifier& CBluetoothPhysicalLinks::Notifier()
	{
	return iNotifier;
	}














//==========================================================
//Active Object Helpers
//

//
//for CBluetoothPhysicalLinks
//

//--
CBTBasebandConnecter* CBTBasebandConnecter::NewL(CBluetoothPhysicalLinks& aParent)
{
	CBTBasebandConnecter* self = new (ELeave) CBTBasebandConnecter(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
}

CBTBasebandConnecter::CBTBasebandConnecter(CBluetoothPhysicalLinks& aParent)
: CActive(CActive::EPriorityStandard),iParent(aParent)
//
//Constructorthe RBluetoothSocket parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
	{
	}

void CBTBasebandConnecter::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTBasebandConnecter::~CBTBasebandConnecter()
//
//Destructor
//
	{
	if(IsActive())
		{
		Cancel();
		}
	}


void CBTBasebandConnecter::BasebandConnect(const TBTDevAddr& aBDAddr)
//
//Attempts to connect to the remote device. 
//
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));
	
	iParent.BTBaseband().Connect(aBDAddr, iStatus);
	SetActive();
	}

void CBTBasebandConnecter::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	//must come LAST .. deletes this active object
	iParent.HandleCreateConnectionCompleteL(iStatus.Int()); 
	}

//
// Swallow any error from RunL
//
TInt CBTBasebandConnecter::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CBTBasebandConnecter::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	iParent.BTBaseband().TerminatePhysicalLink(0); //zam todo - check
	}




//----

CBTDisconnector* CBTDisconnector::NewL(CBluetoothPhysicalLinks& aParent)
//
//NewL: the CBluetoothPhysicalLinkssManager parent is provided. 
//It contains all the ESocky stuff and has suitable "Getters".
//
{
	CBTDisconnector* self = new (ELeave) CBTDisconnector(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();//self
	return self;
}


CBTDisconnector::CBTDisconnector(CBluetoothPhysicalLinks& aParent)
: CActive(CActive::EPriorityStandard),
 iParent(aParent)
//
//Constructor
//
	{
	}

void CBTDisconnector::ConstructL()
	{
	CActiveScheduler::Add(this);
	}

CBTDisconnector::~CBTDisconnector()
//
//Destructor
//
	{
	if(IsActive())
		{
		Cancel();
		}
	}

void CBTDisconnector::Disconnect(const TBTDevAddr& aBDAddr)
	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.BTBaseband().TerminatePhysicalLink(0, aBDAddr, iStatus);
	iCurrentRequest = EDisconnect;
	SetActive();
	}

void CBTDisconnector::DisconnectAll()

	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.BTBaseband().TerminateAllPhysicalLinks(iStatus);
	iCurrentRequest = EDisconnectAll;
	SetActive();
	}

void CBTDisconnector::DisconnectAllForPowerOff()

	{
	__ASSERT_ALWAYS(!IsActive(), Panic(EUnfinishedBusiness));

	iParent.BTBaseband().TerminateAllPhysicalLinksForPowerOff(iStatus);
	iCurrentRequest = EDisconnectAll;
	SetActive();
	}


void CBTDisconnector::RunL()
//
//When logical socket has connected (only async bit), 
//opens baseband socket.
//
	{
	//must come LAST .. deletes this active object
	switch (iCurrentRequest)
		{
		//zam todo do these cover all Read/Recv methods?
		case EDisconnect:
			//must come LAST .. deletes this active object
			iParent.HandleDisconnectCompleteL(iStatus.Int());
			break;
		case EDisconnectAll:
			//must come LAST .. deletes this active object
			iParent.HandleDisconnectAllCompleteL(iStatus.Int());
			break;
		default:
			Panic(EBadRequest);
			iCurrentRequest = ENone;
			break;
		};
	}

//
// Swallow any error from RunL
//
TInt CBTDisconnector::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}

void CBTDisconnector::DoCancel()
//
//Cancels current asynchronous requests.
//
	{
	//zam todo - more here...can we cancel a shutdown?
	iCurrentRequest = ENone;
	}

//
// Function Definitions for the M- Classes associated with CBluetoothPhysicalLinks 
//

/**
This virtual function allows the M- class to be extended in future in a binary
compatible way by providing a method that clients can override in future to
allow extra callbacks to be made via aObject.
*/
EXPORT_C void MBluetoothSynchronousLinkNotifier::MBSLN_ExtensionInterfaceL(TUid /*aInterface*/, void*& aObject)
	{
	aObject = NULL;
	}

/**
This virtual function allows the M- class to be extended in future in a binary
compatible way by providing a method that clients can override in future to
allow extra callbacks to be made via aObject.
*/
EXPORT_C void MBluetoothPhysicalLinksNotifier::MBPLN_ExtensionInterfaceL(TUid /*aInterface*/, void*& aObject)
	{
	aObject = NULL;
	}
	

/**
This virtual function allows the M- class to be extended in future in a binary
compatible way by providing a method that clients can override in future to
allow extra callbacks to be made via aObject.
*/
EXPORT_C void MBluetoothSocketNotifier::MBSN_ExtensionInterfaceL(TUid /*aInterface*/, void*& aObject)
	{
	aObject = NULL;
	}
	
// EOF
