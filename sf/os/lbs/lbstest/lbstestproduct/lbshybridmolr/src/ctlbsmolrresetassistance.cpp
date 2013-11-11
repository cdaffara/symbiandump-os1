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
// This is the class implementation for the Module Information Tests
// EPOC includes.sue
// 
//

// LBS includes. 
#include <lbssatellite.h>
#include <lbs/lbsnetprotocolbase.h>
#include <lbs/lbsassistancedatabuilderset.h>
#include <e32math.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsx3p.h>
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
#include "ctlbsmolrresetassistance.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>

const TInt KTimeOut = 60*1000*1000;
const TInt KTimeoutWaitingForResetAssistanceData = 2 * 1000 * 1000;

/**
Static Constructor
*/
CT_LbsMolrResetAssistance* CT_LbsMolrResetAssistance::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMolrResetAssistance* testStep = new CT_LbsMolrResetAssistance(aParent);
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
CT_LbsMolrResetAssistance::CT_LbsMolrResetAssistance(CT_LbsHybridMOLRServer& aParent) 
	: CT_LbsHybridMOLRStep(aParent), iNetworkMethod(ENetworkMethodInvalid), 
		iNetworkProtocolProxyStep(ENetworkProtocolProxyStepInitial), iProxy(NULL), 
		iAdminGpsMode(CLbsAdmin::EGpsModeUnknown), iPlannedPositionOriginator(EPositionOriginatorUnkown), 
		iGPSModeNotSupported(EFalse)
	{
	SetTestStepName(KLbsMolrResetAssistance);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	}


void CT_LbsMolrResetAssistance::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsMolrResetAssistance::~CT_LbsMolrResetAssistance()
	{
	}

TVerdict CT_LbsMolrResetAssistance::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMolrResetAssistance::doTestStepPreambleL()"));
	CT_LbsHybridMOLRStep::doTestStepPreambleL();
	iNetworkExpectsMeasurments = EFalse;
	iNetworkExpectsPositions = EFalse;

	// Get the GPS mode set in the Admin
	CLbsAdmin* admin = CLbsAdmin::NewL();
	CleanupStack::PushL(admin);

	TESTL(KErrNone == admin->Get(KLbsSettingRoamingGpsMode, iAdminGpsMode));
	CleanupStack::PopAndDestroy(admin);
	
	// Set the test up so it expects that the GPS mode won't be supported if there is an incompatibility
	TPositionModuleInfoExtended::TDeviceGpsModeCapabilities deviceCapabilities;
	TInt err = LbsModuleInfo::GetDeviceCapabilities(KLbsGpsLocManagerUid, deviceCapabilities);
	
		if(!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB) && !(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalAssisted))
			{
			if(iAdminGpsMode == CLbsAdmin::EGpsPreferTerminalAssisted || 
					iAdminGpsMode == CLbsAdmin::EGpsAlwaysTerminalAssisted)
				{
				iGPSModeNotSupported = ETrue;
				}	
			}
		if(!(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeSimultaneousTATB) && !(deviceCapabilities & TPositionModuleInfoExtended::EDeviceGpsModeTerminalBased))
			{
			if(iAdminGpsMode == CLbsAdmin::EGpsPreferTerminalBased || 
					iAdminGpsMode == CLbsAdmin::EGpsAlwaysTerminalBased ||
					iAdminGpsMode == CLbsAdmin::EGpsAutonomous)
				{
				iGPSModeNotSupported = ETrue;
				}	
			}

	// Set the network step
	iNetworkMethod = ENetworkMethodInvalid;
	iNetworkProtocolProxyStep = ENetworkProtocolProxyStepInitial;
	
	SetTestStepResult(EPass);
	T_LbsUtils utils;

	// Get the Network Method
	_LIT(KNetworkMethod, "NetworkMethod");
	TInt networkMethodInt;
	if(GetIntFromConfig(ConfigSection(), KNetworkMethod, networkMethodInt))
		{
		iNetworkMethod = static_cast<TChosenNetworkMethod>(networkMethodInt);
		}
	else
		{
		INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::doTestStepPreambleL() - FAILED: no network method configured"));
		TEST(EFalse);
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
		INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::doTestStepPreambleL() - FAILED: no position originator configured"));
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

	if(!iGPSModeNotSupported)
		{
		iProxy = CNetProtocolProxy::NewL();
		}
	return TestStepResult();
	}

TVerdict CT_LbsMolrResetAssistance::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMolrResetAssistance::doTestStepPostambleL()"));
	iNetworkExpectsMeasurments = EFalse;
	iNetworkExpectsPositions = EFalse;
	iNetworkMethod = ENetworkMethodInvalid;
	iNetworkProtocolProxyStep = ENetworkProtocolProxyStepInitial;
	iPlannedPositionOriginator = EPositionOriginatorUnkown;
	iAdminGpsMode = CLbsAdmin::EGpsModeUnknown;
	delete iProxy;
	iProxy = NULL;
	CT_LbsHybridMOLRStep::doTestStepPostambleL();
	return TestStepResult();
	}

TVerdict CT_LbsMolrResetAssistance::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);
	iModuleWarmedUp = EFalse;
	iExpectExtraRequestForAssistanceData = EFalse;
	DoNetworkProtocolProxyStepL();
	
	do
		{
		iState = EInitializing;
		// Do the /actual/ test step!
		RPositionServer server;
		TESTL(KErrNone == server.Connect());
		CleanupClosePushL(server);	

		RPositioner pos;
		TESTL(KErrNone == pos.Open(server));
		CleanupClosePushL(pos);
		
		CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

		if(iModuleWarmedUp)
			{
			iWarmUpModule = EFalse;
			}

		if(!iWarmUpModule && iStepForSendingReset == EBeforeSession)
			{
			SendResetAssistanceDataL();
			}

		// <<(App) NotifyPositionUpdate().
		INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::doTestStepL() 		<<(App) NotifyPositionUpdate()"));	
		pWatch->IssueNotifyPositionUpdate();
		
		DoNetworkProtocolProxyStepL();
	
		// TEST: Get NotifyPositionUpdate app side - get the ref pos.
		INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::doTestStepL() Waiting for ref pos"));
		CheckForObserverEventTestsL(KTimeOut, *this);
	
		if(!iGPSModeNotSupported)
			{ // if it can still carry on since the module didn't reject the request
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::doTestStepL() 		<<(App) NotifyPositionUpdate()"));	
			pWatch->IssueNotifyPositionUpdate();
		
			DoNetworkProtocolProxyStepL();
		
			// Client recv - the gps position determined by the gps module.
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::doTestStepL() Waiting for gps pos"));
			CheckForObserverEventTestsL(KTimeOut, *this);
			if(!iGPSModeNotSupported)
				{
				TESTL(iState == EGpsLocReceived);
				DoNetworkProtocolProxyStepL();
				}
			}


		// Done. Now cleanup...
		CleanupStack::PopAndDestroy(pWatch);
		CleanupStack::PopAndDestroy(&pos);
		CleanupStack::PopAndDestroy(&server);
		iModuleWarmedUp = ETrue;
		} while(iWarmUpModule);
		
	return TestStepResult();
	}

// MPosServerObserver
void CT_LbsMolrResetAssistance::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::OnGetLastKnownPosition() - FAILED: not expecting this call"));
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsMolrResetAssistance::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	INFO_PRINTF2(_L("CT_LbsMolrResetAssistance::OnNotifyPositionUpdate()	>> NPUD() complete with %d"), aErr);
	if(iGPSModeNotSupported)
		{
		TEST(aErr == KErrNotSupported);
		}
	else
		{
		// Verify error.
		TEST(aErr == KErrNone);
	
		// Verify position.
	
		// Expecting ref pos.
		if (iState == EInitializing)
			{
			iState = ERefLocReceived;
			
			TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
			TEST(aPosInfo.PositionMode() == TPositionModuleInfo::ETechnologyNetwork);
	
			// check for refpos details
			const TPositionInfo posInfo = static_cast<const TPositionInfo&>(aPosInfo);
			TEST(ArgUtils::ComparePositionInfoToMolrRefPos(posInfo));
			}
			
		// Expecting gps pos.
		else if (iState == ERefLocReceived)
			{
			iState = EGpsLocReceived;
	
			TEST(aPosInfo.PositionClassType() == EPositionInfoClass);
			if(iPlannedPositionOriginator == EPositionOriginatorModule)
				{
				TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted));
				}
			if(iPlannedPositionOriginator == EPositionOriginatorNetwork)
				{
				TEST(aPosInfo.PositionMode() == (TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted));
				}
			}
		
		// Not expecting anything else.
		else
			{
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::OnNotifyPositionUpdate() - FAILED: unexpected state"));
			TEST(EFalse);
			}
		}

	ReturnToTestStep();
	}

TLbsNetPosRequestMethod CT_LbsMolrResetAssistance::RequestNetworkMethod()
/**
 This method will chose the appropiate method requested by the network, depending 
 on the test settings
 
 @return The network method
 */
	{
	TLbsNetPosRequestMethod method;

	if(iWarmUpModule)
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
			iNetworkExpectsMeasurments = ETrue;
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestHybridMethod();
			break;
			}
		case ETerminalAssistedNetworkMethod:
			{
			iNetworkExpectsMeasurments = ETrue;
			method = ArgUtils::RequestTAPMethod();
			break;
			}
		case ETerminalAssistedTerminalBasedNetworkMethod:
			{
			iNetworkExpectsMeasurments = ETrue;
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
				iNetworkExpectsMeasurments = ETrue;
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
			User::Invariant();
			}
		}
	return method;
	}

void CT_LbsMolrResetAssistance::DecideWhatNetworkDoesntReceive()
/**
 This method checks what the module was finally set to and decides if the network 
 should not expect measuments, or positions
 */
	{
	//INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DecideWhatNetworkDoesntReceive()"));
	TLbsGpsOptionsArray options;
	TPckg<TLbsGpsOptionsArray> pckgOptions(options);
	TEST(KErrNone == RProperty::Get(KUidSystemCategory, ELbsTestAGpsModuleModeChanges,pckgOptions));

	if(options.ClassType() & ELbsGpsOptionsArrayClass)
		{ 
		if(options.NumOptionItems() > 1)
			{// don't change anything, the module is running in hybrid
			return;
			}
		}
	switch(options.GpsMode())
		{
		case CLbsAdmin::EGpsAutonomous:
			{
			iNetworkExpectsPositions = EFalse;
			iNetworkExpectsMeasurments = EFalse;
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalBased:
		case CLbsAdmin::EGpsAlwaysTerminalBased:
			{
			iNetworkExpectsMeasurments = EFalse;
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalAssisted:
		case CLbsAdmin::EGpsAlwaysTerminalAssisted:
			{
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

void CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL()
	{
	if(iGPSModeNotSupported)
		{ // Do nothing, the network won't do much before the module rejects the request
		return;
		}
	TInt cleanupCnt;
	// INFO_PRINTF2(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() Step %d"),iNetworkProtocolProxyStep);
	switch(iNetworkProtocolProxyStep)
		{
		case ENetworkProtocolProxyStepInitial:
			{
			// >> AdviceSystemStatus()
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> AdviceSystemStatus()"));
			TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgGetCurrentCapabilitiesResponse);
			CLbsNetworkProtocolBase::TLbsSystemStatus status;
			cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
			TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone);
			CleanupStack::PopAndDestroy(cleanupCnt);
			iNetworkProtocolProxyStep = ENetworkProtocolProxyStepAfterFirstNPUD;
			break;
			}
		case ENetworkProtocolProxyStepAfterFirstNPUD:
			{
			// >> RequestSelfLocation()
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestSelfLocation() - as result of assistance data request from module"));
			TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

			if(!iWarmUpModule && iStepForSendingReset == EBeginningOfSession)
				{
				SendResetAssistanceDataL();
				}

			// Process the response.
			TLbsNetSessionId* 					sessionId = NULL;
			TLbsNetPosRequestOptionsAssistance*	opts = NULL;
			
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

			iSessionId.SetSessionNum(sessionId->SessionNum());
			iSessionId.SetSessionOwner(sessionId->SessionOwner());
			
			CleanupStack::PopAndDestroy(cleanupCnt);
			sessionId = NULL;
			opts = NULL;	
			
			// << ProcessStatusUpdate(EServiceSelfLocation)
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessStatusUpdate(EServiceSelfLocation)"));
			MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
			iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask1);

			// << ProcessLocationUpdate(SessionId, RefPosition)
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessLocationUpdate(SessionId, RefPosition)"));
			TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
			iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

			iNetworkProtocolProxyStep = ENetworkProtocolProxyStepAfterSecondNPUD;
			break;
			}
		case ENetworkProtocolProxyStepAfterSecondNPUD:
			{
			// >> RequestAssistanceData - as a result of the second client request.
			TLbsAsistanceDataGroup dataMask;	
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
			TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
			TESTL(dataMask == EAssistanceDataNone);
			CleanupStack::PopAndDestroy(cleanupCnt);

			// << ProcessAssistanceData()
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessAssistanceData()"));
			dataMask = EAssistanceDataReferenceTime;
			RLbsAssistanceDataBuilderSet assistanceData;
			ArgUtils::PopulateLC(assistanceData);
			TInt reason = KErrNone;
			iProxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
			CleanupStack::PopAndDestroy(); // assistanceData

			if(!iWarmUpModule && iStepForSendingReset == EAfterAssistanceData)
				{
				iExpectExtraRequestForAssistanceData = SendResetAssistanceDataL();
				}

			// << ProcessLocationRequest() 
			TBool emergency = EFalse;
			MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
			TLbsNetPosRequestQuality quality = ArgUtils::Quality();
			//quality.SetMaxFixTime(ArgUtils::Alpha2()/10);
		    quality.SetMaxFixTime(ArgUtils::Alpha2()/4);
			TLbsNetPosRequestMethod method   = RequestNetworkMethod();
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessLocationRequest()"));			
			iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
			// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
			TTime startTime;
			startTime.HomeTime();

			TInt response = iProxy->WaitForResponse(KTimeOut);
			TLbsNetSessionId* sessionId = NULL;
			if(response == ENetMsgRespondLocationRequest)
				{
				// >> RespondLocationRequest(KErrNotSupported) - as a result of the NRH request conflicting with the module capabilities 
				INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RespondLocationRequest(KErrNotSupported)"));
				sessionId = NULL;
				TPositionInfo* positionInfo = NULL;
				reason = KErrNone;
				cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
				TESTL(positionInfo->PositionClassType() == EPositionInfoClass);
				TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
				TESTL(reason == KErrNotSupported);
				CleanupStack::PopAndDestroy(cleanupCnt);

				// << ProcessSessionComplete()
				INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessSessionComplete()"));			
				iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
				iGPSModeNotSupported = ETrue;

				// << ProcessStatusUpdate()
				MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
				INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessStatusUpdate(EServiceNone)"));			
				iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);
				
				iNetworkProtocolProxyStep = ENetworkProtocolProxyStepInitial;
				}
			else
				{
				// >> RequestAssistanceData() - as a result of the NRH request. 
				TESTL(response == ENetMsgRequestAssistanceData); 
				cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
				if(!iExpectExtraRequestForAssistanceData || (iNetworkMethod == EAutonomousNetworkMethod && !iWarmUpModule))
					{
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));	
					TESTL(dataMask == EAssistanceDataNone);
					}
				else
					{
					iExpectExtraRequestForAssistanceData = EFalse;
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(EAssistanceDataReferenceTime)"));	
					TESTL(dataMask == EAssistanceDataReferenceTime);
					// << ProcessAssistanceData(EAssistanceDataReferenceTime)
					TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
					RLbsAssistanceDataBuilderSet assistanceData;
					ArgUtils::PopulateLC(assistanceData);
					reason = KErrNone;
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessAssistanceData(EAssistanceDataReferenceTime)"));	
					iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
					CleanupStack::PopAndDestroy(&assistanceData);
					}
				CleanupStack::PopAndDestroy(cleanupCnt);
	
				// lrm: need to update this for cases where network request is rejected:
				// Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
				TTimeIntervalMicroSeconds microseconds;
				TTime stopTime;
				stopTime.HomeTime();
				microseconds = stopTime.MicroSecondsFrom(startTime); 
				TInt64 timeElapsed = microseconds.Int64();
				TInt delta = 2 * 1000 * 1000; // 2 secs.
				
				// >> RespondLocationRequest()
				sessionId = NULL;
				DecideWhatNetworkDoesntReceive();
				const TInt t = 8 * 1000 * 1000; // 8 secs.
				
				if(iNetworkExpectsMeasurments)
					{ // measurements should be sent to the network
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() Network expecting measurements"));
					// Test that we do not get response before alpha2 has expired:
					TInt alpha2timeout = ArgUtils::Alpha2()/4 - timeElapsed - delta > 0 ? ArgUtils::Alpha2()/4 - timeElapsed - delta : 0;
					TESTL(iProxy->WaitForResponse(alpha2timeout) == ENetMsgTimeoutExpired);
					INFO_PRINTF1(_L("				No RespondLocationRequest before Alpha2 timer expired"));
					// Wait for and process the response:
					TESTL(iProxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest);	
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RespondLocationRequest(meas)"));
					sessionId = NULL;
					TPositionGpsMeasurementInfo* measurementInfo = NULL;
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
					TESTL(sessionId->SessionNum() == iSessionId.SessionNum());	// module sends NAN measurement first
					TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
					if(reason != KErrNone)
						{
						INFO_PRINTF2(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() reason = 0x%x"), reason);
						}
					TESTL(reason == KErrNone);
					CleanupStack::PopAndDestroy(cleanupCnt);//sessionId, measurementInfo
	
					// Wait for Assistance Data before resetting assistance data ... otherwise 
					// who knows what mask we would receive!
					// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issuing a location request when it's
					//									max fix time timer expries.
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
	
					// now reset the asistance data
					if(!iWarmUpModule && iStepForSendingReset == EAfterMeasurements)
										{
										iExpectExtraRequestForAssistanceData = SendResetAssistanceDataL();
										}
					quality.SetMaxFixTime(t);
	
					// << ProcessLocationRequest()
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessLocationRequest()"));
					iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	
					// now as we have reset the assistanvce datat we expect
					// to get a non-zero mask.
					// >> RequestAssistanceData()
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					if(!iExpectExtraRequestForAssistanceData || (iNetworkMethod == EAutonomousNetworkMethod && !iWarmUpModule))
						{
						INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(EAssistanceDataReferenceTime)"));	
						TESTL(dataMask == EAssistanceDataReferenceTime);
						}
					else
						{
						iExpectExtraRequestForAssistanceData = EFalse;
						INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(EAssistanceDataReferenceTime)"));	
						TESTL(dataMask == EAssistanceDataReferenceTime);
						// << ProcessAssistanceData(EAssistanceDataReferenceTime)
						TLbsAsistanceDataGroup dataRequestMask = EAssistanceDataReferenceTime;
						RLbsAssistanceDataBuilderSet assistanceData;
						ArgUtils::PopulateLC(assistanceData);
						reason = KErrNone;
						INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessAssistanceData(EAssistanceDataReferenceTime)"));	
						iProxy->CallL(ENetMsgProcessAssistanceData, &dataRequestMask, &assistanceData, &reason);
						CleanupStack::PopAndDestroy(&assistanceData);
						}
					CleanupStack::PopAndDestroy(cleanupCnt);
	
					}
	
				if(iPlannedPositionOriginator == EPositionOriginatorModule)
					{ // the module will deliver the location
					if(iNetworkExpectsPositions)
						{
						INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() Network expecting position"));
						TNetProtocolResponseType type = iProxy->WaitForResponse(t + delta);
						if(type == ENetMsgRequestAssistanceData)
							{
							// >> RequestAssistanceData(0)
							INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
							cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
							TESTL(dataMask == EAssistanceDataNone);
							CleanupStack::PopAndDestroy(cleanupCnt);
							type = iProxy->WaitForResponse(t + delta);
							}
						// >> RespondLocationRequest() - first measurement, second position.
						INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RespondLocationRequest(gpspos)"));
						TESTL(type == ENetMsgRespondLocationRequest);
						sessionId = NULL;
						TPositionInfo* positionInfo = NULL;
						cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
						
						// check it is a position
						TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass));
						TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
						TESTL(reason == KErrNone);
						
						if(!iWarmUpModule && iStepForSendingReset == EAfterPosition)
							{
							SendResetAssistanceDataL();
							}

						// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
						// Return modules' position as FinalNetworkPosition
						INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessLocationUpdate(SessionId, FinalNetworkPosition)"));
						iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, positionInfo);
						CleanupStack::PopAndDestroy(cleanupCnt);
						}
					else	
						{ // network doesn't expect the module to be calculating the position and will terminate 'normally' when it's satisfied
						// Not expecting anything else to arrive at the network, make sure it doesn't:
						TESTL(iProxy->WaitForResponse(t + delta) == ENetMsgTimeoutExpired);
						}
					}
				else if(iPlannedPositionOriginator == EPositionOriginatorNetwork)
					{ // The network should return the position
					TPositionInfo positionInfo = ArgUtils::AccurateNetworkPositionInfo();
					// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
					// Return FinalNetworkPosition
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessLocationUpdate(SessionId, FNP)"));
					iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);

					// >> RequestAssistanceData(0)
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);

					// >> CancelSelfLocation()
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> CancelSelfLocation()"));			
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgCancelSelfLocation);

					// >> RequestAssistanceData(0)
					INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					}
				iNetworkProtocolProxyStep = ENetworkProtocolProxyStepFinal;
				}
			
			break;
			}
		case ENetworkProtocolProxyStepFinal:
			{
			// << ProcessSessionComplete()
			TInt reason = KErrNone;
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessSessionComplete()"));			
			iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

			if(!iWarmUpModule && iStepForSendingReset == EAfterSessionClose)
				{
				SendResetAssistanceDataL();
				}

			// << ProcessStatusUpdate()
			MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
			INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessStatusUpdate(EServiceNone)"));			
			iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);
			iProxy->WaitForResponse(KTimeoutWaitingForResetAssistanceData);
			
			iNetworkProtocolProxyStep = ENetworkProtocolProxyStepAfterFirstNPUD;
			break;
			}
		default:
			{
			User::Invariant();
			}
		}
	}

TBool CT_LbsMolrResetAssistance::SendResetAssistanceDataL()
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
	INFO_PRINTF1(_L("CT_LbsMolrResetAssistance::DoNetworkProtocolProxyStepL() << ProcessAssistanceData(KPositionAssistanceDataReset)"));	
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
