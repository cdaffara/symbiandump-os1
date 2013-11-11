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


#ifndef __CCALLWAITINGSTATUS_H__
#define __CCALLWAITINGSTATUS_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves the status of the network call waiting supplementary service.
*/
class CCallWaitingStatus : public CISVAPIAsync
	{

// Methods
public:
	static CCallWaitingStatus* NewL(MExecAsync* aController);
	~CCallWaitingStatus();

	void DoStartRequestL();

private:
	CCallWaitingStatus(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved information about the call waiting status.
	*/
	CTelephony::TCallWaitingSupplServicesV1 iCallWaitingStatusV1;

private:
	/**
	Package descriptor for iCallWaitingStatusV1.
	*/
	CTelephony::TCallWaitingSupplServicesV1Pckg iCallWaitingStatusV1Pckg;
	};

#endif // __CCALLWAITINGSTATUS_H__
