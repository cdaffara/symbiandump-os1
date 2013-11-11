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
#include <lbs/epos_mpositionerstatus.h>

#include "ctlbstestsingpsyrequesthandler.h"
  
CT_LbsTestSingPsyRequestHandler* CT_LbsTestSingPsyRequestHandler::InstanceL(MPositionerStatus* aPositionerStatus)
    {
    CT_LbsTestSingPsyRequestHandler* self;
    
	if (Dll::Tls())
		{
		self = reinterpret_cast<CT_LbsTestSingPsyRequestHandler*> (Dll::Tls());
		}
    else
        {
		self = new (ELeave) CT_LbsTestSingPsyRequestHandler(aPositionerStatus);
		Dll::SetTls(reinterpret_cast<TAny*>(self));
        }

	self->iReferenceCounter++;
	return self;
    }

// ---------------------------------------------------------
// CTestPsyBase::Release
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CT_LbsTestSingPsyRequestHandler::Release()
	{
	if (--iReferenceCounter == 0)
		{
        Dll::SetTls(NULL);
		delete this;
		}
	}

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestSingPsyRequestHandler::CT_LbsTestSingPsyRequestHandler(MPositionerStatus* aPositionerStatus) :
    iPositionerStatus(aPositionerStatus)
    {
	}

// Destructor
CT_LbsTestSingPsyRequestHandler::~CT_LbsTestSingPsyRequestHandler()
    {
    
    TPositionModuleStatus moduleStatus;
    moduleStatus.SetDeviceStatus(TPositionModuleStatus::EDeviceInactive);
	moduleStatus.SetDataQualityStatus(TPositionModuleStatus::EDataQualityUnknown);
	iPositionerStatus->ReportStatus(moduleStatus);
	
    }


