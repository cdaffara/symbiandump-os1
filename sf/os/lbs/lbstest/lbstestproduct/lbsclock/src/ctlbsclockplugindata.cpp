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
// @file ctlbsclockplugindata.cpp
// This is the cpp file which contains the
// 
//


#include "ctlbsclockplugindata.h"


CT_LbsClockPluginDataBusMonitor* CT_LbsClockPluginDataBusMonitor::NewL(MT_LbsClockPluginDataBusObserver* aObserver)
	{
	CT_LbsClockPluginDataBusMonitor* self = new(ELeave) CT_LbsClockPluginDataBusMonitor(aObserver);

	return self;
	}


CT_LbsClockPluginDataBusMonitor::CT_LbsClockPluginDataBusMonitor(MT_LbsClockPluginDataBusObserver* aObserver)
	:
	CActive(CActive::EPriorityStandard),
	iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}


CT_LbsClockPluginDataBusMonitor::~CT_LbsClockPluginDataBusMonitor()
	{
	Cancel();
	iProperty.Close();
	}


void CT_LbsClockPluginDataBusMonitor::StartMonitorL()
	{
	// Attach and subscribe to listen for data bus updates.
	TInt err = iProperty.Attach(KUidSystemCategory, ELbsTestClockPluginOut);
	User::LeaveIfError(err);
   
	iProperty.Subscribe(iStatus);
	SetActive();
	}
	
void CT_LbsClockPluginDataBusMonitor::CancelMonitor()
	{
	Cancel();
	}
	
void CT_LbsClockPluginDataBusMonitor::RunL()
	{
	// Handle data change.
	TInt err = iStatus.Int();
	if (KErrNone == err)
		{
		TClockPluginDataOut pluginDataOut;
		TPckg<TClockPluginDataOut> pluginDataOutPckg(pluginDataOut);

		err = iProperty.Get(pluginDataOutPckg);
		if (iObserver && (KErrNone == err))
			{
			// Handle the incoming data.
			iObserver->HandleDataBusUpdateL(pluginDataOut);
			}
		}

	// Restart monitor.
	iProperty.Subscribe(iStatus);
	SetActive();
	}

void CT_LbsClockPluginDataBusMonitor::DoCancel()
{
	iProperty.Cancel();
}


