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



//  INCLUDES
#include <lbs.h>
#include "s32file.h"
#include "CSimPsyRequester.h"
#include "CSimPsyMultiRequester.h"
// #include "EPos_CPositionInfo.h"

#include "EPos_SimPsyConstants.h"
#include "EPos_CPosSimulationPositioner.h"         
#include "epossimulationpsy.hrh" 

// CONSTANTS

LOCAL_D TPositionModuleStatus modInfo;
LOCAL_D TPositionModuleStatus prevModInfo;

// GLREF_C TPositionModuleStatus ModuleStatus();

GLDEF_C TPositionModuleStatus ModuleStatus()
    {
    return modInfo;
    }

GLDEF_C TPositionModuleStatus PrevModuleStatus()
    {
    return prevModInfo;
    }

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CSimPsyRequester::CSimPsyRequester(CSimPsyMultiRequester* aParent) 
:   CActive(EPriorityStandard), 
	iCorrectModuleStatus(ETrue),
	iParent(aParent)
    {
    CActiveScheduler::Add(this);
    }
     
// Destructor
CSimPsyRequester::~CSimPsyRequester()
    {
    Cancel();
    delete iPositioner;
    }

void CSimPsyRequester::ConstructL()
    {
	TUid uid; 
	uid.iUid = KPosSimulationPsyImplUid;

	iConstructParams.iImplementationUid = uid;
	iConstructParams.iStatusObserver = this;
	iConstructParams.iParamObserver = this;

    iPositioner = CPosSimulationPositioner::NewL(&iConstructParams);
    }

CSimPsyRequester* CSimPsyRequester::NewL(CSimPsyMultiRequester* aParent)
    {
    CSimPsyRequester* self = new (ELeave) CSimPsyRequester(aParent);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
    return self;
    }

void CSimPsyRequester::ClosePositioner()
    {
    delete iPositioner;
    iPositioner = NULL;            
    }

void CSimPsyRequester::MakeRequest()
    {
    // RDebug::Print(_L("CSimPsyRequester::MakeRequest"));    
	iPosition = TPositionInfo();
    iStartTime.UniversalTime();
	iPositioner->NotifyPositionUpdate(iPosition, iStatus);
    SetActive();
    }

void CSimPsyRequester::CancelRequest()
    {
    iPositioner->CancelNotifyPositionUpdate();
	}

void CSimPsyRequester::RunL()
    {    
    // RDebug::Print(_L("CSimPsyRequester::RunL"));
    TTime now;
    now.UniversalTime();
    iRequestTime = now.MicroSecondsFrom(iStartTime);
    iParent->HandleRequestDone();    
    }

void CSimPsyRequester::DoCancel()
    {    
    iPositioner->CancelNotifyPositionUpdate();
    TTime now;
    now.UniversalTime();
    iRequestTime = now.MicroSecondsFrom(iStartTime);    
    iParent->HandleRequestDone();
    }

void CSimPsyRequester::GetResult(
	TInt& aStatus, TPositionInfo& aPosition, 
	TTimeIntervalMicroSeconds& aRequestTime)
    {  
    aStatus = iStatus.Int();
    aPosition = iPosition;
    aRequestTime = iRequestTime;
    }

TBool CSimPsyRequester::GetModuleStatus()
    {    
	return iCorrectModuleStatus;
    }

TInt CSimPsyRequester::ReportedStatuses()
    {    
	return iReportedStatuses;
    }

void CSimPsyRequester::ResetReportedStatuses()
    {    
	iReportedStatuses = 0;
    }

TPositionModuleStatus CSimPsyRequester::ModuleStatus()
    {    
	return iModuleStatus;
    }
    
// From MPosParameterObserver
void CSimPsyRequester::ReportStatus(
    const TPositionModuleId& aImplementationUid,
    const TPositionModuleStatus& aStatus)
	{
    iReportedStatuses++;
	
	if (aImplementationUid.iUid != KPosSimulationPsyImplUid)
    	{
    	iCorrectModuleStatus = EFalse; // Error	
    	}

	if (aStatus.DeviceStatus() == TPositionModuleStatus::EDeviceInitialising)
		{
		;
		}
	else if (aStatus.DeviceStatus() == TPositionModuleStatus::EDeviceStandBy)
		{
		if (iModuleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady) 
			{
			iCorrectModuleStatus = EFalse; // Error
			}
		}
	else if (aStatus.DeviceStatus() == TPositionModuleStatus::EDeviceReady)
		{
		if (iModuleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceInitialising && 
			iModuleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceStandBy && 
			iModuleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceActive && 
			iModuleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceUnknown && 
			iModuleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady) 
			{
			iCorrectModuleStatus = EFalse; // Error
			}
		}
	else if (aStatus.DeviceStatus() == TPositionModuleStatus::EDeviceActive)
		{
		if (iModuleStatus.DeviceStatus() != TPositionModuleStatus::EDeviceReady)
			{
			iCorrectModuleStatus = EFalse; // Error
			}	
		}

	iModuleStatus = aStatus;

	prevModInfo = modInfo;
	modInfo = iModuleStatus;
	}

TPositionInfoClassType CSimPsyRequester::RequestedClasses() const
	{
	return 1;	
	}

TInt CSimPsyRequester::GetRequiredPositionQuality(TPositionQuality& /*aRequestedPosQuality*/) const
	{
	return 1;
	}

TInt CSimPsyRequester::QueryId() const
	{
	return 1;
	}

void CSimPsyRequester::GetMaxAge(TTime& /*aMaxAge*/) const
	{
	;
	}

TBool CSimPsyRequester::IsPartialUpdateAllowed() const
	{
	return EFalse;
	}

// End of File
