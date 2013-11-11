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
#include "ctlbstestpsy5requesthandler.h"


// ---------------------------------------------------------
// CT_LbsTestPsy4RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy5RequestHandler* CT_LbsTestPsy5RequestHandler::NewL(MPositionerStatus* aPositionerStatus)
    {
    CT_LbsTestPsy5RequestHandler* self;
    
	self = new (ELeave) CT_LbsTestPsy5RequestHandler(aPositionerStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
	return self;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy5RequestHandler::CT_LbsTestPsy5RequestHandler(MPositionerStatus* aPositionerStatus) :
	CT_LbsTestPsyBase(aPositionerStatus)
	{
	}

// Destructor
CT_LbsTestPsy5RequestHandler::~CT_LbsTestPsy5RequestHandler()
    {
	 ReportStatus(
				TPositionModuleStatus::EDeviceInactive, 
				TPositionModuleStatus::EDataQualityUnknown
				);
	}

// ---------------------------------------------------------
// CT_LbsTestPsy5RequestHandler::SetTPositionInfo
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestPsy5RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	++iRequests;

	if(iRequests==1)
		{
		ReportStatus(
					TPositionModuleStatus::EDeviceInitialising, 
					TPositionModuleStatus::EDataQualityUnknown
					);
		ReportStatus(
					TPositionModuleStatus::EDeviceReady, 
					TPositionModuleStatus::EDataQualityUnknown
					);
		ReportStatus(
					TPositionModuleStatus::EDeviceActive, 
					TPositionModuleStatus::EDataQualityLoss
					);
		
		CT_LbsTestPsyBase::SetTPositionInfo(aInfo);

		ReportStatus(
					TPositionModuleStatus::EDeviceReady, 
					TPositionModuleStatus::EDataQualityUnknown
					);
		}
    else
		{
		CT_LbsTestPsyBase::SetTPositionInfo(aInfo);
		}
	}

