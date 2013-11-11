// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//


// INCLUDE FILES
#include "HWRMBatteryMeasurementsAO.h"
#include "HWRMBatteryPowerMeasurementsAO.h"
#include "HWRMtrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMBatteryPowerMeasurementsAO::CHWRMBatteryPowerMeasurementsAO
// C++ constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHWRMBatteryPowerMeasurementsAO::CHWRMBatteryPowerMeasurementsAO(MHWRMBatteryPowerObserver* aCallback, RHWRMClient& aClient)
    : CHWRMBatteryMeasurementsAttributeAO<CHWRMPower::TBatteryPowerMeasurementData>(aClient), iCallback(aCallback)
    {        
    CActiveScheduler::Add(this);        
    }

// -----------------------------------------------------------------------------
// CHWRMBatteryPowerMeasurementsAO::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryPowerMeasurementsAO* CHWRMBatteryPowerMeasurementsAO::NewL(MHWRMBatteryPowerObserver* aCallback, RHWRMClient& aClient)
    {

    CHWRMBatteryPowerMeasurementsAO* newInstance = new (ELeave) CHWRMBatteryPowerMeasurementsAO(aCallback, aClient);

	CleanupStack::PushL( newInstance );
	
	newInstance->ConstructL();

	CleanupStack::Pop();

	return newInstance;
    }


// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryPowerMeasurementsAO::~CHWRMBatteryPowerMeasurementsAO()
    {
    Cancel();
    }

void CHWRMBatteryPowerMeasurementsAO::DoCancel()
    {    
    	SendCancel(EHWRMCancelAveragePowerReporting);
    }
// -----------------------------------------------------------------------------
// CHWRMBatteryVoltageMeasurementsObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHWRMBatteryPowerMeasurementsAO::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CHWRMBatteryVoltageMeasurementsAO::RunL
// invoked when request issued to hwrm server in AsynchGetBatch
// completes
// -----------------------------------------------------------------------------
//
void CHWRMBatteryPowerMeasurementsAO::RunL()
    {    
    TInt err = iStatus.Int();

    CHWRMPower::TBatteryPowerMeasurementData errData;
    errData.iAverageVoltage = 0;
    errData.iAverageCurrent = 0;

    if( (KErrNone != err) && (KErrTimedOut != err) )
    	{
    	//if there is an error and it is not a timed out error 
    	iCallback->PowerMeasurement(err, errData); 
    	return;
    	}

    iCount = iCountPckg();
    
    //dispatch results received from server         
    TInt index = 0;
    while(index < iCount)
    	{   
        iCallback->PowerMeasurement(KErrNone, iResultsArray[index]); 
        ++index;
    	}

    iResultsArray.Reset();
    iCount = 0;
    
    // Re-order measurements only if reporting period has not expired on server
    if(KErrTimedOut != err)
    	{
    	//re-order measuremens batch from server.
    	GetAsyncBatch(EHWRMGetPowerMeasurements); 
    	}
    else
    	{
    	//it is a timed out error
    	iCallback->PowerMeasurement(err, errData); 
    	}
    }

void CHWRMBatteryPowerMeasurementsAO::SetCallBack(TAny* aCallBack)
    {
    iCallback = (MHWRMBatteryPowerObserver*)aCallBack;
    }


// End of File
