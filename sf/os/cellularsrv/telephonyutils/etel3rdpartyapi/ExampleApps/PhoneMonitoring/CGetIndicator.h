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

#ifndef __CGETINDICATOR_H__
#define __CGETINDICATOR_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>


#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves a bitmask pertaining to data on the current state of the network
connection, whether the battery charger is connected and whether there is an
active call or not. Can be notified on change.
*/
class CGetIndicator : public CISVAPIAsync
	{

// Methods
public:
	static CGetIndicator* NewL(MExecAsync* aController);
	~CGetIndicator();

	void DoStartRequestL();
	void DoRequestNotificationL();

private:
	CGetIndicator(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores retrieved indicator information.
	*/
	CTelephony::TIndicatorV1 iIndicatorV1;

private:
	/**
	Package descriptor for iIndicatorV1.
	*/
	CTelephony::TIndicatorV1Pckg iIndicatorV1Pckg;

	};

#endif // __CGETINDICATOR_H__

