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
#include <lbs/epos_mpositionerstatus.h>
#include <lbssatellite.h>
#include "EPos_CPosSimulationPositioner.h"
#include "epossimulationpsy.hrh"
#include "EPos_CPosControllerManager.h"
#include "EPos_CPosControllerBase.h"
#include <e32debug.h>

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosSimulationPositioner::CPosSimulationPositioner() 
    {
	   
    }

// EPOC default constructor can leave.
void CPosSimulationPositioner::ConstructL(
    TAny* aConstructionParameters)
    {
    BaseConstructL(aConstructionParameters);
    
    iControllerManager = CPosControllerManager::InstanceL();

	TRAPD( err, iController = &iControllerManager->ControllerL(*this) );

    // Check if configuration file was not found, then report error
    if (err == KErrPathNotFound || err == KErrNotFound ||
        err == KErrCorrupt || err == KErrNotSupported)
        {
        ReportStatus(TPositionModuleStatus::EDeviceError);
        }
    User::LeaveIfError(err);

    // Device ready to retrieve position.
    ReportStatus(TPositionModuleStatus::EDeviceReady,
        TPositionModuleStatus::EDataQualityNormal);
    }

// Two-phased constructor.
CPosSimulationPositioner* CPosSimulationPositioner::NewL(
    TAny* aConstructionParameters)
    {
    CPosSimulationPositioner* self =
        new (ELeave) CPosSimulationPositioner();
    CleanupStack::PushL(self);
    self->ConstructL(aConstructionParameters);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosSimulationPositioner::~CPosSimulationPositioner()
    {
    ReportStatus(TPositionModuleStatus::EDeviceInactive,
        TPositionModuleStatus::EDataQualityUnknown);
    iControllerManager->ReleaseInstance(*this);
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::CompleteRequest
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosSimulationPositioner::CompleteRequest(
    TRequestStatus& aStatus,
    TInt aErrorCode)
    {
    // If a fix was obtained, save it
    if (aErrorCode >= 0 && iPosInfo)
    	{
    	//ASSERT(iPosInfo);
	    ASSERT(iPosInfo->PositionClassType() & EPositionInfoClass);
	    iControllerManager->SetLastFix( *static_cast<TPositionInfo*>(iPosInfo) );
	    iControllerManager->SetLastFixIsPartial( aErrorCode == KPositionPartialUpdate );
    	}
    
    TRequestStatus* status = &aStatus;
    User::RequestComplete(status, aErrorCode);
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::ReportStatus
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosSimulationPositioner::ReportStatus(
    TInt aDeviceStatus,
    TInt aDataQualityStatus)
    {
    if (aDeviceStatus != KErrNotFound)
        {
        SetDeviceStatus(aDeviceStatus);
        }
    if (aDataQualityStatus != KErrNotFound)
        {
        SetDataQualityStatus(aDataQualityStatus);
        }

    MPositionerStatus* reportTo = PositionerStatus();
    if (reportTo)
        {
        reportTo->ReportStatus(iModuleStatus);
        }
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::PartialUpdateAllowed
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TBool CPosSimulationPositioner::PartialUpdateAllowed() const
    {
    return IsPartialUpdateAllowed();
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::ClearAndSetDefaultPositionData
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosSimulationPositioner::ClearAndSetDefaultPositionData(
    TPositionInfoBase& aPosInfo)
    {
    if (aPosInfo.PositionClassType() & EPositionGenericInfoClass)
        {
        (void) new(&aPosInfo) TPositionInfo;
        HPositionGenericInfo& genInfo =
            static_cast<HPositionGenericInfo&> (aPosInfo);
        genInfo.ClearPositionData();
        }
    else if (aPosInfo.PositionClassType() & EPositionSatelliteInfoClass)
        {
        (void) new(&aPosInfo) TPositionSatelliteInfo;
        }
    else if (aPosInfo.PositionClassType() & EPositionCourseInfoClass)
        {
        (void) new(&aPosInfo) TPositionCourseInfo;
        }
    else // TPositionInfo
        {
        (void) new(&aPosInfo) TPositionInfo;
        }

    TUid implUid = { KPosSimulationPsyImplUid };
    aPosInfo.SetModuleId(implUid);
    aPosInfo.SetUpdateType(EPositionUpdateGeneral);
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::CanUseLastFix
// ----------------------------------------------------------------------------
//
TBool CPosSimulationPositioner::CanUseLastFix() const
	{
	TBool ret(EFalse);
    TTime maxAge(0);
    GetMaxAge(maxAge);
        
    // If client has set a maximum update age
    if (maxAge.Int64() > 0)
    	{
	    TPosition pos;
	    iControllerManager->GetLastFix().GetPosition(pos);
	    
	    // The fix is new enough
	    // and the fix is either complete or the client accepts partial fixes
	    ret = (pos.Time() >= maxAge
	    		&& (!iControllerManager->GetLastFixIsPartial() || IsPartialUpdateAllowed()));
    	}
    if(ret)
    	{
    	RDebug::Print(_L("CanUseLastFix TRUE"));
    	}
    return ret;
	}

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::NotifyPositionUpdate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosSimulationPositioner::NotifyPositionUpdate(
    TPositionInfoBase& aPosInfo,
    TRequestStatus& aStatus)
    {
    ClearAndSetDefaultPositionData(aPosInfo);
    
    // Use the last fix obtained if possible, otherwise get a new fix
    if (CanUseLastFix())
    	{
    	ASSERT(aPosInfo.PositionClassType() & EPositionInfoClass);
    	TPositionInfo& pos = *static_cast<TPositionInfo*>(&aPosInfo); 
    	pos = iControllerManager->GetLastFix();
	    iPosInfo = NULL;
	    // Complete the request with the correct status
	    TInt status(KErrNone);
	    if (iControllerManager->GetLastFixIsPartial())
	    	{
	    	status = KPositionPartialUpdate;
	    	}
	    CompleteRequest(aStatus, status);
    	}
    else
    	{
    	iPosInfo = &aPosInfo;
	    iController->NotifyPositionUpdate(aPosInfo, aStatus, *this);
	    }
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::CancelNotifyPositionUpdate
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosSimulationPositioner::CancelNotifyPositionUpdate()
    {
    iController->CancelNotifyPositionUpdate(*this);
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::SetDeviceStatus
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosSimulationPositioner::SetDeviceStatus(
    TPositionModuleStatus::TDeviceStatus aStatus)
    {
    iModuleStatus.SetDeviceStatus(aStatus);
    }

// ----------------------------------------------------------------------------
// CPosSimulationPositioner::SetDataQualityStatus
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosSimulationPositioner::SetDataQualityStatus(
    TPositionModuleStatus::TDataQualityStatus aStatus)
    {
    iModuleStatus.SetDataQualityStatus(aStatus);
    }

//  End of File
