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
// @file ctlbsx3pStep_Neg.cpp
// This is the class implementation for the LBS Client Test Step Base
// 
//

#include "ctlbsx3pstepnegative.h"

#include <lbs/test/tlbsutils.h>
#include <lbs/test/ctlbsasyncwaiter.h>


#include <lbs.h>
#include <lbs/lbsx3p.h>


//_LIT(KLbsRefPos, "RefPos");


	
/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all initialisation common to derived classes in here.
 * Make it being able to leave if there are any errors here as there's no point in
 * trying to run a test step if anything fails.
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsX3PStep_Negative::doTestStepPreambleL()
	{
	// Process some common pre setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep_Negative::doTestStepPreambleL()"));
	SetTestStepResult(EPass);

	return TestStepResult();
	}


/**
 * @return - TVerdict
 * Implementation of CTestStep base class virtual
 * It is used for doing all after test treatment common to derived classes in here.
 * Make it being able to leave
 * The leave will be picked up by the framework.
 */
TVerdict CT_LbsX3PStep_Negative::doTestStepPostambleL()
	{
	// Process some common post setting to test steps then set SetTestStepResult to EFail or Epass.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep_Negative::doTestStepPostabmleL()"));

	//SetTestStepResult(EPass);  // or EFail
	return TestStepResult();
	}


CT_LbsX3PStep_Negative::~CT_LbsX3PStep_Negative()
	{
	}


CT_LbsX3PStep_Negative::CT_LbsX3PStep_Negative(CT_LbsX3PServer& aParent) : iParent(aParent)
	{
		SetTestStepName(KLbsX3P_Step_Negative);
	}
	
	
	/**
Static Constructor
*/
CT_LbsX3PStep_Negative* CT_LbsX3PStep_Negative::New(CT_LbsX3PServer& aParent)
	{
	CT_LbsX3PStep_Negative* testStep = new CT_LbsX3PStep_Negative(aParent);
	// Note the lack of ELeave.
	// This means that having insufficient memory will return NULL;	

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
	
	

void CT_LbsX3PStep_Negative::ConstructL()
	{

	}




// Need to implement these to actually do a test.
TVerdict CT_LbsX3PStep_Negative::doTestStepL()
	{
		// Generic test step used to test the LBS Client Notify position update API.
	INFO_PRINTF1(_L("&gt;&gt;CT_LbsX3PStep_Transmit::doTestStepL()"));

	if (TestStepResult() == EPass)
		{
			if (GetIntFromConfig(ConfigSection(), KTestCaseId, iTestCaseId))
			{
			switch (iTestCaseId)
				{
				
				// NEGATIVE TEST
				// Test case LBS-X3P-Options-0002 	Attempt to set an invalid timeout
				case 02:
					{
					TLbsTransmitPositionOptions transmitOptions;
					transmitOptions.SetTimeOut(-1);
					
				

					}
					break;
					
					
				// NEGATIVE TEST
				// Test case LBS-X3P-Transmit-0031
				case 131:
				{
					// Test case LBS-X3P-Transmit-0031	Request X3P when we have no server connection
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Test will probably panic before we can verify callback parameters.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone
					
					RLbsTransmitPositionServer testServer;
					RLbsTransmitPosition testPositionTransmitter;					

					testPositionTransmitter.Open(testServer);
	


					}
					break;
								
				// NEGATIVE TEST
				// Test case LBS-X3P-Transmit-0032
				case 132:
					{
					// Test case LBS-X3P-Transmit-0032	Request X3P when we have no subsession connection
					//
					// External step:	Inform module of required single position to be transmitted.
					// Local step:		Do X3P.
					// Local callbacks:	Test will probably panic before we can verify callback parameters.
					//					NotifyMeasurementReportLocation callback, aPosition should match iVerifyPosInfoArr.
					//					TransmitPosition callback, aStatus should be KErrNone

					//	1. Create a sub-session, but don't open it
					RLbsTransmitPositionServer testServer;
									
					User::LeaveIfError(testServer.Connect()); 
					CleanupClosePushL(testServer);

					//	4. RLbsTransmitPosition sub-session
					RLbsTransmitPosition testPositionTransmitter;
					CleanupClosePushL(testPositionTransmitter);
				
					// Do transmit location.
					TPositionInfo* 	pPosInfo;
					TPositionInfo	positionInfo;
					pPosInfo = &positionInfo;
					CleanupStack::PushL(pPosInfo);
			
				    TBool bRefPos;
				    bRefPos = GetIntFromConfig(ConfigSection(), KLbsRefPos, bRefPos);
				    
					// Kick off transmit position.
					TRequestStatus status = 1;
					if(bRefPos)
						testPositionTransmitter.TransmitPosition(KBadDestination, KBadPriority, status, *pPosInfo, status, *pPosInfo);
					else
						testPositionTransmitter.TransmitPosition(KBadDestination, KBadPriority, status, *pPosInfo);
				
					
					CleanupStack::Pop(pPosInfo);
					}
					break;
								
				default:
					{
					User::Panic(KLbsX3P_Step_Negative, KErrUnknown);
					}
					
				}
			}
		}
		
	return TestStepResult();	
	}



