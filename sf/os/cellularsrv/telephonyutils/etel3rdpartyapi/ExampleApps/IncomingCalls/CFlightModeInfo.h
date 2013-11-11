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

#ifndef __CFLIGHTMODEINFO_H__
#define __CFLIGHTMODEINFO_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Informs the user of whether the flight mode is on or off. Can be notified on 
change.
*/
class CFlightModeInfo : public CISVAPIAsync
	{

// Methods
public:
	static CFlightModeInfo* NewL(MExecAsync* aController);
	~CFlightModeInfo();

	void DoStartRequestL();
	void DoRequestNotificationL();
private:
	CFlightModeInfo(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved flight mode status.
	*/
	CTelephony::TFlightModeV1 iFlightModeV1;

private:
	/**
	Package descriptor for iFlightMoveV1.
	*/
	CTelephony::TFlightModeV1Pckg iFlightModeV1Pckg;
	};

#endif // __CFLIGHTMODEINFO_H__
