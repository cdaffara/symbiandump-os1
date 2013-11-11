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
// @file ctlbsconflictstepmain.h
// This contains the header file for Location Data Source Tests
// 
//

#ifndef __CT_LBS_CONFLICT_STEP_X3PMENUPUSH_H__
#define __CT_LBS_CONFLICT_STEP_X3PMENUPUSH_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
//#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbsconflictstep.h"
#include "ctlbsconflictserver.h"
#include "ctlbsdoposupdate.h"
#include "ctlbsdox3p.h"
//#include "ctlbsassdatamoduledata.h"

// Literals used
_LIT(KLbsConflictStep_X3PMenuPush, "LbsConflictStep_X3PMenuPush");


class CT_LbsConflictStep_X3PMenuPush :	public	CT_LbsConflictStep,
										public	MT_LbsDoX3PObserver
	{
public:
	~CT_LbsConflictStep_X3PMenuPush();

	static CT_LbsConflictStep_X3PMenuPush* New(CT_LbsConflictServer& aParent);
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

	// From MT_LbsDoX3PObserver - X3P transmit callback.
	void MT_LbsDoX3PCallback(TInt aTransmitId, TRequestStatus& aStatus);


private:
	CT_LbsConflictStep_X3PMenuPush(CT_LbsConflictServer& aParent);
	void ConstructL();		

	void SetExpectedSeq();
	void VerifyPosInfos();

	void StartX3PL(TInt aTransmitId);

	enum {	EReqX3PMenu = CT_LbsConflictStep::ELast,
			EReqX3PPush};

	/** Active object wrapper to allow X3Ps.
	*/
	CT_LbsDoX3P* iDoX3PMenu;
	CT_LbsDoX3P* iDoX3PPush;
	};

#endif //__CT_LBS_CONFLICT_STEP_X3PMENUPUSH_H__
