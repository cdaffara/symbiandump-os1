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

#include "HWRMChargingStatusObserver.h"
#include "HWRMtrace.h"
#include "HWRMPower.h"
#include <hwrmpowerstatesdkpskeys.h>

// -----------------------------------------------------------------------------
// CHWRMChargingStatusObs::CHWRMChargingStatusObs
// 
// -----------------------------------------------------------------------------
//

    
CHWRMChargingStatusObs::CHWRMChargingStatusObs(MHWRMBatteryChargingStatusObserver* aChargingStatusCallback)
    :iChargingPsObserver(NULL),
    iChargingStatusObserver(aChargingStatusCallback),
    //Charging status is initialised to unknown value
    iChargingStatus(KErrUnknown)
    {
        
    }

CHWRMChargingStatusObs::~CHWRMChargingStatusObs()
    {

        delete iChargingPsObserver;
        iChargingPsObserver = NULL;
        
        COMPONENT_TRACE1(_L( "HWRM CHWRMChargingStatusObs - ~CHWRMChargingStatusObs deleted iChargingPsObserver"));
    }


CHWRMChargingStatusObs* CHWRMChargingStatusObs::NewL(MHWRMBatteryChargingStatusObserver* aChargingStatusCallback)
    {

    COMPONENT_TRACE1(_L( "HWRM CHWRMChargingStatusObs - CHWRMChargingStatusObs::NewL()"));
    
    CHWRMChargingStatusObs* chargingStatusObs = new( ELeave ) CHWRMChargingStatusObs(aChargingStatusCallback);
    
    CleanupStack::PushL( chargingStatusObs );        
    chargingStatusObs->ConstructL();        
    CleanupStack::Pop(); 
    
    COMPONENT_TRACE1(_L( "HWRM CHWRMChargingStatusObs - CHWRMChargingStatusObs::NewL() - return"));
    
    return chargingStatusObs;
    }

void CHWRMChargingStatusObs::ConstructL()
    {
    COMPONENT_TRACE1(_L( "HWRM CHWRMChargingStatusObs - CHWRMChargingStatusObs::ConstructL()"));
    
    iChargingPsObserver = CPsPropertyObserver::NewL(KPSUidHWRMPowerState, KHWRMExtendedChargingStatus, *this);    
    
    COMPONENT_TRACE1(_L( "HWRM CHWRMChargingStatusObs - CHWRMChargingStatusObs::ConstructL() - return"));    
    }


void CHWRMChargingStatusObs::PsPropertyChanged(TInt aKey, TInt aValue, TInt aError)
    {
    if((aKey == KHWRMExtendedChargingStatus)&&(iChargingStatusObserver)&&
        (iChargingStatus != aValue))
        {
        iChargingStatus = aValue;
        
        iChargingStatusObserver->ChargingStatusChange(aError, 
                                (CHWRMPower::TBatteryChargingStatus)aValue);        
        }
    }

TInt CHWRMChargingStatusObs::Get(TInt& val)
    {
        return iChargingPsObserver->Get(val);
    }


//  End of File

