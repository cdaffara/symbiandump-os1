// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsassdatamoduledata.cpp
// This is the cpp file which contains the
// 
//


#include "ctlbsassdatamoduledata.h"


CT_LbsAssDataModuleDataBusMonitor* CT_LbsAssDataModuleDataBusMonitor::NewL(MT_LbsAssDataModuleDataBusObserver* aObserver)
	{
	CT_LbsAssDataModuleDataBusMonitor* self = new(ELeave) CT_LbsAssDataModuleDataBusMonitor(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}


CT_LbsAssDataModuleDataBusMonitor::CT_LbsAssDataModuleDataBusMonitor(MT_LbsAssDataModuleDataBusObserver* aObserver)
	:
	CActive(CActive::EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


CT_LbsAssDataModuleDataBusMonitor::~CT_LbsAssDataModuleDataBusMonitor()
	{
	Cancel();
	iProperty.Close();
	}

void CT_LbsAssDataModuleDataBusMonitor::ConstructL()
	{
	}

void CT_LbsAssDataModuleDataBusMonitor::StartMonitorL()
	{
	// Attach and subscribe to listen for data bus updates.
	TInt err = iProperty.Attach(KUidSystemCategory, ELbsTestAGpsModuleOut);
	User::LeaveIfError(err);
   
	iProperty.Subscribe(iStatus);
	SetActive();
	}
	
void CT_LbsAssDataModuleDataBusMonitor::CancelMonitor()
	{
	Cancel();
	}
	
void CT_LbsAssDataModuleDataBusMonitor::RunL()
	{
	// Handle data change.
	TInt err = iStatus.Int();
	if (KErrNone == err)
		{
		TModuleDataOut modDataOut;
		TPckg<TModuleDataOut> modDataOutPckg(modDataOut);

		err = iProperty.Get(modDataOutPckg);
		if (iObserver && (KErrNone == err))
			{
			// Handle the incoming data.
			iObserver->HandleDataBusUpdateL(modDataOut);
			}
		}

	// Restart monitor.
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CT_LbsAssDataModuleDataBusMonitor::DoCancel()
{
	iProperty.Cancel();
}


