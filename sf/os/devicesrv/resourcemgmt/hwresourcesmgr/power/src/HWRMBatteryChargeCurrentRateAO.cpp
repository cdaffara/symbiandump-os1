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
#include "HWRMBatteryChargeCurrentRateAO.h"
#include "HWRMtrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMBatteryChargeRateCurrentAO::CHWRMBatteryChargeRateCurrentAO
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMBatteryChargeRateCurrentAO::CHWRMBatteryChargeRateCurrentAO(MHWRMBatteryChargingCurrentObserver* aCallback, RHWRMClient& aClient)
    : CHWRMBatteryMeasurementsAttributeAO<TInt>(aClient), iCallback(aCallback)
    {        
    CActiveScheduler::Add(this);        
    }

// -----------------------------------------------------------------------------
// CHWRMBatteryVoltageMeasurementsObserver::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryChargeRateCurrentAO* CHWRMBatteryChargeRateCurrentAO::NewL(MHWRMBatteryChargingCurrentObserver* aCallback, RHWRMClient& aClient)
    {

    CHWRMBatteryChargeRateCurrentAO* newInstance = new (ELeave) CHWRMBatteryChargeRateCurrentAO(aCallback, aClient);

	CleanupStack::PushL( newInstance );
	
	newInstance->ConstructL();

	CleanupStack::Pop();

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryChargeRateCurrentAO::~CHWRMBatteryChargeRateCurrentAO()
    {
    Cancel();
    }

void CHWRMBatteryChargeRateCurrentAO::DoCancel()
    {    
    	SendCancel(EHWRMCancelAverageChargingCurrentReporting);
    }
// -----------------------------------------------------------------------------
// CHWRMBatteryChargeRateCurrentAO::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMBatteryChargeRateCurrentAO::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CHWRMBatteryChargeRateCurrentAO::RunL
// invoked when request issued to hwrm server in AsynchGetBatch
// completes
// -----------------------------------------------------------------------------
//
void CHWRMBatteryChargeRateCurrentAO::RunL()
    {    
    TInt err = iStatus.Int();

    if( (KErrNone != err) && (KErrTimedOut != err) )
    	{
    	//if there is an error and it is not a timed out error 
    	iCallback->AverageChargingCurrentChange(err, 0); 
    	return;
    	}

    iCount = iCountPckg();
    
    //dispatch results received from server         
    TUint index = 0;
    while(index < iCount)
    	{   
        iCallback->AverageChargingCurrentChange(KErrNone, iResultsArray[index]); 
        ++index;
    	}

    iResultsArray.Reset();
    iCount = 0;
    
    // Re-order measurements only if reporting period has not expired on server
    if(KErrTimedOut != err)
    	{
    	//re-order measuremens batch from server.
    	GetAsyncBatch(EHWRMGetAverageChargingCurrentMeasurements); 
    	}
    else
    	{
    	//it is a timed out error
    	iCallback->AverageChargingCurrentChange(err, 0); 
    	}
    }

void CHWRMBatteryChargeRateCurrentAO::CancelClientNotification()
    {
    iCallback->AverageChargingCurrentChange(KErrCancel, 0); 
    }

void CHWRMBatteryChargeRateCurrentAO::SetCallBack(TAny* aCallBack)
    {
    iCallback = (MHWRMBatteryChargingCurrentObserver*)aCallBack;
    }


// End of File
