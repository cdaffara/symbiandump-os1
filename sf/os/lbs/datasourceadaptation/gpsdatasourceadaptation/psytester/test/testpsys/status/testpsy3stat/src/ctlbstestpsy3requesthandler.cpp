// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include <lbspositioninfo.h>
#include <lbssatellite.h>
#include "ctlbstestpsy3requesthandler.h"

// ---------------------------------------------------------
// CT_LbsTestPsy2RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy3RequestHandler* CT_LbsTestPsy3RequestHandler::NewL(MPositionerStatus* aPositionerStatus)
    {
    CT_LbsTestPsy3RequestHandler* self;
    self = new (ELeave) CT_LbsTestPsy3RequestHandler(aPositionerStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
	return self;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy3RequestHandler::CT_LbsTestPsy3RequestHandler(MPositionerStatus* aPositionerStatus) :
	CT_LbsTestPsyBase(aPositionerStatus)
    {
	ReportStatus(
				TPositionModuleStatus::EDeviceInitialising, 
				TPositionModuleStatus::EDataQualityUnknown
				);

	ReportStatus(
				TPositionModuleStatus::EDeviceReady, 
				TPositionModuleStatus::EDataQualityUnknown
				);
	}

// Destructor
CT_LbsTestPsy3RequestHandler::~CT_LbsTestPsy3RequestHandler()
    {
	ReportStatus(
				TPositionModuleStatus::EDeviceDisabled, 
				TPositionModuleStatus::EDataQualityLoss
				);
	}


// ---------------------------------------------------------
// CT_LbsTestPsy3RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy3RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	++iRequests;

	if(iRequests==1)
		{
		ReportStatus(
					TPositionModuleStatus::EDeviceActive, 
					TPositionModuleStatus::EDataQualityUnknown
					);
		
		CT_LbsTestPsyBase::SetTPositionInfo(aInfo);

		ReportStatus(
					TPositionModuleStatus::EDeviceDisabled, 
					TPositionModuleStatus::EDataQualityUnknown
					);
		ReportStatus(
					TPositionModuleStatus::EDeviceInactive, 
					TPositionModuleStatus::EDataQualityNormal
					);
		}
	else
		{
		ReportStatus(
					TPositionModuleStatus::EDeviceActive, 
					TPositionModuleStatus::EDataQualityNormal
					);
		
		CT_LbsTestPsyBase::SetTPositionInfo(aInfo);

		ReportStatus(
					TPositionModuleStatus::EDeviceReady, 
					TPositionModuleStatus::EDataQualityNormal
					);
		}	
	}

