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
//

#include "Te_SimPacketTestStepBase.h"

TVerdict CSimPacketTestStepBase::doTestStepPreambleL()
	{
	__UHEAP_MARK;

	TInt ret = iTelServer.Connect();
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to connect to telephony server"));
		User::Leave(ret);
		}
   	ret=iTelServer.LoadPhoneModule(KSimtsyName);
	if (ret!=KErrNone)
		{
		INFO_PRINTF1(_L("Failed to load phone module"));
		iTelServer.Close();
		User::Leave(ret);
		}

	return TestStepResult();
	}

TVerdict CSimPacketTestStepBase::doTestStepPostambleL()
	{
	iTelServer.UnloadPhoneModule(KSimtsyName);
	iTelServer.Close();

	__UHEAP_MARKEND;
	return TestStepResult();
	}

void CSimPacketTestStepBase::CleanUp(TAny *aPtr)
/**
*  Calls iLine.Close() & iCall.Close in  leave case.
* @param this poiter pass cleanup stack when calls the function
*/
	{
	CSimPacketTestStepBase* ptr = ((CSimPacketTestStepBase*)aPtr) ;
	//3 lines to be sure 
	ptr->iPacketService.Close() ;
	ptr->iPacketContext[0].Close() ;
	ptr->iPacketContext[1].Close() ;
	ptr->iPhone.Close() ;
	}

CSimPacketTestStepBase::operator TCleanupItem()
/**
* Makes TCleanupItem from current instance of CTestSimPacketStep
*/
	{
	return TCleanupItem(CSimPacketTestStepBase::CleanUp,this);
    }


void CSimPacketTestStepBase::TestConfigParamsL(RPacketQoS::TQoSGPRSNegotiated& aArg1, RPacketQoS::TQoSGPRSNegotiated& aArg2)
/**
 *  This tests that the arguments are the same.
 */

    {
    TESTL(aArg1.iPrecedence == aArg2.iPrecedence);
	TESTL(aArg1.iDelay == aArg2.iDelay);
	TESTL(aArg1.iReliability == aArg2.iReliability);
	TESTL(aArg1.iPeakThroughput == aArg2.iPeakThroughput);
	TESTL(aArg1.iMeanThroughput == aArg2.iMeanThroughput);
    }

void CSimPacketTestStepBase::TestConfigParamsL(RPacketContext::TContextConfigGPRS& aArg1, RPacketContext::TContextConfigGPRS& aArg2)
	{
	TESTL(aArg1.iPdpType                     == aArg2.iPdpType);
	TESTL(aArg1.iAccessPointName             == aArg2.iAccessPointName );
	TESTL(aArg1.iPdpCompression              == aArg2.iPdpCompression);
	TESTL(aArg1.iPdpAddress                  == aArg2.iPdpAddress);
	TESTL(aArg1.iAnonymousAccessReqd         == aArg2.iAnonymousAccessReqd);
	TESTL(aArg1.iUseEdge			== aArg2.iUseEdge);

	}

void CSimPacketTestStepBase::SetSimtsyTestNumberL(TInt aTestNumber)
/**
* Sets the test number which is to be executed from the config file
*/
  	{
  	INFO_PRINTF2(_L("Changing the Test Number to %d"),aTestNumber);		
	TInt ret = RProperty::Set(KUidPSSimTsyCategory, KPSSimTsyTestNumber, aTestNumber);
	TEST(ret == KErrNone);
	}

void CSimPacketTestStepBase::WaitWithTimeout(TRequestStatus& aStatus, TInt aNumberOfMicroSeconds)
/**
 * Timeout function
 */
	{
	TRequestStatus timerStatus;
	RTimer timer ;
	timer.CreateLocal() ;
	timer.After(timerStatus,aNumberOfMicroSeconds);

	User::WaitForRequest(aStatus, timerStatus);
	if (timerStatus == KRequestPending)
		{
		timer.Cancel();
		User::WaitForRequest(timerStatus);
		}
	else
		{
		INFO_PRINTF1(_L("Time is over!!!")) ;
		}
	timer.Close() ;
	}
