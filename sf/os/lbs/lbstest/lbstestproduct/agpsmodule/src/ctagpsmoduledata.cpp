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
// @file ctagpsmoduledata.cpp
// This is the cpp file which contains the
// 
//


#include "ctagpsmoduledata.h"


CT_AGpsModuleDataBusMonitor* CT_AGpsModuleDataBusMonitor::NewL(MT_AGpsModuleDataBusObserver* aObserver)
	{
	CT_AGpsModuleDataBusMonitor* self = new(ELeave) CT_AGpsModuleDataBusMonitor(aObserver);

	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();

	return self;
	}


CT_AGpsModuleDataBusMonitor::CT_AGpsModuleDataBusMonitor(MT_AGpsModuleDataBusObserver* aObserver)
	:
	CActive(CActive::EPriorityHigh),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


CT_AGpsModuleDataBusMonitor::~CT_AGpsModuleDataBusMonitor()
	{
	Cancel();
	iProperty.Close();
	}

void CT_AGpsModuleDataBusMonitor::ConstructL()
	{
	}

void CT_AGpsModuleDataBusMonitor::StartMonitorL()
	{
	// Attach and subscribe to listen for data bus updates.
	TInt err = iProperty.Attach(KUidSystemCategory, ELbsTestAGpsModuleIn);
	User::LeaveIfError(err);
   
	iProperty.Subscribe(iStatus);
	SetActive();
	}
	
void CT_AGpsModuleDataBusMonitor::CancelMonitor()
	{
	Cancel();
	}
	
void CT_AGpsModuleDataBusMonitor::RunL()
	{
	// Handle data change.
	TInt err = iStatus.Int();
	if (KErrNone == err)
		{
		TModuleDataIn modDataIn;
		TPckg<TModuleDataIn> modDataInPckg(modDataIn);

		err = iProperty.Get(modDataInPckg);
		if (iObserver && (KErrNone == err))
			{
			// Handle the incoming data.
			iObserver->HandleDataBusUpdateL(modDataIn);
			
			// Publish a response to the in coming data, now that we have handled it.
			TPckgBuf<TModuleDataOut> modDataOutBuf;
			TModuleDataOut& modDataOut = modDataOutBuf();

			modDataOut.iRequestType = TModuleDataOut::EModuleResponse; 
            modDataOut.iResponse = TModuleDataOut::EModuleResponseOk;

			err = RProperty::Set(KUidSystemCategory, ELbsTestAGpsModuleOut, modDataOutBuf);
			}
		}

	// Restart monitor.
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CT_AGpsModuleDataBusMonitor::DoCancel()
{
	iProperty.Cancel();
}


