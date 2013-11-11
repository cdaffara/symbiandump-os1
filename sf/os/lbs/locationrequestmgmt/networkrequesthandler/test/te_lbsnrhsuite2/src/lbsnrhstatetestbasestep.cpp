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
//

/**
 @file lbsnrhstatetestbasestep.cpp
*/

#include "lbscommoninternaldatatypes.h"
#include "lbsnrhstatetestbasestep.h"
#include "lbsnrhngmsgs.h"


const TLbsNetSessionIdInt KDummySessionId;
const TUid KUnitTestProtocolModuleUid = { 0x1028226B };


CLbsNrhStateTestBaseStep::CLbsNrhStateTestBaseStep()
	{
	SetTestStepName(KLbsNrhStateTestBaseStep);
	}

CLbsNrhStateTestBaseStep::~CLbsNrhStateTestBaseStep()
	{
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhStateTestBaseStep::doTestStepPreambleL()
	{
	// Common initialisation  
	CTe_LbsNrhSuiteStepBase::doTestStepPreambleL();
	
	iAgpsChannel = CAgpsChannel::NewL(Logger());
	iNgChannel = CNgChannel::NewL(Logger());
	iPrivCtrlChannel = CPrivCtrlChannel::NewL(Logger());
	iX3pChannel = CX3pChannel::NewL(Logger());
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CLbsNrhStateTestBaseStep::doTestStepL()
	{
	if (TestStepResult() == EPass)
		{

		// Simple privacy request
		NetGateway_SendPrivacyRequestL(/*aSessionId=*/1);    
		PrivacyController_WaitPrivacyRequestL(/*aSessionId=*/1);
		PrivacyController_SendPrivacyResponseL(/*aSessionId=*/1, CLbsPrivacyController::ERequestAccepted);
		NetGateway_WaitPrivacyResponseL(1, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
		AgpsManager_WaitLocationRequestStatusL();
		NetGateway_SendSessionCompleteL(1, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
		PrivacyController_WaitPrivacyCompleteL(/*aSessionId=*/1, KErrNone);
		AgpsManager_WaitLocationRequestCancelL();		

		// Simple MO-LR
		NetGateway_SendNetworkReferencePositionL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
		NetGateway_SendLocationRequestL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
		AgpsManager_WaitLocationRequestL();
		AgpsManager_SendGpsPositionUpdateAccurateL();
		NetGateway_WaitLocationResponseL(/*aSessionId=*/2, KErrNone);
		NetGateway_SendSessionCompleteL(2, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
		AgpsManager_WaitLocationRequestCancelL();
		
		// Inaccurate MO-LR
		NetGateway_SendNetworkReferencePositionL(3, TLbsNetworkEnumInt::EServiceSelfLocation);
		NetGateway_SendLocationRequestL(3, TLbsNetworkEnumInt::EServiceSelfLocation);
		AgpsManager_WaitLocationRequestL();
		AgpsManager_SendGpsPositionUpdateInaccurateL();
		NetGateway_WaitLocationResponseL(/*aSessionId=*/3, KPositionQualityLoss);
		NetGateway_SendSessionCompleteL(3, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
		AgpsManager_WaitLocationRequestCancelL();

		// Standard MT-LR
		NetGateway_SendPrivacyRequestL(/*aSessionId=*/4);
		PrivacyController_WaitPrivacyRequestL(/*aSessionId=*/4);
		PrivacyController_SendPrivacyResponseL(/*aSessionId=*/4, CLbsPrivacyController::ERequestAccepted);
		NetGateway_WaitPrivacyResponseL(4, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
		AgpsManager_WaitLocationRequestStatusL();
		NetGateway_SendNetworkReferencePositionL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);
		PrivacyController_WaitPrivacyReferencePositionL(/*aSessionId=*/4);
		NetGateway_SendLocationRequestL(4, TLbsNetworkEnumInt::EServiceMobileTerminated);
		AgpsManager_WaitLocationRequestL();
		AgpsManager_SendGpsPositionUpdateAccurateL();
		NetGateway_WaitLocationResponseL(4, KErrNone);
		PrivacyController_WaitPrivacyGpsPositionL(/*aSessionId=*/4);		
		NetGateway_SendSessionCompleteL(4, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
		PrivacyController_WaitPrivacyCompleteL(/*aSessionId=*/4, KErrNone);
		AgpsManager_WaitLocationRequestCancelL();

		// Basic X3P
		X3pClient_ConnectClientL(/*aClientId*/1);
		AgpsManager_WaitLocationRequestStatusL();
		X3pClient_SendX3pRequestL(/*aClientId*/1);
		TUint x3pSessionId;
		NetGateway_WaitX3pRequestL(x3pSessionId);
		AgpsManager_WaitLocationRequestStatusL();
		NetGateway_SendNetworkReferencePositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
		X3pClient_WaitX3pReferencePositionL(/*aClientId*/1);
		NetGateway_SendLocationRequestL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
		AgpsManager_WaitLocationRequestL();
		AgpsManager_SendGpsPositionUpdateAccurateL();
		NetGateway_WaitLocationResponseL(x3pSessionId, KErrNone);
		NetGateway_SendNetworkFinalPositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
		NetGateway_SendSessionCompleteL(x3pSessionId, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
		X3pClient_WaitX3pCompleteL(/*aClientId*/1, KErrNone);
		AgpsManager_WaitLocationRequestCancelL();
		X3pClient_DisconnectClientL(/*aClientId*/1);
		AgpsManager_WaitLocationRequestStatusL();
		}
		
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhStateTestBaseStep::doTestStepPostambleL()
	{
	delete iX3pChannel;
	delete iPrivCtrlChannel;
	delete iNgChannel;
	delete iAgpsChannel;
	
	CTe_LbsNrhSuiteStepBase::doTestStepPostambleL();

	return TestStepResult();	
	}

TInt CLbsNrhStateTestBaseStep::GetTestCaseId()
	{
	TInt testCaseId(0);
	TInt err = GetIntFromConfig(ConfigSection(), KTestCaseIdKeyName, testCaseId);
	return testCaseId;
	}

//
// X3p client messages
//

void CLbsNrhStateTestBaseStep::X3pClient_ConnectClientL(TUint aClientId)
	{
	iX3pChannel->ConnectClientL(aClientId);
	}

void CLbsNrhStateTestBaseStep::X3pClient_DisconnectClientL(TUint aClientId)
	{
	iX3pChannel->DisconnectClient(aClientId);
	}

void CLbsNrhStateTestBaseStep::X3pClient_SendX3pRequestL(TUint aClientId,TBool aLowPriority)
	{
	_LIT(KDestinationId, "X3pDestinationId");
	
 
	// TODO iX3pTransmitPositionRequestID
    // TODO iX3pTransmitPositionTimeout 
	
	TBool getReferencePosUpdate = ETrue;
	if ( KErrNotFound != iX3pTransmitPositionAgpsAndRefPos)
		{
		// use value defined in .ini file
		getReferencePosUpdate = iX3pTransmitPositionAgpsAndRefPos;
		}
	TUint transmitPriority = 0;
	if ( KErrNotFound != iX3pTransmitPositionAgpsAndRefPos)
		{
		// use value defined in .ini file
		transmitPriority = iX3pTransmitPositionTransmitPriority;
		}
	if (aLowPriority)
		{
		// send this request with a lower prioity than usual
		// note, zero is the higest priority
		transmitPriority++;
		}
	
	iX3pChannel->SendTransmitLocateRequestL(
		aClientId,
		KDestinationId,
		transmitPriority,
		getReferencePosUpdate);
	}
	
void CLbsNrhStateTestBaseStep::X3pClient_SendX3pCancelL(TUint aClientId)
	{
	iX3pChannel->SendCancelTransmitLocateRequestL(aClientId);
	}
	
void CLbsNrhStateTestBaseStep::X3pClient_WaitX3pCompleteL(TUint aClientId, TInt aReason)
	{
	iX3pChannel->WaitForResponseL(KDefaultTimeout);
	if (iX3pChannel->GetNextResponseType() != CX3pChannel::EX3pResponseComplete)
		{
		ERR_PRINTF2(_L("Expected EX3pResponseComplete, got %S"), 
					&iX3pChannel->X3pResponseTypeToDesC(iX3pChannel->GetNextResponseType()));
		TESTL(EFalse);
		}
	
	TUint clientId;
	TInt reason;
	TPositionExtendedSatelliteInfo posInfo;
	iX3pChannel->GetNextResponseAsRequestCompleteL(clientId, reason, posInfo);
	
	TESTL(aClientId == clientId);
	TESTL(aReason == reason);
	}
	
void CLbsNrhStateTestBaseStep::X3pClient_WaitX3pReferencePositionL(TUint aClientId)
	{
	iX3pChannel->WaitForResponseL(KDefaultTimeout);
	if (iX3pChannel->GetNextResponseType() != CX3pChannel::EX3pResponseRefPosUpdate)
		{
		ERR_PRINTF2(_L("Expected EX3pResponseRefPosUpdate, got %S"), 
					&iX3pChannel->X3pResponseTypeToDesC(iX3pChannel->GetNextResponseType()));
		TESTL(EFalse);
		}
	
	TUint clientId;
	TPositionInfo posInfo;
	iX3pChannel->GetNextResponseAsReferencePositionUpdateL(clientId, posInfo);
	
	TESTL(aClientId == clientId);
	}
	
//
// Network gateway messages
//

void CLbsNrhStateTestBaseStep::NetGateway_SendPrivacyRequestL(
        TUint32 aSessionId,
        TLbsNetPosRequestPrivacyInt::TLbsRequestAdviceInt aAdvice,
        TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt aAction,
        TBool aEmergency)
	{
	INFO_PRINTF2(_L("NRH <- NG: NetGateway_SendPrivacyRequestL(SessionId=%d)"),aSessionId);

	TLbsNetSessionIdInt sessionId(KUnitTestProtocolModuleUid, aSessionId);
	TLbsNetMtLrRequestMsg* privReqMsg = CreatePrivacyMsgLC(sessionId, aAdvice, aAction, aEmergency);
	iNgChannel->SendMessageAndWait(*privReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(privReqMsg);	
	}

	
void CLbsNrhStateTestBaseStep::NetGateway_SendEmergencyPrivacyRequestL(TUint32 aSessionId)
	{
	INFO_PRINTF2(_L("NRH <- NG: NetGateway_SendEmergencyPrivacyRequestL(SessionId=%d)"),aSessionId);

	TLbsNetSessionIdInt sessionId(KUnitTestProtocolModuleUid, aSessionId);
	TLbsNetMtLrRequestMsg* privReqMsg = CreatePrivacyVerificationMsgLC(sessionId, ETrue);
	iNgChannel->SendMessageAndWait(*privReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(privReqMsg);	
	}

void CLbsNrhStateTestBaseStep::NetGateway_SendEmergencyPrivacyRequestAdviceNotifyL(TUint32 aSessionId)
	{
	INFO_PRINTF2(_L("NRH <- NG: NetGateway_SendEmergencyPrivacyRequestL(SessionId=%d)"),aSessionId);

	TLbsNetSessionIdInt sessionId(KUnitTestProtocolModuleUid, aSessionId);
	TLbsNetMtLrRequestMsg* privReqMsg = CreatePrivacyVerificationMsgAdviceNotifyLC(sessionId, ETrue);
	iNgChannel->SendMessageAndWait(*privReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(privReqMsg);	
	}

void CLbsNrhStateTestBaseStep::NetGateway_SendLocationRequestL(
		TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService, 
		TInt aMode0, TInt aMode1, TInt aHorAccuracy, TInt aMaxFixTime)
	{
	INFO_PRINTF3(_L("NRH <- NG: NetGateway_SendLocationRequestL(SessionId=%d, Service=%02x)"),aSessionId, aService);

	TLbsNetSessionIdInt sessionId;
	if (aService == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
		{
		sessionId = TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, aSessionId);
		}
	else
		{
		sessionId = TLbsNetSessionIdInt(KUnitTestProtocolModuleUid, aSessionId);
		}

	TLbsNetPosRequestMethodInt posRequestMethod;
	
	if (aMode0 == KErrNotFound)
		{
		// location request agps mode NOT specified in .ini file so default to TB mode
		TLbsNetPosMethodInt method;
		method.SetPosMethod(KLbsPositioningMeansGps, 
						(TPositionModuleInfo::ETechnologyTerminal 
						 | TPositionModuleInfo::ETechnologyAssisted));
		posRequestMethod.SetPosMethods(&method, 1);
		}
	else
		{
		// use agps mode specified in .ini file
		TLbsNetPosMethodInt posMethodArray[2];
		posMethodArray[0].SetPosMethod(KLbsPositioningMeansGps, aMode0);
		if (aMode1 != KErrNotFound)
			{
			posMethodArray[1].SetPosMethod(KLbsPositioningMeansGps, aMode1);
			posRequestMethod.SetPosMethods(posMethodArray, 2);	
			}
		else
			{
			posRequestMethod.SetPosMethods(posMethodArray, 1);	
			}
		}
	
	TLbsNetPosRequestQualityInt quality;
	
	TTimeIntervalMicroSeconds maxFixTime(5000000);
	quality.SetMaxFixTime(maxFixTime);
	quality.SetMinHorizontalAccuracy(TReal32(40.0));
	quality.SetMinVerticalAccuracy(TReal32(50.0));
	
	if (aHorAccuracy != KErrNotFound)
		{
		quality.SetMinHorizontalAccuracy(TReal32(aHorAccuracy));
		}

	if (aMaxFixTime != KErrNotFound)
		{
		TTimeIntervalMicroSeconds maxFixTime(aMaxFixTime * 1 * 1000 * 1000);
		quality.SetMaxFixTime(maxFixTime);
		}

	TLbsNetLocationRequestMsg* locReqMsg = CreateLocationRequestMsgLC(sessionId, aService, posRequestMethod, quality);
	
	iNgChannel->SendMessageAndWait(*locReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(locReqMsg);
	}

void CLbsNrhStateTestBaseStep::NetGateway_SendEmergencyLocationRequestL(
		TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,TInt aMode0, TInt aMode1)
	{
	INFO_PRINTF3(_L("NRH <- NG: NetGateway_SendEmergencyLocationRequestL(SessionId=%d, Service=%02x)"),aSessionId, aService);

	TLbsNetSessionIdInt sessionId;
	if (aService == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
		{
		sessionId = TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, aSessionId);
		}
	else
		{
		sessionId = TLbsNetSessionIdInt(KUnitTestProtocolModuleUid, aSessionId);
		}
	
	TLbsNetPosRequestMethodInt posRequestMethod;
	
	if (aMode0 == KErrNotFound)
		{
		// location request agps mode NOT specified in .ini file so default to TB mode
		TLbsNetPosMethodInt method;
		method.SetPosMethod(KLbsPositioningMeansGps, 
						(TPositionModuleInfo::ETechnologyTerminal 
						 | TPositionModuleInfo::ETechnologyAssisted));
		posRequestMethod.SetPosMethods(&method, 1);
		}
	else
		{
		// use agps mode specified in .ini file
		TLbsNetPosMethodInt posMethodArray[2];
		posMethodArray[0].SetPosMethod(KLbsPositioningMeansGps, aMode0);
		if (aMode1 != KErrNotFound)
			{
			posMethodArray[1].SetPosMethod(KLbsPositioningMeansGps, aMode1);
			posRequestMethod.SetPosMethods(posMethodArray, 2);	
			}
		else
			{
			posRequestMethod.SetPosMethods(posMethodArray, 1);	
			}
			
		}
	
	TLbsNetLocationRequestMsg* locReqMsg = CreateEmergencyLocationRequestMsgLC(sessionId, aService, posRequestMethod);
	
	iNgChannel->SendMessageAndWait(*locReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(locReqMsg);
	

	}


void CLbsNrhStateTestBaseStep::NetGateway_SendLocationRequestL(TUint32 aSessionId,
	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService,
	TTimeIntervalMicroSeconds aMaxFixAge)
	{
	INFO_PRINTF4(_L("NRH <- NG: NetGateway_SendLocationRequestL(SessionId=%d, Service=%02x, MaxFixAge=%d)"),aSessionId, aService,aMaxFixAge);

	TLbsNetSessionIdInt sessionId;
	if (aService == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
		{
		sessionId = TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, aSessionId);
		}
	else
		{
		sessionId = TLbsNetSessionIdInt(KUnitTestProtocolModuleUid, aSessionId);
		}
	
	TLbsNetPosRequestQualityInt quality;
	TTimeIntervalMicroSeconds maxFixTime(5000000);
	quality.SetMaxFixTime(maxFixTime);
	quality.SetMinHorizontalAccuracy(TReal32(40.0));
	quality.SetMinVerticalAccuracy(TReal32(50.0));
	quality.SetMaxFixAge(aMaxFixAge);
	
	TLbsNetLocationRequestMsg* locReqMsg = CreateLocationRequestMsgLC(sessionId, aService, quality);
	iNgChannel->SendMessageAndWait(*locReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(locReqMsg);	
	}

void CLbsNrhStateTestBaseStep::NetGateway_SendInitialSuplLocationRequestL(
		TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService)
	{
	TLbsNetSessionIdInt sessionId;
	if (aService == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
		{
		sessionId = TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, aSessionId);
		}
	else
		{
		sessionId = TLbsNetSessionIdInt(KUnitTestProtocolModuleUid, aSessionId);
		}
	TLbsNetLocationRequestMsg* locReqMsg = CreateLocationRequestMsgLC(sessionId, aService);
	iNgChannel->SendMessageAndWait(*locReqMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(locReqMsg);
	}
	
void CLbsNrhStateTestBaseStep::NetGateway_SendNetworkReferencePositionL(
		TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService)
	{
	INFO_PRINTF3(_L("NetGateway_SendNetworkReferencePositionL(SessionId=%d, Service=%02x)"), aSessionId, aService);
	
	TLbsNetSessionIdInt sessionId;
	if (aService == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
		{
		sessionId = TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, aSessionId);
		}
	else
		{
		sessionId = TLbsNetSessionIdInt(KUnitTestProtocolModuleUid, aSessionId);
		}
	TLbsNetLocationUpdateMsg* refPosMsg = CreateNetworkReferencePositionMsgLC(sessionId);
 	iNgChannel->SendMessageAndWait(*refPosMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(refPosMsg);
	}
	
void CLbsNrhStateTestBaseStep::NetGateway_SendNetworkFinalPositionL(
		TUint32 aSessionId,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService)
	{
	INFO_PRINTF3(_L("NetGateway_SendNetworkFinalPositionL(SessionId=%d, Service=%02x)"), aSessionId, aService);

	TLbsNetSessionIdInt sessionId;
	if (aService == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
		{
		sessionId = TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, aSessionId);
		}
	else
		{
		sessionId = TLbsNetSessionIdInt(KUnitTestProtocolModuleUid, aSessionId);
		}
	TLbsNetLocationUpdateMsg* finalPosMsg = CreateNetworkFinalPositionMsgLC(sessionId);
	iNgChannel->SendMessageAndWait(*finalPosMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(finalPosMsg);
	}
	
void CLbsNrhStateTestBaseStep::NetGateway_SendSessionCompleteL(
		TUint32 aSessionId, 
		TInt aReason,
		TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aService)
	{
	INFO_PRINTF4(_L("NetGateway_SendSessionCompleteL(SessionId=%d, Reason+%d, Service=%02x)"), aSessionId,aReason,aService);

	TLbsNetSessionIdInt sessionId;
	if (aService == TLbsNetworkEnumInt::EServiceTransmitThirdParty)
		{
		sessionId = TLbsNetSessionIdInt(KLbsNetRequestHandlerUid, aSessionId);
		}
	else
		{
		sessionId = TLbsNetSessionIdInt(KUnitTestProtocolModuleUid, aSessionId);
		}
	TLbsNetSessionCompleteMsg* sessCompMsg = CreateSessionCompeleteMsgLC(sessionId, aReason);
	iNgChannel->SendMessageAndWait(*sessCompMsg, KDefaultTimeout);
	CleanupStack::PopAndDestroy(sessCompMsg);
	}

void CLbsNrhStateTestBaseStep::NetGateway_WaitX3pRequestL(TUint& aSessionId)
	{
	INFO_PRINTF2(_L("NetGateway_WaitX3pRequestL(SessionId=%d)"), aSessionId);

	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::ETransmitLocationRequest == iNgChannel->GetNextResponseType());
	TLbsNetTransmitLocationRequestMsg x3pReq(KDummySessionId, KNullDesC, 0);
	iNgChannel->GetNextResponseL(x3pReq);
	// For X3p, the sessionId is set by the NRH, so record the value here.
	aSessionId = x3pReq.SessionId().SessionNum();
	}
	
void CLbsNrhStateTestBaseStep::NetGateway_WaitX3pCancelL(TUint aSessionId)
	{
	INFO_PRINTF2(_L("NetGateway_WaitX3pCancelL(SessionId=%d)"), aSessionId);

	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::ETransmitLocationCancel == iNgChannel->GetNextResponseType());
	TLbsNetTransmitLocationCancelMsg x3pCancel(KDummySessionId, KErrNone);
	iNgChannel->GetNextResponseL(x3pCancel);
	
	TESTL(aSessionId == x3pCancel.SessionId().SessionNum());
	}
	
void CLbsNrhStateTestBaseStep::NetGateway_WaitPrivacyResponseL(
		TUint32 aSessionId, 
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt aResponse)
	{
	INFO_PRINTF3(_L("NetGateway_WaitPrivacyResponseL(SessionId=%d, Expected Response=%d)"), aSessionId,aResponse);

	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::EPrivacyResponse == iNgChannel->GetNextResponseType());
	TLbsNetMtLrReponseMsg privacyResponse(KDummySessionId, aResponse, KErrNone);
	iNgChannel->GetNextResponseL(privacyResponse);
	TESTL(aSessionId == privacyResponse.SessionId().SessionNum());
	TESTL(aResponse == privacyResponse.Response());	
	}
	
void CLbsNrhStateTestBaseStep::NetGateway_WaitLocationResponseL(TUint32 aSessionId, TInt aReason)
	{
	INFO_PRINTF3(_L("NetGateway_WaitLocationResponseL(SessionId=%d, Expected Reason=%d)"), aSessionId,aReason);

	if (KErrNotFound == aReason)
	    {
	    // default to expecting no error
	    aReason = KErrNone;
	    }
	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::ELocationResponse == iNgChannel->GetNextResponseType());
	
	TPositionExtendedSatelliteInfo posInfo;
	TTime time;
	TLbsNetLocationResponseMsg locResponse(KDummySessionId, KErrNone, posInfo, time);
	iNgChannel->GetNextResponseL(locResponse);
	TESTL(aSessionId == locResponse.SessionId().SessionNum());
	TESTL(aReason == locResponse.Reason());

	if (iPosFromNrhAcc != KErrNotFound)
	        {
	        TPosition position;
	        const TPositionInfoBase & posInfoBase = locResponse.PositionInfo();
	     
	        static_cast<const TPositionInfo&>(posInfoBase).GetPosition(position);
	     
	 
	        // use values from .ini file to check that pos is correct
 
	 
	        TPositionModuleInfo::TTechnologyType techType = posInfoBase.PositionMode();
	                
	        TESTL (iPosFromNrhType == techType);

            TESTL(iPosFromNrhAcc == position.HorizontalAccuracy());
            TESTL(iPosFromNrhLat == position.Latitude() );
            TESTL(iPosFromNrhLng == position.Longitude());
            // also check that position has the type of a REF position
	        }
	
	}


void CLbsNrhStateTestBaseStep::NetGateway_WaitMeasurementResponseL(TUint32 aSessionId, TInt aReason)
	{
	INFO_PRINTF3(_L("NetGateway_WaitMeasurementResponseL(SessionId=%d, Expected Reason=%d)"), aSessionId,aReason);

	
	iNgChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(TLbsNetInternalMsgBase::ELocationResponse == iNgChannel->GetNextResponseType());
	
	TPositionGpsMeasurementInfo measurementInfo;
	TTime time;
	TLbsNetLocationResponseMsg locResponse(KDummySessionId, KErrNone, measurementInfo, time);
	iNgChannel->GetNextResponseL(locResponse);
	TESTL(aSessionId == locResponse.SessionId().SessionNum());
	
	if (aReason != KErrNotFound)
		{
		if (aReason != locResponse.Reason())
			{
			TESTL(aReason == locResponse.Reason());
			}
		}
	
	}
//
// Agps manager messages
//

void CLbsNrhStateTestBaseStep::AgpsManager_SendGpsPositionUpdateAccurateL(TInt aError)
	{
	INFO_PRINTF1(_L("AgpsManager_SendGpsPositionUpdateAccurateL()"));

	TPosition pos;
	

    // use values from .ini file to set the agps pos
    pos.SetAccuracy(iAgpsPosToNrhHacc, TReal32(50.0));
    pos.SetCurrentTime();
    pos.SetCoordinate(iAgpsPosToNrhLat, iAgpsPosToNrhLng, TReal32(50.0));

	TPositionExtendedSatelliteInfo satInfo;
	satInfo.SetPosition(pos);
	satInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal 
							| TPositionModuleInfo::ETechnologyAssisted);
	satInfo.SetUpdateType(EPositionUpdateGeneral);
	satInfo.SetPositionModeReason(EPositionModeReasonNone);
	
	TTime targetTime(0);
	
	iAgpsChannel->SendGpsLocationUpdate(aError, satInfo, targetTime);
	}
	
void CLbsNrhStateTestBaseStep::AgpsManager_SendGpsPositionUpdateInaccurateL()
	{
	INFO_PRINTF1(_L("AgpsManager_SendGpsPositionUpdateInaccurateL()"));

	TPosition pos;
	pos.SetAccuracy(TReal32(500.0), TReal32(750.0));
	pos.SetCurrentTime();
	pos.SetCoordinate(TReal64(52.2455), TReal64(0.1665), TReal32(50.0));
	
	TPositionExtendedSatelliteInfo satInfo;
	satInfo.SetPosition(pos);
	satInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal 
							| TPositionModuleInfo::ETechnologyAssisted);
	satInfo.SetUpdateType(EPositionUpdateGeneral);
	satInfo.SetPositionModeReason(EPositionModeReasonNone);
	
	TTime targetTime(0);
	
	iAgpsChannel->SendGpsLocationUpdate(KErrNone, satInfo, targetTime);	
	}
	
void CLbsNrhStateTestBaseStep::AgpsManager_SendGpsMeasurementUpdateL()
	{
	INFO_PRINTF1(_L("AgpsManager_SendGpsMeasurementUpdate()"));

	// Generate some GPS measurement data
	TPositionGpsMeasurementData gpsData;
	gpsData.SetSatelliteId(3);
	gpsData.SetCarrierNoiseRatio(0x14);
	gpsData.SetDoppler(0x09);
	gpsData.SetWholeGpsChips(0x124);
	gpsData.SetFractionalGpsChips(0x03);
	gpsData.SetMultiPathIndicator(TPositionGpsMeasurementData::EMultiPathHigh);
	gpsData.SetPseudoRangeRmsError(1);
	
	TPositionGpsMeasurementInfo gpsInfo;
	gpsInfo.SetGpsTimeOfWeek(3);
	gpsInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal);
	gpsInfo.SetPositionModeReason(KErrNone);
	gpsInfo.SetUpdateType(EPositionUpdateGeneral);
	gpsInfo.SetModuleId(TUid::Uid(0x10281D6A));
	gpsInfo.AppendMeasurementData(gpsData);
	TESTL(gpsInfo.NumMeasurements() == 1);
	

	iAgpsChannel->SendGpsMeasurementUpdate(KErrNone, gpsInfo);

	}

void CLbsNrhStateTestBaseStep::AgpsManager_WaitLocationRequestOrRequestAndStatusL(TInt /*aMode0*/, TInt /*aMode1*/, TInt /*aHorAccuracy*/, TInt /*aMaxFixTime*/, TInt /*aExpectedPowerAdvice*/)
	{
	INFO_PRINTF1(_L("AgpsManager_WaitLocationRequestOrStatusL()"));
	iAgpsChannel->WaitForRequestL(KDefaultTimeout);
	TLbsPositionUpdateRequestBase::TType type;
	type = iAgpsChannel->GetNextRequestType();
	if (TLbsPositionUpdateRequestBase::EStatus == type)
		{
		// so we have got a status, next should be a location request
		TLbsPositionUpdateRequestBase::TPowerAdvice powerAdvice;
		TLbsPositionUpdateRequestStatus statusReq;
		iAgpsChannel->GetNextRequestL(statusReq);
		powerAdvice = statusReq.PowerAdvice();
	
		TESTL(powerAdvice == TLbsPositionUpdateRequestBase::EPowerAdviceOn);
	
		iAgpsChannel->WaitForRequestL(KDefaultTimeout);
		TLbsPositionUpdateRequestBase::TType type2;
		type2 = iAgpsChannel->GetNextRequestType();
		if(TLbsPositionUpdateRequestBase::EUpdateRequest == type2)
			{
			INFO_PRINTF2(_L("AgpsManager_WaitLocationRequestOrStatusL() had status and now expecting EUpdateRequest and got %d as expected"),type2);
			TLbsPositionUpdateRequest locReq;
			iAgpsChannel->GetNextRequestL(locReq);
			}
		else
			{
			INFO_PRINTF2(_L("AgpsManager_WaitLocationRequestOrStatusL() had status and now expecting EUpdateRequest but got %d instead"),type2);
			TESTL(0);
			}
		}
	
	else if(TLbsPositionUpdateRequestBase::EUpdateRequest == type)
		{
		// update request
		INFO_PRINTF1(_L("AgpsManager_WaitLocationRequestOrStatusL() got EUpdateRequest"));
		TLbsPositionUpdateRequest locReq2;
		iAgpsChannel->GetNextRequestL(locReq2);
		TLbsPositionUpdateRequestBase::TPowerAdvice powerAdvice2;
		powerAdvice2 = locReq2.PowerAdvice();
		TESTL(powerAdvice2 == TLbsPositionUpdateRequestBase::EPowerAdviceOn);
		}
	else
		{
		INFO_PRINTF1(_L("AgpsManager_WaitLocationRequestOrStatusL() expecting EStatus or EUpdateRequest but got neither"));
		TESTL(0);
		}
	}

// aMode0 and aMode1 is  the expected agps position mode in the location request
// aHorAccuracy is the expected  horizontal accuracy in the location request
// aMaxFixTime is the expected  max fix time in the location request
void CLbsNrhStateTestBaseStep::AgpsManager_WaitLocationRequestL(TInt aMode0, TInt aMode1, TInt aHorAccuracy, TInt aMaxFixTime, TInt aExpectedPowerAdvice)
	{
	INFO_PRINTF1(_L("AgpsManager_WaitLocationRequestL()"));

	iAgpsChannel->WaitForRequestL(KDefaultTimeout);
	TESTL(TLbsPositionUpdateRequestBase::EUpdateRequest == iAgpsChannel->GetNextRequestType());
	TLbsPositionUpdateRequest locReq;
	iAgpsChannel->GetNextRequestL(locReq);
	
	TLbsNetPosRequestMethodInt method ;
	method = locReq.RequestMethod();
	TInt numberMethods = method.NumPosMethods();
	
	TLbsNetPosMethodInt meth;
			
	if (aMode0 != KErrNotFound)
		{
		method.GetPosMethod(0, meth);
		TPositionModuleInfo::TTechnologyType type = meth.PosMode();
		TLbsNetPosMethodInt expectedMode;
	
		TPositionModuleInfo::TTechnologyType expectedType = static_cast<TPositionModuleInfo::TTechnologyType>(aMode0);
		TESTL(type == expectedType);
		if (aMode1 != KErrNotFound)
			{
			method.GetPosMethod(1, meth);
			type = meth.PosMode();
			expectedType = static_cast<TPositionModuleInfo::TTechnologyType>(aMode1);
			TESTL(type == expectedType);
			}
		
		// jcm check that advice is always TLbsPositionUpdateRequestBase::EPowerAdviceOn
		}
	
	TLbsLocRequestQualityInt locRequestQuality = locReq.RequestQuality();
	
	if (aMaxFixTime != KErrNotFound)
		{
		// check max fix time
		TTimeIntervalMicroSeconds resultingMaxFixTime = locRequestQuality.MaxFixTime();
		// we can't do a totally absolute check here because the max fix window may be reduced
		// For example, a 8 followd by a 5 would result, not in an eight, but (8 - delta1) where delata1 is the time
		// between the two requests been made plus a little more for the time it took to process the requests!
		// Conversely, a 5 followed by a 8 would result in (8 - delta2) where delta2 is the time for processing the second request.
		// So the resultingMaxFixTime should NEVER be greater and can be less than the expected by a delta - but no more than that!
		// Obviously, a 5 and 8 combination should always produce a max fic time comparable to just less than 8 
		// and not near a value of 5!
		aMaxFixTime = aMaxFixTime * 1 * 1000 * 1000; // convert to micro seconds
		if (resultingMaxFixTime > aMaxFixTime)
			{
			INFO_PRINTF1(_L("test failed resultingMaxFixTime > aMaxFixTime"));
			TESTL(EFalse);
			}
		if (resultingMaxFixTime < (aMaxFixTime- (1 * 1000 * 1000))) // here we make the delat 1 second - that will suffice as all the maxfix inputs are integers
			{
			INFO_PRINTF1(_L("test failed resultingMaxFixTime > aMaxFixTime"));
			TESTL(EFalse);
			}
		}

	if (aHorAccuracy != KErrNotFound)
		{
		// check accuracy
		TReal32 horAcc = locRequestQuality.MinHorizontalAccuracy();
		TESTL(aHorAccuracy == horAcc);
		}
	
	TLbsPositionUpdateRequestBase::TPowerAdvice powerAdvice;
	powerAdvice = locReq.PowerAdvice();
	if (aExpectedPowerAdvice!= KErrNotFound)
		{
		TESTL(powerAdvice == aExpectedPowerAdvice);
		}
	}

void CLbsNrhStateTestBaseStep::AgpsManager_WaitLocationRequestStatusL(TInt aExpectedPowerAdvice)
	{	
	INFO_PRINTF1(_L("AgpsManager_WaitLocationRequestStatusL()"));

	iAgpsChannel->WaitForRequestL(KDefaultTimeout);
	// the status may be delivered in either a Status message or a Cancel
	// its not guaranteed to be in a Status message!
	//TESTL(TLbsPositionUpdateRequestBase::EStatus == iAgpsChannel->GetNextRequestType());
	TLbsPositionUpdateRequestBase::TType type = iAgpsChannel->GetNextRequestType();
	
	TLbsPositionUpdateRequestBase::TPowerAdvice powerAdvice;
	
	if (TLbsPositionUpdateRequestBase::EStatus == type)
		{
		TLbsPositionUpdateRequestStatus statusReq;
		iAgpsChannel->GetNextRequestL(statusReq);
		powerAdvice = statusReq.PowerAdvice();
		iLastPowerAdvice = powerAdvice;	
		}
	else
		{
		TLbsPositionUpdateRequestCancel CancelReq;
		iAgpsChannel->GetNextRequestL(CancelReq);
		powerAdvice = CancelReq.PowerAdvice();
		iLastPowerAdvice = powerAdvice;
		}
	if (aExpectedPowerAdvice!= KErrNotFound)
		{
		TESTL(powerAdvice == aExpectedPowerAdvice);
		}
	
	}
	
void CLbsNrhStateTestBaseStep::AgpsManager_WaitLocationRequestCancelL(TInt aExpectedPowerAdvice)
	{
	INFO_PRINTF1(_L("AgpsManager_WaitLocationRequestCancelL()"));

	iAgpsChannel->WaitForRequestL(KDefaultTimeout);

	if (TLbsPositionUpdateRequestBase::ECancel != iAgpsChannel->GetNextRequestType())
		{
		INFO_PRINTF2(_L("AgpsManager_WaitLocationRequestCancelL() - expecting Cancel but got %d"), iAgpsChannel->GetNextRequestType());
		}

	TESTL(TLbsPositionUpdateRequestBase::ECancel == iAgpsChannel->GetNextRequestType());
	TLbsPositionUpdateRequestCancel cancelReq;
	iAgpsChannel->GetNextRequestL(cancelReq);
	
	iLastPowerAdvice = cancelReq.PowerAdvice();
	
	if (aExpectedPowerAdvice != KErrNotFound)
		{
		TESTL(aExpectedPowerAdvice == cancelReq.PowerAdvice() );
		}
	
	
	}

//
// Privacy Controller messages
//
	
void CLbsNrhStateTestBaseStep::PrivacyController_SendPrivacyResponseL(
		TUint32 aSessionId, 
		CLbsPrivacyController::TRequestVerificationResult aResponse)
	{
	INFO_PRINTF3(_L("PRVCTRL -> NRH: PrivacyController_SendPrivacyResponseL(SessionId=%d, Response=%d)"),aSessionId, aResponse);

	iPrivCtrlChannel->SendPrivacyResponse(
		aSessionId, 
		aResponse,
		KDefaultTimeout);
	}
	
void CLbsNrhStateTestBaseStep::PrivacyController_SendPrivacyCancelL(TUint32 aSessionId)
	{
	INFO_PRINTF2(_L("PRVCTRL -> NRH: PrivacyController_SendPrivacyCancelL(SessionId=%d)"),aSessionId);

	iPrivCtrlChannel->SendCancelPrivacyRequest(
		aSessionId,
		KDefaultTimeout);
	}

void CLbsNrhStateTestBaseStep::PrivacyController_WaitPrivacyRequestL(TUint32 aSessionId)
	{
	INFO_PRINTF2(_L("PrivacyController_WaitPrivacyRequestL(SessionId=%d)"), aSessionId);

	iPrivCtrlChannel->WaitForResponseL(KDefaultDoubleTimeout);
	TESTL(CPrivCtrlChannel::EDataResponsePrivReq == iPrivCtrlChannel->GetNextResponseType());
	TUint requestId;
	MLbsPrivacyObserver::TNotificationType notificationType;
	TLbsExternalRequestInfo reqInfo;
	iPrivCtrlChannel->GetNextResponseAsPrivacyRequestL(
			requestId, 
			notificationType,
			reqInfo);
	TESTL(aSessionId == requestId);
	}
	
void CLbsNrhStateTestBaseStep::PrivacyController_WaitPrivacyReferencePositionL(TUint32 aSessionId)
	{
	INFO_PRINTF2(_L("PrivacyController_WaitPrivacyReferencePositionL(SessionId=%d)"), aSessionId);

	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(CPrivCtrlChannel::EDataResponsePosUpdate == iPrivCtrlChannel->GetNextResponseType());
	TUint requestId;
	TPositionInfo posInfo;
	iPrivCtrlChannel->GetNextResponseAsPositionUpdateL(requestId, posInfo);
	TESTL(aSessionId == requestId);
	}
	
void CLbsNrhStateTestBaseStep::PrivacyController_WaitPrivacyGpsPositionL(TUint32 aSessionId)
	{
	INFO_PRINTF2(_L("PrivacyController_WaitPrivacyGpsPositionL(SessionId=%d)"), aSessionId);

	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	TESTL(CPrivCtrlChannel::EDataResponsePosUpdate == iPrivCtrlChannel->GetNextResponseType());
	TUint requestId;
	TPositionInfo posInfo;
	iPrivCtrlChannel->GetNextResponseAsPositionUpdateL(requestId, posInfo);
	TESTL(aSessionId == requestId);
	// TODO: check technology type is 'Terminal+Assisted'?
	}

void CLbsNrhStateTestBaseStep::PrivacyController_WaitPrivacyCompleteL(TUint32 aSessionId, TInt aReason)
	{
	INFO_PRINTF3(_L("PrivacyController_WaitPrivacyCompleteL(SessionId=%d, Expected Reason=%d)"), aSessionId, aReason);

	iPrivCtrlChannel->WaitForResponseL(KDefaultTimeout);
	CPrivCtrlChannel::TPrivCtrlResponseType type = iPrivCtrlChannel->GetNextResponseType();
	TESTL(CPrivCtrlChannel::EDataResponseReqComplete == iPrivCtrlChannel->GetNextResponseType());
	TInt reason;
	TUint requestId;
	iPrivCtrlChannel->GetNextResponseAsRequestCompleteL(requestId, reason);
	TESTL(aSessionId == requestId);
	TESTL(aReason == reason);
	}

void CLbsNrhStateTestBaseStep::ReadIniFileValues()
    {
    iFirstRequestAgpsMode0 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KModeFirstRequest0, iFirstRequestAgpsMode0);
    
    iFirstRequestAgpsMode1 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KModeFirstRequest1, iFirstRequestAgpsMode1);

    iSecondRequestAgpsMode0 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KModeSecondRequest0, iSecondRequestAgpsMode0);

    iSecondRequestAgpsMode1 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KModeSecondRequest1, iSecondRequestAgpsMode1);

    iFirstAgpsModeToAgpsMan0 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KFirstModeToAgpsMan0, iFirstAgpsModeToAgpsMan0);

    iFirstAgpsModeToAgpsMan1 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KFirstModeToAgpsMan1, iFirstAgpsModeToAgpsMan1);

    iSecondAgpsModeToAgpsMan0 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KSecondModeToAgpsMan0, iSecondAgpsModeToAgpsMan0);

    iSecondAgpsModeToAgpsMan1 = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KSecondModeToAgpsMan1, iSecondAgpsModeToAgpsMan1);

    iReasonInRespLocReq = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KReasonInRespLocRequestName, iReasonInRespLocReq);

  
    iAccuracyFirstRequest = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KAccuracyFirstRequestName, iAccuracyFirstRequest);

    iAccuracySecondRequest = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KAccuracySecondRequestName, iAccuracySecondRequest);

    iMaxFixFirstReq = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KMaxFixFirstReqName, iMaxFixFirstReq);
    
    iMaxFixSecondReq = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KMaxFixSecondReqName, iMaxFixSecondReq);

    iFirstAccToAgpsMan = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KFirstAccToAgpsManName, iFirstAccToAgpsMan);

    iSecondAccToAgpsMan = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KSecondAccToAgpsManName, iSecondAccToAgpsMan);

    iFirstMaxFixToAgpsMan = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KFirstMaxFixToAgpsManName, iFirstMaxFixToAgpsMan);

    iSecondMaxFixToAgpsMan = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KSecondMaxFixToAgpsManName, iSecondMaxFixToAgpsMan);

    iPowerAdviceAfterFirsRequestName = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KPowerAdviceAfterFirstRequestName, iPowerAdviceAfterFirsRequestName);

    iPowerAdviceAfterSecondRequestName = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KPowerAdviceAfterSecondRequestName, iPowerAdviceAfterSecondRequestName);
        
    iSessionType = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KSessionTypeName, iSessionType);
    
    // Member Variable: iRefToNrhLat
    // Valid Options: 
    //      Any Valid Lat
  
    iRefToNrhLat = KDefaultRefToNrhLat;
    GetIntFromConfig(ConfigSection(), KRefToNrhLatName, iRefToNrhLat);
    
    iRefToNrhLng = KDefaultRefToNrhLng;
    GetIntFromConfig(ConfigSection(), KRefToNrhLngName, iRefToNrhLng);
   
    iRefToNrhHacc = KDefaultRefToNrhHacc;
    GetIntFromConfig(ConfigSection(), KRefToNrhHaccName, iRefToNrhHacc);
   
    iPosFromNrhType = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KPosFromNrhTypeName, iPosFromNrhType);
      
    iPosFromNrhLat = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KPosFromNrhLatName, iPosFromNrhLat);
  
    iPosFromNrhLng = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KPosFromNrhLngName, iPosFromNrhLng);
    
    iPosFromNrhAcc = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KPosFromNrhHaccName, iPosFromNrhAcc);
  
    iAgpsPosToNrhError = KDefaultAgpsPosToNrhError;
    GetIntFromConfig(ConfigSection(), KAgpsPosToNrhErrorName, iAgpsPosToNrhError);
    
    iAgpsPosToNrhLat = KDefaultAgpsPosToNrhLat;
    GetIntFromConfig(ConfigSection(), KAgpsPosToNrhLatName, iAgpsPosToNrhLat);
     
    iAgpsPosToNrhLng = KDefaultAgpsPosToNrhLng;
    GetIntFromConfig(ConfigSection(), KAgpsPosToNrhLngName, iAgpsPosToNrhLng);
    
    // For incomplete positioning, we use -1 to represent the need for NaN
    iAgpsPosToNrhHacc = KDefaultAgpsPosToNrhHacc;
    GetIntFromConfig(ConfigSection(), KAgpsPosToNrhHaccName, iAgpsPosToNrhHacc);
    if(iAgpsPosToNrhHacc == KErrNotFound)
        {
        TRealX nan;
        nan.SetNaN();
        iAgpsPosToNrhHacc = nan;
        }
    
    iErrorCodeFromNrh = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KErrorCodeFromNrhTypeName, iErrorCodeFromNrh);
   
    iRefPosBeforeLocationRequest = KRefBeforeLocationrequest; // default if not defined in .ini file: inject before 
    GetIntFromConfig(ConfigSection(), KRefPosBeforeLocationRequestName, iRefPosBeforeLocationRequest);
    
    iRegistrationStatus = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KRegistrationStatusName, iRegistrationStatus);
    
    iNetworkPosToNrhError = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KNetworkPosToNrhErrorName, iNetworkPosToNrhError);

    iNetworkPosToNrhLat = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KNetworkPosToNrhLatName, iNetworkPosToNrhLat);

    iNetworkPosToNrhLng = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KNetworkPosToNrhLngName, iNetworkPosToNrhLng);

    iNetworkPosToNrhHacc = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KNetworkPosToNrhHaccName, iNetworkPosToNrhHacc);
    
    iEmergency = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KEmergencyName, iEmergency);
    
    iSessionCompleteError = KDefaultSessionCompleteErrorCode;
    GetIntFromConfig(ConfigSection(), KSessionCompleteErrorCodeName, iSessionCompleteError);

    iCancelToAgpsManagerAfterSessionComplete = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KCancelToAgpsmanAfterSessCompleteName, iCancelToAgpsManagerAfterSessionComplete);
    
    iX3pTransmitPositionRequestID = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KX3pTransmitPositionRequestIDName, iX3pTransmitPositionRequestID);
  
    iX3pTransmitPositionTransmitPriority = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KX3pTransmitPositionTransmitPriorityName, iX3pTransmitPositionTransmitPriority);
    
    iX3pTransmitPositionAgpsAndRefPos = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KX3pTransmitPositionAgpsAndRefPosName, iX3pTransmitPositionAgpsAndRefPos);
    
    iX3pTransmitPositionTimeout = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KX3pTransmitPositionTimeoutName, iX3pTransmitPositionTimeout);
 
    iPrivacyRequestEmergency = KErrNotFound;
    GetIntFromConfig(ConfigSection(), KPrivacyRequestEmergencyName, iPrivacyRequestEmergency);

    iPrivacyRequestAdvice = TLbsNetPosRequestPrivacyInt::ERequestAdviceVerify;
    GetIntFromConfig(ConfigSection(), KPrivacyRequestAdviceName, iPrivacyRequestAdvice);

    iPrivacyRequestDefaultAction = TLbsNetPosRequestPrivacyInt::ERequestActionAllow;
    GetIntFromConfig(ConfigSection(), KPrivacyRequestDefaultActionName, iPrivacyRequestDefaultAction);

    iPrivacySendResponseAction = CLbsPrivacyController::ERequestAccepted;
    GetIntFromConfig(ConfigSection(), KPrivacySendResponseActionName, iPrivacySendResponseAction);
    
    iPrivacyRcvResponseAction = TLbsNetworkEnumInt::EPrivacyResponseAccepted;
    GetIntFromConfig(ConfigSection(), KPrivacyRcvResponseActionName, iPrivacyRcvResponseAction);
    
    iPrivacyCompleteError = KErrNone;
    GetIntFromConfig(ConfigSection(), KPrivacyCompleteErrorName, iPrivacyCompleteError);
    
    iSequenceNumber = 13; // the default sequence number!
    GetIntFromConfig(ConfigSection(), KSequenceNumberName, iSequenceNumber);
    }
