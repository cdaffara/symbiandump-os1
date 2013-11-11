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
// @file ctlbsNetsimstep.h
// This contains the header file for LBS Net Sim Test Step Base
// 
//

#ifndef __CT_LBS_NET_SIM_STEP_H__
#define __CT_LBS_NET_SIM_STEP_H__

// EPOC includes.
#include <test/testexecutestepbase.h>

// LBS test includes.
#include <lbs/test/lbsnetsimtest.h>
#include <lbs/test/lbsnetsimtestobserver.h>

#include <lbs/test/ctlbstimerutils.h>
#include "ctlbscommon.h"

const TInt KLbsKeepAlivePeriod = 10000;

const TInt KKeepAliveTimerId = 0;
const TInt KAbortTimerId = 34;

/** Callback flags held as bits in a 32 bit word.
*/
typedef TUint32 TLbsCallbackFlags;

const TLbsCallbackFlags KLbsCallback_None															= 0x0000;

const TLbsCallbackFlags KLbsCallback_NetSim_Got_Connect												= 0x0001;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_Disconnect											= 0x0002;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr								= 0x0004;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr								= 0x0008;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation					= 0x0010;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyReleaseLcsLocationNotification				= 0x0020;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult							= 0x0040;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation						= 0x0080;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyMeasurementReportRequestMoreAssistanceData	= 0x0100;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure 				= 0x0200;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_NotifyError											= 0x0400;
const TLbsCallbackFlags KLbsCallback_NetSim_Got_ProcessMeasurementControlLocationError				= 0x0800;

// NOTE: Derived test case will assume that they create new flags starting at 0x1000. If you add additional
// flags here, the derived test cases will have to be updated also.

const TLbsCallbackFlags KLbsCallback_MoLr =
	KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr 				| // --> NET
	KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation	| // <-- NET
	KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation 	| // --> NET
	KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult			| // <-- NET
	KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr;				  // --> NET
	
//const TLbsCallbackFlags KLbsCallback_NetworkBasedLocation =
//	KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr 					| // --> NET
//	KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation		| // <-- NET
//	KLbsCallback_NetSim_Got_NotifyMeasurementReportControlFailure 	| // --> NET

//	KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr;				  	  // --> NET	

const TLbsCallbackFlags KLbsCallback_NetworkBasedLocation =
	KLbsCallback_NetSim_Got_NotifyRegisterLcsMoLr 					| // --> NET
	KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation		| // <-- NET
	KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation			| // --> NET
	KLbsCallback_NetSim_Got_NotifyFacilityLcsMoLrResult				| // <-- NET
	KLbsCallback_NetSim_Got_NotifyReleaseLcsMoLr;				  	  // --> NET	

const TLbsCallbackFlags KLbsCallback_MtLr =
		// note: we get no notification of the 
		KLbsCallback_NetSim_Got_NotifyReleaseLcsLocationNotification |  // response to privacy request
		KLbsCallback_NetSim_Got_NotifyMeasurementControlLocation |		// location request
		KLbsCallback_NetSim_Got_NotifyMeasurementReportLocation;


class CT_LbsNetSimStep : public CTestStep,
						 public	MT_LbsTimerUtilsObserver,
						 public MLbsNetSimTestObserver
{

public:

	virtual ~CT_LbsNetSimStep();

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

	CT_LbsNetSimStep();
	
	/** If aHaltTimerInterval is set, the test will continue for this period of time after all halt flags have been set
	 */
	void ConstructL(TTimeIntervalMicroSeconds32 aHaltTimerInterval = 0);
	
	/** Can be used to set the extended timer interval if not known at construct time
	*/
	void SetExtendedTimerInterval(TTimeIntervalMicroSeconds32 aHaltTimerInterval);

	void SetCallbackFlag(TLbsCallbackFlags aCallbackFlag);
	TBool TestCallbackFlags(TLbsCallbackFlags aExpectedFlags);


	// From MT_LbsTimerUtilsObserver - keep alive timer to ensure we recvice all callbacks.
	virtual void HandleTimerL(TInt aTimerId, const TTime& aTargetTime);

	
	enum  {		EConnecting = 0,	/** Waiting for the Net Sim to connect to the protocol module. */
				EWaiting,			/** Waiting for all the callbacks to be fired. */
				EExtendedWaiting,	/** Waiting for extended timer to fire */
				EAborted,			/** Abort timer has fired before all callbacks fired */
				EDone,				/** All the correct callbacks have been fired (and extended timer, if any, has fired), the test can end. */
				ELast};				/** Used to allow extension of enums in derived classes - do not use! */

	/** State machine variable for the test step.
	*/
	TInt iState;


	/** Used to keep a record off all the callback events.
	*/
	TLbsCallbackFlags iCallbackFlags;
	

	/** Expected callback flags at test end.
	*/
	TLbsCallbackFlags iFlagsToHaltOn;
	
	/** If this is non-zero, the test will continue for this period of time after all halt flags have been set
	*/
	TTimeIntervalMicroSeconds32	iExtendedTimerInterval;
	
	/** Network Simulator, to allow MT-LRs and to monitor network messages.
	*/
	RLbsNetSimTest iNetSim;


	/** Timer used to ensure the test step keeps processing until all the required callbacks
		have been called. Otherwise we would execute the test step calls and return from the
		doTestStepL function and miss the callbacks being fired.
	*/
	CT_LbsTimerUtils* iKeepAliveTimer;
	
	/** Timer used to abort the test step if all callbacks have not fired. Individual test cases can start this time with appropriate time.
		
	*/
	CT_LbsTimerUtils* iAbortTimer;
	
protected:
	TExpectedApiBehaviour iExpectedApiBehaviour;	
	
};

#endif // __CT_LBS_STEP_H__
