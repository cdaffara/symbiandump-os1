
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

#include "asyncscheduler.h"

CAsyncTestActiveScheduler::CAsyncTestActiveScheduler(RAsyncTestStep& aTestStep):
	iTestStep (aTestStep)
	{	
	}

// when Error() is called, we want to stop the test
void CAsyncTestActiveScheduler::Error(TInt aError) const
	{
	iTestStep.HandleError(aError);
	}
