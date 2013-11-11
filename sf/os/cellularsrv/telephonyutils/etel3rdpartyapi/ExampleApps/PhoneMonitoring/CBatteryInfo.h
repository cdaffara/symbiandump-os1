// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef __CBATTERYINFO_H__
#define __CBATTERYINFO_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves the current state of the battery charge level
can be notified on change of battery status.
*/
class CBatteryInfo : public CISVAPIAsync
	{

// Methods
public:
	static CBatteryInfo* NewL(MExecAsync* aController);
	~CBatteryInfo();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CBatteryInfo(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved phone battery information.
	*/
	CTelephony::TBatteryInfoV1 iBatteryInfoV1;

private:
	/**
	Package iBatteryInfoV1.
	*/
	CTelephony::TBatteryInfoV1Pckg iBatteryInfoV1Pckg;

	};

#endif // __CBATTERYINFO_H__
