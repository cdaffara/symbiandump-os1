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
#include <e32base.h>
#include "ctlbstestPsyBase.h"
#include "ctlbstestpsy1requesthandler.h"

 
CT_LbsTestPsy1RequestHandler* CT_LbsTestPsy1RequestHandler::NewL(MPositionerStatus* aPositionerStatus)
    {
    CT_LbsTestPsy1RequestHandler* self;
    self = new (ELeave) CT_LbsTestPsy1RequestHandler(aPositionerStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
	return self;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy1RequestHandler::CT_LbsTestPsy1RequestHandler(MPositionerStatus* aPositionerStatus) :
	CT_LbsTestPsyBase(aPositionerStatus)
    {

	}

// Destructor
CT_LbsTestPsy1RequestHandler::~CT_LbsTestPsy1RequestHandler()
    {
    ReportStatus(
				TPositionModuleStatus::EDeviceInactive, 
				TPositionModuleStatus::EDataQualityUnknown
				);
	delete iStandby;
	}

void CT_LbsTestPsy1RequestHandler::ConstructL()
	{

	ReportStatus(
				TPositionModuleStatus::EDeviceInitialising, 
				TPositionModuleStatus::EDataQualityUnknown
				);

	CT_LbsTestPsyBase::ConstructL();
	iStandby = CT_LbsTestPsy1Standby::NewL(*this);
	ReportStatus(
				TPositionModuleStatus::EDeviceReady, 
				TPositionModuleStatus::EDataQualityUnknown
				);
	}

void CT_LbsTestPsy1RequestHandler::SetTPositionInfo(TPositionInfo& aInfo, const TBool& /*aUseCache*/)
	{
	++iRequests;

	if(iStandby->IsStandby())
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
	iStandby->Cancel();
	iStandby->Start();

	if(iRequests==1)
		{
		ReportStatus(
					TPositionModuleStatus::EDeviceActive, 
					TPositionModuleStatus::EDataQualityLoss
					);
		
		CT_LbsTestPsyBase::SetTPositionInfo(aInfo);

		ReportStatus(
					TPositionModuleStatus::EDeviceReady, 
					TPositionModuleStatus::EDataQualityPartial
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
