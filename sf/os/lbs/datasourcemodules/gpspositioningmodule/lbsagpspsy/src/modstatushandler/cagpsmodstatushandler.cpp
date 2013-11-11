// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// agpsmodstatushandler.cpp
// 
//

/**
 @file
 @InternalComponent
*/

#include "cagpsmodstatushandler.h"
#include "lbsprocessuiddefs.h"
#include "lbsdevloggermacros.h"
#include "psypanic.h"
#include "cpositionerq.h"
#include "mpositioner.h"

/**
 CAgpsModStatusHandler::NewL
 Two-phased constructor.
*/
CAgpsModStatusHandler* CAgpsModStatusHandler::NewL(CPositionerQ* aPositionerQ)
    {
    CAgpsModStatusHandler* self = new( ELeave ) CAgpsModStatusHandler();
    
    CleanupStack::PushL( self );
    self->ConstructL(aPositionerQ);
    CleanupStack::Pop();

    return self;
    }

/*
 CAgpsModStatusHandler::CAgpsModStatusHandler
 C++ default constructor can NOT contain any code, that
 might leave.
*/
CAgpsModStatusHandler::CAgpsModStatusHandler()
    {
    }

/**
* subscribe to the status bus by module ID
*/
void CAgpsModStatusHandler::ConstructL(CPositionerQ* aPositionerQ)
	{
	iPositionerQ = aPositionerQ;
	
	iModStatusBus.OpenL(KLbsGpsLocManagerUid);
	
	iModStatusBus.NotifyModuleStatusChange(iStatus);
	
	CActiveScheduler::Add(this);
	
	SetActive();
	}


CAgpsModStatusHandler::~CAgpsModStatusHandler()
    {
    Cancel();
    iModStatusBus.Close();
	}

/**
from CActive
*/
void CAgpsModStatusHandler::DoCancel()
	{
	iModStatusBus.CancelNotifyModuleStatusChange();
	}

void CAgpsModStatusHandler::RunL()
	{
	TInt status = iStatus.Int();
	
	// keep listening:
	iModStatusBus.NotifyModuleStatusChange(iStatus);
	SetActive(); 	
	
	if(status == KErrNone)
		{
		TPositionModuleStatus moduleStatus;

		TInt err = RLbsModuleStatus::GetModuleStatus(&moduleStatus, moduleStatus.PositionClassSize(), KLbsGpsLocManagerUid);
		if((err == KErrNone) && (iPositionerQ->Count()))
			{
	    	(iPositionerQ->Positioner(0)).ReportStatus(moduleStatus);
			}
    	}
	}
