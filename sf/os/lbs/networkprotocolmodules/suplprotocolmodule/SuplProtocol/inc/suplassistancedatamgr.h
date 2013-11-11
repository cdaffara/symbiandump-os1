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
// This file defines the class that manages assistance data
// during active location requests.
// 
//

/**
 @file
 @internalTechnology
 @deprecated
*/

#ifndef __SUPLASSISTANCEDATAMGR_H__
#define __SUPLASSISTANCEDATAMGR_H__

#include <e32base.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include "lbstimer.h"
#include "suplprotocolmanager.h"

/** Assistance Data Manager class.

This class contains the logic needed for handling Assistance Data request and reports
that is needed due to the fact that there are multiple potential sources of assistance
data (MOLR session, MTLR session, etc).

Throughout this class state machines (MOLR, MTLR) are identified by their service type
(@see TLbsNetProtocolService)

This class is used to remember what assitance data types were requested from each state
machine and reports back to LBS any missing assistance data types as well as any valid
assitance data types that have been collected by the machines.

This class takes also care to notify LBS of a Reference Location if it is found in an
assistance data report.

*/
NONSHARABLE_CLASS(CSuplAssistanceDataManager) : public CBase, public MLbsCallbackTimerObserver
	{

public:

	static CSuplAssistanceDataManager* NewL(MSuplProtocolManagerObserver& aGateway);
	~CSuplAssistanceDataManager();

	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask ProcessDataRequest(TLbsAsistanceDataGroup& aDataRequestMask);
	void AssistanceDataReport(const RLbsAssistanceDataBuilderSet& aAssitanceData,
							  const TLbsAsistanceDataGroup& aDataReportMask, TInt aReason,
							  const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aStateMachine,
							  const TLbsNetSessionId& sessionId);

	void MachineReadyForAssistanceDataRequest(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aStateMachine);
	void MachineTerminated(const MLbsNetworkProtocolObserver::TLbsNetProtocolService& aStateMachine);

	// MLbsCallbackTimerObserver methods	
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);


private:

	CSuplAssistanceDataManager(MSuplProtocolManagerObserver& aGateway);
	void ConstructL();

private:

	/** Protocol Manager Observer
	Reference to the Gateway Interface for 
	reporting assistance data (including errors)
	back to LBS.
	*/
	MSuplProtocolManagerObserver& iGateway;

	/** Reported data mask
	This reflects the set of data reported to LBS in the last
	second (used to prevent re-requesting assistance data
	when an assistance data request from LBS crosses in transit
	with an assitance data report sent to LBS)
	*/
	TLbsAsistanceDataGroup iReportedDataMask;

	/** Peding Assistance Data mask
	This reflects the set of data types that have been
	requested by LBS but it has not been possible to 
	request from any state machine yet (no machine has
	become ready since the request was made).
	*/
	TLbsAsistanceDataGroup iPendingDataMask;

	/** MOLR Assitance Data Mask
	Assistance Data Types being obtained through
	the MOLR state machine
	*/
	TLbsAsistanceDataGroup iMoLrRequestedDataMask;

	/** MTLR Assistance Data Mask
	Assistance Data Types being obtained through
	the MTLR state machine
	*/
	TLbsAsistanceDataGroup iMtLrRequestedDataMask;

	/** Used to hold a bit mask of state machines
	that are ready to take assitance data requests.
	The state machines are identified in the mask
	by their corresponding TLbsNetProtocolService
	value
	*/
	MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask iReadyMachinesMask;

	/** Timer for remembering for 1 second what
	assistance data have been reported to LBS.
	*/
	CLbsCallbackTimer* iAssistDataReportTimer;
	};

#endif // __SUPLASSISTANCEDATAMGR_H__
