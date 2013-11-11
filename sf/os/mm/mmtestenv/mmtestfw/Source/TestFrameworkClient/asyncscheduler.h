
// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef ASYNCSCHEDULER_H
#define ASYNCSCHEDULER_H

#include <e32base.h>
#include <testframework.h>

//
// CAsyncTestActiveScheduler - call StopTest on error
// 

NONSHARABLE_CLASS (CAsyncTestActiveScheduler) : public CActiveScheduler
	{
public:
	CAsyncTestActiveScheduler(RAsyncTestStep& aTestStep);
protected:
	// from CActiveScheduler
	void Error(TInt aError) const;
private:
	RAsyncTestStep& iTestStep;
	};
	
#endif
