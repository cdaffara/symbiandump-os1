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



// INCLUDE FILES
#include <lbspositioninfo.h>
#include "ctlbstestPsyBase.h"
#include "ctlbstestPsy1Standby.h"

// CONSTANTS

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsTestPsy1Standby::CT_LbsTestPsy1Standby(CT_LbsTestPsyBase& aPsyBase) :
	CActive(EPriorityStandard),
	iPsyBase(aPsyBase)
    {
    CActiveScheduler::Add(this);
	}

// EPOC default constructor can leave.
void CT_LbsTestPsy1Standby::ConstructL()
    {
    User::LeaveIfError(iTimer.CreateLocal());
	}

// Two-phased constructor.
CT_LbsTestPsy1Standby* CT_LbsTestPsy1Standby::NewLC(CT_LbsTestPsyBase& aPsyBase)
    {
    CT_LbsTestPsy1Standby* self = new(ELeave) CT_LbsTestPsy1Standby(aPsyBase);
    CleanupStack::PushL(self); 
    self->ConstructL();
    return self;
    }
    
// Two-phased constructor.
CT_LbsTestPsy1Standby* CT_LbsTestPsy1Standby::NewL(CT_LbsTestPsyBase& aPsyBase)
    {
    CT_LbsTestPsy1Standby* self = NewLC(aPsyBase);
    CleanupStack::Pop();
    return self;
    }
    
// Destructor
CT_LbsTestPsy1Standby::~CT_LbsTestPsy1Standby()
    {
	Cancel();
	iTimer.Close();
	}

void CT_LbsTestPsy1Standby::Start()
	{
	iStandby = EFalse;
	iTimer.After(iStatus, 3000000);
	SetActive();
	}

void CT_LbsTestPsy1Standby::RunL()
	{
	iPsyBase.ReportStatus(
					TPositionModuleStatus::EDeviceStandBy, 
					TPositionModuleStatus::EDataQualityUnknown);
	
	iStandby = ETrue;
	}
     
void CT_LbsTestPsy1Standby::DoCancel()
	{
	iTimer.Cancel();
	}

TBool CT_LbsTestPsy1Standby::IsStandby()
	{
	return iStandby;
	}
