// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the class implementation for the MTLR GPS Mode Tests
// EPOC includes.sue
// 
//

// LBS includes. 
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <e32math.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbslocdatasourceclasstypes.h>
#include <lbs/lbslocdatasourcegpsbase.h>
#include <e32property.h>
#include <lbserrors.h>
#include "lbsnetinternalapi.h"
#include "LbsInternalInterface.h"
#include "LbsExtendModuleInfo.h"
#include "lbsdevloggermacros.h"

// LBS test includes.
#include "ctlbsmtlrresetassistance.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>

const TInt KTimeOut = 60*1000*1000;
const TInt KAdviceSystemStatusTimeout = 40*1000*1000;
const TInt KSmallTimeOut = 3*1000*1000;
const TInt KTimeoutWaitingForResetAssistanceData = 2 * 1000 * 1000;

/**
Static Constructor
*/
CT_LbsMTLRResetAssistance* CT_LbsMTLRResetAssistance::New(CT_LbsHybridMTLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMTLRResetAssistance* testStep = new CT_LbsMTLRResetAssistance(aParent);
	if (testStep)
		{
		TInt err = KErrNone;

		TRAP(err, testStep->ConstructL());
		if (err)
			{
			delete testStep;
			testStep = NULL;
			}
		}
	return testStep;
	}


/**
 * Constructor
 */
CT_LbsMTLRResetAssistance::CT_LbsMTLRResetAssistance(CT_LbsHybridMTLRServer& aParent) 
	: CT_LbsHybridMTLRStep(aParent), iProxy(NULL), 
		iNetworkMethod(ENetworkMethodInvalid), 
		iPlannedPositionOriginator(EPositionOriginatorUnkown),
		iNetRequestRejected(EFalse),
		iAdminGpsMode(CLbsAdmin::EGpsModeUnknown)
	{
	SetTestStepName(KLbsMTLRResetAssistance);
	iState = EInitializing;
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0005);	
	}


void CT_LbsMTLRResetAssistance::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMTLRStep::ConstructL();
	iController = CLbsPrivacyController::NewL(*this);
	}

/**
 * Destructor
 */
CT_LbsMTLRResetAssistance::~CT_LbsMTLRResetAssistance()
	{	
	delete iController;
	}

TVerdict CT_LbsMTLRResetAssistance::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMTLRResetAssistance::doTestStepPreambleL()"));
	CT_LbsHybridMTLRStep::doTestStepPreambleL();
	iNetworkExpectsMeasurements = EFalse;
	iNetworkExpectsPositions = EFalse;
	
	// Get the GPS mode set in the Admin
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);

	TESTL(KErrNone == admin->Get(KLbsSettingRoamingGpsMode, iAdminGpsMode));
	CleanupStack::PopAndDestroy(admin);
	
	// Set the network step
	iNetworkMethod = ENetworkMethodInvalid;
	
	SetTestStepResult(EPass);
	T_LbsUtils utils;

	// Get the Network Method
	_LIT(KNetworkMethod, "NetworkMethod");
	TInt networkMethodInt;
	if(GetIntFromConfig(ConfigSection(), KNetworkMethod, networkMethodInt))
		{
		iNetworkMethod = static_cast<TChosenNetworkMethod>(networkMethodInt);
		TEST(iNetworkMethod != ENetworkMethodInvalid);
		}
	else
		{
		INFO_PRINTF1(_L("CT_LbsMTLRResetAssistance::doTestStepPreambleL() - FAILED: no network method configured"));
		TEST(EFalse);
		}

	// Is network method supported by module?
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities deviceCapabilities;
	TInt err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, deviceCapabilities);	
	if((!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted)) && (!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB)))
		{
		if(iNetworkMethod == ETerminalAssistedNetworkMethod)
			{
			iNetRequestRejected = ETrue;
			}
		}
	if((!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased)) && (!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB)))
		{
		if(iNetworkMethod == ETerminalBasedNetworkMethod)
			{
			iNetRequestRejected = ETrue;
			}
		}	
	
	// Get the position originator
	_LIT(KPositionOriginator, "PositionOriginator");
	TInt positionOriginatorInt;
	if(GetIntFromConfig(ConfigSection(), KPositionOriginator, positionOriginatorInt))
		{
		iPlannedPositionOriginator = static_cast<TPlannedPositionOriginator>(positionOriginatorInt);
		TEST(iPlannedPositionOriginator != EPositionOriginatorUnkown);
		}
	else
		{
		INFO_PRINTF1(_L("CT_LbsMTLRResetAssistance::doTestStepPreambleL() - FAILED: no position originator configured"));
		TEST(EFalse);
		}

	// Get the step for sending reset
	_LIT(KStepForSendingReset, "StepForReset");
	TInt stepForResetInt;
	if(GetIntFromConfig(ConfigSection(), KStepForSendingReset, stepForResetInt))
		{
		iStepForSendingReset = static_cast<TNetworkStep>(stepForResetInt);
		TEST(iStepForSendingReset >= EInitial && iStepForSendingReset <= EAfterSessionClose);
		}
	else
		{
		INFO_PRINTF1(_L("CT_LbsMTLRResetAssistance::doTestStepPreambleL() - FAILED: no step for sending reset configured"));
		TEST(EFalse);
		}

	// Get the warm up option
	_LIT(KWarmUpModule, "WarmUpModule");
	TInt warmUpModuleInt;
	if(GetIntFromConfig(ConfigSection(), KWarmUpModule, warmUpModuleInt))
		{
		iWarmUpModule = warmUpModuleInt == 0 ? EFalse : ETrue;
		}
	else
		{
		iWarmUpModule = EFalse;
		}

	// Get the warm up option
	_LIT(KResetMark, "ResetMask");
	TInt resetMask;
	if(!GetIntFromConfig(ConfigSection(), KResetMark, resetMask))
		{
		resetMask = 0xFFFFFFFF;
		}
	iResetMask = resetMask;

	iProxy = CNetProtocolProxy::NewL();
	return TestStepResult();
	}

TVerdict CT_LbsMTLRResetAssistance::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMTLRResetAssistance::doTestStepPostambleL()"));
	iNetworkExpectsMeasurements = EFalse;
	iNetworkExpectsPositions = EFalse;
	iNetworkMethod = ENetworkMethodInvalid;
	iPlannedPositionOriginator = EPositionOriginatorUnkown;
	delete iProxy;
	iProxy = NULL;
	CT_LbsHybridMTLRStep::doTestStepPostambleL();
	return TestStepResult();
	}

TVerdict CT_LbsMTLRResetAssistance::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsMTLRResetAssistance::doTestStepL()"));	
	// Stop the test if the preamble failed
	TESTL(TestStepResult() == EPass);
	
	// >> AdviceSystemStatus(0)
	TESTL(iProxy->WaitForResponse(KAdviceSystemStatusTimeout) == ENetMsgGetCurrentCapabilitiesResponse);
	INFO_PRINTF1(_L("&gt;&gt;			AdviceSystemStatus(ESystemStatusNone)"));		
	CLbsNetworkProtocolBase::TLbsSystemStatus status;
	TInt cleanupCnt;
	cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status); 
	TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
	CleanupStack::PopAndDestroy(cleanupCnt);

	TBool moduleWarmedUp(EFalse);
	TBool expectExtraRequestForAssistanceData(EFalse);
	do
		{
		iState = EInitializing;
		if(moduleWarmedUp)
			{
			iWarmUpModule = EFalse;
			}

		if(!iWarmUpModule && iStepForSendingReset == EBeforeSession)
			{
			SendResetAssistanceDataL();
			}
	
	// Initiate MTLR Start	
		// << ProcessStatusUpdate()
		MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
		INFO_PRINTF1(_L("&lt;&lt;			ProcessStatusUpdate(EServiceMobileTerminated)"));		
		iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);
		
		// << ProcessPrivacyRequest()
		const TBool emergency = EFalse;	// lrm - why is this true in hybrid mtlr testing!?
		TLbsNetPosRequestPrivacy privacy    = ArgUtils::Privacy();	// notify allow
		TLbsExternalRequestInfo requestInfo = ArgUtils::RequestInfo();
		INFO_PRINTF1(_L("&lt;&lt;			ProcessPrivacyRequest()"));	
		iProxy->CallL(ENetMsgProcessPrivacyRequest, &iSessionId, &emergency, &privacy, &requestInfo);	
		
		// >> ProcessNetworkLocationRequest() callback
		CheckForObserverEventTestsL(KTimeOut, *this);	
		
		// >> Respond Privacy Request
		TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRespondPrivacyRequest);
		INFO_PRINTF1(_L("&gt;&gt;			RespondPrivacyRequest(EPrivacyResponseAccepted)"));	
		TLbsNetSessionId* getSessionId = NULL;
		CLbsNetworkProtocolBase::TLbsPrivacyResponse getPrivacy;
		cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondPrivacyRequest, &getSessionId, &getPrivacy);
		TESTL(getSessionId->SessionNum()==iSessionId.SessionNum());
		TESTL(getPrivacy==CLbsNetworkProtocolBase::EPrivacyResponseAccepted);
		CleanupStack::PopAndDestroy(cleanupCnt);//getSessionId	
	
		if(!iWarmUpModule && iStepForSendingReset == EBeginningOfSession)
			{
			SendResetAssistanceDataL();
			}

	// MTLR Reference Position Notification Start
		// << ProcessLocationUpdate()
		TPositionInfo positionInfo = ArgUtils::ReferencePositionInfo();
		INFO_PRINTF1(_L("&lt;&lt;			ProcessLocationUpdate(ref pos)"));	
		iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);
	// MTLR Reference Position Notification Stop	
		TInt reason = KErrNone;

		if(iNetworkMethod != EAutonomousNetworkMethod || iWarmUpModule)
			{
			// MTLR Assistance Data Notification Start
			// << ProcessAssistanceData()
			TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
			RLbsAssistanceDataBuilderSet assistanceData;
			ArgUtils::PopulateLC(assistanceData);
			reason = KErrNone;
			INFO_PRINTF1(_L("&lt;&lt;			ProcessAssistanceData(EAssistanceDataReferenceTime)"));	
			iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
			CleanupStack::PopAndDestroy(); //assistanceData
			// MTLR Assistance Data Notification Stop
		
			if(!iWarmUpModule && iStepForSendingReset == EAfterAssistanceData)
				{
				expectExtraRequestForAssistanceData = SendResetAssistanceDataL();
				}
			}
	
	// MTLR Network Location Request Start
		// << ProcessLocationRequest()
		MLbsNetworkProtocolObserver::TLbsNetProtocolService service = MLbsNetworkProtocolObserver::EServiceMobileTerminated;
		TLbsNetPosRequestQuality quality = ArgUtils::QualityAlpha2(); 
		TLbsNetPosRequestMethod method   = RequestNetworkMethod(iWarmUpModule);
		INFO_PRINTF1(_L("&lt;&lt;			ProcessLocationRequest(timeout = alpha2)"));		
		iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	// MTLR Network Location Request Stop
	// Initiate MTLR End	
		
		//Start the timer
		TTime timerStart;
		timerStart.HomeTime();
		
		// >> ProcessNetworkPositionUpdate(refPosition) callback
		CheckForObserverEventTestsL(KSmallTimeOut, *this);	
		
		if(!iNetRequestRejected)
			{
			// >> RequestAssistanceData()
			TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
			TLbsAsistanceDataGroup dataGroup;
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
			if(!expectExtraRequestForAssistanceData || (iNetworkMethod == EAutonomousNetworkMethod && !iWarmUpModule))
				{
				INFO_PRINTF1(_L("&gt;&gt;			RequestAssistanceData(0)"));	
				TESTL(dataGroup == EAssistanceDataNone);
				}
			else
				{
				expectExtraRequestForAssistanceData = EFalse;
				INFO_PRINTF1(_L("&gt;&gt;			RequestAssistanceData(EAssistanceDataReferenceTime)"));	
				TESTL(dataGroup == EAssistanceDataReferenceTime);
				// << ProcessAssistanceData(EAssistanceDataReferenceTime)
				TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
				RLbsAssistanceDataBuilderSet assistanceData;
				ArgUtils::PopulateLC(assistanceData);
				reason = KErrNone;
				INFO_PRINTF1(_L("&lt;&lt;			ProcessAssistanceData(EAssistanceDataReferenceTime)"));	
				iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
				CleanupStack::PopAndDestroy(&assistanceData);
				}
			CleanupStack::PopAndDestroy(cleanupCnt);		
			}
		
		DecideWhatNetworkReceives();	// lrm - hit this line before request from network has been processed!?	
	
		/*** NRH's Alpha2 timer expires; should get a response now ***/	
		//Find the time elapsed from timer
		TTimeIntervalMicroSeconds microseconds;
	 	TTime timerStop;
	 	timerStop.HomeTime();
	 	microseconds = timerStop.MicroSecondsFrom(timerStart); 
		TInt64 timeElapsed = microseconds.Int64();
		TInt delta = 1000*1000; // 1s
		
		getSessionId = NULL;
		TInt getReason = KErrNone;
		TPositionSatelliteInfo* getPositionInfo = NULL;
		TPositionGpsMeasurementInfo* getMeasurementInfo = NULL;		
		// >> RespondLocationRequest()	
		if(iNetRequestRejected)	// network has asked for an unsupported gps mode
			{// we may get an error response _before_ alpha2 timer expires
			TESTL(iProxy->WaitForResponse(KAlpha2Timeout-timeElapsed+delta) == ENetMsgRespondLocationRequest); 	
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
			INFO_PRINTF2(_L("&gt;&gt;			RespondLocationRequest(%d)"), getReason);		
			TEST(getReason==KErrNotSupported);
			CleanupStack::PopAndDestroy(cleanupCnt);		
			}
		else
			{
			// The measurements:
			if(iNetworkExpectsMeasurements)	// If network expecting measurements, they will come first 		
				{
				// >> RespondLocationRequest(meas)
				//Test that we do not get response before alpha2 has expired
				TESTL(iProxy->WaitForResponse(KAlpha2Timeout-timeElapsed-delta) == ENetMsgTimeoutExpired); 
				INFO_PRINTF1(_L("				No RespondLocationRequest before Alpha2 timer expired"));			
				TESTL(iProxy->WaitForResponse(2*delta) == ENetMsgRespondLocationRequest); 
				INFO_PRINTF1(_L("&gt;&gt;		RespondLocationRequest(meas)"));
				getSessionId = NULL;
				TInt getReason = KErrNone;
				cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getMeasurementInfo);
				TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
				TESTL(getReason==KErrNone); 
				CleanupStack::PopAndDestroy(cleanupCnt); 
	
				if(!iWarmUpModule && iStepForSendingReset == EAfterMeasurements)
					{
					expectExtraRequestForAssistanceData = SendResetAssistanceDataL();
					}
				
				// Make another request from the network
				//<< ProcessLocationRequest() 
				quality = ArgUtils::Quality(); //Set timeout to t 
				INFO_PRINTF1(_L("&lt;&lt;		ProcessLocationRequest(timeout = t)"));	
				iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
				
				TESTL(iProxy->WaitForResponse(KSmallTimeOut) == ENetMsgRequestAssistanceData); 
				TLbsAsistanceDataGroup dataGroup;
				cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataGroup);
				if(!expectExtraRequestForAssistanceData)
					{
					INFO_PRINTF1(_L("&gt;&gt;			RequestAssistanceData(0)"));	
					TESTL(dataGroup == EAssistanceDataNone);
					}
				else
					{
					expectExtraRequestForAssistanceData = EFalse;
					INFO_PRINTF1(_L("&gt;&gt;			RequestAssistanceData(EAssistanceDataReferenceTime)"));	
					TESTL(dataGroup == EAssistanceDataReferenceTime);
					// << ProcessAssistanceData(EAssistanceDataReferenceTime)
					TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
					RLbsAssistanceDataBuilderSet assistanceData;
					ArgUtils::PopulateLC(assistanceData);
					reason = KErrNone;
					INFO_PRINTF1(_L("&lt;&lt;			ProcessAssistanceData(EAssistanceDataReferenceTime)"));	
					iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
					CleanupStack::PopAndDestroy(&assistanceData);
					}
				CleanupStack::PopAndDestroy(cleanupCnt);		
				}
			// The final position:
			if(iPlannedPositionOriginator == EPositionOriginatorModule)
				{
				// The module should return the position
				INFO_PRINTF1(_L("				Network expecting position"));
				// >> RespondLocationRequest()  position.
				TESTL(iProxy->WaitForResponse(KTTimeout + delta) == ENetMsgRespondLocationRequest);
				INFO_PRINTF1(_L("&gt;&gt;		RespondLocationRequest(gpspos)"));			
				getPositionInfo = NULL;
				getSessionId = NULL;
				getReason = KErrNone;			
				cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &getSessionId, &getReason, &getPositionInfo);
				// check it is a position
				TESTL(getPositionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
				TESTL(getSessionId->SessionNum() == iSessionId.SessionNum());
				TESTL(getReason == KErrNone);	
				TESTL(getPositionInfo->PositionMode() == TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
				// Test position is the same as in the ini file data fed to the GPS module ($update,1,0,49.2,3.5,50,20,20*)
				TPosition gpsPos;
				getPositionInfo->GetPosition(gpsPos);
				TESTL(gpsPos.Latitude()==49.2 && gpsPos.Longitude()==3.5 && gpsPos.Altitude()==50 && gpsPos.HorizontalAccuracy()==20 && gpsPos.VerticalAccuracy()==20); 			
				CleanupStack::PopAndDestroy(cleanupCnt);	
	
				if(!iWarmUpModule && iStepForSendingReset == EAfterPosition)
					{
					SendResetAssistanceDataL();
					}
	
				//<< ProcessNetworkPositionUpdate(GPS Pos) Callback
				CheckForObserverEventTestsL(KTimeOut, *this);
				TESTL(iState==EGpsLocReceived);				
				}
			else	// position supplied by network 
				{
				__ASSERT_DEBUG(iPlannedPositionOriginator == EPositionOriginatorNetwork, User::Invariant());
				// privacy app doesn't see network-calculated position, so nothing much happens in this case
				}
		
			}	
		
		// MTLR Session Completion Start
		// << ProcessSessionComplete()
		if(iNetRequestRejected)
			{
			reason = KErrNotSupported;
			}
		else
			{
			reason = KErrNone;
			}
		INFO_PRINTF1(_L("&lt;&lt; ProcessSessionComplete()"));	
		iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
	
		if(!iWarmUpModule && iStepForSendingReset == EAfterSessionClose)
			{
			SendResetAssistanceDataL();
			}
	
		// << ProcessStatusUpdate()
		MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
		INFO_PRINTF1(_L("&lt;&lt; ProcessStatusUpdate(EServiceNone)"));		
		iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);
	
		// >> ProcessRequestComplete() callback
		CheckForObserverEventTestsL(KTimeOut, *this);
	
		// Verify that the last callback was to ProcessSessionComplete()
		TESTL(iState==ERequestComplete);
		moduleWarmedUp = ETrue;
		} while(iWarmUpModule);
// MTLR Session Completion Stop
	
	return TestStepResult();
	}


TLbsNetPosRequestMethod CT_LbsMTLRResetAssistance::RequestNetworkMethod(TBool aWarmUp)
/**
 This method will chose the appropiate method requested by the network, depending 
 on the test settings
 
 @return The network method
 */
	{
	TLbsNetPosRequestMethod method;
	if(aWarmUp)
		{ // warm up in TB
		iNetworkExpectsPositions = ETrue;
		method = ArgUtils::RequestTerminalBasedMethod();
		return method;
		}
	
	switch(iNetworkMethod)
		{
		case ETerminalBasedNetworkMethod:
			{
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestTerminalBasedMethod();
			break;
			}
		case ETerminalBasedTerminalAssistedNetworkMethod:
			{
			iNetworkExpectsMeasurements = ETrue;
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestHybridMethod();
			break;
			}
		case ETerminalAssistedNetworkMethod:
			{
			iNetworkExpectsMeasurements = ETrue;
			method = ArgUtils::RequestTAPMethod();
			break;
			}
		case ETerminalAssistedTerminalBasedNetworkMethod:
			{
			iNetworkExpectsMeasurements = ETrue;
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestTerminalAssistedAndTerminalBasedMethod();
			break;
			}
		case EAutonomousNetworkMethod:
			{
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestAutonomousMethod();
			break;
			}
		case ENetworkMethodNotSet:
			{
			//we should set mode according to default admin 
			TEST(iAdminGpsMode != CLbsAdmin::EGpsModeUnknown);
			switch(iAdminGpsMode)
			{
			case CLbsAdmin::EGpsPreferTerminalBased:
				{
				iNetworkExpectsPositions = ETrue;
				break;
				}
			case CLbsAdmin::EGpsPreferTerminalAssisted:
			case CLbsAdmin::EGpsAlwaysTerminalAssisted:
				{
				iNetworkExpectsMeasurements = ETrue;
				break;
				}
			case CLbsAdmin::EGpsAutonomous:	// test framework doesn't currently support this
			default:
				{
				User::Invariant();
				}
			}
				
			method = ArgUtils::RequestUnspecifiedMethod();
			break;
			}			
		default:
			{
			method = ArgUtils::RequestHybridMethod();
			User::Invariant();
			}
		}
	
	return method;
	}


void CT_LbsMTLRResetAssistance::DecideWhatNetworkReceives()
/**
 This method checks what the module was finally set to and decides if the network 
 should expect measuments, or positions
 */
	{
	TLbsGpsOptionsArray options;
	TPckg<TLbsGpsOptionsArray> pckgOptions(options);
	TEST(KErrNone == RProperty::Get(KUidSystemCategory, ELbsTestAGpsModuleModeChanges, pckgOptions));
	
	if(options.ClassType() & ELbsGpsOptionsArrayClass)
		{ 
		if(options.NumOptionItems() > 1)
			{
			// don't change anything (from what was set in RequestNetworkMethod()), the module is running in hybrid
			return;
			}
		}
	switch(options.GpsMode())
		{
		case CLbsAdmin::EGpsAutonomous:
			{
			INFO_PRINTF1(_L(" Module was asked to use Autonomous mode"));
			iNetworkExpectsPositions = EFalse;
			iNetworkExpectsMeasurements = EFalse;
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalBased:
		case CLbsAdmin::EGpsAlwaysTerminalBased:
			{
			INFO_PRINTF1(_L(" Module was asked to use Terminal Based mode"));
			iNetworkExpectsMeasurements = EFalse;
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalAssisted:
		case CLbsAdmin::EGpsAlwaysTerminalAssisted:
			{
			INFO_PRINTF1(_L(" Module was asked to use Terminal Assisted mode"));
			iNetworkExpectsPositions = EFalse;
			break;
			}
		default:
			{
			// change nothing
			}
		return;
		}
	}

void CT_LbsMTLRResetAssistance::ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& /*aRequestInfo*/, const TNotificationType& /*aNotificationType*/)
	{
	TEST(iState==EInitializing);
	INFO_PRINTF1(_L("&gt;&gt;			ProcessNetworkLocationRequest()"));
	iController->RespondNetworkLocationRequest(aRequestId, CLbsPrivacyController::ERequestAccepted);
	iState = EPrivacyCheckOk;
	ReturnToTestStep();
	}
   
void CT_LbsMTLRResetAssistance::ProcessNetworkPositionUpdate(TUint /*aRequestId*/, const TPositionInfo& aPosInfo)
	{
	if(iState==EPrivacyCheckOk)	// waiting for ref pos
		{
		iState=ERefLocReceived;	
		INFO_PRINTF1(_L("&gt;&gt; Privacy App - ProcessNetworkPositionUpdate(Ref pos)"));
		}
	else if(iState==ERefLocReceived)	// waiting for gps pos
		{
		TPosition getPos;
		aPosInfo.GetPosition(getPos);	
		TESTL(aPosInfo.PositionModeReason() == EPositionModeReasonNone);		
		// Verify we got what network sent: 
		if(iPlannedPositionOriginator == EPositionOriginatorModule)
			{ 
			TESTL(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
			// compare to gps pos in test ini ($update,1,0,49.2,3.5,50,20,20*) :			
			TESTL(getPos.Latitude()==49.2 && getPos.Longitude()==3.5 && getPos.Altitude()==50 && getPos.HorizontalAccuracy()==20 && getPos.VerticalAccuracy()==20);		
			}
		else
			{// compare to gps pos supplied by network:			
			TPositionInfo positionInfo = ArgUtils::AccurateNetworkPositionInfo();
			TPosition getFNP;
			positionInfo.GetPosition(getFNP);			
			TESTL(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);						
			TESTL(getPos.Latitude()==getFNP.Latitude() && getPos.Longitude()==getFNP.Longitude() && getPos.Altitude()==getFNP.Altitude() && getPos.HorizontalAccuracy()==getFNP.HorizontalAccuracy() && getPos.VerticalAccuracy()==getFNP.VerticalAccuracy());	
			}
		
		INFO_PRINTF1(_L("&gt;&gt; Privacy App - ProcessNetworkPositionUpdate(Gps pos)"));
		iState=EGpsLocReceived;	
		}
	ReturnToTestStep();
	}

void CT_LbsMTLRResetAssistance::ProcessRequestComplete(TUint /*aRequestId*/, TInt aReason)
	{
	INFO_PRINTF2(_L("&gt;&gt; Privacy App - ProcessRequestComplete(%d)"), aReason);
	if(!iNetRequestRejected && (iPlannedPositionOriginator == EPositionOriginatorModule))
		{
		TEST(iState==EGpsLocReceived);
		}
	if(iNetRequestRejected)
		{
		TEST(aReason == KErrNotSupported);
		}
	else
		{
		TEST(aReason == KErrNone);
		}
	iState=ERequestComplete; 
	ReturnToTestStep();
	}

TBool CT_LbsMTLRResetAssistance::SendResetAssistanceDataL()
	{
	// First make sure that no reset of assistance data has been received by the GPS module
	RProperty resetAssistanceDataReceivedProperty;
	User::LeaveIfError(resetAssistanceDataReceivedProperty.Attach(KUidSystemCategory, ELbsTestAGpsModuleResetAssistanceDataFlag));
	TInt resetReceivedStatus;
	TInt err = resetAssistanceDataReceivedProperty.Get(resetReceivedStatus);
	TESTL(err == KErrNone || err == KErrNotFound);
	if(err == KErrNone)
		{
		TESTL(resetReceivedStatus == ELbsTestAGpsResetAssistanceDataNotReceived);
		}

	// << ProcessAssistanceData(KPositionAssistanceDataReset)
	RLbsAssistanceDataBuilderSet assistanceData;
	CleanupClosePushL(assistanceData);
	assistanceData.OpenL();
	TInt reason = KPositionAssistanceDataReset;
	INFO_PRINTF1(_L("&lt;&lt;			ProcessAssistanceData(KPositionAssistanceDataReset)"));	
	iProxy->CallL(ENetMsgProcessAssistanceData, &iResetMask, &assistanceData, &reason);
	CleanupStack::PopAndDestroy(&assistanceData);	
	
	// Now check the the module receives and verifies the ResetAssistanceData
	err = resetAssistanceDataReceivedProperty.Get(resetReceivedStatus);
	TESTL(err == KErrNone || err == KErrNotFound);
	if(err == KErrNotFound || resetReceivedStatus != ELbsTestAGpsResetAssistanceDataReceived)
		{
		RTimer timeout;
		timeout.CreateLocal();
		CleanupClosePushL(timeout);
		
		TRequestStatus statTime = KRequestPending, 
					   statProp = KRequestPending;
		timeout.After(statTime, KTimeoutWaitingForResetAssistanceData);
		resetAssistanceDataReceivedProperty.Subscribe(statProp);
		User::WaitForRequest(statProp, statTime);
		
		if(KRequestPending == statProp.Int())
			{
			resetAssistanceDataReceivedProperty.Cancel();
			err = KErrTimedOut;
			}
		else
			{
			err = KErrNone;
			timeout.Cancel();
			}
			
		User::WaitForRequest(statProp, statTime); // Coz the one that didn't signal earlier has
												  // just been completed with KErrCancel, and
												  // we want to avoid the otherwise inevitable
												  // stray signal in any (nested) schedulers.
		if(err == KErrNone)
			{
			err = resetAssistanceDataReceivedProperty.Get(resetReceivedStatus);
			}
		CleanupStack::PopAndDestroy(&timeout);
		TESTL(err == KErrNone && resetReceivedStatus == ELbsTestAGpsResetAssistanceDataReceived);
		}
	// Reset the status on the property
	err = resetAssistanceDataReceivedProperty.Set(ELbsTestAGpsResetAssistanceDataNotReceived);
	TESTL(err == KErrNone);
	if((iResetMask & EAssistanceDataReferenceTime) != 0)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}
