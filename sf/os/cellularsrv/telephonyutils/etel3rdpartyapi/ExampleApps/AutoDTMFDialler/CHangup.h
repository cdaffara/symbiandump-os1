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

#ifndef __CHANGUP_H__
#define __CHANGUP_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"
#include "CMainMenu.h"

/**
Hang up the currently active call whose callId is specified as an argument to DoStartRequestL()
*/
class CHangup : public CISVAPIAsync
	{

// Methods
public:
	static CHangup* NewL(MExecAsync* aController);
	~CHangup();

	void DoStartRequestL(CTelephony::TCallId aCallId);

private:
	CHangup(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

	};

#endif // __CHANGUP_H__
