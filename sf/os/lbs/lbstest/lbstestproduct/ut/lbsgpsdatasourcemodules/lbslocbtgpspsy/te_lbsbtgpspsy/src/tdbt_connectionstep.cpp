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
// This step contains tests to return a position, and to request a position but expect
// a cancel from the bt gps psy.
//



/**
 @file tdbt_connectionstep.cpp
 @internalTechnology
*/

#include "tdbt_connectionstep.h"
#
//#include "LcfCommonProcedures.cpp"
#include <btmanclient.h>
CTDBT_ConnectStep::~CTDBT_ConnectStep()
/**
 * Destructor
 */
	{
	}

CTDBT_ConnectStep::CTDBT_ConnectStep()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBT_ConnectStep);
	}


TVerdict CTDBT_ConnectStep::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{
	  if (TestStepResult()==EPass)
		{
		TInt test;
		if(	!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteInt,test)
			)
			{
			// Leave if there's any error.
			User::Leave(KErrNotFound);
			}
		SetTestStepResult(EPass);
		StartL(test);
		}
	  return TestStepResult();
	}


// ---------------------------------------------------------
// CTDBT_ConnectStep::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CTDBT_ConnectStep::StartL(TInt aIndex)
    {
    switch(aIndex)
        {
        case 0:
        	_LIT(KIndexName0, "**** Retrieve a position ****");
            INFO_PRINTF1(KIndexName0);
            GetPositionL();
            break;
        case 1:
        	_LIT(KIndexName1, "**** Attempt position request but expect it to fail ****");
            /*
             * This test tries to connect to a new device to which has an invalid address. Pairing to
             * this fails. The previous step 'SetSecondaryHwAddrStep' sets the flag which indicates that
             * the test notifier should just give up trying to find a new bt device.
             */
        	INFO_PRINTF1(KIndexName1);
        	ExpectPositionCancelL();
            break;
        }    
    }


// Try and get a position
void CTDBT_ConnectStep::GetPositionL()
	{
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    TPositionModuleInfo modInfo;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    RPositionServer posServer;
    
    TInt err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    INFO_PRINTF1(KGetPsyInformationTxt);
    err = posServer.GetModuleInfoById(KbtGPSPsyId, modInfo);    
    AssertTrueSecL( err == KErrNone, KPSYNotRetreivedTxt, err );

    CheckModuleInfoL( modInfo );
    
    RPositioner BTGpsPsy;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, _L( "Setting the requestor failed for BT GPS PSY, code %d" ), err );

    common->FlushEventsL();
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    User::WaitForRequest(status);
    
    if(status.Int()==KErrNone)
    	{
		_LIT(KPosReceived, "A position has been received" );
		INFO_PRINTF1(KPosReceived);
    	}
    else
    	{
    	_LIT(KNoPosReceived, "No position received error: %d");
    	INFO_PRINTF2(KNoPosReceived,status.Int());
    	SetTestStepResult(EFail);
    	}
    common->FlushEventsL();
    // if request failed
    AssertTrueSecL( status.Int() == KErrNone, KPositionRequestFailedTxt, status.Int() );
  
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    CleanupStack::PopAndDestroy(common);
	}


// Try and get a position
void CTDBT_ConnectStep::ExpectPositionCancelL()
	{
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    TPositionModuleInfo modInfo;
    
    TRequestStatus status = KRequestPending;
    TPositionInfo basic;
    RPositionServer posServer;
    
    TInt err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, KMLFWNotOpenTxt, err );
    
    INFO_PRINTF1(KGetPsyInformationTxt);
    err = posServer.GetModuleInfoById(KbtGPSPsyId, modInfo);    
    AssertTrueSecL( err == KErrNone, KPSYNotRetreivedTxt, err );

    CheckModuleInfoL( modInfo );
    
    RPositioner BTGpsPsy;
    
    INFO_PRINTF1(KOpenPSYTxt);
    err = BTGpsPsy.Open( posServer, KbtGPSPsyId );
    CleanupClosePushL(BTGpsPsy);
    AssertTrueSecL( err == KErrNone, KPSYNotOpenTxt, err );
    
    INFO_PRINTF1(KSetRequestorTxt);
    err = BTGpsPsy.SetRequestor( CRequestor::ERequestorService, CRequestor::EFormatApplication, KtdBT1 );
    AssertTrueSecL( err == KErrNone, _L( "Setting the requestor failed for BT GPS PSY, code %d" ), err );

    common->FlushEventsL();
    
    TPositionUpdateOptions updateOpts;
    updateOpts.SetUpdateTimeOut(60*1000000);
    BTGpsPsy.SetUpdateOptions(updateOpts);
    BTGpsPsy.NotifyPositionUpdate( basic, status );
    User::WaitForRequest(status);
    
    if(status.Int()==1)
    	{
		_LIT(KPosReceived, "A position has not been found (as expected)" );
		INFO_PRINTF1(KPosReceived);
    	}
    common->FlushEventsL();
    // if request failed
    AssertTrueSecL( status.Int() == 1, KPositionRequestFailedTxt, status.Int() );
    
    CleanupStack::PopAndDestroy(2, &posServer); // posServer, BTGpsPsy
    CleanupStack::PopAndDestroy(common);
	}
