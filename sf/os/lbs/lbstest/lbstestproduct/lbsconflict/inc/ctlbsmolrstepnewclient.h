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
// @file ctlbsmolrstepnewclient.h
// This contains the header file for MoLr new client Tests
// 
//

#ifndef __CT_LBS_MOLR_STEP_NEW_CLIENT_H__
#define __CT_LBS_MOLR_STEP_NEW_CLIENT_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>

// LBS test includes.
#include "ctlbsconflictstep.h"
#include "ctlbsconflictserver.h"
#include "ctlbsdoposupdate.h"

// Literals used
_LIT(KLbsMolrStep_NewClient, "LbsMolrStep_NewClient");

class CT_LbsMolrStep_NewClient :	public	CT_LbsConflictStep,
								public	MT_LbsDoPosUpdateObserver
	{
public:
	~CT_LbsMolrStep_NewClient();

	static CT_LbsMolrStep_NewClient* New(CT_LbsConflictServer& aParent);
	virtual TVerdict doTestStepL();

	// From CT_LbsNetSimStep via CT_LbsConflictStep
	void Connected();
	void Disconnected();
	void NotifyRegisterLcsMoLr(const TDesC& aData);
	void NotifyReleaseLcsMoLr(TInt aReason);
	void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
	void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);
	void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
	void NotifyMeasurementReportControlFailure(TInt aReason);

	// From MT_LbsDoPosUpdateObserver - Position notify update callback.
	void MT_LbsDoPosUpdateCallback(TRequestStatus& aStatus);

private:
	CT_LbsMolrStep_NewClient(CT_LbsConflictServer& aParent);
	void ConstructL();		

	void VerifyPosInfos();
	
	/** Current test case being executed.
	*/
	TInt iTestCaseId;

	//Flag to make sure Molr gets called only two times
	TBool iPosUpdateFlag;

	// move to base class, no keep here
	CT_LbsDoPosUpdate* iDoPosUpdate;

	//objects to store MoLr location
	TPositionInfo*	iPosInfo1;
	TPositionInfo*  iPosInfo2;

	};

#endif //__CT_LBS_MOLR_STEP_NEW_CLIENT_H__