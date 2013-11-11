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


#ifndef __CRESUME_H__
#define __CRESUME_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>
#include <etel3rdparty.h>

#include "CISVAPIAsync.h"
#include "CMainMenu.h"

/**
Resumes the currently held call specified as an argument to DoStartRequestL().
*/
class CResume : public CISVAPIAsync
	{

// Methods
public:
	static CResume* NewL(MExecAsync* aController);
	~CResume();

	void DoStartRequestL(CTelephony::TCallId aCallId);

private:
	CResume(MExecAsync* aController);
	void ConstructL();

	void RunL();
	void DoCancel();

	};

#endif // __CRESUME_H__
