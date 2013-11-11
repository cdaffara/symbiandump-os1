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

// INCLUDE FILES
#include "HWRMBatteryMeasurementsAO.h"
#include "HWRMBatteryChargeTimeMeasurementsAO.h"
#include "HWRMtrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// HWRMBatteryChargeTimeMeasurementsAO::HWRMBatteryChargeTimeMeasurementsAO
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMBatteryChargeTimeMeasurementsAO::CHWRMBatteryChargeTimeMeasurementsAO(MHWRMBatteryFullChargingTimeObserver* aCallback, RHWRMClient& aClient)
    : CHWRMBatteryMeasurementsAttributeAO<TInt>(aClient), iCallback(aCallback)
    {        
    CActiveScheduler::Add(this);        
    }

// -----------------------------------------------------------------------------
// HWRMBatteryChargeTimeMeasurementsAO::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryChargeTimeMeasurementsAO* CHWRMBatteryChargeTimeMeasurementsAO::NewL(MHWRMBatteryFullChargingTimeObserver* aCallback, RHWRMClient& aClient)
    {

    CHWRMBatteryChargeTimeMeasurementsAO* newInstance = new (ELeave) CHWRMBatteryChargeTimeMeasurementsAO(aCallback, aClient);

	CleanupStack::PushL( newInstance );
	
	newInstance->ConstructL();

	CleanupStack::Pop();

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryChargeTimeMeasurementsAO::~CHWRMBatteryChargeTimeMeasurementsAO()
    {
    Cancel();
    }

void CHWRMBatteryChargeTimeMeasurementsAO::DoCancel()
    {    
    	SendCancel(EHWRMCancelBatteryFullChargingTimeReporting);
    }
// -----------------------------------------------------------------------------
// HWRMBatteryChargeTimeMeasurementsAO::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMBatteryChargeTimeMeasurementsAO::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// HWRMBatteryChargeTimeMeasurementsAO::RunL
// invoked when request issued to hwrm server in AsynchGetBatch
// completes
// -----------------------------------------------------------------------------
//
void CHWRMBatteryChargeTimeMeasurementsAO::RunL()
    {    
    TInt err = iStatus.Int();

    if( (KErrNone != err) && (KErrTimedOut != err) )
    	{
    	//if there is an error and it is not a timed out error 
    	iCallback->BatteryFullChargingTimeChange(err, 0); 
    	return;
    	}

    iCount = iCountPckg();
    
    //dispatch results received from server         
    TUint index = 0;
    while(index < iCount)
    	{   
        iCallback->BatteryFullChargingTimeChange(KErrNone, iResultsArray[index]); 
        ++index;
    	}

    iResultsArray.Reset();
    iCount = 0;
    
    // Re-order measurements only if reporting period has not expired on server
    if(KErrTimedOut != err)
    	{
    	//re-order measuremens batch from server.
    	GetAsyncBatch(EHWRMGetBatteryFullChargingTimeMeasurements); 
    	}
    else
    	{
    	//it is a timed out error
    	iCallback->BatteryFullChargingTimeChange(err, 0); 
    	}
    }

void CHWRMBatteryChargeTimeMeasurementsAO::CancelClientNotification()
    {
    iCallback->BatteryFullChargingTimeChange(KErrCancel, 0); 
    }

void CHWRMBatteryChargeTimeMeasurementsAO::SetCallBack(TAny* aCallBack)
    {
    iCallback = (MHWRMBatteryFullChargingTimeObserver*)aCallBack;
    }


// End of File
