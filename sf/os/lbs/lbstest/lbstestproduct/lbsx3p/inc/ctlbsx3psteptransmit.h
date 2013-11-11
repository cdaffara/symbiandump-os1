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
// @file ctlbsx3psteptransmit.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_X3P_STEP_TRANSMIT_H__
#define __CT_LBS_X3P_STEP_TRANSMIT_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>

#include <lbs/lbslocerrors.h>

// LBS test includes.
#include "ctlbsx3pstep.h"
#include "ctlbsx3pserver.h"
#include "ctlbsx3pdotransmitpos.h"
#include <lbs/test/tlbsutils.h>
// LBS test includes.

#include "ctlbsnetsimstep.h"

// Literals used
_LIT(KLbsX3PStep_Transmit, "LbsX3PStep_Transmit");



const TInt KMaxCount = 3; //Number of parallel/cuncurrent instances of teststep 13 we expect.
// Kick off transmit position.
_LIT(KDefaultDestination, "07712345678");
_LIT(KInvalidDestination, "INVALID_PHONE_NUMBER_BECAUSE_ITS_CHARACTERS");
const TUint KTransmitPriority = X3P_PRIORITY_PUSH;

class CT_LbsX3PStep_Transmit : public 	CT_LbsX3PStep, 
										MT_LbsX3PDoTransmitPosObserver
	{
public:
	~CT_LbsX3PStep_Transmit();

	static CT_LbsX3PStep_Transmit* New(CT_LbsX3PServer& aParent);
	virtual TVerdict doTestStepL();
	void ConstructL();

	// From MLbsNetSimTestObserver.	
	// In order...
	void Connected();
	void NotifyRegisterLcsMoLr(const TDesC& aData);
	void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyReleaseLcsMoLr(TInt aReason);
	
	void NotifyMeasurementReportControlFailure(TInt aReason);
	
	// Callback is fired on completion of the Transmitposition() call
	void MT_LbsX3PDoTransmitPosCallback(TRequestStatus& aStatus, CT_LbsX3PDoTransmitPos* aDoTransmitPosAO);
	void MT_LbsX3PDoRefPosCallback(const TRequestStatus& aStatus,const TPositionInfo& aRefPosInfo);
	
	void ProcessMeasurementControlLocationError(TInt aError);

private:
	CT_LbsX3PStep_Transmit(CT_LbsX3PServer& aParent);
	
	// Doing CleanupClosePushL/PopAndDestroy on these member variables in each test case.
	// Not in constructor/destructor. 
	RLbsTransmitPositionServer iServer;
	RLbsTransmitPosition iPositionTransmitter;

	//TransmitPosition objects used for parallel subsession tests
	RLbsTransmitPosition iTransmitPos1;
	RLbsTransmitPosition iTransmitPos2;
	RLbsTransmitPosition iTransmitPos3;
	
	TLbsNetPosRequestQuality iNetPosQuality;
	
		
	CT_LbsX3PDoTransmitPos* iDoTransmitPos;
	
	T_LbsUtils 	iUtils;
	TBool 		iRefPosFlag;
	/** Reference position used to setup Net sim and later verification. */
	TPosition iRefPos;
	/** used to compare position update timeout **/ 
	TTime iTimeRequested,iTimeReceived;
	
	RPointerArray<CT_LbsX3PDoTransmitPos> iDoTransmitPosArr;
	TInt iTestCaseId;
	TInt iReleaseCallBackCount;
	};

#endif //__CT_LBS_X3P_STEP_TRANSMIT_H__
