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
// @file ctlbsconflictstep.h
// This contains the header file for LBS Conflict Test Step Base
// 
//

#ifndef __CT_LBS_CONFLICT_STEP_H__
#define __CT_LBS_CONFLICT_STEP_H__


// EPOC includes.
#include <test/testexecutestepbase.h>

// LBS test includes.
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>

#include "ctlbsconflictserver.h"
#include <lbs/test/ctlbstimerutils.h>


const TInt KLbsKeepAlivePeriod = 10000;
const TUint KLbsAbortPeriod = 10000000;	// The test case should finish before 10 seconds, use different value if not in test step code.

const TInt KKeepAliveTimerId = 0;
const TInt KAbortTimerId = 1;

// X3P helper items.
_LIT(KTransmitPushDestination, 	"02");
_LIT(KTransmitMenuDestination, 	"08");
_LIT(KTransmitTimerDestination, "10");

_LIT(KTransmitPush2Destination, 	"02");
_LIT(KTransmitMenu2Destination, 	"08");

const TUint KTransmitPushPriority 	= 0x02;
const TUint KTransmitMenuPriority 	= 0x08;
const TUint KTransmitTimerPriority 	= 0x10;

const TUint KTransmitPush2Priority 	= 0x02;
const TUint KTransmitMenu2Priority 	= 0x08;

const TInt KLbsConflictX3PPushId 	= 0x02;
const TInt KLbsConflictX3PMenuId 	= 0x08;
const TInt KLbsConflictX3PTimerId 	= 0x10;

const TInt KLbsConflictX3PPush2Id 	= 0x022;
const TInt KLbsConflictX3PMenu2Id 	= 0x088;


class CT_LbsConflictStep :
							public CTestStep,
							public MT_LbsTimerUtilsObserver,
							public MLbsNetSimTestObserver
{

public:
	virtual ~CT_LbsConflictStep();

	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

	// From MLbsNetSimTestObserver.	
	virtual void Connected();
	virtual void Disconnected();
	virtual void NotifyRegisterLcsMoLr(const TDesC& aData);
	virtual void NotifyReleaseLcsMoLr(TInt aReason);
	virtual void NotifyMeasurementControlLocation(const TPositionInfo& aPosition, const RLbsAssistanceDataBuilderSet& aData, const TLbsNetPosRequestQuality& aQuality);
	virtual void NotifyReleaseLcsLocationNotification(const CLbsNetworkProtocolBase::TLbsPrivacyResponse& aResult);
	virtual void NotifyFacilityLcsMoLrResult(TInt aReason, const TPositionInfo& aPosition);
	virtual void NotifyMeasurementReportLocation(const TPositionInfo& aPosition);
	virtual void NotifyMeasurementReportRequestMoreAssistanceData(const TLbsAssistanceDataGroup& aFilter);
	virtual void NotifyMeasurementReportControlFailure(TInt aReason);
	virtual void NotifyError(MLbsNetSimTestObserver::EFunction aFunction, int aError);
	virtual void ProcessMeasurementControlLocationError(TInt aError);

protected:
	CT_LbsConflictStep(CT_LbsConflictServer& aParent);

	void ConstructL();

	TInt OpenNetSim(MLbsNetSimTestObserver* aObserver);
	void CloseNetSim();

	void EnterEmergencyState();
	void ExitEmergencyState();
	void StartNetworkPrivacyRequest();
	void SetVerifySeqEvent(TInt aSeqEventId);
	void SetVerifyAdhocEvent(TInt aAdhocEventId);
	void SetCurrentSeqEvent(TInt aSeqEventId);
	TBool VerifySeqEvents();


	// From MT_LbsTimerUtilsObserver - keep alive timer to ensure we recvice all callbacks.
	virtual void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);

enum {
		// Self Locate.
		ESelf_MOLR_NetSim_Got_NotifyRegisterLcsMoLr = 0x0010,					// --> TO NET
		ESelf_MOLR_NetSim_Got_NotifyReleaseLcsMoLr,								// <-- FROM NET
		ESelf_MOLR_NetSim_Got_NotifyMeasurementControlLocation,					// <-- FROM NET
		ESelf_MOLR_NetSim_Got_NotifyReleaseLcsLocationNotification,				// ??				consider removing
		ESelf_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult,						// <-- FROM NET
		ESelf_MOLR_NetSim_Got_NotifyMeasurementReportLocation,					// --> TO NET
		ESelf_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData,	// --> TO NET
		ESelf_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure,
		ESelf_MOLR_NetSim_Got_NotifyError,
		ESelf_MOLR_NetSim_Got_ProcessMeasurementControlLocationError,
		EClient_Got_PosUpdate_Complete,
	
		// X3P MENU.
		EX3P_MENU_MOLR_NetSim_Got_NotifyRegisterLcsMoLr = 0x0020,
		EX3P_MENU_MOLR_NetSim_Got_NotifyReleaseLcsMoLr,
		EX3P_MENU_MOLR_NetSim_Got_NotifyMeasurementControlLocation,
		EX3P_MENU_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult,
		EX3P_MENU_MOLR_NetSim_Got_NotifyMeasurementReportLocation,
		EX3P_MENU_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData,
		EX3P_MENU_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure,
		EX3P_MENU_MOLR_NetSim_Got_NotifyError,
		EX3P_MENU_MOLR_NetSim_Got_ProcessMeasurementControlLocationError,

		// X3P TIMER.
		EX3P_TIMER_MOLR_NetSim_Got_NotifyRegisterLcsMoLr = 0x0030,
		EX3P_TIMER_MOLR_NetSim_Got_NotifyReleaseLcsMoLr,
		EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementControlLocation,
		EX3P_TIMER_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult,
		EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportLocation,
		EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData,
		EX3P_TIMER_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure,
		EX3P_TIMER_MOLR_NetSim_Got_NotifyError,
		EX3P_TIMER_MOLR_NetSim_Got_ProcessMeasurementControlLocationError,

		// X3P PUSH.
		EX3P_PUSH_MOLR_NetSim_Got_NotifyRegisterLcsMoLr = 0x0040,
		EX3P_PUSH_MOLR_NetSim_Got_NotifyReleaseLcsMoLr,
		EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementControlLocation,
		EX3P_PUSH_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult,
		EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportLocation,
		EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData,
		EX3P_PUSH_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure,
		EX3P_PUSH_MOLR_NetSim_Got_NotifyError,
		EX3P_PUSH_MOLR_NetSim_Got_ProcessMeasurementControlLocationError,

		EClient_Got_X3P_Complete,

		// MTLR.
		EMTLR_NetSim_Got_NotifyReleaseLcsLocationNotification = 0x0050,		// --> TO NET ?
		EMTLR_NetSim_Got_NotifyMeasurementControlLocation,					// <-- FROM NET
		EMTLR_NetSim_Got_NotifyMeasurementReportLocation,					// --> TO NET

		EMTLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData,

		EMTLR_NetSim_Got_NotifyMeasurementReportControlFailure,
		EMTLR_NetSim_Got_NotifyError,
		EMTLR_NetSim_Got_ProcessMeasurementControlLocationError,

		EClient_Priv_Got_ProcessNetworkLocationRequest,
		EClient_Priv_Got_ProcessNetworkPositionUpdate,
		EClient_Priv_Got_ProcessRequestComplete,
		
		//X3P MENU2 (SECOND REQ)
		EX3P_MENU2_MOLR_NetSim_Got_NotifyRegisterLcsMoLr = 0x0060,
		EX3P_MENU2_MOLR_NetSim_Got_NotifyReleaseLcsMoLr,
		EX3P_MENU2_MOLR_NetSim_Got_NotifyMeasurementControlLocation,
		EX3P_MENU2_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult,
		EX3P_MENU2_MOLR_NetSim_Got_NotifyMeasurementReportLocation,
		EX3P_MENU2_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData,
		EX3P_MENU2_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure,
		EX3P_MENU2_MOLR_NetSim_Got_NotifyError,
		EX3P_MENU2_MOLR_NetSim_Got_ProcessMeasurementControlLocationError,

		//X3P PUSH2 (SECOND REQ)
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyRegisterLcsMoLr = 0x0070,
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyReleaseLcsMoLr,
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyMeasurementControlLocation,
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyFacilityLcsMoLrResult,
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyMeasurementReportLocation,
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData,
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyMeasurementReportControlFailure,
		EX3P_PUSH2_MOLR_NetSim_Got_NotifyError,
		EX3P_PUSH2_MOLR_NetSim_Got_ProcessMeasurementControlLocationError
	};

	
	enum  {		EConnecting = 0,	/** Waiting for the Net Sim to connect to the protocol module. */
				EWaiting,			/** Waiting for all the callbacks to be fired. */
				EAborted,			/** Abort timer has fired before all callbacks fired */
				EDone,				/** All the correct callbacks have been fired (and extended timer, if any, has fired), the test can end. */
				ELast};				/** Used to allow extension of enums in derived classes - do not use! */



	/** Network Simulator, to allow MT-LRs and to monitor network messages.
	*/
	RLbsNetSimTest iNetSim;

	/** State machine variable for the test step.
	*/
	TInt iState;


	/** Verify sequence array of expected events (callbacks) for a test.
	*/
	RArray<TInt> iVerifySeqArr;
	
	/** Verify adhoc array of expected events (callbacks) for a test.
	*/
	RArray<TInt> iVerifyAdhocArr;
	
	/** Current sequence array of actual events which were generated during the test.
	*/
	RArray<TInt> iCurrentSeqArr;


	/** Reference position used to setup Net sim and later verification. */
	TPosition iRefPos;


	/** Timer used to ensure the test step keeps processing until all the required callbacks
		have been called. Otherwise we would execute the test step calls and return from the
		doTestStepL function and miss the callbacks being fired.
	*/
	CT_LbsTimerUtils* iKeepAliveTimer;

	/** Timer used to abort the test step if all callbacks have not fired. Individual test
		cases can start this time with appropriate time.
	*/
	CT_LbsTimerUtils* iAbortTimer;


	/** Gives access to server member variables.
	*/	
	CT_LbsConflictServer& iParent;
};

#endif // __CT_LBS_CONFLICT_STEP_H__
