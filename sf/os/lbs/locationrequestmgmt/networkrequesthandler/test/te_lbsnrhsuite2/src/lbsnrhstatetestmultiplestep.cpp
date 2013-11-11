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
 @file lbsnrhstatetestmultiplestep.cpp
*/
#include "lbsprocessuiddefs.h"

#include "lbsnrhstatetestmultiplestep.h"


const TPositionModuleInfo::TTechnologyType KTerminalAssistedMode = (TPositionModuleInfo::ETechnologyNetwork |
																	TPositionModuleInfo::ETechnologyAssisted);

CLbsNrhStateTestMultipleStep::CLbsNrhStateTestMultipleStep()
	{
	SetTestStepName(KLbsNrhStateTestMultipleStep);
	}

CLbsNrhStateTestMultipleStep::~CLbsNrhStateTestMultipleStep()
	{
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhStateTestMultipleStep::doTestStepPreambleL()
	{
	// Common initialisation  
	CLbsNrhStateTestBaseStep::doTestStepPreambleL();
	
	SetTestStepResult(EPass);
	return TestStepResult();
	}


/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
TVerdict CLbsNrhStateTestMultipleStep::doTestStepL()
	{
	if (TestStepResult() == EPass)
		{
		ReadIniFileValues();
		
		switch (GetTestCaseId())
			{
			case 0:	{Multi_MtLr_MoLrL();				break;	}
			case 1:	{Multi_MtLr_MoLr_CellL();			break; }
			case 2: {Multi_MtLr_X3pL();					break; }
			case 3:	{Multi_MoLr_MoLr_CellL();			break; }
			case 4:	{Multi_MoLr_X3pL();					break; }
			case 5:	{Multi_MoLr_Cell_X3pL();			break; }
			case 6:	{Multi_MtLr_MoLr_MoLr_Cell_X3pL();	break; }
			case 7:	{Multi_MtLr_MtLr();					break; }
			case 8:	{Multi_MtLr_MtLr_B();				break; }
			case 9:	{Multi_MtLr_MtLr_CheckMode();		break; }
			case 10:{Multi_MtLr_NotSupported_CheckMode();	break; }
			case 11:{Multi_MtLr_Use_Admin_CheckMode();		break; }
			case 12:{MTLrAndThenAnEmergencyMtLrL();			break; }
			case 13:{Multi_MtLr_MtLr_CheckModeErrorToAllSessions();	break; }
			case 14: // capaable of TAP only
			case 15: // capabale of Hybrid
					{Multi_MtLr_MtLr_CheckMode_Measurements();		break; }

			default:
				{
				WARN_PRINTF2(_L("CLbsNrhStateTestMultipleStep::doTestStepL: Unknown test case Id: %d"), 
							 GetTestCaseId());
				break;
				}
			}
		}
		
	return TestStepResult();
	}

/**
 * @return - TVerdict code
 * Override of base class virtual
 */
TVerdict CLbsNrhStateTestMultipleStep::doTestStepPostambleL()
	{	
	CLbsNrhStateTestBaseStep::doTestStepPostambleL();

	return TestStepResult();	
	}

void CLbsNrhStateTestMultipleStep::Multi_MtLr_MoLrL()
	{
	//
	// Test 1: Start a MT-LR, then start a MO-LR after the privacy response.
	//
	TUint32 mtlrSessionId(1);
	TUint32 molrSessionId(2);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	AgpsManager_WaitLocationRequestL();
	
	// Start MO-LR
	NetGateway_SendNetworkReferencePositionL(molrSessionId, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(molrSessionId, TLbsNetworkEnumInt::EServiceSelfLocation);	
	AgpsManager_WaitLocationRequestL();
	
	AgpsManager_SendGpsPositionUpdateAccurateL();
	
	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);	
	NetGateway_WaitLocationResponseL(molrSessionId, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
	
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);
	
	NetGateway_SendSessionCompleteL(molrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestCancelL();
	
	//
	// Test 2: Start a MO-LR, then start a MT-LR before the location update
	//
	mtlrSessionId = 3;
	molrSessionId = 4;

	// Start MO-LR
	NetGateway_SendNetworkReferencePositionL(molrSessionId, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(molrSessionId, TLbsNetworkEnumInt::EServiceSelfLocation);	
	AgpsManager_WaitLocationRequestL();
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);

	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	AgpsManager_WaitLocationRequestL();

	AgpsManager_SendGpsPositionUpdateAccurateL();
	
	NetGateway_WaitLocationResponseL(molrSessionId, KErrNone);
	NetGateway_SendSessionCompleteL(molrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);	
	AgpsManager_WaitLocationRequestCancelL();
	}
	
void CLbsNrhStateTestMultipleStep::Multi_MtLr_MtLr()
	{
	//
	// Test Multi_MtLr_MtLr: Start a MT-LR, then start another MT-LR after the privacy response.
	//
	TUint32 mtlrSessionId(1);
	TUint32 mtlrSecondSessionId(2);
	
	// Start MT-LR

	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	AgpsManager_WaitLocationRequestStatusL();
	
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	
	AgpsManager_WaitLocationRequestL();
	
	// Start second MT-LR
	
	NetGateway_SendPrivacyRequestL(mtlrSecondSessionId);

	PrivacyController_WaitPrivacyRequestL(mtlrSecondSessionId);
	
	PrivacyController_SendPrivacyResponseL(mtlrSecondSessionId, CLbsPrivacyController::ERequestAccepted);

	NetGateway_WaitPrivacyResponseL(mtlrSecondSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);

	NetGateway_SendNetworkReferencePositionL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(mtlrSecondSessionId);

	NetGateway_SendLocationRequestL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);

	AgpsManager_WaitLocationRequestL();
	
	AgpsManager_SendGpsPositionUpdateAccurateL();

	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);	

	NetGateway_WaitLocationResponseL(mtlrSecondSessionId, KErrNone);

	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);

	PrivacyController_WaitPrivacyGpsPositionL(mtlrSecondSessionId);
		
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);

	NetGateway_SendSessionCompleteL(mtlrSecondSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSecondSessionId, KErrNone);
		
	INFO_PRINTF1(_L("Done"));

	}

void CLbsNrhStateTestMultipleStep::Multi_MtLr_MtLr_B()
	{
	//
	// Test Multi_MtLr_MtLr_B: Start a MT-LR, then start another  MT-LR 
	// before the first ones request has been responded to.
	//
	TUint32 mtlrSessionId(1);
	TUint32 mtlrSecondSessionId(2);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	
	// now before the privacy controller responds start another MTLR
	NetGateway_SendPrivacyRequestL(mtlrSecondSessionId);

	PrivacyController_WaitPrivacyRequestL(mtlrSecondSessionId);
		
	// so now there are two privacy request waiting to be responded to
	
	// firstly, respond to the first one
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	AgpsManager_WaitLocationRequestStatusL();
	
	// now respond to the second privacy request

	PrivacyController_SendPrivacyResponseL(mtlrSecondSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSecondSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);

	// now send in REF position for first session
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
		
	// and then send in REF position for second session
	NetGateway_SendNetworkReferencePositionL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSecondSessionId);
		
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
		
	// power on expected
	AgpsManager_WaitLocationRequestL();
		
	NetGateway_SendLocationRequestL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	
	// again, as for the REF position, AGPS positions are broadcast to all the sessions in the NRH
	AgpsManager_SendGpsPositionUpdateAccurateL();

	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);	

	NetGateway_WaitLocationResponseL(mtlrSecondSessionId, KErrNone);

	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);

	PrivacyController_WaitPrivacyGpsPositionL(mtlrSecondSessionId);
		
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);

	NetGateway_SendSessionCompleteL(mtlrSecondSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSecondSessionId, KErrNone);
		
	INFO_PRINTF1(_L("Done"));

	}

void CLbsNrhStateTestMultipleStep::Multi_MtLr_Use_Admin_CheckMode()
	{
	//
	// Tests a TA+TB or TB+TA arriving when there is no request active
	// It is the value in AGPS the Admin that detemines what happens next ...
	//
	TUint32 mtlrSessionId(1);
		
		// Start MT-LR
		NetGateway_SendPrivacyRequestL(mtlrSessionId);
		
		PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
		
		PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
		
		NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
		// note, advice should be on and not standby
		AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceOn);
		
		// now send in REF position for first session
		NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
		
		PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	
		NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
		
		AgpsManager_WaitLocationRequestL(iFirstAgpsModeToAgpsMan0, iFirstAgpsModeToAgpsMan1);

		AgpsManager_SendGpsPositionUpdateAccurateL();

		// we should always het this callback for the first session
		PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);

		NetGateway_WaitLocationResponseL(mtlrSessionId, iReasonInRespLocReq);	

		NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

		PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);
			
		INFO_PRINTF1(_L("Done"));


	}

void CLbsNrhStateTestMultipleStep::Multi_MtLr_NotSupported_CheckMode()
	{
	//
	// When there is no active request when a new request arrives if the 
	// integration module does not support the requested mode then KErrNotSupported error is
	// sent to the Network Gateway
	//
	TUint32 mtlrSessionId(1);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	AgpsManager_WaitLocationRequestStatusL();
	
	// now send in REF position for first session
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
		
	// the next request should fail because this mode is not supported
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);

	NetGateway_WaitLocationResponseL(mtlrSessionId, iReasonInRespLocReq);	
	
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);
		
	INFO_PRINTF1(_L("Done"));

	}

void CLbsNrhStateTestMultipleStep::Multi_MtLr_MtLr_CheckMode()
	{
	//
	// Test Multi_MtLr_MtLr_CheckMode: Start a MT-LR, then start another  MT-LR 
	// before the first ones request has been responded to.
	// This test checks that the AGPS mode selected by the NRH is correct.

	TUint32 mtlrSessionId(1);
	TUint32 mtlrSecondSessionId(2);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	
	// now before the privacy controller responds start another MTLR
	NetGateway_SendPrivacyRequestL(mtlrSecondSessionId);

	PrivacyController_WaitPrivacyRequestL(mtlrSecondSessionId);
		
	// so now there are two privacy request waiting to be responded to
	
	// firstly, respond to the first one
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	// note after OK priv response - power advice should be ON and not standby
	AgpsManager_WaitLocationRequestStatusL(TLbsPositionUpdateRequestBase::EPowerAdviceOn);
	
	// now respond to the second privacy request

	PrivacyController_SendPrivacyResponseL(mtlrSecondSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSecondSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);

	// now send in REF position for first session
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
		
	// and then send in REF position for second session
	NetGateway_SendNetworkReferencePositionL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSecondSessionId);
		
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1,iAccuracyFirstRequest, iMaxFixFirstReq );
		
	AgpsManager_WaitLocationRequestL(iFirstAgpsModeToAgpsMan0, iFirstAgpsModeToAgpsMan1, iFirstAccToAgpsMan, iFirstMaxFixToAgpsMan, TLbsPositionUpdateRequestBase::EPowerAdviceOn);
	
	// these two booleans tell us whether the first and second sessions are TAP sessions
	TBool tapModeFirst = ((iFirstRequestAgpsMode0 == KTerminalAssistedMode) && (iFirstRequestAgpsMode1 == KErrNotFound));
	TBool tapModeSecond = ((iSecondRequestAgpsMode0 == KTerminalAssistedMode) && (iSecondRequestAgpsMode1 == KErrNotFound));

	NetGateway_SendLocationRequestL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iSecondRequestAgpsMode0, iSecondRequestAgpsMode1,iAccuracySecondRequest, iMaxFixSecondReq);
	
	if (iReasonInRespLocReq == KErrNone)
		{
		// if the second request was NOT rejected by the NRH
		AgpsManager_WaitLocationRequestL(iSecondAgpsModeToAgpsMan0, iSecondAgpsModeToAgpsMan1,iSecondAccToAgpsMan, iSecondMaxFixToAgpsMan);
		}
	else
		{ 
		// and the error is sent immedialetely to the NG
		NetGateway_WaitLocationResponseL(mtlrSecondSessionId, iReasonInRespLocReq);
		}
	
	// AGPS positions are broadcast to all the sessions in the NRH
	// We note that the Tap mode sessions ignore these positions
	// and  only measurements are delivered to the protocol module
	AgpsManager_SendGpsPositionUpdateAccurateL();

	if (!tapModeFirst)
		{
		// when the first session is not a TAP then a location response to network
		NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);	
		}
	
	
	if (iReasonInRespLocReq == KErrNone)
		{
		if (!tapModeSecond)
			{
			// only non TAP sessions consume by the second session
			NetGateway_WaitLocationResponseL(mtlrSecondSessionId, iReasonInRespLocReq);
			}
		}
	
	
	// for tap mode - no location sent to privacy controller
	if (!tapModeFirst)
		{
		PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
		}

	if ( (iReasonInRespLocReq == KErrNone) && !tapModeSecond )
		{
		// error - no gps position  sent to agps manager 
		// tap mode - no position sent to agps manager 
		PrivacyController_WaitPrivacyGpsPositionL(mtlrSecondSessionId);
		}
	
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);

	if (tapModeSecond)
		{
		// if the second request was associated with a TAP session then it would not be satified with the injected agps position 
		// so now the NRH will re-issue the second location to satisfy the second session that
		// hasn't yet been satified
		if (iReasonInRespLocReq == KErrNone) // but note, if the second request was rejected then it won't be re-issued
			{
			AgpsManager_WaitLocationRequestL(iSecondAgpsModeToAgpsMan0, iSecondAgpsModeToAgpsMan1,iSecondAccToAgpsMan, iSecondMaxFixToAgpsMan);
			}
		}
	
	NetGateway_SendSessionCompleteL(mtlrSecondSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSecondSessionId, KErrNone);
		
	
	//	expect a cancel now. 
	AgpsManager_WaitLocationRequestCancelL(TLbsPositionUpdateRequestBase::EPowerAdviceOff);

	INFO_PRINTF1(_L("Done"));

	}

void CLbsNrhStateTestMultipleStep::Multi_MtLr_MtLr_CheckMode_Measurements()
	{
	//
	// Starta a MT-LR, then start another  MT-LR 
	// before the first ones request has been responded to.
	// The test, acting as AGPS manager, injects measurements
	// Only those sessions either in TAP mode or hybrid mode should
	// send the measuremnts out to the Network Gateway (the test acting as)
	//
	TUint32 mtlrSessionId(1);
	TUint32 mtlrSecondSessionId(2);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	
	// now before the privacy controller responds start another MTLR
	NetGateway_SendPrivacyRequestL(mtlrSecondSessionId);

	PrivacyController_WaitPrivacyRequestL(mtlrSecondSessionId);
		
	// so now there are two privacy request waiting to be responded to
	
	// firstly, respond to the first one
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	AgpsManager_WaitLocationRequestStatusL();
	
	// now respond to the second privacy request

	PrivacyController_SendPrivacyResponseL(mtlrSecondSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSecondSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);

	// now send in REF position for first session
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
		
	// and then send in REF position for second session
	NetGateway_SendNetworkReferencePositionL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSecondSessionId);
		
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
		
	AgpsManager_WaitLocationRequestL(iFirstAgpsModeToAgpsMan0, iFirstAgpsModeToAgpsMan1);
		
	NetGateway_SendLocationRequestL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iSecondRequestAgpsMode0, iSecondRequestAgpsMode1);
	
	if (iReasonInRespLocReq != KErrNone)
		{
		// the second request was rejected by the NRH
		}
	else
		{
		AgpsManager_WaitLocationRequestL(iSecondAgpsModeToAgpsMan0, iSecondAgpsModeToAgpsMan1);
		}
	
	if (iReasonInRespLocReq != KErrNone)
		{ 
		// and the error is sent immedialely to the NG
		NetGateway_WaitLocationResponseL(mtlrSecondSessionId, iReasonInRespLocReq);
		}
	
	// AGPS measuremnts are broadcast to all the sessions in the NRH
	// but note that for TB mode sessions the NRH ignores these positions
	// and  only positions on these sessions are delivered to the protocol module
	
	TInt testId = GetTestCaseId();
	
	if (testId == 14)
		{
		// Int mod capable of TAP only so do not send in a position - just measurements
		AgpsManager_SendGpsMeasurementUpdateL();
		}
	else
		{ // testId == 15,  capable of Hybrid so send in both pos and measurements
		
		AgpsManager_SendGpsMeasurementUpdateL();
		AgpsManager_SendGpsPositionUpdateAccurateL();
	
		}
	if ((iFirstAgpsModeToAgpsMan0 == 6)&& (iSecondAgpsModeToAgpsMan1 == KErrNotFound)) // tap 
		{
		NetGateway_WaitMeasurementResponseL(mtlrSessionId, KErrNone);
		}
	else if ((iFirstAgpsModeToAgpsMan0 == 5) && (iFirstAgpsModeToAgpsMan1 == 6)) // hybrid
		{
		NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);
		NetGateway_WaitMeasurementResponseL(mtlrSessionId, KErrNone);
		PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
		}
	else if ((iFirstAgpsModeToAgpsMan0 == 5) && (iFirstAgpsModeToAgpsMan1 == KErrNotFound))
		{
		NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone); // tb
		PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
		}
	
	if ((iSecondAgpsModeToAgpsMan0 == 6)&& (iSecondAgpsModeToAgpsMan1 == KErrNotFound)) // tap 
		{
		NetGateway_WaitMeasurementResponseL(mtlrSecondSessionId, KErrNone);	
		}
	else if ((iSecondAgpsModeToAgpsMan0 == 5) && (iSecondAgpsModeToAgpsMan1 == 6)) // hybrid
		{
		NetGateway_WaitLocationResponseL(mtlrSecondSessionId, KErrNone);
		// note that we do NOT get the measuremnts when the alpha2 timer expires because
		// the request has already been satifide with the AGPS position
		PrivacyController_WaitPrivacyGpsPositionL(mtlrSecondSessionId);
		}
	else if ((iSecondAgpsModeToAgpsMan0 == 5) && (iSecondAgpsModeToAgpsMan1 == KErrNotFound)) // tb
		{
		NetGateway_WaitLocationResponseL(mtlrSecondSessionId, KErrNone);
		PrivacyController_WaitPrivacyGpsPositionL(mtlrSecondSessionId);
		}
	
	
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);

	NetGateway_SendSessionCompleteL(mtlrSecondSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSecondSessionId, KErrNone);
		
	INFO_PRINTF1(_L("Done"));

	}
void CLbsNrhStateTestMultipleStep::Multi_MtLr_MtLr_CheckModeErrorToAllSessions()
	{
	//
	// Test Multi_MtLr_MtLr_B: Start a MT-LR, then start another  MT-LR 
	// before the first ones request has been responded to.
	// For this test the agps position has an error assocaited with it.
	// Note that the NG should receive the error for all sessions - regradless of the mode (TA or Hybrid etc.) 
	
	TUint32 mtlrSessionId(1);
	TUint32 mtlrSecondSessionId(2);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	
	// now before the privacy controller responds start another MTLR
	NetGateway_SendPrivacyRequestL(mtlrSecondSessionId);

	PrivacyController_WaitPrivacyRequestL(mtlrSecondSessionId);
		
	// so now there are two privacy request waiting to be responded to
	
	// firstly, respond to the first one
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	AgpsManager_WaitLocationRequestStatusL();
	
	// now respond to the second privacy request

	PrivacyController_SendPrivacyResponseL(mtlrSecondSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSecondSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);

	// now send in REF position for first session
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
		
	// and then send in REF position for second session
	NetGateway_SendNetworkReferencePositionL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSecondSessionId);
		
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
		
	AgpsManager_WaitLocationRequestL(iFirstAgpsModeToAgpsMan0, iFirstAgpsModeToAgpsMan1);
		
	NetGateway_SendLocationRequestL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iSecondRequestAgpsMode0, iSecondRequestAgpsMode1);
	
	if (iReasonInRespLocReq != KErrNone)
		{
		// the second request was rejected by the NRH
		}
	else
		{
		AgpsManager_WaitLocationRequestL(iSecondAgpsModeToAgpsMan0, iSecondAgpsModeToAgpsMan1);
		}
	
	if (iReasonInRespLocReq != KErrNone)
		{ 
		// and the error is sent immedialely to the NG
		NetGateway_WaitLocationResponseL(mtlrSecondSessionId, iReasonInRespLocReq);
		}
	
	// AGPS error positions are broadcast to all the sessions in the NRH
	
	AgpsManager_SendGpsPositionUpdateAccurateL(iAgpsPosToNrhError);

	// regardless of the mode the error should be dleivered to both sessions
	NetGateway_WaitLocationResponseL(mtlrSessionId, iAgpsPosToNrhError);	
	NetGateway_WaitLocationResponseL(mtlrSecondSessionId, iAgpsPosToNrhError);	

//	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
//	PrivacyController_WaitPrivacyGpsPositionL(mtlrSecondSessionId);
	
	NetGateway_SendSessionCompleteL(mtlrSessionId, iAgpsPosToNrhError, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, iAgpsPosToNrhError);

	NetGateway_SendSessionCompleteL(mtlrSecondSessionId, iAgpsPosToNrhError, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(mtlrSecondSessionId, iAgpsPosToNrhError);
		
	INFO_PRINTF1(_L("Done"));

	}

void CLbsNrhStateTestMultipleStep::Multi_MtLr_MoLr_CellL()
	{
	//
	// Test 1: Start a MT-LR, then start a MO-LR_Cell after the privacy response.
	//
	TUint32 mtlrSessionId(1);
	TUint32 molrSessionId(2);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	AgpsManager_WaitLocationRequestL();
	
	// Start MO-LR_Cell
	NetGateway_SendNetworkReferencePositionL(molrSessionId, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_SendLocationRequestL(molrSessionId, TLbsNetworkEnumInt::EServiceNetworkLocation);
	
	AgpsManager_SendGpsPositionUpdateAccurateL();

	NetGateway_WaitLocationResponseL(molrSessionId, KErrNone);
	NetGateway_SendSessionCompleteL(molrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceNetworkLocation);
	
	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();	
	}
	
void CLbsNrhStateTestMultipleStep::Multi_MtLr_X3pL()
	{
	//
	// Test 1: Start a MT-LR, then start a X3P after the privacy response.
	//
	TUint32 mtlrSessionId(5);
	TUint32 x3pClientId(1);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();
	
	// Start X3P
	X3pClient_ConnectClientL(x3pClientId);
	X3pClient_SendX3pRequestL(x3pClientId);
	TUint x3pSessionId;
	NetGateway_WaitX3pRequestL(x3pSessionId);
	
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	AgpsManager_WaitLocationRequestL();
	
	NetGateway_SendNetworkReferencePositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pReferencePositionL(1);
	NetGateway_SendLocationRequestL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	AgpsManager_WaitLocationRequestL();
	
	AgpsManager_SendGpsPositionUpdateAccurateL();
	
	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);		

	NetGateway_WaitLocationResponseL(x3pSessionId, KErrNone);
	NetGateway_SendNetworkFinalPositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);
	
	NetGateway_SendSessionCompleteL(x3pSessionId, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pCompleteL(x3pClientId, KErrNone);
	
    
    if(iLastPowerAdvice != TLbsPositionUpdateRequestBase::EPowerAdviceStandby)
        {
        AgpsManager_WaitLocationRequestCancelL();
        }
    
	X3pClient_DisconnectClientL(x3pClientId);
	AgpsManager_WaitLocationRequestStatusL();
    if(iLastPowerAdvice != TLbsPositionUpdateRequestBase::EPowerAdviceOff)
        {
        AgpsManager_WaitLocationRequestStatusL();
        }
	}
	
void CLbsNrhStateTestMultipleStep::Multi_MoLr_MoLr_CellL()
	{
/*
	// Standard MO-LR
	NetGateway_SendNetworkReferencePositionL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(2, KErrNone);
	NetGateway_SendSessionCompleteL(2, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestCancelL();
	
	// Standard MO-LR Cell-based
	NetGateway_SendNetworkReferencePositionL(2, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_SendLocationRequestL(2, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_WaitLocationResponseL(2, KErrNone);
	NetGateway_SendSessionCompleteL(2, KErrNone, TLbsNetworkEnumInt::EServiceNetworkLocation);	
*/
	}
	
void CLbsNrhStateTestMultipleStep::Multi_MoLr_X3pL()
	{
/*
	// Standard MO-LR
	NetGateway_SendNetworkReferencePositionL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(2, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(2, KErrNone);
	NetGateway_SendSessionCompleteL(2, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestCancelL();
	
	// Standard X3P
	X3pClient_ConnectToServerL(1);
	AgpsManager_WaitLocationRequestStatusL();
	X3pClient_SendX3pRequestL(1);
	TUint x3pSessionId;
	NetGateway_WaitX3pRequestL(x3pSessionId);
	AgpsManager_WaitLocationRequestStatusL();
	NetGateway_SendNetworkReferencePositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pReferencePositionL(1);
	NetGateway_SendLocationRequestL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(x3pSessionId, KErrNone);
	NetGateway_SendNetworkFinalPositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	NetGateway_SendSessionCompleteL(x3pSessionId, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pCompleteL(1, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();
*/
	}
	
void CLbsNrhStateTestMultipleStep::Multi_MoLr_Cell_X3pL()
	{
/*
	// Standard MO-LR Cell-based
	NetGateway_SendNetworkReferencePositionL(2, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_SendLocationRequestL(2, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_WaitLocationResponseL(2, KErrNone);
	NetGateway_SendSessionCompleteL(2, KErrNone, TLbsNetworkEnumInt::EServiceNetworkLocation);	
	
	// Standard X3P
	X3pClient_ConnectToServerL(1);
	AgpsManager_WaitLocationRequestStatusL();
	X3pClient_SendX3pRequestL(1);
	TUint x3pSessionId;
	NetGateway_WaitX3pRequestL(x3pSessionId);
	AgpsManager_WaitLocationRequestStatusL();
	NetGateway_SendNetworkReferencePositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pReferencePositionL(1);
	NetGateway_SendLocationRequestL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	AgpsManager_WaitLocationRequestL();
	AgpsManager_SendGpsPositionUpdateAccurateL();
	NetGateway_WaitLocationResponseL(x3pSessionId, KErrNone);
	NetGateway_SendNetworkFinalPositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	NetGateway_SendSessionCompleteL(x3pSessionId, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pCompleteL(1, KErrNone);
	AgpsManager_WaitLocationRequestCancelL();
*/
	}
	
void CLbsNrhStateTestMultipleStep::Multi_MtLr_MoLr_MoLr_Cell_X3pL()
	{
	//
	// Test 1: Start a MT-LR, then MO-LR, then a MO-LR_Cell and finally a X3P.
	//
	TUint32 mtlrSessionId(10);
	TUint32 molrSessionId(11);
	TUint32 molrcellSessionId(12);
	TUint32 x3pClientId(1);
	
	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	AgpsManager_WaitLocationRequestStatusL();
	
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
	AgpsManager_WaitLocationRequestL();
	
	// Start MO-LR
	NetGateway_SendNetworkReferencePositionL(molrSessionId, TLbsNetworkEnumInt::EServiceSelfLocation);
	NetGateway_SendLocationRequestL(molrSessionId, TLbsNetworkEnumInt::EServiceSelfLocation);
	AgpsManager_WaitLocationRequestL();
	
	// Start MO-LR_Cell
	NetGateway_SendNetworkReferencePositionL(molrcellSessionId, TLbsNetworkEnumInt::EServiceNetworkLocation);
	NetGateway_SendLocationRequestL(molrcellSessionId, TLbsNetworkEnumInt::EServiceNetworkLocation);
	
	// MO-LR_Cell should finish straight away
	NetGateway_WaitLocationResponseL(molrcellSessionId, KErrNone);
	
	// Start X3P
	X3pClient_ConnectClientL(x3pClientId);
	X3pClient_SendX3pRequestL(x3pClientId);
	TUint x3pSessionId;
	NetGateway_WaitX3pRequestL(x3pSessionId);
	NetGateway_SendNetworkReferencePositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pReferencePositionL(x3pClientId);
	NetGateway_SendLocationRequestL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	AgpsManager_WaitLocationRequestL();
	
	// Send accurate location update	
	AgpsManager_SendGpsPositionUpdateAccurateL();
	
	// MT-LR finishes
	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);
	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
	
	// MO-LR finishes
	NetGateway_WaitLocationResponseL(molrSessionId, KErrNone);
	
	// X3P finishes
	NetGateway_WaitLocationResponseL(x3pSessionId, KErrNone);
	
	// Complete MT-LR
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);
	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);
	
	// Complete MO-LR
	NetGateway_SendNetworkFinalPositionL(molrSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	NetGateway_SendSessionCompleteL(molrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceSelfLocation);
	
	// Complete MO-LR_Cell
	NetGateway_SendSessionCompleteL(molrcellSessionId, KErrNone, TLbsNetworkEnumInt::EServiceNetworkLocation);
	
	// Complete X3P
	NetGateway_SendNetworkFinalPositionL(x3pSessionId, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	NetGateway_SendSessionCompleteL(x3pSessionId, KErrNone, TLbsNetworkEnumInt::EServiceTransmitThirdParty);
	X3pClient_WaitX3pCompleteL(x3pClientId, KErrNone);
	
	// All requests finished, should get a cancel sent to the AGPS manager.
	AgpsManager_WaitLocationRequestCancelL();
	
	// Disconencting the X3P client should cause another power advice (off).
	X3pClient_DisconnectClientL(x3pClientId);
	AgpsManager_WaitLocationRequestStatusL();
	}


void CLbsNrhStateTestMultipleStep::MTLrAndThenAnEmergencyMtLrL()
	{
	//
	// Test MTLrAndThenAnEmergencyMtLrL: Start a MT-LR, then start another  
	// Emergency MT-LR before the first ones request has been responded to.
	// This test checks that the correct Emergency behaviour.
	
	TUint32 mtlrSessionId(1);
	TUint32 mtlrSecondSessionId(2);
	

	// Start MT-LR
	NetGateway_SendPrivacyRequestL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyRequestL(mtlrSessionId);
	
	// now before the privacy controller responds start another MTLR
	NetGateway_SendEmergencyPrivacyRequestL(mtlrSecondSessionId);

	// this emergency privacy request is automatically accepted by the NRH
	// after the notification
	PrivacyController_WaitPrivacyRequestL(mtlrSecondSessionId);
	
	// there should be a response
	NetGateway_WaitPrivacyResponseL(mtlrSecondSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
		
	// so now now only the first privacy request is pending -  respond 
	PrivacyController_SendPrivacyResponseL(mtlrSessionId, CLbsPrivacyController::ERequestAccepted);
	
	NetGateway_WaitPrivacyResponseL(mtlrSessionId, TLbsNetworkEnumInt::EPrivacyResponseAccepted);
	
	AgpsManager_WaitLocationRequestStatusL();
	
	// now send in REF position for first session
	NetGateway_SendNetworkReferencePositionL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);
		
	// and then send in REF position for second session
	NetGateway_SendNetworkReferencePositionL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyReferencePositionL(mtlrSessionId);
	
	PrivacyController_WaitPrivacyReferencePositionL(mtlrSecondSessionId);
		
	NetGateway_SendLocationRequestL(mtlrSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iFirstRequestAgpsMode0,iFirstRequestAgpsMode1);
		
	AgpsManager_WaitLocationRequestL(iFirstAgpsModeToAgpsMan0, iFirstAgpsModeToAgpsMan1);
	
	NetGateway_SendEmergencyLocationRequestL(mtlrSecondSessionId, TLbsNetworkEnumInt::EServiceMobileTerminated, iSecondRequestAgpsMode0, iSecondRequestAgpsMode1);
	
	if (iReasonInRespLocReq != KErrNone)
		{
		// the second request was rejected by the NRH
		}
	else
		{
		// now, LBS has autonomously completed a pseudo privacy request cycle
		// and that always results in a power status being sent
		// to the agps manager on the P&S internal interface.
		// The location request may, or may not overwrite the status
		// and therefore the test may, receive the status followed by the location request 
		// or just the location request. Both messages should have the correct status.
		// jcm AgpsManager_WaitLocationRequestOrRequestAndStatusL(iSecondAgpsModeToAgpsMan0, iSecondAgpsModeToAgpsMan1);
		
		AgpsManager_WaitLocationRequestL(iSecondAgpsModeToAgpsMan0, iSecondAgpsModeToAgpsMan1);
		}
	
	if (iReasonInRespLocReq != KErrNone)
		{ 
		// and the error is sent immedialely to the NG
		NetGateway_WaitLocationResponseL(mtlrSecondSessionId, iReasonInRespLocReq);
		}
	
	// AGPS positions are broadcast to all the sessions in the NRH
	AgpsManager_SendGpsPositionUpdateAccurateL();

	NetGateway_WaitLocationResponseL(mtlrSessionId, KErrNone);	
	
	if (iReasonInRespLocReq != KErrNone)
		{
		// the error has already been sent out to the NG
		}
	else
		{
		if ((iSecondRequestAgpsMode0 == KTerminalAssistedMode) && (iSecondRequestAgpsMode1 == KErrNotFound))
			{
			// if the second session is in TAP mode the AGPS position is not sent to the NG
			}
		else
			{
			NetGateway_WaitLocationResponseL(mtlrSecondSessionId, iReasonInRespLocReq);
			}
		}
	
	// we should always get this callback for the first session
	PrivacyController_WaitPrivacyGpsPositionL(mtlrSessionId);
	
	if (iReasonInRespLocReq != KErrNone)
		{
		// privacy controller not involved here because no position is returned to network for
		// the second session - just an error
		}
	else
		{
		if ((iSecondRequestAgpsMode0 == KTerminalAssistedMode) && (iSecondRequestAgpsMode1 == KErrNotFound))
			{
			// if the second session is in TAP mode the AGPS position is not sent to the NG
			// and it is not sent to privacy controller
			}
		else
			{
			PrivacyController_WaitPrivacyGpsPositionL(mtlrSecondSessionId);
			}
		}
	
	NetGateway_SendSessionCompleteL(mtlrSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSessionId, KErrNone);

	NetGateway_SendSessionCompleteL(mtlrSecondSessionId, KErrNone, TLbsNetworkEnumInt::EServiceMobileTerminated);

	PrivacyController_WaitPrivacyCompleteL(mtlrSecondSessionId, KErrNone);
		
	INFO_PRINTF1(_L("Done"));

	}


