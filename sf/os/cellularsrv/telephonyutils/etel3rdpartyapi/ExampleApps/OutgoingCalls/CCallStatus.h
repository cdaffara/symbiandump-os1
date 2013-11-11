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

#ifndef __CCALLSTATUS_H_
#define __CCALLSTATUS_H_

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves data pertaining to the current status of the call specified by
iCallId. Can be notified on change.
*/
class CCallStatus : public CISVAPIAsync
	{

// Methods
public:
	static CCallStatus* NewL(MExecAsync* aController, CTelephony::TCallId aCallId);
	~CCallStatus();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CCallStatus(MExecAsync* aController, CTelephony::TCallId aCallId);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved call status of iCallId.
	*/
	CTelephony::TCallStatusV1 iCallStatusV1;
	/**
	The call whose status will be interrogated.
	*/
	CTelephony::TCallId iCallId;

private:
	/**
	Packages iCallStatus.
	*/
	CTelephony::TCallStatusV1Pckg iCallStatusV1Pckg;

	};

#endif // __CCALLSTATUS_H_
