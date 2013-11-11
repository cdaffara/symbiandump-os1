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
// netmodstatushandler.cpp
// 
//

/**
 @file
 @InternalComponent
*/

#include "cnetmodstatushandler.h"
#include "lbsprocessuiddefs.h"
#include "lbsdevloggermacros.h"
#include "psypanic.h"
#include "cpositionerq.h"
#include "mpositioner.h"

/*
* CNETModStatusHandler::NewL
* Two-phased constructor.
*/
CNETModStatusHandler* CNETModStatusHandler::NewL(CPositionerQ* aPositionerQ)
    {
    CNETModStatusHandler* self = new( ELeave ) CNETModStatusHandler();
    
    CleanupStack::PushL( self );
    self->ConstructL(aPositionerQ);
    CleanupStack::Pop();

    return self;
    }

/*
* CNETModStatusHandler::CNETModStatusHandler
* C++ default constructor can NOT contain any code, that
* might leave.
*/
CNETModStatusHandler::CNETModStatusHandler()
    {
    }

/**
* subscribe to the status bus by module ID
*/
void CNETModStatusHandler::ConstructL(CPositionerQ* aPositionerQ)
	{
	iPositionerQ = aPositionerQ;
	
	iModStatusBus.OpenL(KLbsNetLocManagerUid);
	
	iModStatusBus.NotifyModuleStatusChange(iStatus);
	
	CActiveScheduler::Add(this);
	
	SetActive();
	}


CNETModStatusHandler::~CNETModStatusHandler()
    {
    Cancel();
    iModStatusBus.Close();
	}

/**
from CActive
*/
void CNETModStatusHandler::DoCancel()
	{
	iModStatusBus.CancelNotifyModuleStatusChange();
	}

void CNETModStatusHandler::RunL()
	{
	TInt status = iStatus.Int();
	
	// keep listening:
	iModStatusBus.NotifyModuleStatusChange(iStatus);
	SetActive(); 	
	
	if(status == KErrNone)
		{
		TPositionModuleStatus moduleStatus;

		TInt err = RLbsModuleStatus::GetModuleStatus(&moduleStatus, moduleStatus.PositionClassSize(), KLbsNetLocManagerUid);
		if((err == KErrNone) && (iPositionerQ->Count()))
			{
			(iPositionerQ->Positioner(0)).ReportStatus(moduleStatus);
			}
		}
	}
