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
// @file ctlbsclientsteppartialupdate.h
// This contains the header file for  Update Options Tests
// 
//

#ifndef __CT_LBS_CLIENT_PARTIALUPDATE_H__
#define __CT_LBS_CLIENT_PARTIALUPDATE_H__

#include "ctlbsclientserver.h"
#include "ctlbsnetsimstep.h"
#include "ctlbsdoposupdate.h"


// Literals used
_LIT(KLbsClientStep_PartialUpdate, "LbsClientStep_PartialUpdate");

class CT_LbsClientStep_PartialUpdate : public CT_LbsNetSimStep,	MT_LbsDoPosUpdateObserver
	{
public:
	~CT_LbsClientStep_PartialUpdate();

	static CT_LbsClientStep_PartialUpdate* New(CT_LbsClientServer& aParent);
	virtual TVerdict doTestStepL();

	// From MLbsNetSimTestObserver.	
	virtual void Connected();
	virtual void NotifyRegisterLcsMoLr(const TDesC& aData);
	virtual void NotifyReleaseLcsMoLr(TInt aReason);
	virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	virtual void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	virtual void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);

	void MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus);

private:
	CT_LbsClientStep_PartialUpdate(CT_LbsClientServer& aParent);
	void ConstructL();

	TInt OpenNetSim();
	void CloseNetSim();
	void VerifyPosInfos();

private:

	enum TPosUpdateStatus 
		{
		EPositionUpdateStart,
		EPositionUpdateContinue,
		EPositionUpdateComplete
		};

	
	TInt iTestCaseId;
	TPosUpdateStatus iPosUpdateStatus;

	CT_LbsDoPosUpdate* iDoPosUpdatePtr;
    CT_LbsClientServer& iParent;

	TPositionInfo*	iPosInfoPtr1;
	TPositionInfo*	iPosInfoPtr2;
	TPosition 		iRefPos;
	
	};

#endif //__CT_LBS_CLIENT_PARTIALUPDATE_H__
