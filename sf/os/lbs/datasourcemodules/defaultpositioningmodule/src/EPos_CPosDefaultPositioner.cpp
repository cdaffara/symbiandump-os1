// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include <lbscommon.h>
#include <lbspositioninfo.h>
#include <f32file.h>

#include "epos_defaultproxycommon.h"
#include "EPos_CPosDefaultPositioner.h"
#include "EPos_CPosRequestController.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosDefaultPositioner::CPosDefaultPositioner()
    {
    }

// EPOC default constructor can leave.
void CPosDefaultPositioner::ConstructL(TAny* aConstructionParameters)
    {
    TRACESTRING( "CPosDefaultPositioner::ConstructL start... " )
    
    CProxyPositioner::BaseConstructL(aConstructionParameters);
    iRequestController = CPosRequestController::NewL(
        *this,
        ModuleSettingsManager(),
        ModuleStatusManager() );
        
    TRACESTRING( "CPosDefaultPositioner::ConstructL end " )
    }

// Two-phased constructor.
CPosDefaultPositioner* CPosDefaultPositioner::NewL(TAny* aConstructionParameters)
    {
    CPosDefaultPositioner* self = new (ELeave) CPosDefaultPositioner;
    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosDefaultPositioner::~CPosDefaultPositioner()
    {
    delete iRequestController;
    }

// ---------------------------------------------------------
// CPosDefaultPositioner::NotifyPositionUpdate
// ---------------------------------------------------------
//
void CPosDefaultPositioner::NotifyPositionUpdate(
    TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
    {
    iRequestController->NotifyPositionUpdate(aPosInfo, aStatus);
    }

// ---------------------------------------------------------
// CPosDefaultPositioner::CancelNotifyPositionUpdate(TInt aError)
// ---------------------------------------------------------
//
void CPosDefaultPositioner::CancelNotifyPositionUpdate(TInt aError)
    {
    iRequestController->CancelNotifyPositionUpdate(aError);
    }

// ---------------------------------------------------------
// CPosDefaultPositioner::CancelNotifyPositionUpdate
// ---------------------------------------------------------
//
void CPosDefaultPositioner::CancelNotifyPositionUpdate()
    {
    iRequestController->CancelNotifyPositionUpdate();
    }

// ---------------------------------------------------------
// CPosDefaultPositioner::TrackingOverridden
// ---------------------------------------------------------
//
TBool CPosDefaultPositioner::TrackingOverridden() const
    {
    return ETrue;
    }

// ---------------------------------------------------------
// CPosDefaultPositioner::StartTrackingL
// ---------------------------------------------------------
//
void CPosDefaultPositioner::StartTrackingL(
    const TTimeIntervalMicroSeconds& aInterval)
    {
    iRequestController->StartTracking(aInterval);
    
    iTrackingInterval = aInterval;
    }

// ---------------------------------------------------------
// CPosDefaultPositioner::StopTracking
// ---------------------------------------------------------
//
void CPosDefaultPositioner::StopTracking()
    {
    iRequestController->StopTracking();

    iTrackingInterval = TTimeIntervalMicroSeconds(0);
    }

// ---------------------------------------------------------
// CPosDefaultPositioner::TrackingInterval
// ---------------------------------------------------------
//
TTimeIntervalMicroSeconds CPosDefaultPositioner::TrackingInterval() const
    {
    return iTrackingInterval;
    }

//  End of File
