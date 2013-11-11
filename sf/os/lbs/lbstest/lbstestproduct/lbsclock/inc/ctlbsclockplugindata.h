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
// @file ctagpsmoduledata.h
// This is the header file for a data bus monitor used by the A-GPS test module.
// 
//

#ifndef __CT_LBS_CLOCK_PLUGIN_DATA_H__
#define __CT_LBS_CLOCK_PLUGIN_DATA_H__

#include <e32base.h>
#include <e32property.h>

#include <lbs/test/tlbsutils.h>

class MT_LbsClockPluginDataBusObserver
	{
public:
	virtual void HandleDataBusUpdateL(const TClockPluginDataOut& aClockPluginDataOut) = 0;
	};


class CT_LbsClockPluginDataBusMonitor : public CActive
	{
public:
	static CT_LbsClockPluginDataBusMonitor* NewL(MT_LbsClockPluginDataBusObserver* aObserver);
	
	~CT_LbsClockPluginDataBusMonitor();
	
	void StartMonitorL();

	void CancelMonitor();

protected:
	CT_LbsClockPluginDataBusMonitor(MT_LbsClockPluginDataBusObserver* aObserver);
	void RunL();
	void DoCancel();
	
private:
	MT_LbsClockPluginDataBusObserver* iObserver;

	RProperty iProperty;
	};

#endif // __CT_LBS_CLOCK_PLUGIN_DATA_H__


