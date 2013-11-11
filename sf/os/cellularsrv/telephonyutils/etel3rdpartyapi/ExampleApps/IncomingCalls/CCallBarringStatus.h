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

#ifndef __CCALLBARRINGSTATUS_H__
#define __CCALLBARRINGSTATUS_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"

#include "CMainMenu.h"

/**
Retrieves the status of the network call barring supplementary service.
*/
class CCallBarringStatus : public CISVAPIAsync
	{

// Methods
public:
	static CCallBarringStatus* NewL(MExecAsync* aController);
	~CCallBarringStatus();

	void DoStartRequestL();

private:
	CCallBarringStatus(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores the status of call barring services. Filled by a call to 
	CTelephony::GetCallBarringStatus().
	*/
	CTelephony::TCallBarringSupplServicesV1 iCallBarringStatusV1;

private:
	/**
	Package descriptor for iCallBarringStatusV1.
	*/
	CTelephony::TCallBarringSupplServicesV1Pckg iCallBarringStatusV1Pckg;
	};

#endif // __CCALLBARRINGSTATUS_H__
