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
// @file ctlbsstepverifyloginfo.h
// This contains the header file for verify position info array test step.
// 
//

#ifndef __CT_LBS_VERIFY_LOG_INFO_H__
#define __CT_LBS_VERIFY_LOG_INFO_H__

#include "ctlbsstep.h"
#include "ctlbsserver.h"

// From test implementation
#include <lbs/lbstransmitlocationlogevent.h>
//#include "Te_LbsLoggingSuiteStepBase.h"

#include "LbsLogger.h"
#include <logcli.h>
#include <logview.h>
#include <logwrap.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <logfilterandeventconstants.hrh>
#endif


#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>

// Includes for Log Comparison
#include <lbs/lbslogevent.h>
#include <lbs/lbsselflocatelogevent.h>
#include <lbs/lbsexternallocatelogevent.h>
#include <lbs/lbstransmitlocationlogevent.h>
#include <lbs/lbsnetworklocatelogevent.h>
#include <lbs/lbsassistancedatalogevent.h>

// Literals used
_LIT(KLbsStep_VerifyLogInfo, "LbsStep_VerifyLogInfo");
_LIT(KDest, "07712345678");

#define REFPOS_LAT				52.2
#define REFPOS_LONG				0.2
#define REFPOS_ALT				10
#define REFPOS_HORZ_ACCURACY	100
#define REFPOS_VERT_ACCURACY	100

#define REFPOS_LAT_OTHER		49.2
#define REFPOS_LONG_OTHER		3.5
#define REFPOS_ALT_OTHER		50.0
#define REFPOS_HORZ_ACCURACY_OTHER	2.0
#define REFPOS_VERT_ACCURACY_OTHER	3.0

class CT_LbsStep_VerifyLogInfo : public CT_LbsStep
	{
public:
	static CT_LbsStep_VerifyLogInfo* New(CT_LbsServer& aParent);
	static CT_LbsStep_VerifyLogInfo* NewL(CT_LbsServer& aParent);
	~CT_LbsStep_VerifyLogInfo();
	CT_LbsStep_VerifyLogInfo();

	TVerdict doTestStepPreambleL();
	TVerdict doTestStepPostambleL();


	virtual TVerdict doTestStepL();
	static TInt SelfLocateLogEventTimestampCompare(const CLbsSelfLocateLogEvent& aLog1,
										  const CLbsSelfLocateLogEvent& aLog2);


	enum TLogType
	{
	/** Logging an Autonomous self Locate location request 3.3.1 */
	EAutonomousSelfLocateRequest = 0,
	
	/** Logging an MO-LR Self Locate location request 3.3.2 */
	EMOLRSelfLocateRequest,

	/** Logging an MO-LR Cell Based location request 3.3.3 */
	EMOLRCellBasedLocationRequest,

	/** Logging an MO-LR Transmit to 3rd Party Location 3.3.4 */
	EMOLRTransmitTo3rdPartyLocationRequest,

	/** Logging a Mobile Terminated Location Request 3.3.5 */
	EMTLR,
	
	/** Logging MO-LR Tracking Location Requests 3.3.6 */
	EMOLRTrackingLocationRequest,
	
	/** Logging Autonomous Tracking Location Requests 3.3.7 */
	EAutonomousTrackingLocationRequest,
	
	/** Update an existing Event log entry 3.3.8 */
	EUpdateExistingEventLogEntry,
		
	};


private:
	CT_LbsStep_VerifyLogInfo(CT_LbsServer& aParent);
	
	TVerdict ReadFromLogL(TInt aUidForTheLogType);
	void ClearLog();
	void InitialiseRHS();
	void InitialiseRHSforMTLR();
	TPositionInfo* CommonPositionInfo();
	
	//5 log type test mechanisms
	void SelfLocateLogTest();
	void NetworkLocateLogTest();
	void ExternalLocateLogTest();
	void X3PLocateLogTest();
	void AssistanceDataLogTest();
	
	//Top level functionality
	void AutonomousSelfLocateLogTest();
	void MOLRSelfLocateRequest();
	void MOLRCellBasedLocationRequest();
	void MOLRTransmitTo3rdPartyLocationRequest();
	void MTLR();
	void MOLRTrackingLocationRequest();
	void AutonomousTrackingLocationRequest();
	void SecondPositionFromArray(TPositionInfo* aPosInfoSecond);
	
	
	TInt iLogType; // Can be defined in ENums
	T_LbsUtils iUtils;
	
	// Member variables for Log reading
	RFs itheFs;
	CLogClient* iclient;
	CLogViewEvent* iview;
	CT_LbsAsyncWaiter* iActiveWaiter;
	TPositionInfo* iRefPosTwo;
		
	// Right hand sides of comparisons
	CLbsSelfLocateLogEvent*			iSelfLocateLogIntiailisedFromValues;
	CLbsExternalLocateLogEvent*		iExternalLocateLogIntiailisedFromValues;
	CLbsTransmitLocationLogEvent*	iX3PLogIntiailisedFromValues;
	CLbsNetworkLocateLogEvent*		iNetworkLocateLogIntiailisedFromValues;
	CLbsAssistanceDataLogEvent*		iAssistanceDataLogIntiailisedFromValuesFirst; 
	
	
	// Comparison accuracy
	T_LbsUtils::TComparisonAccuracyType iWhatAccuracyToUse;
	



protected:
	void ConstructL();

	};

#endif //__CT_LBS_VERIFY_LOG_INFO_H__
