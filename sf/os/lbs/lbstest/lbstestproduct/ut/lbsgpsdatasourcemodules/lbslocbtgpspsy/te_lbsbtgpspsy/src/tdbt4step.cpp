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
// Example CTestStep derived implementation
//



/**
 @file TDBT4Step.cpp
 @internalTechnology
*/
#include "TDBT4Step.h"
#include <centralrepository.h>
#include <BtGpsPsyPrivateCRKeys.h>
#include <BtGpsPsyPrivatePSKeys.h>
#include <e32property.h>

CTDBT4Step::~CTDBT4Step()
/**
 * Destructor
 */
	{
	}

CTDBT4Step::CTDBT4Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KTDBT4Step);
	}

TVerdict CTDBT4Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	CTe_LbsBtGpsPsyStepBase::doTestStepPreambleL();
	if (TestStepResult()!=EPass)
	    return   TestStepResult();
	// process some pre setting to this test step then set SetTestStepResult to EFail or Epass.
	SetTestStepResult(EPass);
	return TestStepResult();
	}


TVerdict CTDBT4Step::doTestStepL()
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
		if(!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteInt,test)
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


TVerdict CTDBT4Step::doTestStepPostambleL()
/**
 * @return - TVerdict code
 * Override of base class virtual
 */
	{
	return TestStepResult();
	}


// ---------------------------------------------------------
// CTDBT4Step::StartL
// (other items were commented in a header).
// ---------------------------------------------------------
//       
void CTDBT4Step::StartL(TInt aIndex)
    {
    switch(aIndex)
        {
        case 0:
        	_LIT(KIndexName0, "**** TCN1001 - Checking Dialog Settings ****");
        	INFO_PRINTF1(KIndexName0);
            Sub0TCB1001PSYInfoAndPositionRequestingL();
            break;
        case 3:
        	_LIT(KIndexName3, "**** TCN1004 - Check Events API ****");
        	INFO_PRINTF1(KIndexName3);
            CheckEventsApiL();
            break;
        case 4:
        	_LIT(KIndexName4, "**** TCN1005 - Night mode ****");
        	INFO_PRINTF1(KIndexName4);
            CheckNightModeSettingsL();
            break;
        }
    }


void CTDBT4Step::Sub0TCB1001PSYInfoAndPositionRequestingL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase0L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    User::After( 10*1000*1000 );
   
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "BT GPS PSY module info check and position requests case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
	_LIT(KIndexName0, "Ended: **** TCN1001 - Checking Dialog Settings ****");
	INFO_PRINTF1(KIndexName0);
    }

void CTDBT4Step::RunSubCase0L( CLcfCommonProcedures* /*aCommon*/ )
    {
    //todo: clean all the global notes reference
    /*
    
    
    iUserInfo->ShowDialog(_L("Launch location reference application at background, and wait for fixes coming. Then click OK"),
        EUtfwDialogTypeOk, 
        ETrue);
    
    iUserInfo->ShowDialog(_L("Now plug and unplug charger of Nokia BT GPS"),
        EUtfwDialogTypeOk, 
        ETrue);
    
    TUtfwUserAnswer ans = iUserInfo->ShowDialog(_L("Did you see BT GPS external power connected/unconnected note?"),
        EUtfwDialogTypeYesNo,
        ETrue);
        
    AssertTrueSecL(ans==EUtfwUserAnswerNo, _L("Dialog still shown when it's turned off"), KErrGeneral);
    
    iUserInfo->ShowDialog(_L("Now plug and unplug external antenna of Nokia BT GPS"),
        EUtfwDialogTypeOk, 
        ETrue);
    
    ans = iUserInfo->ShowDialog(_L("Did you see BT GPS external antenna connected/unconnected note?"),
        EUtfwDialogTypeYesNo,
        ETrue);
        
    AssertTrueSecL(ans==EUtfwUserAnswerNo, _L("Dialog still shown when it's turned off"), KErrGeneral);
    
    //Enable dialogs setting
    EnableDialogsL();
    
    iUserInfo->ShowDialog(_L("Now plug and unplug charger of Nokia BT GPS"),
        EUtfwDialogTypeOk, 
        ETrue);
    
    ans = iUserInfo->ShowDialog(_L("Did you see BT GPS external power connected/unconnected note?"),
        EUtfwDialogTypeYesNo,
        ETrue);
        
    AssertTrueSecL(ans==EUtfwUserAnswerYes, _L("Dialogs not shown when it's turned on"), KErrGeneral);
    
    iUserInfo->ShowDialog(_L("Now plug and unplug external antenna of Nokia BT GPS"),
        EUtfwDialogTypeOk, 
        ETrue);
    
    ans = iUserInfo->ShowDialog(_L("Did you see BT GPS external antenna connected/unconnected note?"),
        EUtfwDialogTypeYesNo,
        ETrue);
        
    AssertTrueSecL(ans==EUtfwUserAnswerYes, _L("Dialogs not shown when it's turned on"), KErrGeneral);
    */
    }


void CTDBT4Step::CheckEventsApiL()
    {
    TInt err = KErrNone;
    
    INFO_PRINTF1(KStartTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    CLcfCommonProcedures* common = CLcfCommonProcedures::NewL();
    CleanupStack::PushL(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    TRAP( err, RunSubCase6L( common ) );
    
    //iUserInfo->ShowDialog( _L( "Check BT logo dissappears and after that press OK" ), EUtfwDialogTypeOk, EFalse );
    //User::After( 10*1000*1000 );
   
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStartTxt);
    
    CleanupStack::PopAndDestroy(common);
    
    INFO_PRINTF1(KNewlineTxt);
    
    INFO_PRINTF1(KEnvCleaningStopTxt);
    
    INFO_PRINTF1(KNewlineTxt);
    
    _LIT( KErrBTGPSPSYModInfoAndPosReq, "TCSA1001 and TCSA1002 case failed with code %d" );
    AssertTrueSecL( err == KErrNone, KErrBTGPSPSYModInfoAndPosReq, err );
	_LIT(KIndexName3, "Ended: **** TCN1004 - Check Events API ****");
	INFO_PRINTF1(KIndexName3);
    }

void CTDBT4Step::RunSubCase6L( CLcfCommonProcedures* /*aCommon*/ )
    {
    RProperty events;
    //1. Check PSY State
    INFO_PRINTF1(_L("Start checking PSY State"));
    TInt psyState;
    
    //Psy unloaded
    if(events.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState, psyState)==KErrNone)
        {
        AssertTrueSecL(
            psyState==EPSYNotLoaded, 
            _L("Expected PSY not loaded, but psy state is %d"), 
            psyState);
        }
    
   //todo: clean all the global notes reference iUserInfo->ShowDialog(_L("Delete all pairing first. Connect to a Non-Nokia BT GPS device in next test"), EUtfwDialogTypeOk, ETrue);

    //load the psy
    RPositionServer posServer;
    TInt err = posServer.Connect();
    CleanupClosePushL(posServer);
    AssertTrueSecL( err == KErrNone, _L( "MLFW server could not be opened, code %d" ), err );
    RPositioner bTGpsPsy;
    err = bTGpsPsy.Open( posServer, KbtGPSPsyId );
    AssertTrueSecL( err == KErrNone, _L( "BTGPSPY could not be opened, code %d" ), err );
    CleanupClosePushL(bTGpsPsy);

    //Check if PSY is loaded
    User::LeaveIfError(events.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState, psyState));
    AssertTrueSecL(psyState==EPSYLoaded, _L("Expected PSY loaded, but psy state is %d"), psyState);
    
    //Set update option and requestor
    TPositionUpdateOptions  updateops(5000000);//5seconds
    err = bTGpsPsy.SetUpdateOptions(updateops);
    AssertTrueSecL( err == KErrNone  , _L( "Update options failed with %d" ), err);
    err = bTGpsPsy.SetRequestor( CRequestor::ERequestorService , CRequestor::EFormatApplication , _L("BTGPSPSY Tester4") );
    AssertTrueSecL( err == KErrNone, _L( "Could not set requestor, code %d" ), err );
    
    //Make location request
    TRequestStatus status = KRequestPending;
    TPositionInfo posInfo;
    bTGpsPsy.NotifyPositionUpdate( posInfo, status );
    User::WaitForRequest( status ); 
    
    //Check Non-Nokia BT GPS is used.
    User::LeaveIfError(events.Get(KPsUidBluetoothGpsPsy, KBluetoothGpsPsyState, psyState));
    AssertTrueSecL(psyState==EPSYLoadedAndPNOKNotSupportedBTGPSUsed, _L("Expected Non-Nokia GPS PSY connected, but psy state is %d"), psyState);

    CleanupStack::PopAndDestroy(&bTGpsPsy);
    CleanupStack::PopAndDestroy(&posServer); 
    
    }


void CTDBT4Step::SetNightModeL(TBool aMode)
    {
    if(aMode)
        {
        INFO_PRINTF1(_L("Set night mode on"));
        }
    else
        {
        INFO_PRINTF1(_L("Set night mode off"));
        }
            
            
    CRepository* settings = CRepository::NewLC(KCRUidBtGpsPsy);
    if(aMode)
        {
        User::LeaveIfError(settings->Set(KBluetoothGpsPsyNightModeState, 1));
        }
    else
        {
        User::LeaveIfError(settings->Set(KBluetoothGpsPsyNightModeState, 0));
        }
    CleanupStack::PopAndDestroy(settings);
    INFO_PRINTF1(_L("Dialog disabled"));
    }

void CTDBT4Step::CheckNightModeSettingsL()
    {
    //todo: clean all the global notes reference
    /*
    iUserInfo->ShowDialog(_L("Launch Location Reference Application and connect to a Nokia BT GPS"), EUtfwDialogTypeOk, ETrue);
    
    SetNightModeL(ETrue);
    User::After(K1Second);
    if(iUserInfo->ShowDialog(_L("Is the BT GPS in night mode?"), EUtfwDialogTypeYesNo, ETrue) != EUtfwUserAnswerYes)
        {
        INFO_PRINTF1(_L("GPS is not in night mode when night mode is set on from settings API"));
        User::Leave(KErrGeneral);
        }
        
    iUserInfo->ShowDialog(_L("Exit Location Reference Application and start it again "), EUtfwDialogTypeOk, ETrue);

    if(iUserInfo->ShowDialog(_L("Is the BT GPS still in night mode?"), EUtfwDialogTypeYesNo, ETrue) != EUtfwUserAnswerYes)
        {
        INFO_PRINTF1(_L("GPS is not in night mode when night mode is set on from settings API"));
        User::Leave(KErrGeneral);
        }
    
    SetNightModeL(EFalse);
    
    User::After(K1Second);
    
    if(iUserInfo->ShowDialog(_L("Is the BT GPS in night mode?"), EUtfwDialogTypeYesNo, ETrue) == EUtfwUserAnswerYes)
        {
        INFO_PRINTF1(_L("GPS is  in night mode when night mode is set off from settings API"));
        User::Leave(KErrGeneral);
        }
        */
	_LIT(KIndexName4, "Ended: **** TCN1005 - Night mode ****");
	INFO_PRINTF1(KIndexName4);
    }


// End of File
