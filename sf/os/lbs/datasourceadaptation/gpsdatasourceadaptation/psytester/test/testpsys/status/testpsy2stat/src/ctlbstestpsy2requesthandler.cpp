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
#include "ctlbstestpsy2requesthandler.h"

// ---------------------------------------------------------
// CT_LbsTestPsy2RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy2RequestHandler* CT_LbsTestPsy2RequestHandler::NewL(MPositionerStatus* aPositionerStatus)
    {
    CT_LbsTestPsy2RequestHandler* self;	
	self = new (ELeave) CT_LbsTestPsy2RequestHandler(aPositionerStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy2RequestHandler::CT_LbsTestPsy2RequestHandler(MPositionerStatus* aPositionerStatus) :
	CT_LbsTestPsyBase(aPositionerStatus)
    {
	ReportStatus(
				TPositionModuleStatus::EDeviceActive, 
				TPositionModuleStatus::EDataQualityUnknown
				);
	}

// Destructor
CT_LbsTestPsy2RequestHandler::~CT_LbsTestPsy2RequestHandler()
    {
	}

void CT_LbsTestPsy2RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	++iRequests;

	CT_LbsTestPsyBase::SetTPositionInfo(aInfo);
	
	if(iRequests==1)
		{
		ReportStatus(
					TPositionModuleStatus::EDeviceError, 
					TPositionModuleStatus::EDataQualityUnknown
					);
		}
	else
		{	
		ReportStatus(
					TPositionModuleStatus::EDeviceUnknown, 
					TPositionModuleStatus::EDataQualityUnknown
					);
		}	
	}
