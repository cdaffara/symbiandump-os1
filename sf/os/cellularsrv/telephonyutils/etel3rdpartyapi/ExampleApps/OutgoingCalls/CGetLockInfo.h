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

#ifndef __CGETLOCKINFO_H__
#define __CGETLOCKINFO_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves information pertaining the status of the PIN 1 or 2 lock. Can be
notified on change.
*/
class CGetLockInfo : public CISVAPIAsync
	{

// Methods
public:
	static CGetLockInfo* NewL(MExecAsync* aController);
	~CGetLockInfo();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CGetLockInfo(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved ICC lock information.
	*/
	CTelephony::TIccLockInfoV1 iIccLockInfoV1;

private:
	/**
	Packages iIccLockInfoV1.
	*/
	CTelephony::TIccLockInfoV1Pckg iIccLockInfoV1Pckg;

	};

#endif // __CGETLOCKINFO_H__
