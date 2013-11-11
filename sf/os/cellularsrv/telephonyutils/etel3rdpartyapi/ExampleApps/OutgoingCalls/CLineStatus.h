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

#ifndef __CLINESTATUS_H__
#define __CLINESTATUS_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves data pertaining to the current status of the voice line. Can be
notified on change.
*/
class CLineStatus : public CISVAPIAsync
	{

// Methods
public:
	static CLineStatus* NewL(MExecAsync* aController);
	~CLineStatus();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CLineStatus(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Store the retrieved line status.
	*/
	CTelephony::TCallStatusV1 iLineStatusV1;

private:
	/**
	Packages iLineStatusV1.
	*/
	CTelephony::TCallStatusV1Pckg iLineStatusV1Pckg;

	};

#endif // __CLINESTATUS_H__
