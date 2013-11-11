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
// This file contains the implementation of the
// handling of MIP handovers in a CDMA2000 network.
// 
//

/**
 @file mipcdma.cpp
*/

#include "MipCdma.h"
#include "PSDAGTBase.h"
#include <comms-infras/nifprvar.h>

/**
@internalComponent
*/
const TInt K10Seconds = 10000000;

/**
@internalComponent
*/
_LIT(KTsyNameExtension,".tsy");

CMipCdmaHandoverHandler* CMipCdmaHandoverHandler::NewLC(CPSDAgent& aAgent, CCommsDbAccess& aDb)
/**
2 Phase constructor

This function creates the whole CDMA2000 MIP Handover handler object, including the subclass
CMIPCdmaEtelZoneChangeRequest.

@param aAgent Reference to owner of this class
@param aDb access to CommDB.
@exception Leaves if no memory is available
@return A newly created CMipCdmaHandoverHandler object
*/
	{
	CMipCdmaHandoverHandler* self = new(ELeave) CMipCdmaHandoverHandler(aAgent, aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMipCdmaHandoverHandler* CMipCdmaHandoverHandler::NewL(CPSDAgent& aAgent, CCommsDbAccess& aDb)
/**
2 Phase constructor

This function creates the whole MIP Handover handler object, including the subclass
CMIPCdmaEtelZoneChangeRequest.

@param aAgent Reference to owner of this class
@param aDb access to CommDB.
@exception Leaves if no memory is available
@return A newly created CMipCdmaHandoverHandler object
*/
	{
	CMipCdmaHandoverHandler* self = NewLC(aAgent, aDb);
	CleanupStack::Pop(); //self
	return self;
	}

CMipCdmaHandoverHandler::CMipCdmaHandoverHandler(CPSDAgent& aAgent, CCommsDbAccess& aDb) 
	: CActive(CActive::EPriorityStandard),
		iAgent(aAgent), iDb(aDb)
/**
Constructor

Adds itself to the Active Sceduler. Also initalizes all member data.

@param aAgent Reference to owner of this class
@param aDb access to CommDB.
*/
	{
	CActiveScheduler::Add(this);
	}

void CMipCdmaHandoverHandler::ConstructL()
/**
2nd phase Constructor

Creates an ETEL Zone change listener object.
Resets the timer object.

@exception Leaves if no memory is available
*/
	{
	//Create ETEL Zone Listener Object
	iMipCdmaEtelZoneChanger = CMIPCdmaEtelZoneChangeRequest::NewL(*this, iAgent, iDb);
	
	//Reset the timer (initialize the timer)
	(void)iTimer.CreateLocal();	
	}

CMipCdmaHandoverHandler::~CMipCdmaHandoverHandler()
/**
Destructor

Calls cancel on the Active object which will cancel the timer.
Deletes the ETEL Zone change listener object.
*/
	{
	Cancel();		//Cancel potential outstanding timer request.
	iTimer.Close();	//Needed to free resources on server side.
	delete iMipCdmaEtelZoneChanger;
	}

TInt CMipCdmaHandoverHandler::StartListening()
/**
This function is called when the link is brought up. It is responsible to 
start this object and start listening to incoming events from both PPP and ETEL.

@return Returns whether the starting of the object went ok or not.
*/
	{
	TInt err = KErrNone;
	TBool val;
	if(iAgent.ReadBool(TPtrC(CDMA_MIP), val)==KErrNone && val)
		{
		err = iMipCdmaEtelZoneChanger->StartListening();
		}
	return err;
	}

void CMipCdmaHandoverHandler::StopListening()
/**
This function is called when the link is brought down. It is responsible to 
stop this object and its listener.
*/
	{
	iMipCdmaEtelZoneChanger->StopListening();
	}

TBool CMipCdmaHandoverHandler::PPPLinkDownEvent()
/**
This function is called from the Reconnect() method in the CPSDAgent.
It will report back to the CPSDAgent if we are using MIP or not.
It will also start a timer that if it expires will force a manual reconnect of the link.
If an ETEL notification event comes in before, or has already come in, an autmatic reconnect takes place.

@return ETrue if we are using MIP else EFalse.
*/
	{
	TBool val;
	if(iAgent.ReadBool(TPtrC(CDMA_MIP), val)==KErrNone && val)
		{
		//We are using MIP
		iPPPLinkDownEvent = ETrue;
		if (iETELChangedZoneEvent)
			{
			Cancel();
			RestartPPP();
			}
		else
			{
			//If timer is already started, cancel it and start it again.
			Cancel();	//Will only cancel the timer if the AO is active.
			StartTimer();
			}		
		return ETrue;
		}
	else
		{
		//We are NOT using MIP
		return EFalse;
		}
	}

void CMipCdmaHandoverHandler::ETELChangedZoneEvent()
/**
This function is called from the RunL() method in the CMIPCdmaEtelZoneChangeRequest.
It will also start a timer that if it expires will force a manual reconnect of the link.
If an PPP Link Down event comes in before, or has already come in, an autmatic reconnect takes place.
*/
	{
	iETELChangedZoneEvent = ETrue;
	if (iPPPLinkDownEvent)
		{
		Cancel();
		// Notify nifman of the Changed Zone Event
		iAgent.AgentEvent(EEtelEvent,ECurrentNetworkChangeEvent,KNullDesC8);
		// Reconnect PPP
		RunL();
		}
	else
		{
		//If timer is already started, cancel it and start it again.
		Cancel();	//Will only cancel the timer if the AO is active.
		StartTimer();
		}
	}

void CMipCdmaHandoverHandler::StartTimer()
/**
Help function that will start the active object. E.g. it will start the timer and activate the object.
*/
	{
	TUint32 time;
	if(iAgent.ReadInt(TPtrC(CDMA_MIP_TIMEOUT), time)==KErrNone && time)
		{
		iTimer.After(iStatus, time);
		}
	else //default
		{
		iTimer.After(iStatus, K10Seconds); 
		}

	SetActive();
	}

void CMipCdmaHandoverHandler::RestartPPP()
/**
This function is responsible to do an automatic reconnect of the link. Will also "zero" the state variables
*/
	{
	iPPPLinkDownEvent = EFalse;
	iETELChangedZoneEvent = EFalse;

	//Notify client to restart PPP silently
	iAgent.MDPOReconnectComplete(KErrNone);
	}

void CMipCdmaHandoverHandler::DoCancel()
/**
Inherited from the CActive class. Will cancel the timer request.
*/
	{
	// Cancel the timer request
	iTimer.Cancel();
	}

void CMipCdmaHandoverHandler::RunL()
/**
Inherited from the CActive class. Will "zero" the state variables and restart the link manually.
*/
	{
	iPPPLinkDownEvent = EFalse;
	iETELChangedZoneEvent = EFalse;
	//Notify client to restart PPP as we would in a normal Simple IP scenario
	iAgent.Reconnect();
	}

CMIPCdmaEtelZoneChangeRequest* CMIPCdmaEtelZoneChangeRequest::NewLC(CMipCdmaHandoverHandler& aHandoverHandler, CPSDAgent& aAgent, CCommsDbAccess& aDb)
/**
2 Phase constructor

This function creates a ETEL Zone Change Listener object.

@param aHandoverHandler Reference to owner of this class
@param aAgent Reference to the Controller
@param aDb access to CommDB.
@exception Leaves if no memory is available
@return A newly created CMIPCdmaEtelZoneChangeRequest object
*/
	{
	CMIPCdmaEtelZoneChangeRequest* self = new(ELeave) CMIPCdmaEtelZoneChangeRequest(aHandoverHandler, aAgent, aDb);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMIPCdmaEtelZoneChangeRequest* CMIPCdmaEtelZoneChangeRequest::NewL(CMipCdmaHandoverHandler& aHandoverHandler, CPSDAgent& aAgent, CCommsDbAccess& aDb)
/**
2 Phase constructor

This function creates a ETEL Zone Change Listener object.

@param aHandoverHandler Reference to owner of this class
@param aAgent Reference to the Agent
@param aDb access to CommDB.
@exception Leaves if no memory is available
@return A newly created CMIPCdmaEtelZoneChangeRequest object
*/
	{
	CMIPCdmaEtelZoneChangeRequest* self = NewLC(aHandoverHandler, aAgent, aDb);
	CleanupStack::Pop(); //self
	return self;
	}

CMIPCdmaEtelZoneChangeRequest::CMIPCdmaEtelZoneChangeRequest(CMipCdmaHandoverHandler& aHandoverHandler, CPSDAgent& aAgent, CCommsDbAccess& aDb) 
	: CActive(CActive::EPriorityStandard), iFirstTime(ETrue),
	iHandoverHandler(aHandoverHandler), iAgent(aAgent), iDb(aDb),
	iPhoneNetworkInfoPckg(iAsyncCurrentNetwork)
/**
Constructor

Adds itself to the Active Sceduler. Also initalizes all member data.

@param aHandoverHandler Reference to owner of this class
@param aAgent Reference to the Agent
*/
	{
	CActiveScheduler::Add(this);
	}

void CMIPCdmaEtelZoneChangeRequest::ConstructL()
/**
2nd phase Constructor
*/
	{
	}

CMIPCdmaEtelZoneChangeRequest::~CMIPCdmaEtelZoneChangeRequest()
/**
Destructor

Calls cancel on the Active object which will cancel the outstanding request to ETEL.
Closes the ETEL phone and server.
*/	
	{
	StopListening();
	}

TInt CMIPCdmaEtelZoneChangeRequest::StartListening()
/**
This function is called when the link is brought up.
It is responsible to initiate and create a phone object.

@return Returns whether the starting of the object and phone went ok or not.
*/
	{
	if (!iStarted)
		{
		iStarted = ETrue;
		TRAPD(err, InitPhoneL());
		if(err != KErrNone)
			{
			return err;
			}

		//Start request for notification
		iFirstTime = ETrue;
		StartRequest();
		}
	return KErrNone;
	}

void CMIPCdmaEtelZoneChangeRequest::StopListening()
/**
This function is called when the link is brought down. It is responsible to stop
the phone object and the phone server 
*/
	{
	if(iStarted)
		{
		iStarted=EFalse;
		Cancel();

		iMmPhone.Close();
		(void)iTelServer.UnloadPhoneModule(*iTsyName);
		delete iTsyName;
		iTsyName = NULL;
		iTelServer.Close();
		}
	}

void CMIPCdmaEtelZoneChangeRequest::InitPhoneL()
/**
This function is help function that creates and intializes a phone object 

@exception Leaves if no memory is available or if initiation of phone or server failed.
*/
	{
	TFileName tsyName;
	iDb.GetTsyNameL(tsyName);
	if (tsyName.Right(4).CompareF(KTsyNameExtension) == 0)
		tsyName = tsyName.Left(tsyName.Length() - 4);

	// Connect to ETel
	(void)User::LeaveIfError(iTelServer.Connect());

	TInt err = iTelServer.LoadPhoneModule(tsyName);
	if(err == KErrNotFound)
		User::Leave(KErrNone);
	else if(err != KErrNone)
		User::Leave(err);

	// Remember the tsy name so it can be unloaded
	iTsyName = tsyName.AllocL();

	// Find out how many phones are supported e.g. how many TSYs
	TInt count = 0;
	(void)User::LeaveIfError(iTelServer.EnumeratePhones(count));
	if (count <= 0)
		User::Leave(KErrNotFound);

	RTelServer::TPhoneInfo info;
	TBool found = EFalse;

	// Loop through all the phones and find correct TSY
	for (TInt i = 0; i < count; i++)
		{
		TBuf<KCommsDbSvrMaxFieldLength> currentTsyName;
		(void)User::LeaveIfError(iTelServer.GetTsyName(i, currentTsyName));
		// If the loaded object has .tsy extension, compare the name
		// with aTsyName
		if (currentTsyName.Right(4).CompareF(KTsyNameExtension) == 0)
			currentTsyName = currentTsyName.Left(currentTsyName.Length() - 4);
		if (currentTsyName.CompareF(tsyName) == 0)
			{
			// Get phone info from the TSY
			(void)User::LeaveIfError(iTelServer.GetPhoneInfo(i, info));
			found = ETrue;
			break;
			}	
		}
	if (!found)
		User::Leave(KErrNotFound);
	// Open multimode phone object
	(void)User::LeaveIfError(iMmPhone.Open(iTelServer,info.iName));
	}

void CMIPCdmaEtelZoneChangeRequest::StartRequest()
/**
Trigger function. Will start the request for notifications for a zone change event from ETEL.

If it is the first time the object is called we will get the network information and store it locally
After the initial request we will only ask to get notfied when there is a change in network information.
*/	
	{
	if (iFirstTime)
		{
		//Get current netowrk status
		iMmPhone.GetCurrentNetwork(iStatus, iPhoneNetworkInfoPckg, iAsyncLocArea);
		}
	else
		{
		//start etel request for notifications
		iMmPhone.NotifyCurrentNetworkChange(iStatus, iPhoneNetworkInfoPckg, iAsyncLocArea);	
		}

	SetActive();
	}

void CMIPCdmaEtelZoneChangeRequest::DoCancel()
/**
Inherited from the CActive class. Will cancel the ETEL request.
*/
	{
	if (iFirstTime)
		{
		iMmPhone.CancelAsyncRequest(EMobilePhoneGetCurrentNetwork);
		}
	else
		{
		// Cancel the ETEL Request
		iMmPhone.CancelAsyncRequest(EMobilePhoneNotifyCurrentNetworkChange);
		}
	}

void CMIPCdmaEtelZoneChangeRequest::RunL()
/**
Inherited from the CActive class. Will notify the owner that a notification has ben received.
Will requeue itself, e.g. will re-request notifications from ETEL.
*/
	{
	if (iFirstTime)
		{
		iCurrentCdmaSID = iPhoneNetworkInfoPckg().iCdmaSID;
		iCurrentNetworkId = iPhoneNetworkInfoPckg().iNetworkId;
		iFirstTime = EFalse;
		}
	else
		{
		if ((iPhoneNetworkInfoPckg().iCdmaSID !=iCurrentCdmaSID) || (iPhoneNetworkInfoPckg().iNetworkId !=iCurrentNetworkId))
			iHandoverHandler.ETELChangedZoneEvent();
		}

	//Requeue
	StartRequest();
	}

