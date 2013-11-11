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
 @internalTechnology
*/

#ifndef SYMBIAN_SS_CONNECTIONSESSION_H
#define SYMBIAN_SS_CONNECTIONSESSION_H

#include <comms-infras/ss_metaconnprov.h>
#include <comms-infras/ss_connselect.h>
#include <comms-infras/ss_common.h>
#include <elements/sd_std.h>
#include <comms-infras/es_availability.h>
#include <es_prot.h>
#include <comms-infras/es_connectionservparameterbundle.h>

namespace ESock
{

const TUint KMaxNotificationQueueLength = 8; // seems to be a popular value round these parts


const TUint ETierStatusActivity = 1;
const TUint ETierNotificationActivity = 2;


/**
Server side representation of RConnectionServ.
@class CConnectionSession
@internalTechnology
*/
NONSHARABLE_CLASS(CConnectionSession) : public Den::CWorkerSession, public MProvdSecurityChecker, public Messages::ASimpleNodeIdBase
	{
public:
	static CConnectionSession* NewL(TProcessId aProcessId, TUidType aUid, Den::TSessionUniqueId aSessionUniqueId);
	virtual ~CConnectionSession();

protected:
	CConnectionSession(TUidType aUid, Den::TSessionUniqueId aSessionUniqueId);

protected:
	void CompleteMessage(const RMessage2& aMessage, TInt aResult);

protected:
	void Error(TInt aError);

	virtual void ReceivedL(const Messages::TRuntimeCtxId& aSender, const Messages::TNodeId& aRecipient, Messages::TSignatureBase& aMessage);

	void ProviderUp(const Messages::TNodeId& aProvider);

private:
    // Client message implementations
	void StartAttachToTierManager(const RMessage2& aMessage);

	void EnumerateConnectionsL();
	void GetConnectionInfoL();

	// Availability
	void RegisterForAvailabilityL();
	void CancelAvailability(TInt aReason = KErrCancel);
	void ProcessAvailabilityL(Messages::TSignatureBase& aCFMessage);
	void ProcessError(TUint16 aActId, Messages::TSignatureBase& aCFMessage);
	void ProcessError(TUint16 aActId, TInt aErrCode);

	// Availability helpers
	void PopAvailabilityQueue();

	const Messages::TNodeId& ServiceProvider() const
	{
    	return iTierManager;
	}

	inline const Messages::TNodeId& ServiceProviderL() const
		{
		const Messages::TNodeId& sp = ServiceProvider();
		if(sp.IsNull())
			{
			// must be attached to something!
			//  log something here
			User::Leave(KErrNotReady);
			}
		return sp;
		}


	void LeaveServiceProvider();
    void HandleTierStatus(const Messages::TSignatureBase& aCFMessage);
    void HandleTierNotification(const Messages::TSignatureBase& aCFMessage);
	void AttachToTierManager(const Messages::TNodeId& aTierManager);

	virtual void CommsApiExtBindIfaceL(const RMessage2& aMessage);
	virtual void CommsApiExtIfaceSendReceiveL(const RMessage2& aMessage);
	virtual void CloseExtensionInterface(const RMessage2& aMessage);
	virtual void CancelAndCloseAllClientExtIfaces();

	void PanicClient(TESockPanic aPanic);
	static void PanicClient(const RMessage2& aMessage,TESockPanic aPanic);

	virtual void ServiceL(const RMessage2& aMessage);

public:
	// Methods implementing MProvdSecurityChecker methods
	virtual TInt CheckPolicy(const TSecurityPolicy& aPolicy, const char *aDiagnostic);

private:
	TUint iTierId;
	Messages::TNodeId iTierManager; // associated tier manager (if any)

	RMessage2 iAttachMessage;
	RMessage2 iStatusMessage;
	CRefCountOwnedParameterBundle* iStatusBundle;

	TBool iNotificationActive;
	RMessage2 iNotificationMessage;
	RPointerArray<CRefCountOwnedParameterBundle> iNotificationQueue; // queue of availability events received
	};

} // namespace ESock

#endif
//SYMBIAN_SS_CONNECTIONSESSION_H

