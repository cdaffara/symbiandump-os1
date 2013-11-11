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


#ifndef __CCURRENTNETWORKINFO_H__
#define __CCURRENTNETWORKINFO_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves information about the current network including Cell ID and current
area code. Can be notified on change.
*/
class CCurrentNetworkInfo : public CISVAPIAsync
	{

// Methods
public:
	static CCurrentNetworkInfo* NewL(MExecAsync* aController);
	~CCurrentNetworkInfo();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CCurrentNetworkInfo(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved network information.
	*/
	CTelephony::TNetworkInfoV1 iCurrentNetworkInfoV1;

private:
	/**
	Packages iCurrentNetworkInfoV1.
	*/
	CTelephony::TNetworkInfoV1Pckg iCurrentNetworkInfoV1Pckg;

	};

#endif // __CCURRENTNETWORKINFO_H__
