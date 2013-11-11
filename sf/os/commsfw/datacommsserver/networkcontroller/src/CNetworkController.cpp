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
//

/**
 @file CNetworkController.cpp
*/

#include "CNetworkController.h"
#include <comms-infras/nifagt.h>
#include <comms-infras/nifif.h>
#include <cdblen.h>
#include "NetConPanic.h"
#include "NetConLog.h"
#include "CTelBearer.h"
#include "CNetConDlgProcessor.h"

CNetworkController::~CNetworkController()
/**
Destuctor

Implementation of CNetworkController
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("~CNetworkController()")); )

	if (iImplicitNotificationCb)
		{
		delete iImplicitNotificationCb;
		}

	iImplicitNotifyList.Close();

	if(iImplicitConnectionAgentName)
		{
		delete iImplicitConnectionAgentName;
		iImplicitConnectionAgentName = NULL;
		}

	// delete all CNetConRequestBase objects
	while(!iRequestQueue.IsEmpty())
		{
		CNetConRequestBase* request = iRequestQueue.First();
		iRequestQueue.Remove(*request);
		delete request;
		}
	iRequestQueue.Reset();

	if (iCurrentRequest)
		{
		iCurrentRequest->CancelRequest();
		delete iCurrentRequest;
		iCurrentRequest = NULL;
		}

	if (iProcessRequestCb)
		{
		delete iProcessRequestCb;
		}


	LOG ( NetConLog::Printf(_L("--------------- Network Controller Finished ---------------")); )
	}

CNetworkController* CNetworkController::NewL()
/**
Factory function

@return the new CNetworkController object
@exception leaves if memory allocation fails
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::NewL()")); )
	CNetworkController* self = new(ELeave) CNetworkController();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

CNetworkController::CNetworkController()
/**
Constructor
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController()")); )

	iRequestQueue.SetOffset(_FOFF(CNetConRequestBase, iLink));
	}

void CNetworkController::ConstructL()
/**
2nd phase of construction

@exception leaves if callback, database, dialog processor or bearer construction fails
*/
	{
	LOG ( NetConLog::Printf(_L("--------------- Network Controller Starting ---------------")); )
	LOG_DETAILED( NetConLog::Printf(_L("Detailed logging enabled")); )
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::ConstructL()")); )

	// create a callback for processing requests
	TCallBack callback(ProcessRequestCb, this);
	iProcessRequestCb = new(ELeave) CAsyncCallBack(callback, CActive::EPriorityStandard);

	// there is no existing implicitly started connection
	iImplicitConnectionAgentName = NULL;

	// set up callback for implicit connection event notification
	TCallBack implicitCallBack(ImplicitNotificationCb, this);
	iImplicitNotificationCb = new(ELeave) CAsyncCallBack(implicitCallBack, CActive::EPriorityStandard);

	// set up telephony bearer with this object as observer and dbaccess
	// note that all bearers are deleted in the CNetworkControllerBase d'tor
	CTelBearer* telBearer = CTelBearer::NewLC(this);
	AddBearerL(telBearer);
	CleanupStack::Pop(); // telBearer

	// ensure that the telephony bearer was added where we expect
	__ASSERT_DEBUG(iBearers[KTelBearerPosition] == telBearer, NetConPanic(NetworkController::ENetConTelBearerMissing));
	}

TInt CNetworkController::FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess* aDatabase) const
/**
Searches for an Agent associated with the selected IAP

@note that there is a one-to-one relationship between agents and IAPs
@param aAgent if found a pointer to the CNifAgentBase
@return KErrNotFound if the agent with selected IAP is not found else KErrNone
@pre the selected IAP setting is stored in the DbAccess object.
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::FindExistingAgentForSelection()")); )

	// read the CommDb ID of the current IAP
	TUint32 iapId(0);
	TInt err(KErrNone);

	TRAP(err, aDatabase->GetIntL(TPtrC(IAP), TPtrC(COMMDB_ID), iapId));
	if (err != KErrNone)
		{
		return KErrNotFound;
		}

	// read network ID of the current IAP
	TUint32 networkId(0);
	TRAP(err, aDatabase->GetIntL(TPtrC(IAP), TPtrC(IAP_NETWORK), networkId));
	if (err != KErrNone)
		{
		return KErrNotFound;
		}

	// find the network
	CNetwork* network = NULL;
	err = FindNetworkById(networkId, network);
	if (err!=KErrNone)
		{
		return err;
		}

	// ask network to find the agent
	return network->FindAgentByIap(iapId, aAgent);
	}

void CNetworkController::SelectAgent(MNetworkControllerObserver* aObserver, MServiceChangeObserver* apServiceChangeObserver, TConnStartType aStartType, TInt aConnectionAttempt, TInt aLastConnectionError, const TConnPref& aPrefs)

/**
Adds a selection request to the queue and calls the Process Request Callback
This allows the current callstack to unwind.  Processing is resumed at ProcessRequestCb

@param aObserver the pointer to the MNetworkControllerObserver that requested the selection
@param apServiceChangeObserver that requests service change notification
@param aStartType how this connection request was started - either Implicit or Explicit
@param aConnectionAttempt the connection attempt
@param aLastConnectionError if this is not the first connection attempt the error from the last connection
@param aPrefs the connection preferences to use in preference(?!) to the ones stored in commdb
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::SelectAgent()")); )

	// create a new request and add it to the queue
	CNetConRequestBase* newRequest = NULL;
	TRAPD(err, newRequest = CSelectionRequest::NewL(this, aObserver, aStartType, aPrefs, aConnectionAttempt, aLastConnectionError));
	if(err!=KErrNone)
		{
		aObserver->SelectComplete(err);
		return;
		}

    newRequest->ipServiceChangeObserver = apServiceChangeObserver;
	iRequestQueue.AddLast(*newRequest);

	LOG( NetConLog::Printf(_L("\tAdded request [0x%08x] to queue"), newRequest); )

	// start to process request if not already processing
	if(!iCurrentRequest)
		{
		iProcessRequestCb->CallBack();
		}
	}

void CNetworkController::SelectAgent(MNetworkControllerObserver* aObserver, MServiceChangeObserver* apServiceChangeObserver, TConnStartType aStartType, TInt aConnectionAttempt, TInt aLastConnectionError)

/**
SelectAgent

@param aObserver the pointer to the MNetworkControllerObserver that requested the selection
@param apServiceChangeObserver that requests service change notification
@param aStartType how this connection request was started - either Implicit or Explicit
@param aConnectionAttempt the connection attempt
@param aLastConnectionError if this is not the first connection attempt the error from the last connection
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::SelectAgent2()")); )

	// construct some default connection preference - this will cause the default commdb settings to be used
	TConnPref prefs;

	SelectAgent(aObserver, apServiceChangeObserver, aStartType, aConnectionAttempt, aLastConnectionError, prefs);
	}

void CNetworkController::Reconnect(MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent)
/**
Reconnect

@param aObserver the pointer to the MNetworkControllerObserver that requested the selection
@param aAgent,the agent who's connection failed
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::Reconnect()")); )

	Reconnect(aObserver, aAgent, NULL);
	}

void CNetworkController::Reconnect(MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides)
/**
Adds a reconnect request to the queue and calls the Process Request Callback
This allows the current callstack to unwind.  Processing is resumed at ProcessRequestCb

@param aObserver the pointer to the MNetworkControllerObserver that requested the selection
@param aAgent,the agent who's connection failed
@param aOverrides,pointer to the override settings to store.
@note that ownership of these overrides is retained by the caller.
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::Reconnect2()")); )

	CNetConRequestBase* newRequest = NULL;
	TRAPD(err, newRequest = CReconnectRequest::NewL(this, aObserver, aAgent, aOverrides);)
	if(err!=KErrNone)
		{
		aObserver->ReconnectComplete(err);
		return;
		}

	iRequestQueue.AddLast(*newRequest);

	LOG( NetConLog::Printf(_L("\tAdded request [0x%08x] to queue"), newRequest); )

	// start to process request if not already processing
	if(!iCurrentRequest)
		{
		iProcessRequestCb->CallBack();
		}
	}

TInt CNetworkController::CancelRequest(MNetworkControllerObserver* aObserver)
/**
Cancel the request made by aObserver.
If this request is currently being processed then stop processing immediately.

@param aObserver pointer to the object that initiated the request
@return KErrNone if the request was found and cancelled ok otherwise KErrNotFound
 */
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::CancelRequest()")); )

	TInt retval(KErrNotFound);
	CNetConRequestBase* request = NULL;

	// find the request to cancel
	if (iCurrentRequest && iCurrentRequest->Observer() == aObserver)
		{
		LOG( NetConLog::Printf(_L("\tCancelled request [0x%08x]"), iCurrentRequest); )

		// cancel the active request - this will cancel all bearer checking and any dialog box activity
		iCurrentRequest->CancelRequest();

		// delete the request
		delete iCurrentRequest;
		iCurrentRequest = NULL;

		// trigger callback to process next request in the queue
		iProcessRequestCb->CallBack();

		retval = KErrNone;
		}
	else
		{
		TSglQueIter<CNetConRequestBase> iter(iRequestQueue);

		while ((request = iter++) != NULL)
			{
			if (request->Observer() == aObserver)
				{
				// remove request from queue
				iRequestQueue.Remove(*request);

				LOG( NetConLog::Printf(_L("\tCancelled request [0x%08x]"), request); )

				// note: don't call Cancel() on this request as it is not active

				// delete the request
				delete request;
				request = NULL;

				retval = KErrNone;
				break;
				}
			}
		}

	return retval;
	}

void CNetworkController::AgentConnectionFailure(CNifAgentBase* aAgent, TInt aError)
/**
A connection has failed.

@param aAgent the agent who's connection failed
@param aError the reason for the failure
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::AgentConnectionFailure()")); )

	if(!aAgent)
		{
		return;
		}

	// retrieve agent name
	TNifAgentInfo info;
	aAgent->Info(info);

	LOG( NetConLog::Printf(_L("\tAgent '%S' has disconnected with error %d"), &info.iName, aError); )

	// remove warning - the error code is not used in all builds
	(void)aError;

	// if the agent was the one for implicit connections
	// then remove the stored name
	if(iImplicitConnectionAgentName && info.iName == *iImplicitConnectionAgentName)
		{
		delete iImplicitConnectionAgentName;
		iImplicitConnectionAgentName = NULL;

		(void)GetConnectionInfo(aAgent, iImplicitNotifyInfo);
		iImplicitNotifyEvent = MImplicitConnectionNotify::EImplicitConnectionDown;
		iImplicitNotificationCb->CallBack();
		}

	TInt i(0);
	// remove this agent from any bearers
	for (i=0; i<iBearers.Count(); ++i)
		{
		CBearerBase* bearer = iBearers[i];
		// try and remove the agent from this bearer - ignore Not Found error
		TRAPD(err, bearer->RemoveAgentL(aAgent));
		if (KErrNone != err)
			{	
			LOG( NetConLog::Printf(_L("\tERROR: Agent has disconnected from bearer with error %d"), err); )
			}
		}

	// remove this agent from its network
	for (i=0; i<iNetworks.Count(); ++i)
		{
		CNetwork* network = iNetworks[i];
		// try and remove the agent from this network - ignore Not Found error
		TRAPD(err, network->RemoveAgentL(aAgent));
		if (KErrNone != err)
			{	
			LOG( NetConLog::Printf(_L("\tERROR: Agent has disconnected from network with error %d"), err); )
			}
		}
	}

TInt CNetworkController::GetConnectionInfo(CNifAgentBase* aAgent, TDes8& aConnectionInfo)
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::GetConnectionInfo()")); )

	_LIT(KIapId, "IAP\\Id");
	_LIT(KNetId, "IAP\\IAPNetwork");

	TSoIfConnectionInfo info;
	TInt err = aAgent->ReadInt(KIapId(), info.iIAPId);
	if(err!=KErrNone)
		{
		return err;
		}

	err = aAgent->ReadInt(KNetId(), info.iNetworkId);
	if(err!=KErrNone)
		{
		return err;
		}


	aConnectionInfo = TPckg<TSoIfConnectionInfo>(info);
	return KErrNone;
	}

TInt CNetworkController::ProcessRequestCb(TAny* aThisPtr)
/**
ProcessRequestCb

@param aThisPtr the pointer to the object that initiated the callback
@return KErrNone always
*/
	{
	CNetworkController* self = static_cast<CNetworkController*>(aThisPtr);
	self->ProcessRequest();
	return KErrNone;
	}

void CNetworkController::ProcessRequest()
/**
ProcessRequest
Deque a request and start to process it.
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::ProcessRequest()")); )
	LOG (
		if (iCurrentRequest)
			NetConLog::Printf(_L("\tCurrently processing request [0x%08x]"), iCurrentRequest);
		else
			NetConLog::Printf(_L("\tNot currently processing any request"));

		if (!iRequestQueue.IsEmpty())
			NetConLog::Printf(_L("\tSome requests are queued"));
		else
			NetConLog::Printf(_L("\tRequest queue is empty"));
		)

	if (iCurrentRequest)
		{
		return;	// we are already processing a request
		}

	if (!iRequestQueue.IsEmpty())
		{
		iCurrentRequest = iRequestQueue.First();
		iRequestQueue.Remove(*iCurrentRequest);
		LOG( NetConLog::Printf(_L("--------------- Starting to process request [0x%08x] ---------------"), iCurrentRequest); )
		iCurrentRequest->StartRequest();
		}
	}

void CNetworkController::RequestComplete(const CSelectionRequest* aRequest, TInt aError)
/**
A selection request has been processed - queue another one.

@param aRequest pointer to the actual selection request
@param aError contains the reason for failure of processing the request or KErrNone
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::RequestComplete()")); )
	__ASSERT_ALWAYS((CNetConRequestBase*)aRequest == iCurrentRequest, NetConPanic(NetworkController::ENetConBadRequestCallback));

	LOG( NetConLog::Printf(_L("--------------- Finished processing request [0x%08x] ---------------"), iCurrentRequest); )

	// Trigger callback to process next request in the queue
	iProcessRequestCb->CallBack();

	// if this was an implicit request then store the name of the agent for other implicit requests
	if (aError == KErrNone && aRequest->ConnectionStartType() == EConnStartImplicit)
		{
		if(!iImplicitConnectionAgentName)
			{
			TRAPD(err, iImplicitConnectionAgentName = (aRequest->AgentName()).AllocL());
			if (err != KErrNone)
				{
				aError = err;
				}
			else
				{
				iImplicitConnectionPrefs = aRequest->ConnPrefs();
				iImplicitNotifyInfo = aRequest->AgentConnectionInfo();
				iImplicitNotifyEvent = MImplicitConnectionNotify::EImplicitConnectionUp;
				iImplicitNotificationCb->CallBack();
				}
			}
		else
			{
			ASSERT(*iImplicitConnectionAgentName == aRequest->AgentName());
			}
		}

	ASSERT(aRequest->Observer());

	// signal the observer that the selection is complete
	if(aError==KErrNone)
		{
		aRequest->Observer()->SelectComplete(aRequest->AgentName());
		}
	else
		{
		aRequest->Observer()->SelectComplete(aError);
		}

	delete iCurrentRequest;
	iCurrentRequest = NULL;

	CancelBearerAvailabilityCheck();
	}

void CNetworkController::RequestComplete(const CReconnectRequest* aRequest, TInt aError)
/**
RequestComplete

A reconnect request has been processed - queue another one.

@param aRequest pointer to the actual reconnect request
@param aError contains the reason for failure of processing the request or KErrNone
*/
	{
	__ASSERT_ALWAYS( (CReconnectRequest*)aRequest == iCurrentRequest, NetConPanic(NetworkController::ENetConBadRequestCallback));

	LOG_DETAILED ( NetConLog::Printf(_L("CNetworkController::RequestComplete(CReconnectRequest aRequest = 0x%08x, aError = %d)"), aRequest, aError); )
	LOG( NetConLog::Printf(_L("--------------- Finished processing request [0x%08x] ---------------"), iCurrentRequest); )

	// Trigger callback to process next request in the queue
	iProcessRequestCb->CallBack();

	// signal the observer that the selection is complete
	aRequest->Observer()->ReconnectComplete(aError);

	delete iCurrentRequest;
	iCurrentRequest = NULL;
	}

TInt CNetworkController::RequestImplicitConnectionNotification(MImplicitConnectionNotify* aObserver)
/**
Register for implicit connection event notification
Implicit Connection Notification

@param aObserver to notify of event changes
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::RequestImplicitConnectionNotification()")); )

	if(iImplicitConnectionAgentName)
		{
		aObserver->ImplicitConnectionEvent(iImplicitNotifyInfo, MImplicitConnectionNotify::EImplicitConnectionUp);
		}

	return iImplicitNotifyList.Append(aObserver);
	}

void CNetworkController::CancelImplicitConnectionNotification(MImplicitConnectionNotify* aObserver)
/**
Deregister for implicit connection event notification

@param aObserver to notify of event changes
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::CancelImplicitConnectionNotification()")); )

	TInt index = iImplicitNotifyList.Find(aObserver);
	if(index>=0)
		{
		iImplicitNotifyList.Remove(index);
		}
	}

TInt CNetworkController::ImplicitNotificationCb(TAny* aThisPtr)
/**
Static callback function

@param aThisPtr the pointer to the instance of this class that triggered the callback
*/
	{
	CNetworkController* self = STATIC_CAST(CNetworkController*, aThisPtr);
	self->SendImplicitConnectionNotification(self->iImplicitNotifyInfo, self->iImplicitNotifyEvent);
	return KErrNone;
	}

void CNetworkController::SendImplicitConnectionNotification(const TDesC8& aConnectionInfo, MImplicitConnectionNotify::TEvent aEvent)
/**
Notify all registered observers about the implicit connection event

@param aConnectionInfo a TSoIfConnectionInfo describing the implicit connection IapId and NetId
@param aEvent either Up or Down
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::SendImplicitConnectionNotification()")); )

	for(TInt i=0; i<iImplicitNotifyList.Count(); ++i)
		{
		iImplicitNotifyList[i]->ImplicitConnectionEvent(aConnectionInfo, aEvent);
		}
	}

//
//                                              //
// Bearer Availability Checking                 //
//                                              //
//

void CNetworkController::CheckBearerAvailability(TBool aIsReconnect)
/**
Check Bearer Availability

@param aIsReconnect if this is a reconnection then any asynchronous requests are skipped in order to speed things up
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::CheckBearerAvailability()")); )

	// ensure that the telephony bearer is in the array
	__ASSERT_DEBUG(iBearers[KTelBearerPosition], NetConPanic(NetworkController::ENetConTelBearerMissing));

	// ask the telephony bearer to start checking for availability
	TBool ret = iBearers[KTelBearerPosition]->StartChecking(aIsReconnect);
	__ASSERT_DEBUG(ret, NetConPanic(NetworkController::ENetConTelBearerAlreadyChecking));
	(void)ret;  // remove warning in release builds
	}

void CNetworkController::CancelBearerAvailabilityCheck()
/**
CancelBearerAvailabilityCheck
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::CancelBearerAvailabilityCheck()")); )

	// ensure that the telephony bearer is in the array
	__ASSERT_DEBUG(iBearers[KTelBearerPosition], NetConPanic(NetworkController::ENetConTelBearerMissing));

	iBearers[KTelBearerPosition]->StopChecking();
	}

/**
   Construct a new database accessor
   @returns the new database accessor
*/
CCommsDbAccess* CNetworkController::NewDatabaseL()
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::NewDatabaseL()")); )

	CCommsDbAccess *database = CCommsDbAccess::NewL(ETrue);
	ASSERT( database );
	return database;
	}


/**
   Construct a new dialog processor
   @returns the new dialog processor
*/
CNetConDlgProcessor* CNetworkController::NewDialogProcessorL()
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::NewDialogProcessorL()")); )

	return CNetConDlgProcessor::NewL();
	}


TInt CNetworkController::RequestSecondPhaseAvailability()
/**
Check Second Phase Bearer Availability - e.g. signal strength

@returns KErrNone if the bearer availability is above the threshold stored in CommDB, otherwise KErrNetConInadequateSignalStrengh
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::RequestSecondPhaseAvailability()")); )

	CBearerBase* const telBearer = iBearers[KTelBearerPosition];

	// ensure that the telephony bearer is in the array
	__ASSERT_DEBUG(telBearer, NetConPanic(NetworkController::ENetConTelBearerMissing));

	// ask the telephony bearer to start checking for availability
	TInt availability = telBearer->SecondPhaseAvailability();

	// we have to tell the bearer to stop checking for availability here, otherwise
	// subsequent availability requests will fail
	// the logic is that every selection request triggers a *new* bearer availabilty
	// check - in a one box device this could be optimised to a single check.
	telBearer->StopChecking();
	return availability;
	}

void CNetworkController::BearerStateChange(CBearerBase* aBearer)
/**
The bearer set available has changed - update the current
request


Implementation of MBearerObserver Interface

@param aBearer the bearer who's availability has changed
@note that this is quite a simple implementation
of bearer availability checking because we only
have a single bearer.  If further bearers are
implemented then a 'bearer manager' class may
be used to take the responsibility of retrieveing
the availability of all bearers and then returing
to the network controller
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::BearerStateChange()")); )

	__ASSERT_DEBUG(iCurrentRequest, NetConPanic(NetworkController::ENetConNoCurrentRequest));

	// fetch the available bearer set from the telephony bearer
	TUint32 availableBearerSet(aBearer->AvailableBearerSet());

	// the LAN bearer is assumed to be always available so mask it in
	availableBearerSet |= (KCommDbBearerLAN|KCommDbBearerVirtual);

	// update the current request
	iCurrentRequest->SetAvailableBearers(availableBearerSet);
	}

CCommsDbAccess* CNetworkController::DbAccess()
/** Provide a pointer to the CCommsDbAccess class to facilitate access to commdb
@return CCommsDbAccess
*/
 	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::DbAccess()")); )

 	ASSERT(iCurrentRequest);
 	ASSERT(iCurrentRequest->DbAccess());
 	return iCurrentRequest->DbAccess();
 	}

//
//                        //
//  MNetConEnv Interface  //
//                        //
//

const HBufC* CNetworkController::ImplicitConnectionAgentName() const
/**
Retrieve the name of the Agent that is used for implicit connection
requests - i.e. connection startup from RSocket SendTo()/Connect() or
RHostResolver GetByName()

@returns an Agent name
*/
	{
    LOG_DETAILED(
        if (iImplicitConnectionAgentName)	    
            NetConLog::Printf(_L("CNetworkController::ImplicitConnectionAgentName() %S"), iImplicitConnectionAgentName);
        else
            NetConLog::Printf(_L("CNetworkController::ImplicitConnectionAgentName() returns NULL"));
        )

	return iImplicitConnectionAgentName;
	}

const TConnPref& CNetworkController::ImplicitConnectionPrefs() const
/**
Retrieve the connection preferences associated with the current
implicit connection

@returns a TConnPref reference
*/
	{

	return iImplicitConnectionPrefs;
	}

void CNetworkController::AddAgentToNetworkL(CNifAgentBase* aAgent, TUint32 aNetworkId)
/**
Create an association between an Agent and a Network ID.

@param aAgent pointer to the Agent
@param aNetworkId the CommDb ID of the Network
*/
	{
	LOG_DETAILED( NetConLog::Printf(_L("CNetworkController::AddAgentToNetworkL()")); )
	
	// Is the network already available?
	CNetwork* network = NULL;
	TInt findErr = FindNetworkById(aNetworkId, network);

	// ...if not create a new network
	if(findErr!=KErrNone)
		{
		network=CNetwork::NewLC(aNetworkId, this);
		AddNetworkL(network);
		CleanupStack::Pop();  // network
		}

	// add this agent to the network.
	network->AddAgentL(aAgent);

	LOG( NetConLog::Printf(_L("\tAdded agent [0x%08x] to Network %d [0x%08x]"), aAgent, aNetworkId, network); )
	}

