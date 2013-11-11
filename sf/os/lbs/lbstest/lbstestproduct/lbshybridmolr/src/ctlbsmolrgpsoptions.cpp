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
#include "lbsnetinternalapi.h"
#include "LbsInternalInterface.h"
#include "LbsExtendModuleInfo.h"
#include "lbsdevloggermacros.h"

// LBS test includes.
#include "ctlbsmolrgpsoptions.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>

const TInt KTimeOut = 60*1000*1000;

/**
Static Constructor
*/
CT_LbsMolrGpsOptions* CT_LbsMolrGpsOptions::New(CT_LbsHybridMOLRServer& aParent)
	{
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;
	CT_LbsMolrGpsOptions* testStep = new CT_LbsMolrGpsOptions(aParent);
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
CT_LbsMolrGpsOptions::CT_LbsMolrGpsOptions(CT_LbsHybridMOLRServer& aParent) 
	: CT_LbsHybridMOLRStep(aParent), iNetworkMethod(ENetworkMethodInvalid), 
		iNetworkProtocolProxyStep(ENetworkProtocolProxyStepInitial), iProxy(NULL), 
		iAdminGpsMode(CLbsAdmin::EGpsModeUnknown), iPlannedPositionOriginator(EPositionOriginatorUnkown), 
		iGPSModeNotSupported(EFalse)
	{
	SetTestStepName(KLbsMolrGpsOptions);
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	}


void CT_LbsMolrGpsOptions::ConstructL()
	{
	// Create the base class objects.
	CT_LbsHybridMOLRStep::ConstructL();
	}

/**
 * Destructor
 */
CT_LbsMolrGpsOptions::~CT_LbsMolrGpsOptions()
	{
	}

TVerdict CT_LbsMolrGpsOptions::doTestStepPreambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMolrGpsOptions::doTestStepPreambleL()"));
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
		INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::doTestStepPreambleL() - FAILED: no network method configured"));
		TEST(EFalse);
		}
	
	// Get the ImmediateMeasurements flag
	_LIT(KImmediateMeasurements, "ImmediateMeasurements");
	TInt immediateMeasurementsInt = EFalse;
	if(GetIntFromConfig(ConfigSection(), KImmediateMeasurements, immediateMeasurementsInt))
		{
		iImmediateMeasurements = static_cast<TBool>(immediateMeasurementsInt);
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
		INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::doTestStepPreambleL() - FAILED: no position originator configured"));
		TEST(EFalse);
		}

	if(!iGPSModeNotSupported)
		{
		iProxy = CNetProtocolProxy::NewL();
		}
	return TestStepResult();
	}

TVerdict CT_LbsMolrGpsOptions::doTestStepPostambleL()
	{
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsMolrGpsOptions::doTestStepPostambleL()"));
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

TVerdict CT_LbsMolrGpsOptions::doTestStepL()
	{
	INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::doTestStepL()"));	
	// Stop the test if the preable failed
	TESTL(TestStepResult() == EPass);
	DoNetworkProtocolProxyStepL();
	
// Do the /actual/ test step!
	RPositionServer server;
	TESTL(KErrNone == server.Connect());
	CleanupClosePushL(server);	

	RPositioner pos;
	TESTL(KErrNone == pos.Open(server));
	CleanupClosePushL(pos);
	
	CPosServerWatcher *pWatch = CPosServerWatcher::NewLC(pos, *this);

	// <<(App) NotifyPositionUpdate().
	INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::doTestStepL() 		<<(App) NotifyPositionUpdate()"));	
	pWatch->IssueNotifyPositionUpdate();
	
	DoNetworkProtocolProxyStepL();

	// TEST: Get NotifyPositionUpdate app side - get the ref pos.
	INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::doTestStepL() Waiting for ref pos"));
	CheckForObserverEventTestsL(KTimeOut, *this);

	if(!iGPSModeNotSupported)
		{ // if it can still carry on since the module didn't reject the request
		INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::doTestStepL() 		<<(App) NotifyPositionUpdate()"));	
		pWatch->IssueNotifyPositionUpdate();
	
		DoNetworkProtocolProxyStepL();
	
		// Client recv - the gps position determined by the gps module.
		INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::doTestStepL() Waiting for gps pos"));
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
		
	return TestStepResult();
	}

// MPosServerObserver
void CT_LbsMolrGpsOptions::OnGetLastKnownPosition(TInt32 /*aErr*/, const TPositionInfoBase& /*aPosInfo*/)
	{
	INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::OnGetLastKnownPosition() - FAILED: not expecting this call"));
	TEST(EFalse); // Shouldn't see this...
	ReturnToTestStep();
	}


void CT_LbsMolrGpsOptions::OnNotifyPositionUpdate(TInt32 aErr, const TPositionInfoBase& aPosInfo)
	{
	INFO_PRINTF2(_L("CT_LbsMolrGpsOptions::OnNotifyPositionUpdate()	>> NPUD() complete with %d"), aErr);
	if(iGPSModeNotSupported)
		{
		if(EApiVariant1 == iExpectedApiBehaviour)
			{
			TEST(aErr == KErrNotSupported);
			}
		else if(EApiVariant2 == iExpectedApiBehaviour)
			{
			TEST(((aErr == KErrCorrupt) || (aErr == KErrInUse) || (aErr == KErrNotSupported)));
			}
		else
			{
			//Should be no other options
			TEST(EFalse);
			}
		
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
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::OnNotifyPositionUpdate() - FAILED: unexpected state"));
			TEST(EFalse);
			}
		}

	ReturnToTestStep();
	}

TLbsNetPosRequestMethod CT_LbsMolrGpsOptions::RequestNetworkMethod()
/**
 This method will chose the appropiate method requested by the network, depending 
 on the test settings
 
 @return The network method
 */
	{
	TLbsNetPosRequestMethod method;
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

void CT_LbsMolrGpsOptions::CheckQualityL(TLbsNetPosRequestOptionsAssistance* aOpts)
	{
	//INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::CheckQualityL()"));
	TBool qualitycheck;
	switch(iAdminGpsMode)
	{
	case CLbsAdmin::EGpsPreferTerminalBased:
		{
		qualitycheck = ArgUtils::CompareQuality(aOpts, 
				ETrue, 									// new client
				KMinHorizontalAcc, 
				KMinVerticalAcc, 
				KDefaultMaxFixTime,
				0, 
				EAssistanceDataReferenceTime, 
				(TPositionModuleInfo::ETechnologyTerminal 
				| TPositionModuleInfo::ETechnologyAssisted)
				);
		break;
		}
	case CLbsAdmin::EGpsPreferTerminalAssisted:
	case CLbsAdmin::EGpsAlwaysTerminalAssisted:
		{
		qualitycheck = ArgUtils::CompareQuality(aOpts, 
				ETrue, 
				KMinHorizontalAcc, 
				KMinVerticalAcc, 
				KDefaultMaxFixTime,
				0, 
				EAssistanceDataReferenceTime, 
				(TPositionModuleInfo::ETechnologyNetwork 
				| TPositionModuleInfo::ETechnologyAssisted)
				);
		break;
		}
	default:
		{
		qualitycheck = EFalse;
		break;
		}
	}
	TESTL(qualitycheck);
	}

void CT_LbsMolrGpsOptions::DecideWhatNetworkDoesntReceive()
/**
 This method checks what the module was finally set to and decides if the network 
 should not expect measuments, or positions
 */
	{
	//INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DecideWhatNetworkDoesntReceive()"));
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

void CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL()
	{
	if(iGPSModeNotSupported)
		{ // Do nothing, the network won't do much before the module rejects the request
		return;
		}
	TInt cleanupCnt;
	// INFO_PRINTF2(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() Step %d"),iNetworkProtocolProxyStep);
	switch(iNetworkProtocolProxyStep)
		{
		case ENetworkProtocolProxyStepInitial:
			{
			// >> AdviceSystemStatus()
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> AdviceSystemStatus()"));
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
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RequestSelfLocation() - as result of assistance data request from module"));
			TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestSelfLocation);

			// Process the response.
			TLbsNetSessionId* 					sessionId = NULL;
			TLbsNetPosRequestOptionsAssistance*	opts = NULL;
			
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);

			CheckQualityL(opts);
			iSessionId.SetSessionNum(sessionId->SessionNum());
			iSessionId.SetSessionOwner(sessionId->SessionOwner());
			
			CleanupStack::PopAndDestroy(cleanupCnt);
			sessionId = NULL;
			opts = NULL;	
			
			// << ProcessStatusUpdate(EServiceSelfLocation)
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessStatusUpdate(EServiceSelfLocation)"));
			MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
			iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask1);

			// << ProcessLocationUpdate(SessionId, RefPosition)
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessLocationUpdate(SessionId, RefPosition)"));
			TPositionInfo refPosInfo = ArgUtils::MolrReferencePositionInfo();
			iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &refPosInfo);

			iNetworkProtocolProxyStep = ENetworkProtocolProxyStepAfterSecondNPUD;
			break;
			}
		case ENetworkProtocolProxyStepAfterSecondNPUD:
			{
			// >> RequestAssistanceData - as a result of the second client request.
			TLbsAsistanceDataGroup dataMask;	
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
			TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
			cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
			TESTL(dataMask == EAssistanceDataNone);
			CleanupStack::PopAndDestroy(cleanupCnt);

			// << ProcessAssistanceData()
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessAssistanceData()"));
			dataMask = EAssistanceDataReferenceTime;
			RLbsAssistanceDataBuilderSet assistanceData;
			ArgUtils::PopulateLC(assistanceData);
			TInt reason = KErrNone;
			iProxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
			CleanupStack::PopAndDestroy(); // assistanceData

			// << ProcessLocationRequest() 
			TBool emergency = EFalse;
			MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
			TLbsNetPosRequestQuality quality = ArgUtils::Quality();
			quality.SetMaxFixTime(ArgUtils::Alpha2());
			TLbsNetPosRequestMethod method   = RequestNetworkMethod();
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessLocationRequest()"));			
			iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
			// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
			TTime startTime;
			startTime.HomeTime();

			TInt response = iProxy->WaitForResponse(KTimeOut);
			TLbsNetSessionId* sessionId = NULL;
			if(response == ENetMsgRespondLocationRequest)
				{
				// >> RespondLocationRequest(KErrNotSupported) - as a result of the NRH request conflicting with the module capabilities 
				INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RespondLocationRequest(KErrNotSupported)"));
				sessionId = NULL;
				TPositionInfo* positionInfo = NULL;
				reason = KErrNone;
				cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
				TESTL(positionInfo->PositionClassType() == EPositionInfoClass);
				TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
				TESTL(reason == KErrNotSupported);
				CleanupStack::PopAndDestroy(cleanupCnt);

				// << ProcessSessionComplete()
				INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessSessionComplete()"));			
				iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);
				iGPSModeNotSupported = ETrue;

				// << ProcessStatusUpdate()
				MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
				INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessStatusUpdate(EServiceNone)"));			
				iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);
				
				iNetworkProtocolProxyStep = ENetworkProtocolProxyStepInitial;
				}
			else
				{
				// >> RequestAssistanceData() - as a result of the NRH request. 
				INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
				TESTL(response == ENetMsgRequestAssistanceData); 
				cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
				TESTL(dataMask == EAssistanceDataNone);
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
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() Network expecting measurements"));
					if (!iImmediateMeasurements)
						{
						TESTL(iProxy->WaitForResponse(ArgUtils::Alpha2() - timeElapsed - delta) == ENetMsgTimeoutExpired);
						INFO_PRINTF1(_L("				No RespondLocationRequest before Alpha2 timer expired"));
						}
					INFO_PRINTF1(_L("				Waiting for measurement response"));
				
					// Wait for and process the response:
					TESTL(iProxy->WaitForResponse(2 * delta) == ENetMsgRespondLocationRequest);	
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RespondLocationRequest(meas)"));
					sessionId = NULL;
					TPositionGpsMeasurementInfo* measurementInfo = NULL;
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);
					TESTL(sessionId->SessionNum() == iSessionId.SessionNum());	// module sends NAN measurement first
					TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
					if(reason != KErrNone)
						{
						INFO_PRINTF2(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() reason = 0x%x"), reason);
						}
					TESTL(reason == KErrNone);
					CleanupStack::PopAndDestroy(cleanupCnt);//sessionId, measurementInfo
	
					// Recv -> RequestAssistanceData - we get an extra msg as the result of the A-GPS manager re-issuing a location request when it's
					//									max fix time timer expries.
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					quality.SetMaxFixTime(t);
	
					// << ProcessLocationRequest()
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessLocationRequest()"));
					iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	
					// >> RequestAssistanceData(0)
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
	
					// >> RespondLocationRequest() - first measurement, second position.
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RespondLocationRequest(meas)"));
					TESTL(iProxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);
					sessionId = NULL;
					measurementInfo = NULL;
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &measurementInfo);			
	
					// Check it is measurement
					TESTL(measurementInfo->PositionClassType() == EPositionGpsMeasurementInfoClass);
					TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
					TESTL(reason == KErrNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
	
					// >> RequestAssistanceData() - we get an extra msg as the result of the A-GPS manager re-issuing a location request when it's
					//									max fix time timer expries.
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
					TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData);
					cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					if(iPlannedPositionOriginator == EPositionOriginatorModule && iNetworkExpectsPositions)
						{ // if hybrid need to do a few extra things
						// << ProcessLocationRequest(SessionId, HybridMode, t)
						INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessLocationRequest(SessionId, HybridMode, t)"));
						iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	
						// >> RequestAssistanceData(0)
						INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RequestAssistanceData(0)"));
						TESTL(iProxy->WaitForResponse(KTimeOut) == ENetMsgRequestAssistanceData); 
						cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
						TESTL(dataMask == EAssistanceDataNone);
						CleanupStack::PopAndDestroy(cleanupCnt);					
						}
					}
	
				if(iPlannedPositionOriginator == EPositionOriginatorModule)
					{ // the module will deliver the location
					if(iNetworkExpectsPositions)
						{
						INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() Network expecting position"));
						// >> RespondLocationRequest() - first measurement, second position.
						INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() >> RespondLocationRequest(gpspos)"));
						TESTL(iProxy->WaitForResponse(t + delta) == ENetMsgRespondLocationRequest);
						sessionId = NULL;
						TPositionInfo* positionInfo = NULL;
						cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
						
						// check it is a position
						TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
						TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
						TESTL(reason == KErrNone);
						
						// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
						// Return modules' position as FinalNetworkPosition
						INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessLocationUpdate(SessionId, FinalNetworkPosition)"));
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
					INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessLocationUpdate(SessionId, FNP)"));
					iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);
					}
				iNetworkProtocolProxyStep = ENetworkProtocolProxyStepFinal;
				}
			
			break;
			}
		case ENetworkProtocolProxyStepFinal:
			{
			// << ProcessSessionComplete()
			TInt reason = KErrNone;
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessSessionComplete()"));			
			iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

			// << ProcessStatusUpdate()
			MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
			INFO_PRINTF1(_L("CT_LbsMolrGpsOptions::DoNetworkProtocolProxyStepL() << ProcessStatusUpdate(EServiceNone)"));			
			iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);
			
			iNetworkProtocolProxyStep = ENetworkProtocolProxyStepInitial;
			break;
			}
		default:
			{
			User::Invariant();
			}
		}
	}
