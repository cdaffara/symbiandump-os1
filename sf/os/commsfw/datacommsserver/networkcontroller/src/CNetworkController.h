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
*
*/



/**
 @file CNetworkController.h
 @internalComponent
*/

#if !defined (__CNETWORKCONTROLLER_H__)
#define __CNETWORKCONTROLLER_H__
#include <comms-infras/cnetworkcontrollerbase.h>
#include <comms-infras/dialogprocessor.h>
#include "CSelectionRequest.h"
#include "CReconnectRequest.h"

class MNetConDialogProcAccess;
class MNetConEnv
/**
For upcalls from the requests

@internalComponent
*/
	{
public:

	/**
	A selection request has been processed - queue another one.

	@param aRequest pointer to the actual selection request
	@param aError contains the reason for failure of processing the request or KErrNone
	*/
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError) = 0;

	/**
	A reconnect request has been processed - queue another one.

	@param aRequest pointer to the actual reconnect request
	@param aError contains the reason for failure of processing the request or KErrNone
	*/
	virtual void RequestComplete(const CReconnectRequest* aRequest, TInt aError) = 0;

	/**
	Retrieve the name of the Agent that is used for implicit connection
	requests - i.e. connection startup from RSocket SendTo()/Connect() or
	RHostResolver GetByName()

	@returns an Agent name
	*/
	virtual const HBufC* ImplicitConnectionAgentName() const = 0;

	/**
	Retrieve the connection preferences associated with the current
	implicit connection

	@returns a TConnPref reference
	*/
	virtual const TConnPref& ImplicitConnectionPrefs() const = 0;

	/**
	Searches for an Agent associated with the selected IAP
	@note that there is a one-to-one relationship between agents and IAPs

	@param aAgent if found a pointer to the CNifAgentBase
	@return TInt KErrNotFound or KErrNone
	@pre the selected IAP setting is stored in the DbAccess object.
	*/
	virtual TInt FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess* aDatabase) const = 0;

	/**
	Check Bearer Availability

	@param aIsReconnect if this is a reconnection then any asynchronous requests are skipped in order to speed things up
	*/
	virtual void CheckBearerAvailability(TBool aIsReconnect) = 0;

	/**
	Check Second Phase Bearer Availability - e.g. signal strength

	@returns KErrNone if the bearer availability is above the threshold stored in CommDB, otherwise KErrNetConInadequateSignalStrengh
	*/
	virtual TInt RequestSecondPhaseAvailability() = 0;

	/**
	Create an association between an Agent and a Network ID.

	@param aAgent pointer to the Agent
	@param aNetworkId the CommDb ID of the Network
	*/
	virtual void AddAgentToNetworkL(CNifAgentBase* aAgent, TUint32 aNetworkId) = 0;

	/**
	CancelBearerAvailabilityCheck
	*/
	virtual void CancelBearerAvailabilityCheck() = 0;

	/**
	Factory function to provide a database access object.
	*/
	virtual CCommsDbAccess* NewDatabaseL() = 0;
	/**
	Factory function to proviae a dialog processor
	*/
	virtual CNetConDlgProcessor* NewDialogProcessorL() = 0;

	};

const TInt KTelBearerPosition = 0; ///< position of the telephony bearer in the array

NONSHARABLE_CLASS(CNetworkController) : public CNetworkControllerBase, public MNetConEnv
/**
The class is responsible for Queing Selection and Reconnection requests received from NIFMAN in a FIFO queue of CNetConRequestBase objects

@internalComponent
*/
	{
public:
	virtual ~CNetworkController();
	static CNetworkController* NewL();

	// calls from Nifman 
	virtual void SelectAgent(MNetworkControllerObserver* aObserver, MServiceChangeObserver* apServiceChangeObserver, TConnStartType aStartType, TInt aConnectionAttempt, TInt aLastConnectionError);
	virtual void SelectAgent(MNetworkControllerObserver* aObserver, MServiceChangeObserver* apServiceChangeObserver, TConnStartType aStartType, TInt aConnectionAttempt, TInt aLastConnectionError, const TConnPref& aPrefs);
	virtual void Reconnect(MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent);
	virtual void Reconnect(MNetworkControllerObserver* aObserver, CNifAgentBase* aAgent, CStoreableOverrideSettings* aOverrides);
	virtual TInt CancelRequest(MNetworkControllerObserver* aObserver);
	virtual void AgentConnectionFailure(CNifAgentBase* aAgent, TInt aError);
	virtual TInt RequestImplicitConnectionNotification(MImplicitConnectionNotify* aObserver);
	virtual void CancelImplicitConnectionNotification(MImplicitConnectionNotify* aObserver);

	// implementation of MBearerObserver interface 
	virtual void BearerStateChange(CBearerBase* aBearer);

	virtual CCommsDbAccess* DbAccess();

	// implementation of MNetConEnv interface
	virtual void RequestComplete(const CSelectionRequest* aRequest, TInt aError);
	virtual void RequestComplete(const CReconnectRequest* aRequest, TInt aError);
	virtual const HBufC* ImplicitConnectionAgentName() const;
	virtual const TConnPref& ImplicitConnectionPrefs() const;
	virtual TInt FindExistingAgentForSelection(CNifAgentBase*& aAgent, CCommsDbAccess* aDatabase) const;
	virtual void CheckBearerAvailability(TBool aIsReconnect);
	virtual TInt RequestSecondPhaseAvailability();
	virtual void AddAgentToNetworkL(CNifAgentBase* aAgent, TUint32 aNetworkId);
	virtual void CancelBearerAvailabilityCheck();

	virtual CCommsDbAccess* NewDatabaseL();
	virtual CNetConDlgProcessor* NewDialogProcessorL();

protected:
	CNetworkController();
	void ConstructL();

private:
	void SelectComplete(TInt aError, const TConnectionSettings& aSettings);
	static TInt ProcessRequestCb(TAny* aThisPtr);
	void ProcessRequest();
	static TInt ImplicitNotificationCb(TAny* aThisPtr);
	void SendImplicitConnectionNotification(const TDesC8& aConnectionInfo, MImplicitConnectionNotify::TEvent aEvent);
	TInt GetConnectionInfo(CNifAgentBase* aAgent, TDes8& aConnectionInfo);

private:
	CNetConRequestBase* iCurrentRequest;
	CAsyncCallBack* iProcessRequestCb;
	TSglQue<CNetConRequestBase> iRequestQueue;
	RPointerArray<MImplicitConnectionNotify> iImplicitNotifyList;
	TConnPref iImplicitConnectionPrefs;
	HBufC* iImplicitConnectionAgentName;
	CAsyncCallBack* iImplicitNotificationCb;
	TPckgBuf<TSoIfConnectionInfo> iImplicitNotifyInfo;
	MImplicitConnectionNotify::TEvent iImplicitNotifyEvent;
	};

#endif

