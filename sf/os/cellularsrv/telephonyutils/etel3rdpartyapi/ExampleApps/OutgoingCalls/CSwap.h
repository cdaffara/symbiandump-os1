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


#ifndef __CSWAP_H__
#define __CSWAP_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"
#include "CMainMenu.h"

/**
Puts the currently active call specified as argument 1 to DoStartRequestL() on 
hold. Resumes the currently held call specified as argument 2 to DoStartRequestL().
*/
class CSwap : public CISVAPIAsync
	{

// Methods
public:
	static CSwap* NewL(MExecAsync* aController);
	~CSwap();

	void DoStartRequestL(CTelephony::TCallId aCallId1, CTelephony::TCallId aCallId2);

private:
	CSwap(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

	};

#endif // __CSWAP_H__
											 
