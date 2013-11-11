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


#ifndef __CDYNAMICCAPS_H_
#define __CDYNAMICCAPS_H_

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <Etel3rdParty.h>

#include "CISVAPIAsync.h"
#include "CMainMenu.h"

/**
Retrieves dynamic capabilities relating to a call.  This includes
whether a call can be put on hold, resumed or swapped with another
call.
*/
class CDynamicCaps : public CISVAPIAsync
	{

// Methods
public:
	static CDynamicCaps* NewL(MExecAsync* aController, CTelephony::TCallId aCallId);
	~CDynamicCaps();

	void doStartRequestL();

private:
	CDynamicCaps(MExecAsync* aController, CTelephony::TCallId aCallId);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	Stores the retrieved capabilities of the call identified by iCallId.
	*/
	CTelephony::TCallCapsV1 iCallCapsV1;
	/**
	Identifies the call whose capabilities are interrogated.
	*/
	CTelephony::TCallId iCallId;

private:
	/**
	Package descriptor for iCallId.
	*/
	CTelephony::TCallCapsV1Pckg iCallCapsV1Pckg;

	};

#endif // __CDYNAMICCAPS_H_
