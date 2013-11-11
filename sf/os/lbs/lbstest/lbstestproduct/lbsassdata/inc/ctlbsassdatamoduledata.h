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
// @file ctlbsassdatamoduledata.h
// This is the header file for a data bus monitor used by the A-GPS test module.
// 
//

#ifndef __CT_LBS_ASS_DATA_MODULE_DATA_H__
#define __CT_LBS_ASS_DATA_MODULE_DATA_H__

#include <e32base.h>
#include <e32property.h>

#include <lbs/test/tlbsutils.h>

class MT_LbsAssDataModuleDataBusObserver
	{
public:
	virtual void HandleDataBusUpdateL(const TModuleDataOut& aModuleDataOut) = 0;
	};


class CT_LbsAssDataModuleDataBusMonitor : public CActive
	{
public:
	static CT_LbsAssDataModuleDataBusMonitor* NewL(MT_LbsAssDataModuleDataBusObserver* aObserver);
	
	~CT_LbsAssDataModuleDataBusMonitor();
	
	void StartMonitorL();

	void CancelMonitor();

protected:
	CT_LbsAssDataModuleDataBusMonitor(MT_LbsAssDataModuleDataBusObserver* aObserver);
	void ConstructL();
	void RunL();
	void DoCancel();
	
private:
	MT_LbsAssDataModuleDataBusObserver* iObserver;

	RProperty iProperty;
	};

#endif // __CT_LBS_ASS_DATA_MODULE_DATA_H__


