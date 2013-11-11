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
#include "HWRMPower.h"
#include "HWRMtrace.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHWRMBatteryMeasurementsAO::CHWRMBatteryMeasurementsAO
// C++ Constructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryMeasurementsAO::CHWRMBatteryMeasurementsAO(RHWRMClient& aClient)
 	: CActive(EPriorityStandard),iCountPckg(iCount),
 	iClient(aClient) 
 	{} 
 	
// -----------------------------------------------------------------------------
// CHWRMBatteryMeasurementsAO::~CHWRMBatteryMeasurementsAO
// C++ Destructor
// -----------------------------------------------------------------------------
//
CHWRMBatteryMeasurementsAO::~CHWRMBatteryMeasurementsAO()
	{

	}


// -----------------------------------------------------------------------------
// CHWRMBatteryMeasurementsAO::SendCancel
// Used by the derived classes to to cancel  any outstanding GetMeasurements requests.
// -----------------------------------------------------------------------------
//
void CHWRMBatteryMeasurementsAO::SendCancel(TInt aCommand)
	{
	iClient.ExecuteOperation(aCommand, TIpcArgs());
	}

//-----------------------------------------------------------------------------
// Interface for changing the callback pointer.
// Needs to be implemented in derived class.
// Default implementation is empty.
//-----------------------------------------------------------------------------

void CHWRMBatteryMeasurementsAO::SetCallBack(TAny*)
    {
     
    }


#ifdef SYMBIAN_HWRM_EXTPOWERINFO
//-----------------------------------------------------------------------------
// If measurements are stopped by HWRM (because of charger disconnected,...)
// complete the callback with KErrCancel.
// Default implementation is empty.
//-----------------------------------------------------------------------------
 void CHWRMBatteryMeasurementsAO::CancelClientNotification()
    {
    
    }

#endif //SYMBIAN_HWRM_EXTPOWERINFO

// -----------------------------------------------------------------------------
// CHWRMBatteryMeasurementsAttributeAO::GetAsyncBatch
// Issue an asynchronous request to hwrm server to retrieve measurements.
// aCommand is either: EHWRMGetVoltageMeasurement or EHWRMGetConsumptionMeasurements.
// -----------------------------------------------------------------------------
//
template<typename T>
CHWRMBatteryMeasurementsAttributeAO<T>::CHWRMBatteryMeasurementsAttributeAO(RHWRMClient& aClient)
 	: CHWRMBatteryMeasurementsAO(aClient),
 	iMeasurementsArrayPckg(iResultsArray)
    {
    }


template<typename T>
void CHWRMBatteryMeasurementsAttributeAO<T>::GetAsyncBatch(TInt aCommand)
	{		 	 
	if (IsActive())
		{
		#ifdef _DEBUG
		_LIT(KHWRMBatteryMeasurementsPanic,"CHWRMBatteryMeasurementsAO");
		#endif		
		__ASSERT_DEBUG(!IsActive(), User::Panic(KHWRMBatteryMeasurementsPanic,KErrInUse));
		}
	else
		{		
		iCount = 20;//request up to 20 - check the number of items received
		iClient.ExecuteAsyncOperation(aCommand, TIpcArgs(&iMeasurementsArrayPckg, &iCountPckg), iStatus);	 
		SetActive();
		}
	}


//Explicit Instantiation.
//All new type of measurements needs to be instatiated here.
template class CHWRMBatteryMeasurementsAttributeAO<TInt>;
template class CHWRMBatteryMeasurementsAttributeAO<CHWRMPower::TBatteryPowerMeasurementData>;


// End of File
