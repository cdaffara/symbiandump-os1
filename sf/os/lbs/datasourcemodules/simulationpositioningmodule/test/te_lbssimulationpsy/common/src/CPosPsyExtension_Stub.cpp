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
#include "EPos_CPosPsyExtension.h"
#include "EPos_MPosStatusObserver.h"

// ================= MEMBER FUNCTIONS =======================

// Static constructor.
CPosPsyExtension::CPosPsyExtension(
    TUid aImplementationUid,
    MPosStatusObserver* aStatusObserver)
:   iImplementationUid(aImplementationUid),
    iStatusObserver(aStatusObserver)
    {
    }
    
// Destructor
CPosPsyExtension::~CPosPsyExtension()
    {
    }

// ---------------------------------------------------------
// CPosPsyExtension::ReportStatus
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPsyExtension::ReportStatus(const TPositionModuleStatus& aStatus)
    {
    // Ignore if device disabled is reported
    TPositionModuleStatus::TDeviceStatus deviceStatus = aStatus.DeviceStatus();
    if (deviceStatus == TPositionModuleStatus::EDeviceDisabled)
        {
        return;
        }

    // Check that no device status value is out of range, if so ignore the status
    if ((deviceStatus != TPositionModuleStatus::EDeviceUnknown) &&
        (deviceStatus != TPositionModuleStatus::EDeviceError) &&
        (deviceStatus != TPositionModuleStatus::EDeviceInactive) &&
        (deviceStatus != TPositionModuleStatus::EDeviceInitialising) &&
        (deviceStatus != TPositionModuleStatus::EDeviceStandBy) &&
        (deviceStatus != TPositionModuleStatus::EDeviceReady) &&
        (deviceStatus != TPositionModuleStatus::EDeviceActive))
        {
        return;
        }

    // Check that no data quality status value is out of range, if so ignore the status
    TPositionModuleStatus::TDataQualityStatus dataQualityStatus = aStatus.DataQualityStatus();
    if ((dataQualityStatus != TPositionModuleStatus::EDataQualityUnknown) &&
        (dataQualityStatus != TPositionModuleStatus::EDataQualityLoss) &&
        (dataQualityStatus != TPositionModuleStatus::EDataQualityPartial) &&
        (dataQualityStatus != TPositionModuleStatus::EDataQualityNormal))
        {
        return;
        }

    iStatusObserver->ReportStatus(iImplementationUid, aStatus);
    }

// ---------------------------------------------------------
// CPosPsyExtension::RegisterPositioner
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosPsyExtension::RegisterPositioner()
    {
    iRefCount++;
    }

// ---------------------------------------------------------
// CPosPsyExtension::DeregisterPositioner
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosPsyExtension::DeregisterPositioner()
    {
    return --iRefCount;
    }

//  End of File  
