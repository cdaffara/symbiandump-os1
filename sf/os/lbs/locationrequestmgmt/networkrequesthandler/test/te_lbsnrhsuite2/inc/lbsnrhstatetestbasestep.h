/**
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/



/**
 @file lbsnrhstatetestbasestep.h
*/
 
#ifndef LBSNRHSTATETESTBASESTEP_H_
#define LBSNRHSTATETESTBASESTEP_H_

#include <test/testexecutestepbase.h>
#include "te_lbsnrhsuite2stepbase.h"
#include "te_lbsnrhsuite2defs.h"
#include "lbsnrhngchannel.h"
#include "lbsnrhprivctrlchannel.h"
#include "lbsnrhagpschannel.h"
#include "lbsnrhx3pchannel.h"


/* Class to simplify testing the state changes / message sequences of the NRH.

The functions in this class are meant to be easy to use to build up simple 
sequences for the major NRH operations, e.g. MO-LR, X3P, MT-LR.

They don't do much validation of parameters - that should be covered by other tests.
*/
class CLbsNrhStateTestBaseStep : public CTe_LbsNrhSuiteStepBase
	{
public:
	CLbsNrhStateTestBaseStep();
	~CLbsNrhStateTestBaseStep();
	
	TVerdict doTestStepPreambleL();
	TVerdict doTestStepL();
	TVerdict doTestStepPostambleL();
	
	TInt GetTestCaseId();
	
	// X3P client api messages
	
	void X3pClient_ConnectClientL(TUint aClientId);
	void X3pClient_DisconnectClientL(TUint aClientId);
	void X3pClient_SendX3pRequestL(TUint aClientId, TBool aLowPriority = FALSE);
	void X3pClient_SendX3pCancelL(TUint aClientId);
	void X3pClient_WaitX3pCompleteL(TUint aClientId, TInt aReason);
	void X3pClient_WaitX3pReferencePositionL(TUint aClientId);
	
	// NG channel messages
	
	void NetGateway_SendPrivacyRequestL(
	        TUint32 aSessionId,
	        TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt aAdvice=TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify,
	        TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt aAction=TLbsNetPosRequestPrivacyInt::ERequestActionAllow,
	        TBool aEmergency=EFalse);
	void NetGateway_SendEmergencyPrivacyRequestL(TUint32 aSessionId);
	void NetGateway_SendEmergencyPrivacyRequestAdviceNotifyL(TUint32 aSessionId);

	void NetGateway_SendLocationRequestL(TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
		TInt aMode0 = KErrNotFound, TInt aMode1 = KErrNotFound,
		TInt aHorAccuracy = KErrNotFound,
		TInt aMaxFixTime = KErrNotFound);
	void NetGateway_SendEmergencyLocationRequestL(TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
		TInt aMode0 = KErrNotFound, TInt aMode1 = KErrNotFound);
	
	void NetGateway_SendLocationRequestL(TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
		TTimeIntervalMicroSeconds aMaxFixAge);
	void NetGateway_SendEmergencyLocationRequestL(TUint32 aSessionId,
			TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
			TTimeIntervalMicroSeconds aMaxFixAge);
		
	void NetGateway_SendInitialSuplLocationRequestL(TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService);
	void NetGateway_SendNetworkReferencePositionL(TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService);
	void NetGateway_SendNetworkFinalPositionL(TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService);
	void NetGateway_SendSessionCompleteL(TUint32 aSessionId, TInt aReason,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService);
	void NetGateway_WaitX3pRequestL(TUint& aSessionId);
	void NetGateway_WaitX3pCancelL(TUint aSessionId);
	void NetGateway_WaitPrivacyResponseL(TUint32 aSessionId, 
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt aResponse);
	void NetGateway_WaitLocationResponseL(TUint32 aSessionId, TInt aReason);
	void NetGateway_WaitMeasurementResponseL(TUint32 aSessionId, TInt aReason);

	// AGPS Manager messages

	void AgpsManager_SendGpsPositionUpdateAccurateL(TInt aError = KErrNone);
	void AgpsManager_SendGpsPositionUpdateInaccurateL();
	void AgpsManager_SendGpsMeasurementUpdateL();

	void AgpsManager_WaitLocationRequestOrRequestAndStatusL(TInt aMode0=KErrNotFound, TInt aMode1=KErrNotFound, TInt aHorAccuracy=KErrNotFound, TInt aMaxFixTime=KErrNotFound,TInt aExpectedPowerAdvice=KErrNotFound);
	void AgpsManager_WaitLocationRequestL(TInt aMode0=KErrNotFound, TInt aMode1=KErrNotFound, TInt aHorAccuracy=KErrNotFound, TInt aMaxFixTime=KErrNotFound,TInt aExpectedPowerAdvice=KErrNotFound);
	void AgpsManager_WaitLocationRequestStatusL(TInt aExpectedPowerAdvice=KErrNotFound);
	void AgpsManager_WaitLocationRequestCancelL(TInt aExpectedPowerAdvice=KErrNotFound);
	// Privacy Controller client api messages
	
	void PrivacyController_SendPrivacyResponseL(TUint32 aSessionId, 
		CLbsPrivacyController::TRequestVerificationResult aResponse);
	void PrivacyController_SendPrivacyCancelL(TUint32 aSessionId);
	void PrivacyController_WaitPrivacyRequestL(TUint32 aSessionId);
	void PrivacyController_WaitPrivacyReferencePositionL(TUint32 aSessionId);
	void PrivacyController_WaitPrivacyGpsPositionL(TUint32 aSessionId);
	void PrivacyController_WaitPrivacyCompleteL(TUint32 aSessionId, TInt aReason);

	void ReadIniFileValues();

    // Remember our last power advice. Allows us to control program flow
    // when winscw armv5 timings vary.
    TLbsPositionUpdateRequestBase::TPowerAdvice iLastPowerAdvice;
    
private:
	CNgChannel* iNgChannel;
	CAgpsChannel* iAgpsChannel;
	CPrivCtrlChannel* iPrivCtrlChannel;
	CX3pChannel* iX3pChannel;

	};

_LIT(KLbsNrhStateTestBaseStep, "lbsnrhstatetestbasestep");

#endif /*LBSNRHSTATETESTBASESTEP_H_*/
