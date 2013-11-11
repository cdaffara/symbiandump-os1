// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// INCLUDES
//



#include <e32math.h> 
#include <e32std.h>
#include <lbspositioninfo.h>
#include <lbs/epos_mpositionerstatus.h>

#include "ctlbstestproxypsybase.h"

//CONSTANTS

// C++ Constructor 
CT_LbsTestProxyPsyBase::CT_LbsTestProxyPsyBase(MPositionerStatus* aPositionerStatus) :
    CActive(EPriorityStandard),
    iPositionerStatus(aPositionerStatus),
    iTimerDelay(100000)
    {
    CActiveScheduler::Add(this);
    iTimer.CreateLocal();
    
    ReportStatus(
				TPositionModuleStatus::EDeviceReady, 
				TPositionModuleStatus::EDataQualityUnknown
				);
    }
   


// C++ destructor 
CT_LbsTestProxyPsyBase::~CT_LbsTestProxyPsyBase() 
    {  
    ReportStatus(
				TPositionModuleStatus::EDeviceInactive, 
				TPositionModuleStatus::EDataQualityUnknown
				);
    iTimer.Close();
    }

// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::SetModuleId
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::SetModuleId(const TUid& aModuleId)
	{
	iModuleId = aModuleId; 
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::NotifyPositionUpdate
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::NotifyPositionUpdate(TPositionInfo* aPosInfo, TRequestStatus* aStatus)
    {
	*aStatus = KRequestPending;
	iRequestStatus = aStatus;
    aPosInfo->SetModuleId(iModuleId);

    iRequestPosInfo = aPosInfo;
	StartRequest();	
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::StartRequest
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::StartRequest()
	{
	iTimer.After(iStatus,iTimerDelay.Int64());
	SetActive();
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::RunL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::RunL()
	{
    
    TPosition pos;
    iRequestPosInfo->GetPosition(pos);
    if( Math::IsNaN(pos.Longitude()) && Math::IsNaN(pos.Latitude()) &&
        Math::IsNaN(pos.Altitude()))
    {
    	
    SetTPositionInfo(*iRequestPosInfo);
    }
	
    User::RequestComplete(iRequestStatus, iErrorCode);
    }
	
// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::DoCancel
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::DoCancel()
	{
	iTimer.Cancel();
	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::CancelGetPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::CancelNotifyPositionUpdate()
    {
    
    User::RequestComplete(iRequestStatus, KErrCancel);
	
    Cancel();
    }

// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::ReportStatus
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::ReportStatus(const TInt aDeviceStatus, const TInt aDataQuality)
	{
	TPositionModuleStatus moduleStatus;
	moduleStatus.SetDeviceStatus(aDeviceStatus);
	moduleStatus.SetDataQualityStatus(aDataQuality);
	iPositionerStatus->ReportStatus(moduleStatus);
	}

// ---------------------------------------------------------
// CTestPsyBase::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//                          
void CT_LbsTestProxyPsyBase::SetTPositionInfo(TPositionInfo& aInfo)
	{
	TPosition pos;
	
	pos.SetAccuracy(10, 10);
    pos.SetCoordinate(10, 10, 10);
	aInfo.SetPosition(pos);	

	}

// ---------------------------------------------------------
// CT_LbsTestProxyPsyBase::SetErrorCode
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::SetErrorCode(const TInt& aErrorCode)
	{
	iErrorCode = aErrorCode; 
	}

//---------------------------------------------------------
// CT_LbsTestProxyPsyBase::SetTimerDelay
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::SetTimerDelay(const TTimeIntervalMicroSeconds& aTimerDelay)
	{
	iTimerDelay=aTimerDelay;
	}


//---------------------------------------------------------
// CT_LbsTestProxyPsyBase::StartTracking
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::StartTracking()
    {
	 ReportStatus(
				TPositionModuleStatus::EDeviceInitialising, 
				TPositionModuleStatus::EDataQualityUnknown
				);
    }

//---------------------------------------------------------
// CT_LbsTestProxyPsyBase::StopTracking
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestProxyPsyBase::StopTracking()
    {
	 ReportStatus(
				TPositionModuleStatus::EDeviceStandBy, 
				TPositionModuleStatus::EDataQualityUnknown
				);
    }

//  End of File
