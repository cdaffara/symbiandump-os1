// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file defines the class for the MO-LR protocol state machine.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __SUPLMTLRFSMSESSION_H__
#define __SUPLMTLRFSMSESSION_H__

#include "suplfsmsessionbase.h"
#include <lbs/lbshostsettings.h>
#include "suplinit.h"

/** MT-LR protocol state machine.
This class manages the MT-LR protocol procedure and is employed when the protocol
module receives a SUPL INIT message.

The class implements the pure virtual methods defined by CSuplFsmSessionBase.
@see CSuplFsmSessionBase

The class also implements a timer observer interface defined by MLbsCallbackTimerObserver
@see MLbsCallbackTimerObserver
*/
NONSHARABLE_CLASS(CSuplMtlrFsmSession) : public CSuplFsmSessionBase
	{
		
public:

	static CSuplMtlrFsmSession* NewL(MSuplFsmSessionObserver& aObserver);
	virtual ~CSuplMtlrFsmSession();
	
	// CSuplFsmSessionBase pure virtual methods
	TBool SelectNextState(TBool& aForceRedo);
	void PostTransition();
	void CancelProcedure();
	//void DoQueuedRequest(TInt aRequest);

	// Called by the Protocol Manager (events from LBS)
	void MtLrReq(const TLbsNetSessionId& aSessionId, const CSuplInit* aSuplInit);
	void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo);
	void PrivacyResp(const TLbsNetSessionId& aSessionId, const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResponse);
	void AssistanceDataReq(const TLbsAssistanceDataGroup& aFilter);
	// Note: for cancelling requests from LBS PM uses base class method CancelMachine() 
	
	// Derived from MSuplPositioningProtocolFsmObserver (events from the Positioning Protocol)
	void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason);
	void ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality,
									const TLbsNetPosRequestMethod& aPosMethod);					
	void PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload);
	void PositioningProtocolError(const TInt& aError);

	// MLbsCallbackTimerObserver methods
	void OnTimerEventL(TInt aTimerId);

	// Called by the state handler when the procedure ends
	void CompleteProcedure();
	
	// Called by the state handler
	TInt GetHostId(TLbsHostSettingsId& aHostId);

private:

	CSuplMtlrFsmSession(MSuplFsmSessionObserver& aObserver);
	void ConstructL();
	void InitialiseProcedure();
	void StoreNetResultLoc(const TPositionInfoBase& aPosInfo);
	void ProcessExternalLocationRequestL(const CSuplInit* aSuplInit);
	TCancelReason StoreExternalLocationRequest(const CSuplInit* aSuplInit);
	virtual TBool DetermineStateFromInitReceived();
	virtual TBool DetermineStateFromConnectionStarted(TBool& aForceRedo);
	};

#endif // __SUPLMTLRFSMSESSION_H__

