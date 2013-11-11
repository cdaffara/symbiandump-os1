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



// INCLUDE FILES
#include <e32base.h>
#include <lbs/epos_cpositioner.h>
#include "EPos_CPosSimulationPositioner.h"
#include "EPos_MPosParameterObserver.h"
#include "EPos_PositionerConstructParams.h"
#include "EPos_CPositionerExtension.h"
#include "EPos_CPosPsyExtension.h"

// ================= MEMBER FUNCTIONS =======================

// Static constructor.
EXPORT_C CPositioner* CPositioner::NewL(
    TAny* /*aConstructionParameters*/)
    {
    return NULL;
    }
    
// Static constructor.
EXPORT_C CPositioner* CPositioner::NewL(
    TUid aImplementationUid,
    const CPositioner& aPositioner)
    {
    TPositionerConstructParams params;

    params.iImplementationUid = aImplementationUid;
    params.iParamObserver = aPositioner.iExtension->iParamObserver;
    params.iStatusObserver = 
        aPositioner.iExtension->iPsyExtension->iStatusObserver;

    return NewL(&params);
    }
    
// Destructor
EXPORT_C CPositioner::~CPositioner()
    {
    delete iExtension;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
EXPORT_C CPositioner::CPositioner()
    {
    }

// EPOC default constructor can leave.
EXPORT_C void CPositioner::BaseConstructL(TAny* aConstructionParameters)
    {
    TPositionerConstructParams* params = 
        reinterpret_cast<TPositionerConstructParams*>(aConstructionParameters);

    iExtension = CPositionerExtension::NewL(
        params->iImplementationUid, 
        params->iStatusObserver,
        params->iParamObserver);
    }

// ---------------------------------------------------------
// CPositioner::TrackingOverridden
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool CPositioner::TrackingOverridden() const
    {
    return EFalse;
    }

// ---------------------------------------------------------
// CPositioner::StartTrackingL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPositioner::StartTrackingL(
    const TTimeIntervalMicroSeconds& /*aInterval*/)
    {
    User::Leave(KErrNotSupported);
    }

// ---------------------------------------------------------
// CPositioner::StopTracking
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPositioner::StopTracking()
    {
    }

// ---------------------------------------------------------
// CPositioner::ServiceL
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPositioner::ServiceL(RMessage2& /*aMessage*/) 
    {
    User::Leave(KErrNotSupported);
    }

// ---------------------------------------------------------
// CPositioner::ImplementationUid
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TUid CPositioner::ImplementationUid() const
    {
    return iExtension->iPsyExtension->iImplementationUid;
    }

// ---------------------------------------------------------
// CPositioner::GetMaxAge
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPositioner::GetMaxAge(TTime& aMaxAge) const
    {
    iExtension->iParamObserver->GetMaxAge(aMaxAge);
    }

// ---------------------------------------------------------
// CPositioner::IsPartialUpdateAllowed
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C TBool CPositioner::IsPartialUpdateAllowed() const
    {
    return iExtension->iParamObserver->IsPartialUpdateAllowed();
    }

// ---------------------------------------------------------
// CPositioner::ReportStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C MPositionerStatus* CPositioner::PositionerStatus()
    {
    if (iExtension)
        {
        return iExtension->iPsyExtension;
        }
    else 
        {
        return NULL;
        }
    }


/**
 * Cancels the request with the error code provided.
 * 
 * @param aError The error code to cancel request with.
 */
EXPORT_C void CPositioner::CancelNotifyPositionUpdate(TInt /*aError*/)
    {
    //Intentionally left blank. This function must be overriden in the derived class.
    }
    
// ---------------------------------------------------------
// CPositioner::CPositioner_Reserved2
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
EXPORT_C void CPositioner::CPositioner_Reserved2() 
    {
    }

//  End of File  
