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


#ifndef __CIDENTITYSERVICESTATUS_H__
#define __CIDENTITYSERVICESTATUS_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves the status of the network call identity supplementary service.
*/
class CIdentityServiceStatus : public CISVAPIAsync
	{

// Methods
public:
	static CIdentityServiceStatus* NewL(MExecAsync* aController);
	~CIdentityServiceStatus();

	void DoStartRequestL();

private:
	CIdentityServiceStatus(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved identity service status.
	*/
	CTelephony::TIdentityServiceV1 iIdentityServiceStatusV1;

private:
	/**
	Package descriptor for iIdentityServiceStatusV1.
	*/
	CTelephony::TIdentityServiceV1Pckg iIdentityServiceStatusV1Pckg;

	};

#endif // __CIDENTITYSERVICESTATUS_H__
