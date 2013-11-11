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

#ifndef __CNETWORKREGINFO_H__
#define __CNETWORKREGINFO_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Obtains the current registration status of the phone. Can be notified on change.
*/
class CNetworkRegInfo : public CISVAPIAsync
	{
	// Methods
public:
	static CNetworkRegInfo* NewL(MExecAsync* aController);
	~CNetworkRegInfo();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CNetworkRegInfo(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

  	// Data
public:
	/**
	Stores the network registration status retrieved.
	*/
	CTelephony::TNetworkRegistrationV1 iNetworkRegV1;

private:
	/**
	Package descriptor for iNetworkRegV1.
	*/
	CTelephony::TNetworkRegistrationV1Pckg iNetworkRegV1Pckg;
	};

#endif // __CNETWORKREGINFO_H__
