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

#ifndef __CDIALCALL_H__
#define __CDIALCALL_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"
#include "CMainMenu.h"

/**
Dials a call to the number passed as an argument to DoStartRequestL().
*/
class CDialCall : public CISVAPIAsync
	{

// Methods
public:
	static CDialCall* NewL(MExecAsync* aController);
	~CDialCall();

	void DoStartRequestL(const TDesC& aNumber);

private:
	CDialCall(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

// Data
public:
	/**
	The ID of the call which will be made.
	*/
	CTelephony::TCallId iCallId;
	};

#endif // __CDIALCALL_H__
