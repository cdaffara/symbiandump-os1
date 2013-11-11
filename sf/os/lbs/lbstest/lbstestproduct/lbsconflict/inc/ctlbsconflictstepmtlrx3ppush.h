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
// This contains the header file for a MTLR- X3P Push test.
// 
//

#ifndef __CT_LBS_CONFLICT_STEP_MTLR_X3P_PUSH_H__
#define __CT_LBS_CONFLICT_STEP_MTLR_X3P_PUSH_H__

// LBS includes.
#include <lbs.h>
#include <lbscommon.h>
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsprivacycontroller.h>

// LBS test includes.
#include "ctlbsconflictstep.h"
#include "ctlbsconflictserver.h"
#include "ctlbsdox3p.h"


// Literals used
_LIT(KLbsConflictStep_MTLRX3PPush, "LbsConflictStep_MTLRX3PPush");

class CT_LbsConflictStep_MTLRX3PPush :	public	CT_LbsConflictStep,
								public	MT_LbsDoX3PObserver,
								public	MLbsPrivacyObserver
	{
public:
	~CT_LbsConflictStep_MTLRX3PPush();

	static CT_LbsConflictStep_MTLRX3PPush* New(CT_LbsConflictServer& aParent);
	virtual TVerdict doTestStepL();

	// From CT_LbsConflictStep.
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
	void MT_LbsDoX3PCallback(TInt aTransmitId, TRequestStatus& aStatus);

	// From MLbsPrivacyObserver.
	void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType);
	void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TUint aRequestId, TInt aReason);

private:
	CT_LbsConflictStep_MTLRX3PPush(CT_LbsConflictServer& aParent);
	void ConstructL();		

	void SetExpectedSeq();
	void VerifyPosInfos();

	enum {	/** Just started a MTLR priv request. */
			EStartMTLRPrivReq = CT_LbsConflictStep::ELast,
			
			/** MTLR privacy request complete, started a MTLR location request. */
			EStartMTLRLocReq,
			
			/** MTLR location request complete with reference position, waiting for
				second location complete. */
			EWaitMTLRLocRes
			};

	/** Active object wrapper to allow the X3P.
	*/
	CT_LbsDoX3P* iDoX3PPush;

	/** Privacy controller, required to preform a MT-LR.
	*/
	CLbsPrivacyController* iPrivacyController;

	/** MT-LR request id, used to verify the request ids passed to MT-LR callbacks.
	*/	
	TUint iMtlrRequestId;
	};

#endif //__CT_LBS_CONFLICT_STEP_MTLR_X3P_PUSH_H__
