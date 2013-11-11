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


#ifndef __CSIGNALINFO_H__
#define __CSIGNALINFO_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>


#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves information regarding the current signal strength. Can be notified on
change.
*/
class CSignalInfo : public CISVAPIAsync
	{

// Methods
public:
	static CSignalInfo* NewL(MExecAsync* aController);
	~CSignalInfo();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CSignalInfo(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved signal strength of phone.
	*/
	CTelephony::TSignalStrengthV1 iSignalStrengthV1;

private:
	/**
	Package descriptor for iSignalStrengthV1Pckg.
	*/
	CTelephony::TSignalStrengthV1Pckg iSignalStrengthV1Pckg;

	};

#endif // __CSIGNALINFO_H__

