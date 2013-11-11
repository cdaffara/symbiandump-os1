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

#ifndef __SUPLMOLRFSMSESSION_H__
#define __SUPLMOLRFSMSESSION_H__

#include "suplfsmsessionbase.h"
#include <lbs/lbshostsettings.h>

/** MO-LR protocol state machine.
This class manages the MO-LR protocol procedure and is employed when the protocol
module is required to handle interactions for a mobile self-locate.

The class implements the pure virtual methods defined by CSuplFsmSessionBase.
@see CSuplFsmSessionBase

The class also implements a timer observer interface defined by MLbsCallbackTimerObserver
@see MLbsCallbackTimerObserver
*/
NONSHARABLE_CLASS(CSuplMolrFsmSession) : public CSuplFsmSessionBase
	{
public:
		
	/** Type of MO-LR state machine with regards to the
		positioning methods it supports.
	*/
	enum TSuplMolrMachineType
		{
		/** This type of machine only declares to the network
		that CellBased positioning method is possible*/
		ESuplMolrCellBased,
		/** This type of machine declares to the network all of
		LBS's supported positioning methods (i.e. cell-based and
		agps variants supported by underlying hardware)*/
		ESuplMolrUnspecified
		};

public:

	static CSuplMolrFsmSession* NewL(MSuplFsmSessionObserver& aObserver, TSuplMolrMachineType aMachineType = ESuplMolrUnspecified);
	virtual ~CSuplMolrFsmSession();
	
	void StartSuplResponseTimer();

	// CSuplFsmSessionBase pure virtual methods
	TBool SelectNextState(TBool& aForceRedo);
	void PostTransition();
	void CancelProcedure();
	//void DoQueuedRequest(TInt aRequest);

	// Called by the Protocol Manager (events from LBS)
	void MoLrReq(const TLbsNetSessionId& aSessionId, const TLbsNetPosRequestOptionsBase& aOptions);
	void LocationResp(TInt aReason, const TPositionInfoBase& aPosInfo);
	void AssistanceDataReq(const TLbsAssistanceDataGroup& aFilter);
	void AdoptNewLbsSessionId(const TLbsNetSessionId& aSessionId);
	// Note: for cancelling requests from LBS PM uses base class method CancelMachine() 
	
	// Derived from MSuplPositioningProtocolFsmObserver (events from the Positioning Protocol)
	void ProcessAssistanceData(const TLbsAsistanceDataGroup& aGroupMask, TInt aReason);
	void ProcessPositioningRequest(const TLbsNetPosRequestQuality& aQuality,
									const TLbsNetPosRequestMethod& aPosMethod);					
	void PositioningPayloadToNetwork(const CSuplPosPayload* aPositioningPayload);
	void PositioningProtocolError(const TInt& aError);	

	// MLbsCallbackTimerObserver methods
	void OnTimerEventL(TInt aTimerId);

	// Called by the state handler when the procedure completes
	void CompleteProcedure();
	
	// Called by the state handler
	CSuplMolrFsmSession::TSuplMolrMachineType MachineType();
	TInt GetHostId(TLbsHostSettingsId& aHostId);

private:
	
	CSuplMolrFsmSession(MSuplFsmSessionObserver& aObserver, TSuplMolrMachineType aMachineType);
	void ConstructL();
	void InitialiseProcedure();
	TBool DetermineStateFromStartSent();
	
private:
	
	/* Type of MO-LR state machine depending on 
	supported positioning methods*/
	TSuplMolrMachineType iMachineType;
	};

#endif // __SUPLMOLRFSMSESSION_H__

