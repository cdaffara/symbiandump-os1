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
// This file contains the class to do the last connected bt device tests. Features 4 tests
// 0 - Connect to a bt device and return position. Ensure device type is as expected
// 1 - Connect silently to a bt device which was previously connected to, ensure test notifier unused.
// 2 - Connect to a bt device and return a position. Ensure test notifier used once.
// 3 - Connect to a bt device and return a position silently. Check test notifier unused and
// dont care about the device type.
//



/**
 @file tdbt_lastdevicestep.cpp
 @internalTechnology
*/

#include "tdbt_lastdevicestep.h"
#
//#include "LcfCommonProcedures.cpp"
#include <btmanclient.h>
#include <BtGpsPsyPrivateCRKeys.h>

#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
#include "lbsbtgpsconfiginternal.h"
#include "lbsbtgpsdevicerecord.h"
#endif

    
CTDBT_LastDeviceStep::~CTDBT_LastDeviceStep()
/**
 * Destructor
 */
	{
	}

CTDBT_LastDeviceStep::CTDBT_LastDeviceStep()
/**
 * Constructor
 */
	{
	// **MUST** call SetTestStepName in the constructor as the controlling
	// framework uses the test step name immediately following construction to set
	// up the step's unique logging ID.
	SetTestStepName(KTDBT_LastDeviceStep);
	}


TVerdict CTDBT_LastDeviceStep::doTestStepL()
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
// CTDBT1Step::StartL
// (other items were commented in a header).
// ----------------------------------------------------------
//     
void CTDBT_LastDeviceStep::StartL(TInt aIndex)
    {
    switch(aIndex)
        {
        case 0:
        	_LIT(KIndexName0, "**** Connect to device, ensure last device is expected type ****");
            INFO_PRINTF1(KIndexName0);
            GetPositionL();
        	IsExpectedDeviceTypeL();
        	CheckTestNotifierStatusL(EBTTestNotifierUsed,EFalse);
            break;
        case 1:
        	_LIT(KIndexName1, "**** Connect to device silently, ensure test notifier unused ****");
            INFO_PRINTF1(KIndexName1);
            SetTestNotifierStatusL(EBTTestNotifierUnused);
            GetPositionL();
        	IsExpectedDeviceTypeL();
        	CheckTestNotifierStatusL(EBTTestNotifierUnused,ETrue); 
        	break;
        case 2:
        	_LIT(KIndexName2, "**** Check notifier used only once ****");
            INFO_PRINTF1(KIndexName2);
            SetTestNotifierStatusL(EBTTestNotifierUnused);
            GetPositionL();
        	CheckTestNotifierStatusL(EBTTestNotifierUsed,ETrue); 
        	break;
        case 3:
        	_LIT(KIndexName3, "**** Check notifier not used, don't care about device type ****");
            INFO_PRINTF1(KIndexName3);
            SetTestNotifierStatusL(EBTTestNotifierUnused);
            GetPositionL();
        	CheckTestNotifierStatusL(EBTTestNotifierUnused,ETrue); 
        	break;
        default:
        	User::Leave(KErrGeneral);
        }    
    }





// Try and get a position
void CTDBT_LastDeviceStep::GetPositionL()
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

    TPositionUpdateOptions updateOpts;
    // If we dont get a position from the bt device in 1 minute, we probably wont ever get one.
    updateOpts.SetUpdateTimeOut(60*1000000);
    BTGpsPsy.SetUpdateOptions(updateOpts);

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


// BTPSY available, Pairing has been done before - Connect to the device silently and get a position.
void CTDBT_LastDeviceStep::IsExpectedDeviceTypeL()
	{
    TInt deviceType = 0;
	
#ifdef SYMBIAN_LOCATION_BTGPSCONFIG
	// Bluetooth GPS PSY is using the new configuration API to store HW information

	CLbsBtGpsConfigInternal* config = CLbsBtGpsConfigInternal::NewL();
	CleanupStack::PushL(config);
	
	RPointerArray<TLbsBtGpsDeviceRecord> deviceList;
	config->GetDeviceRecordListL(deviceList);

	if (deviceList.Count() != 1)
		{
    	INFO_PRINTF2(_L("BT GPS Device list should contain one item, found %d"), deviceList.Count());
		SetTestStepResult(EFail);
		}
	else
		{
		TLbsBtGpsDeviceInfo::TLbsBtGpsDeviceType type = deviceList[0]->Type();
		switch (type)
			{
			case TLbsBtGpsDeviceInfo::ELbsBtGpsDeviceNonNokGps:
				deviceType = 1;
				break;
			
			case TLbsBtGpsDeviceInfo::ELbsBtGpsDeviceNokGps:
				deviceType = 2;
				break;
				
			default:
				deviceType = 0;
				break;
			}
		}
		
	CleanupStack::PopAndDestroy(config);
	
#else
	// Bluetooth GPS PSY is using cenrep to store HW information  
    
    const TUid KCRUidBtGpsPsy = { 0x101FE999 };
    
    CRepository* rep = CRepository::NewL(KCRUidBtGpsPsy);
    CleanupStack::PushL(rep);
    TInt err = rep->Get(KBluetoothGpsPsyDeviceAddressValidity,deviceType);

    CleanupStack::PopAndDestroy(rep); // repository
#endif

    TInt expectedDeviceType = 0;
    if(!GetIntFromConfig(ConfigSection(),KTe_LbsBtGpsPsySuiteDeviceType,expectedDeviceType)
		)
		{
		// Leave if there's any error.
    	SetTestStepResult(EFail);
		User::Leave(KErrNotFound);
		}
    
    if(expectedDeviceType!=deviceType)
    	{
    	_LIT(KWrongDeviceType, "The last connected device is of the wrong type - Expected %d got %d");
    	INFO_PRINTF3(KWrongDeviceType,expectedDeviceType,deviceType);
    	SetTestStepResult(EFail);
    	}
  
	}

void CTDBT_LastDeviceStep::SetTestNotifierStatusL(TInt aNotifierState)
	{
	User::LeaveIfError(iBtGpsSim.SetParameter(EBtGpsSimDevNotifierUsage, aNotifierState));
	}

void CTDBT_LastDeviceStep::CheckTestNotifierStatusL(TInt aExpectedState,TBool aFailIfWrong)
	{
    TInt actualState;
	TInt err = iBtGpsSim.GetParameter(EBtGpsSimDevNotifierUsage, actualState);
	if (actualState == 0)
		{
		actualState = EBTTestNotifierUnused;
		}
	else
		{
		actualState = EBTTestNotifierUsed;
		}

    if(err!=KErrNone)
    	{
    	_LIT(KRetrievalError, "There was an error getting notifer usage information: %d");
    	INFO_PRINTF2(KRetrievalError,err);
    	
    	}
	_LIT(KNotiferStatusInfo, "The notifier status was %d and the expected status was %d");
	INFO_PRINTF3(KNotiferStatusInfo,actualState,aExpectedState);
	
    if((actualState!=aExpectedState)&&(aFailIfWrong))
    	{
    	SetTestStepResult(EFail);
    	}
	}
