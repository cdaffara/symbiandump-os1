// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
// 
//

/**
 @file lbsnetgateway6Step.cpp
*/
#include "lbsnetgateway6Step.h"
#include "Te_lbsnetgatewaySuiteDefs.h"

const TLbsNetSessionId KNetworkLocationSessionId = TLbsNetSessionId(KLbsNetLocManagerUid, 0x10000001);

Clbsnetgateway6Step::~Clbsnetgateway6Step()
/**
 * Destructor
 */
	{
	}

Clbsnetgateway6Step::Clbsnetgateway6Step()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(Klbsnetgateway6Step);
	}

TVerdict Clbsnetgateway6Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	   return   TestStepResult();

    InitialiseUidArray();
    iStubPMChannelArray = new RNetGatewayProtocolTestChannel;
    iStubPMChannelArray->OpenL(KNetProtocolChannelKey, KTestAppChannelKey, *this, KTe_LbsNetGatewayTestProtocolIndex);
	
	iReferencePositionUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionReference);
	iFinalPositionUpdates.OpenL(RLbsNetworkPositionUpdates::ENetworkPositionFinal);

	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict Clbsnetgateway6Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	if (TestStepResult() == EPass)
		{
		//
		// Test that positions from the network are published on 
		// the internal api's correctly. So, 'reference' positions
		// should go onto the ENetworkPositionReference channel
		// of RLbsNetworkPositionUpdates and 'final' positions
		// should go on the ENetworkPositionFinal channel.
		//
		
		// 
		// Setup data used in test
		//
		TPosition referencePosition;
		referencePosition.SetAccuracy(TReal32(150.0), TReal32(150.0));
		referencePosition.SetCoordinate(TReal64(11145.6), TReal64(4567.3), TReal32(99.0));
		referencePosition.SetCurrentTime();

		TPosition finalPosition;
		finalPosition.SetAccuracy(TReal32(10.0), TReal32(10.0));
		finalPosition.SetCoordinate(TReal64(5582.7), TReal64(13877.6), TReal32(1408.7));
		finalPosition.SetCurrentTime();
		
		TPositionInfo referencePositionInfo;
		referencePositionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork);
		referencePositionInfo.SetPositionModeReason(EPositionModeReasonNone);
		referencePositionInfo.SetUpdateType(EPositionUpdateGeneral);
		referencePositionInfo.SetPosition(referencePosition);

		TPositionInfo finalPositionInfo;
		finalPositionInfo.SetPositionModeReason(EPositionModeReasonNone);
		finalPositionInfo.SetUpdateType(EPositionUpdateGeneral);
		finalPositionInfo.SetPosition(finalPosition);
		
		//
		// 1) Test positions with technology type of 'ETechnologyNetwork'
		//    are treated as 'reference' positions.
		//
			{
			SNetMsgProcessLocationUpdate netBasedLoc;
			netBasedLoc.iSessionId = KNetworkLocationSessionId;
			
			Mem::Copy(&netBasedLoc.iReferenceLocation, 
					  &referencePositionInfo,
					  referencePositionInfo.PositionClassSize());
			
			TNetGatewayMsg pmMsg;
			pmMsg.iType = ENetMsgProcessLocationUpdate;
			Mem::Copy(pmMsg.Data(), &netBasedLoc, sizeof(SNetMsgProcessLocationUpdate));
			
			TRequestStatus status;
			iReferencePositionUpdates.NotifyNetworkLocationUpdate(status);		
			iStubPMChannelArray->SendNetProtocolMessageL(pmMsg);
			User::WaitForRequest(status);
			TESTL(status.Int() == KErrNone);
			
			TTime targetTime;
			TTime actualTime;
			TPositionInfo posInfo;
			TLbsNetSessionIdInt sessionId;
			iReferencePositionUpdates.GetPositionInfo(sessionId, posInfo, targetTime, actualTime);
			TESTL(TestPositionInfosAreEqual(posInfo, referencePositionInfo));
			}
		
		//
		// 2) Test positions with technology type of 'ETechnologyNetwork | ETechnologyAssisted'
		//    are treated as 'final' positions.
		//
			{
			finalPositionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyNetwork
											  | TPositionModuleInfo::ETechnologyAssisted);

			SNetMsgProcessLocationUpdate netBasedLoc;
			netBasedLoc.iSessionId = KNetworkLocationSessionId;
			
			Mem::Copy(&netBasedLoc.iReferenceLocation, 
					  &finalPositionInfo,
					  finalPositionInfo.PositionClassSize());
			
			TNetGatewayMsg pmMsg;
			pmMsg.iType = ENetMsgProcessLocationUpdate;
			Mem::Copy(pmMsg.Data(), &netBasedLoc, sizeof(SNetMsgProcessLocationUpdate));
			
			TRequestStatus status;
			iFinalPositionUpdates.NotifyNetworkLocationUpdate(status);		
			iStubPMChannelArray->SendNetProtocolMessageL(pmMsg);
			User::WaitForRequest(status);
			TESTL(status.Int() == KErrNone);
			
			TTime targetTime;
			TTime actualTime;
			TPositionInfo posInfo;
			TLbsNetSessionIdInt sessionId;
			iFinalPositionUpdates.GetPositionInfo(sessionId, posInfo, targetTime, actualTime);
			TESTL(posInfo.ModuleId() == TUid::Uid(0x10281D43));
			finalPositionInfo.SetModuleId(TUid::Uid(0x10281D43));
			TESTL(TestPositionInfosAreEqual(posInfo, finalPositionInfo));
			}

		//
		// 3) Test positions with technology type of 'ETechnologyTerminal | ETechnologyAssisted'
		//    are treated as 'final' positions.
		//
			{
			finalPositionInfo.SetPositionMode(TPositionModuleInfo::ETechnologyTerminal
											  | TPositionModuleInfo::ETechnologyAssisted);

			SNetMsgProcessLocationUpdate netBasedLoc;
			netBasedLoc.iSessionId = KNetworkLocationSessionId;
			
			Mem::Copy(&netBasedLoc.iReferenceLocation, 
					  &finalPositionInfo,
					  finalPositionInfo.PositionClassSize());
			
			TNetGatewayMsg pmMsg;
			pmMsg.iType = ENetMsgProcessLocationUpdate;
			Mem::Copy(pmMsg.Data(), &netBasedLoc, sizeof(SNetMsgProcessLocationUpdate));
			
			TRequestStatus status;
			iFinalPositionUpdates.NotifyNetworkLocationUpdate(status);		
			iStubPMChannelArray->SendNetProtocolMessageL(pmMsg);
			User::WaitForRequest(status);
			TESTL(status.Int() == KErrNone);
			
			TTime targetTime;
			TTime actualTime;
			TPositionInfo posInfo;
			TLbsNetSessionIdInt sessionId;
			iFinalPositionUpdates.GetPositionInfo(sessionId, posInfo, targetTime, actualTime);
			TESTL(posInfo.ModuleId() == TUid::Uid(0x10281D44));
			finalPositionInfo.SetModuleId(TUid::Uid(0x10281D44));
			TESTL(TestPositionInfosAreEqual(posInfo, finalPositionInfo));
			}		
		
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}



TVerdict Clbsnetgateway6Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	iFinalPositionUpdates.Close();
	iReferencePositionUpdates.Close();
	CTe_lbsnetgatewaySuiteStepBase::doTestStepPostambleL();
	return TestStepResult();
	}
