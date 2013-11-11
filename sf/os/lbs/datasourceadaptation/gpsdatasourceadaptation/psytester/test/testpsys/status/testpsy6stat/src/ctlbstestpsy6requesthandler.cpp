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
#include "ctlbstestpsy6requesthandler.h"

// ---------------------------------------------------------
// CT_LbsTestPsy6RequestHandler::InstanceL
// 
// (other items were commented in a header).
// ---------------------------------------------------------
//  
CT_LbsTestPsy6RequestHandler* CT_LbsTestPsy6RequestHandler::NewL(MPositionerStatus* aPositionerStatus)
    {
    CT_LbsTestPsy6RequestHandler* self;
   	self = new (ELeave) CT_LbsTestPsy6RequestHandler(aPositionerStatus);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); // self
	return self;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy6RequestHandler::CT_LbsTestPsy6RequestHandler(MPositionerStatus* aPositionerStatus) :
	CT_LbsTestPsyBase(aPositionerStatus)
    {
	ReportStatus(
				TPositionModuleStatus::EDeviceReady,
				TPositionModuleStatus::EDataQualityUnknown
				);
	}

// Destructor
CT_LbsTestPsy6RequestHandler::~CT_LbsTestPsy6RequestHandler()
    {
	ReportStatus(
				TPositionModuleStatus::EDeviceInactive,
				TPositionModuleStatus::EDataQualityLoss
				);
	}
