// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent
*/

#ifndef STRESSLETTHREAD_H
#define STRESSLETTHREAD_H

#include "e32base.h"

#include "stresslet.h"

class CBaseStressletAllocator;

class CStressletThread : public CActive
	{
public:
	CStressletThread();
	~CStressletThread();

	void StartL(CBaseStressletAllocator * aAllocator, MTestStepReporter& aReporter);
	
private:
	void RunL();
	void DoCancel();
	void StartThreadL();
	
private:
	static TBuf<64> iNextName;
	static TInt iNextId;

private:
	CStresslet::CStressletParams* iStressletParams;
	CStresslet * iStresslet;
	RThread iThread;
	};

#endif // STRESSLETTHREAD_H