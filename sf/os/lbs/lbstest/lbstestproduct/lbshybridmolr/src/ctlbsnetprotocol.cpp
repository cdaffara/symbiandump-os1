// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <lbs/lbslocerrors.h>
#include "lbsnetinternalapi.h"
#include "LbsInternalInterface.h"
#include "LbsExtendModuleInfo.h"
#include "lbsdevloggermacros.h"

// LBS test includes.
#include "ctlbsnetprotocol.h"
#include <lbs/test/tlbsutils.h>
#include "argutils.h"

#include <lbs/test/activeyield.h>

const TInt KTimeOut = 2*1000*1000; // 2 secs
const TInt KSmallTimeOut = 500*1000; // 0.5 secs
const TInt KDelta = 1000;
const TInt KT = 2 * 1000 * 1000; // 2 secs.
const TInt KAlphaTimer = ArgUtils::Alpha2()/12;

/**
Static Constructor
*/
CT_LbsNetProtocol* CT_LbsNetProtocol::NewL(MTestFlowObserver& aObserver, CTestStep& aStep)
	{
	CT_LbsNetProtocol* self = new CT_LbsNetProtocol(aObserver, aStep);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CT_LbsNetProtocol::StartNetworkL()
	{
	iProxy->WaitForResponseL(KTimeOut, iStatus);
	SetActive();
	iObserver.OnSignalNetworkStep(iRelativeSessionId, iNetworkProtocolProxyStep);
	}

void CT_LbsNetProtocol::SignalClientsFinished()
	{
	iClientsFinished = ETrue;
	}

CT_LbsNetProtocol::TPlannedPositionOriginator CT_LbsNetProtocol::PlannedPositionOriginator()
	{
	return iPlannedPositionOriginator;
	}

void CT_LbsNetProtocol::SetPlannedPositionOriginator(TInt aOriginator)
	{
	iPlannedPositionOriginator = static_cast<TPlannedPositionOriginator>(aOriginator);
	iStep.TEST(iPlannedPositionOriginator != EPositionOriginatorUnkown);
	}


void CT_LbsNetProtocol::SetNetworkMethod(TInt aMethod)
	{
	iNetworkMethod = static_cast<TChosenNetworkMethod>(aMethod);
	iStep.TEST(iNetworkMethod != ENetworkMethodNotSet);
	}


/**
 * Constructor
 */
CT_LbsNetProtocol::CT_LbsNetProtocol(MTestFlowObserver& aObserver, CTestStep& aStep) 
	: CActive(EPriorityHigh), iObserver(aObserver), iStep(aStep), iNetworkMethod(ENetworkMethodNotSet), 
		iNetworkProtocolProxyStep(ENetworkProtocolProxyStepInitial), iRelativeSessionId(0), 
		iClientsFinished(EFalse), iPlannedPositionOriginator(EPositionOriginatorUnkown)
	{
	iSessionId.SetSessionOwner(KRequestUid);
	iSessionId.SetSessionNum(0x0001);
	CActiveScheduler::Add(this);
	}


void CT_LbsNetProtocol::ConstructL()
	{
	iProxy = CNetProtocolProxy::NewL();
	}

/**
 * Destructor
 */
CT_LbsNetProtocol::~CT_LbsNetProtocol()
	{
	Cancel();
	delete iProxy;
	}

TLbsNetPosRequestMethod CT_LbsNetProtocol::RequestNetworkMethod()
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
			iStep.INFO_PRINTF1(_L("The network requests TB"));
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestTerminalBasedMethod();
			break;
			}
		case ETerminalBasedTerminalAssistedNetworkMethod:
			{
			iStep.INFO_PRINTF1(_L("The network requests hybrid"));
			iNetworkExpectsMeasurments = ETrue;
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestHybridMethod();
			break;
			}
		case ETerminalAssistedNetworkMethod:
			{
			iStep.INFO_PRINTF1(_L("The network requests TAP"));
			iNetworkExpectsMeasurments = ETrue;
			method = ArgUtils::RequestTAPMethod();
			break;
			}
		case ETerminalAssistedTerminalBasedNetworkMethod:
			{
			iStep.INFO_PRINTF1(_L("The network requests hybrid"));
			iNetworkExpectsMeasurments = ETrue;
			iNetworkExpectsPositions = ETrue;
			method = ArgUtils::RequestTerminalAssistedAndTerminalBasedMethod();
			break;
			}
		default:
			{
			User::Invariant();
			}
		}
	return method;
	}

void CT_LbsNetProtocol::DecideWhatNetworkDoesntReceive()
/**
 This method checks what the module was finally set to and decides if the network 
 should not expect measuments, or positions
 */
	{
	TLbsGpsOptionsArray options;
	TPckg<TLbsGpsOptionsArray> pckgOptions(options);
	iStep.TEST(KErrNone == RProperty::Get(KUidSystemCategory, ELbsTestAGpsModuleModeChanges,pckgOptions));

	if(options.ClassType() & ELbsGpsOptionsArrayClass)
		{ 
		if(options.NumOptionItems() > 1)
			{// don't change anything, the module is running in hybrid
			iStep.INFO_PRINTF1(_L("The module is running in hybrid"));
			return;
			}
		}
	switch(options.GpsMode())
		{
		case CLbsAdmin::EGpsAutonomous:
			{
			iNetworkExpectsPositions = EFalse;
			iNetworkExpectsMeasurments = EFalse;
			iStep.INFO_PRINTF1(_L("The module is running in autonomous"));
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalBased:
		case CLbsAdmin::EGpsAlwaysTerminalBased:
			{
			iStep.INFO_PRINTF1(_L("The module is running in terminal based"));
			iNetworkExpectsMeasurments = EFalse;
			break;
			}
		case CLbsAdmin::EGpsPreferTerminalAssisted:
		case CLbsAdmin::EGpsAlwaysTerminalAssisted:
			{
			iStep.INFO_PRINTF1(_L("The module is running in terminal assisted"));
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



void CT_LbsNetProtocol::Process_RequestSelfLocationL()
	{
	// >> RequestSelfLocation()
	iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestSelfLocation()"));
	TLbsNetSessionId* sessionId = NULL;
	TLbsNetPosRequestOptionsAssistance*	opts = NULL;
	TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestSelfLocation, &sessionId, &opts);
	++iRelativeSessionId;
	iSessionId.SetSessionNum(sessionId->SessionNum());
	iSessionId.SetSessionOwner(sessionId->SessionOwner());

	iStep.INFO_PRINTF2(_L("NetworkProtocolProxy >> RequestSelfLocation() mode = 0x%x"), opts->PosMode());
	if(opts->PosMode() == TPositionModuleInfo::ETechnologyTerminal)	// autonomous request, just complete the session
		{
		// << ProcessSessionComplete()
		TInt reason = KErrNone;
		iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessSessionComplete()"));			
		iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

		// << ProcessStatusUpdate()
		MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask = MLbsNetworkProtocolObserver::EServiceNone;	
		iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessStatusUpdate(EServiceNone)"));			
		iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask);						
		iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
		}					
	else
		{
		// << ProcessStatusUpdate(EServiceSelfLocation)
		iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessStatusUpdate(EServiceSelfLocation)"));
		MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask1 = MLbsNetworkProtocolObserver::EServiceSelfLocation;
		iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask1);

		if(opts->DataRequestMask() != EAssistanceDataNone)
			{
			// << ProcessAssistanceData()
			iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessAssistanceData"));
			TLbsAsistanceDataGroup dataMask = EAssistanceDataReferenceTime;
			RLbsAssistanceDataBuilderSet assistanceData;
			ArgUtils::PopulateLC(assistanceData);
			TInt reason = KErrNone;
			iProxy->CallL(ENetMsgProcessAssistanceData, &dataMask, &assistanceData, &reason);
			CleanupStack::PopAndDestroy(&assistanceData);
			}

		// << ProcessLocationRequest()
		iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessLocationRequest"));			
		TBool emergency = EFalse;
		MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
		TLbsNetPosRequestQuality quality = ArgUtils::Quality();
		quality.SetMaxFixTime(KAlphaTimer);
		TLbsNetPosRequestMethod method  = RequestNetworkMethod();
		iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
		
		// Now that the hybrid/alpha2 has been requested, record current time to verify alpha2 timer expires correctly.
		iAlpha2StartTime.HomeTime();
		
		iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionNrhRequestSent;
		}	
	CleanupStack::PopAndDestroy(cleanupCnt);
	}

void CT_LbsNetProtocol::RunL()
	{
	TInt response(iStatus.Int());
	switch(iNetworkProtocolProxyStep)
		{
		case ENetworkProtocolProxyStepInitial:
			{
			switch(response)
				{
				case ENetMsgCancelSelfLocation:
					{
					// >> CancelSelfLocation()
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation() for an older session"));
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgGetCurrentCapabilitiesResponse:
					{
					// >> AdviceSystemStatus()
					CLbsNetworkProtocolBase::TLbsSystemStatus status;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
					iStep.TESTL(status == CLbsNetworkProtocolBase::ESystemStatusNone || status == CLbsNetworkProtocolBase::ESystemStatusClientTracking);
					if(status == CLbsNetworkProtocolBase::ESystemStatusClientTracking)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusClientTracking)"));
						}
					else if(status == CLbsNetworkProtocolBase::ESystemStatusNone)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusNone)"));
						}
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgRequestAssistanceData:
					{
					// >> RequestAssistanceData(0)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestAssistanceData(0)"));
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					iStep.TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgTimeoutExpired:
					{ // can happen that no request has been sent through, so start waiting again
					if(iClientsFinished)
						{
						iObserver.StopTest();						
						}
					else
						{
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();
						}
					break;
					}
				case ENetMsgRequestSelfLocation:
					{
					Process_RequestSelfLocationL();
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				default:
					{
					iStep.INFO_PRINTF2(_L("Network in state ENetworkProtocolProxyStepInitial received unexpected response: %d"), response);
					User::Leave(KErrNotSupported);
					}
				}
			break;
			}

		case ENetworkProtocolProxyStepSessionNrhRequestSent:
			{
			switch(response)
				{
				case ENetMsgRequestAssistanceData:
					{
					// >> RequestAssistanceData(0)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestAssistanceData(0)"));
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					iStep.TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);

					DecideWhatNetworkDoesntReceive();
					
					if(iNetworkExpectsMeasurments)
						{ // measurements should be sent to the network
						// Determine the value to take off the alpha2 value. This is required because we had to wait for the assistance data response.
						TTimeIntervalMicroSeconds microseconds;
						TTime stopTime;
						stopTime.HomeTime();
						microseconds = stopTime.MicroSecondsFrom(iAlpha2StartTime); 
						TInt64 timeElapsed = microseconds.Int64();

						// Test that we do not get response before alpha2 has expired:
						TInt timeOut = KAlphaTimer - timeElapsed - KDelta > 0 ? KAlphaTimer - timeElapsed - KDelta : 0;
						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionWaitingForFirstMeasurmentsTimeOut;
						iProxy->WaitForResponseL(timeOut, iStatus);
						SetActive();
						}
					else
						{
						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionMeasurmentsReceived;
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();
						}

					break;
					}
				case ENetMsgCancelSelfLocation:
					{
					// >> CancelSelfLocation()
					TLbsNetSessionId* sessionId = NULL;
					TInt cancelReason = KErrNone;
					TInt cleanupCnt;
					
					cleanupCnt = iProxy->GetArgsLC(ENetMsgCancelSelfLocation, &sessionId, &cancelReason);
					if(sessionId->SessionNum() == iSessionId.SessionNum())
						{ // should complete the session
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation()"));
						CleanupStack::PopAndDestroy(cleanupCnt);
						
						// << ProcessSessionComplete()
						TInt reason = KErrNone;
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessSessionComplete()"));			
						iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

						// << ProcessStatusUpdate()
						MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessStatusUpdate(EServiceNone)"));			
						iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();						
						}
					else
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation() for an older session"));
						iStep.TESTL(sessionId->SessionNum() < iSessionId.SessionNum());
						CleanupStack::PopAndDestroy(cleanupCnt);
						sessionId = NULL;
		
						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionWaitingForFirstMeasurments;
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();
						}
					break;
					}
				case ENetMsgGetCurrentCapabilitiesResponse:
					{
					// >> AdviceSystemStatus()
					CLbsNetworkProtocolBase::TLbsSystemStatus status;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
					if(status == CLbsNetworkProtocolBase::ESystemStatusClientTracking)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusClientTracking)"));
						}
					else if(status == CLbsNetworkProtocolBase::ESystemStatusNone)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusNone)"));
						}
					else
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus() with wrong status"));
						User::Leave(KErrNotSupported);
						}
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
					SetActive();
					break;
					}					
				case ENetMsgRequestSelfLocation:
					{
					Process_RequestSelfLocationL();
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				default:
					{
					iStep.INFO_PRINTF2(_L("Network in state ENetworkProtocolProxyStepSessionNrhRequestSent received unexpected response: %d"), response);
					User::Leave(KErrNotSupported);
					}
				}
			break;
			}
		case ENetworkProtocolProxyStepSessionWaitingForFirstMeasurmentsTimeOut:
			{
			switch(response)
				{
				case ENetMsgRequestAssistanceData:
					{
					// >> RequestAssistanceData(0)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestAssistanceData(0)"));
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					iStep.TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					TTimeIntervalMicroSeconds microseconds;
					TTime stopTime;
					stopTime.HomeTime();
					microseconds = stopTime.MicroSecondsFrom(iAlpha2StartTime); 
					TInt64 timeElapsed = microseconds.Int64();

					// Test that we do not get response before alpha2 has expired:
					TInt timeOut = KAlphaTimer - timeElapsed - KDelta > 0 ? KAlphaTimer - timeElapsed - KDelta : 0;
					iProxy->WaitForResponseL(timeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgGetCurrentCapabilitiesResponse:
					{
					// >> AdviceSystemStatus()
					CLbsNetworkProtocolBase::TLbsSystemStatus status;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
					if(status == CLbsNetworkProtocolBase::ESystemStatusClientTracking)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusClientTracking)"));
						}
					else if(status == CLbsNetworkProtocolBase::ESystemStatusNone)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusNone)"));
						}
					else
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus() with wrong status"));
						User::Leave(KErrNotSupported);
						}
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					TTimeIntervalMicroSeconds microseconds;
					TTime stopTime;
					stopTime.HomeTime();
					microseconds = stopTime.MicroSecondsFrom(iAlpha2StartTime); 
					TInt64 timeElapsed = microseconds.Int64();

					// Test that we do not get response before alpha2 has expired:
					TInt timeOut = KAlphaTimer - timeElapsed - KDelta > 0 ? KAlphaTimer - timeElapsed - KDelta : 0;
					iProxy->WaitForResponseL(timeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgCancelSelfLocation:
					{
					// >> CancelSelfLocation()
					TLbsNetSessionId* sessionId = NULL;
					TInt cancelReason = KErrNone;
					TInt cleanupCnt;
					
					cleanupCnt = iProxy->GetArgsLC(ENetMsgCancelSelfLocation, &sessionId, &cancelReason);
					if(sessionId->SessionNum() == iSessionId.SessionNum())
						{ // should complete the session
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation()"));
						CleanupStack::PopAndDestroy(cleanupCnt);
						
						// << ProcessSessionComplete()
						TInt reason = KErrNone;
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessSessionComplete()"));			
						iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

						// << ProcessStatusUpdate()
						MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessStatusUpdate(EServiceNone)"));			
						iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();						
						}
					else
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation() for an older session"));
						iStep.TESTL(sessionId->SessionNum() < iSessionId.SessionNum());
						CleanupStack::PopAndDestroy(cleanupCnt);
						sessionId = NULL;
		
						TTimeIntervalMicroSeconds microseconds;
						TTime stopTime;
						stopTime.HomeTime();
						microseconds = stopTime.MicroSecondsFrom(iAlpha2StartTime); 
						TInt64 timeElapsed = microseconds.Int64();
	
						// Test that we do not get response before alpha2 has expired:
						TInt timeOut = KAlphaTimer - timeElapsed - KDelta > 0 ? KAlphaTimer - timeElapsed - KDelta : 0;
						iProxy->WaitForResponseL(timeOut, iStatus);
						SetActive();
						}
					break;
					}
				case ENetMsgTimeoutExpired:
					{
					// >> Alpha2 timeout
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy - Network expecting measurements after timeout"));
					
					iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionWaitingForFirstMeasurments;
					iProxy->WaitForResponseL(2 * 1000* KDelta, iStatus);
					SetActive();
					break;
					}
				case ENetMsgRespondLocationRequest:
					{ // If the module sends an accurate enough position then the Alpha2 timer might not time-out so check we received positions
					// >> RespondLocationRequest(gpspos)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RespondLocationRequest(gpspos)"));
					RequestNetworkMethod();
					DecideWhatNetworkDoesntReceive();
					iStep.TESTL(iNetworkExpectsPositions);
					TLbsNetSessionId* sessionId = NULL;
					TPositionInfo* positionInfo = NULL;
					TInt reason;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
					
					// check it is a position
					iStep.TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
					iStep.TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
					iStep.TESTL(reason == KErrNone);
					
					// << ProcessLocationUpdate(FNP)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessLocationUpdate(SessionId, FinalNetworkPosition)"));
					iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, positionInfo);
					CleanupStack::PopAndDestroy(cleanupCnt);

					iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionPositionSent;
					iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
					SetActive();
					break;
					}
				default:
					{
					iStep.INFO_PRINTF2(_L("Network in state ENetworkProtocolProxyStepSessionWaitingForFirstMeasurmentsTimeOut received unexpected response: %d"), response);
					User::Leave(KErrNotSupported);
					}
				}
			break;
			}
		case ENetworkProtocolProxyStepSessionWaitingForFirstMeasurments:
			{
			switch(response)
				{
				case ENetMsgRequestAssistanceData:
					{
					// >> RequestAssistanceData(0)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestAssistanceData(0)"));
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					iStep.TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgCancelSelfLocation:
					{
					// >> CancelSelfLocation()
					TLbsNetSessionId* sessionId = NULL;
					TInt cancelReason = KErrNone;
					TInt cleanupCnt;
					
					cleanupCnt = iProxy->GetArgsLC(ENetMsgCancelSelfLocation, &sessionId, &cancelReason);
					if(sessionId->SessionNum() == iSessionId.SessionNum())
						{ // should complete the session
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation()"));
						CleanupStack::PopAndDestroy(cleanupCnt);
						
						// << ProcessSessionComplete()
						TInt reason = KErrNone;
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessSessionComplete()"));			
						iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

						// << ProcessStatusUpdate()
						MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessStatusUpdate(EServiceNone)"));			
						iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();						
						}
					else
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation() for an older session"));
						iStep.TESTL(sessionId->SessionNum() < iSessionId.SessionNum());
						CleanupStack::PopAndDestroy(cleanupCnt);
						sessionId = NULL;
		
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();
						}
					break;
					}
				case ENetMsgRespondLocationRequest:
					{
					// >> RespondLocationRequest(meas)
					TLbsNetSessionId* sessionId = NULL;
					TPositionInfo* positionInfo = NULL;
					TInt reason;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
					RequestNetworkMethod();
					DecideWhatNetworkDoesntReceive();

					// check it is a position
					if(positionInfo->PositionClassType() == EPositionGpsMeasurementInfoClass)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RespondLocationRequest(meas)"));
						iStep.TESTL(iNetworkExpectsMeasurments);
						iStep.TESTL(sessionId->SessionNum() == iSessionId.SessionNum());	// module sends NAN measurement first
						if(reason != KErrNone)
							{
							iStep.INFO_PRINTF2(_L("NetworkProtocolProxy reason = 0x%x"), reason);
							}
						iStep.TESTL(reason == KErrNone);
						CleanupStack::PopAndDestroy(cleanupCnt);
						if(iPlannedPositionOriginator == EPositionOriginatorModule)
							{
							if(iNetworkExpectsPositions)
								{
								// << ProcessLocationRequest(SessionId, HybridMode, t)
								iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessLocationRequest(SessionId, HybridMode, t)"));
								TBool emergency = EFalse;
								MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
								TLbsNetPosRequestQuality quality = ArgUtils::Quality();
								quality.SetMaxFixTime(KT);
								TLbsNetPosRequestMethod method  = RequestNetworkMethod();
								iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
			
								iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionMeasurmentsReceived;
								iProxy->WaitForResponseL(KTimeOut, iStatus);
								SetActive();
								}
							else
								{
								iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionPositionSent;
								iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
								SetActive();
								}
							}
						else if(iPlannedPositionOriginator == EPositionOriginatorNetwork)
							{
							TPositionInfo positionInfo = ArgUtils::AccurateNetworkPositionInfo();
							// << ProcessLocationUpdate(SessionId, FinalNetworkPosition)
							// Return FinalNetworkPosition
							iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessLocationUpdate(SessionId, FNP)"));
							iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, &positionInfo);
							
							iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionPositionSent;
							iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
							SetActive();
							}
						}
					else
						{
						// >> RespondLocationRequest(gpspos)
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RespondLocationRequest(gpspos)"));
						// check it is a position
						iStep.TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
						iStep.TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
						iStep.INFO_PRINTF3(_L("NetworkProtocolProxy reason = %d(0x%x)"), reason,reason);
						if(reason != KErrPositionNoGpsUpdate)
							{
							iStep.TESTL(reason == KErrNone);
							}
						iStep.TESTL(iNetworkExpectsPositions);
						
						// << ProcessLocationUpdate(FNP)
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessLocationUpdate(SessionId, FinalNetworkPosition)"));
						iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, positionInfo);
						CleanupStack::PopAndDestroy(cleanupCnt);

						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionPositionSent;
						iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
						SetActive();
						}
					break;
					}
				default:
					{
					iStep.INFO_PRINTF2(_L("Network in state ENetworkProtocolProxyStepSessionWaitingForFirstMeasurments received unexpected response: %d"), response);
					User::Leave(KErrNotSupported);
					}
				}
			break;
			}
		case ENetworkProtocolProxyStepSessionMeasurmentsReceived:
			{
			switch(response)
				{
				case ENetMsgRequestAssistanceData:
					{
					// >> RequestAssistanceData(0)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestAssistanceData(0)"));
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					iStep.TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgRespondLocationRequest:
					{ // If the module sends an accurate enough position then the Alpha2 timer might not time-out so check we received positions
					// >> RespondLocationRequest(gpspos/meas)
					TLbsNetSessionId* sessionId = NULL;
					TPositionInfo* positionInfo = NULL;
					TInt reason;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRespondLocationRequest, &sessionId, &reason, &positionInfo);
					RequestNetworkMethod();
					DecideWhatNetworkDoesntReceive();
					
					// check it is a position
					if(positionInfo->PositionClassType() == EPositionGpsMeasurementInfoClass)
						{ // received more measurments
						// >> RespondLocationRequest(meas)
						iStep.TESTL(iNetworkExpectsMeasurments);
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RespondLocationRequest(meas)"));
						iStep.TESTL(iNetworkExpectsMeasurments);
						iStep.TESTL(sessionId->SessionNum() == iSessionId.SessionNum());	// module sends NAN measurement first
						if(reason != KErrNone)
							{
							iStep.INFO_PRINTF2(_L("NetworkProtocolProxy reason = 0x%x"), reason);
							}
						iStep.TESTL(reason == KErrNone);
						iStep.TESTL(iPlannedPositionOriginator == EPositionOriginatorModule);
						iStep.TESTL(iNetworkExpectsPositions);
						// << ProcessLocationRequest(SessionId, HybridMode, t)
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessLocationRequest(SessionId, HybridMode, t)"));
						TBool emergency = EFalse;
						MLbsNetworkProtocolObserver::TLbsNetProtocolService	service = MLbsNetworkProtocolObserver::EServiceSelfLocation;
						TLbsNetPosRequestQuality quality = ArgUtils::Quality();
						quality.SetMaxFixTime(KT);
						TLbsNetPosRequestMethod method  = RequestNetworkMethod();
						iProxy->CallL(ENetMsgProcessLocationRequest, &iSessionId, &emergency, &service, &quality, &method);
	
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();
						}
					else
						{ // received a position
						iStep.TESTL(iNetworkExpectsPositions);
						iStep.TESTL(positionInfo->PositionClassType() == (EPositionInfoClass|EPositionCourseInfoClass|EPositionSatelliteInfoClass|EPositionExtendedSatelliteInfoClass));
						iStep.TESTL(sessionId->SessionNum() == iSessionId.SessionNum());
						iStep.TESTL(reason == KErrNone);
						
						// << ProcessLocationUpdate(FNP)
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessLocationUpdate(SessionId, FinalNetworkPosition)"));
						iProxy->CallL(ENetMsgProcessLocationUpdate, &iSessionId, positionInfo);

						iNetworkProtocolProxyStep = ENetworkProtocolProxyStepSessionPositionSent;
						iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
						SetActive();
						}
					CleanupStack::PopAndDestroy(cleanupCnt);
					break;
					}
				default:
					{
					iStep.INFO_PRINTF2(_L("Network in state ENetworkProtocolProxyStepSessionMeasurmentsReceived received unexpected response: %d"), response);
					User::Leave(KErrNotSupported);
					}
				}
			break;
			}

		case ENetworkProtocolProxyStepSessionPositionSent:
			{
			switch(response)
				{
				case ENetMsgRequestAssistanceData:
					{
					// >> RequestAssistanceData(0)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestAssistanceData(0)"));
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					iStep.TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgGetCurrentCapabilitiesResponse:
					{
					// >> AdviceSystemStatus()
					CLbsNetworkProtocolBase::TLbsSystemStatus status;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
					if(status == CLbsNetworkProtocolBase::ESystemStatusClientTracking)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusClientTracking)"));
						}
					else if(status == CLbsNetworkProtocolBase::ESystemStatusNone)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusNone)"));
						}
					else
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus() with wrong status"));
						User::Leave(KErrNotSupported);
						}
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iProxy->WaitForResponseL(KSmallTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgCancelSelfLocation:
					{
					// >> CancelSelfLocation()
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation() - as result of FNP"));

					// << ProcessSessionComplete()
					TInt reason = KErrNone;
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessSessionComplete()"));			
					iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

					// << ProcessStatusUpdate()
					MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessStatusUpdate(EServiceNone)"));			
					iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

					iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgTimeoutExpired:
					{
					// << ProcessSessionComplete()
					TInt reason = KErrNone;
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessSessionComplete()"));			
					iProxy->CallL(ENetMsgProcessSessionComplete, &iSessionId, &reason);

					// << ProcessStatusUpdate()
					MLbsNetworkProtocolObserver::TLbsNetProtocolServiceMask activeServiceMask2 = MLbsNetworkProtocolObserver::EServiceNone;	
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy << ProcessStatusUpdate(EServiceNone)"));			
					iProxy->CallL(ENetMsgProcessStatusUpdate, &activeServiceMask2);

					iNetworkProtocolProxyStep = ENetworkProtocolProxyStepNoSessionRunning;
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgRequestSelfLocation:
					{
					Process_RequestSelfLocationL();
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				default:
					{
					iStep.INFO_PRINTF2(_L("Network in state ENetworkProtocolProxyStepSessionPositionSent received unexpected response: %d"), response);
					User::Leave(KErrNotSupported);
					}
				}
			break;
			}

		case ENetworkProtocolProxyStepNoSessionRunning:
			{
			switch(response)
				{
				case ENetMsgCancelSelfLocation:
					{
					// >> CancelSelfLocation()
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> CancelSelfLocation() for an older session"));
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgTimeoutExpired:
					{ // can happen that no request has been sent through, so start waiting again
					if(iClientsFinished)
						{
						iObserver.StopTest();						
						}
					else
						{
						iProxy->WaitForResponseL(KTimeOut, iStatus);
						SetActive();
						}
					break;
					}
				case ENetMsgRequestAssistanceData:
					{
					// >> RequestAssistanceData(0)
					iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> RequestAssistanceData(0)"));
					TLbsAsistanceDataGroup dataMask;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgRequestAssistanceData, &dataMask);
					iStep.TESTL(dataMask == EAssistanceDataNone);
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgGetCurrentCapabilitiesResponse:
					{
					// >> AdviceSystemStatus()
					CLbsNetworkProtocolBase::TLbsSystemStatus status;
					TInt cleanupCnt = iProxy->GetArgsLC(ENetMsgGetCurrentCapabilitiesResponse, &status);
					if(status == CLbsNetworkProtocolBase::ESystemStatusClientTracking)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusClientTracking)"));
						}
					else if(status == CLbsNetworkProtocolBase::ESystemStatusNone)
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus(ESystemStatusNone)"));
						}
					else
						{
						iStep.INFO_PRINTF1(_L("NetworkProtocolProxy >> AdviceSystemStatus() with wrong status"));
						User::Leave(KErrNotSupported);
						}
					CleanupStack::PopAndDestroy(cleanupCnt);
					
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				case ENetMsgRequestSelfLocation:
					{
					Process_RequestSelfLocationL();
					iProxy->WaitForResponseL(KTimeOut, iStatus);
					SetActive();
					break;
					}
				default:
					{
					iStep.INFO_PRINTF2(_L("Network in state ENetworkProtocolProxyStepNoSessionRunning received unexpected response: %d"), response);
					User::Leave(KErrNotSupported);
					}
				}
			break;
			}

		default:
			{
			iStep.INFO_PRINTF1(_L("Network in unexpected state"));
			User::Leave(KErrCorrupt);
			}
		}
	iObserver.OnSignalNetworkStep(iRelativeSessionId, iNetworkProtocolProxyStep);
	}

TInt CT_LbsNetProtocol::RunError(TInt aError)
	{
	iStep.SetTestStepError(aError);
	iStep.SetTestStepResult(EFail);
	iObserver.StopTest();
	return KErrNone;
	}

void CT_LbsNetProtocol::DoCancel()
	{
	iProxy->CancelWait();
	}
