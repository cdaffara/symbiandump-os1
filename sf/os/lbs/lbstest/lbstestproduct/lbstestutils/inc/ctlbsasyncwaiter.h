// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// @file ctlbsasyncwaiter.h
// This is the header file for CT_LbsAsyncWaiter 
// 
//

/**
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_LBS_ASYNC_WAITER_H__
#define __CT_LBS_ASYNC_WAITER_H__

//epoc includes
#include <e32base.h>

// Implements functions related to active objects
class CT_LbsAsyncWaiter : public CActive
	{
public:
	IMPORT_C static CT_LbsAsyncWaiter* NewL();
	~CT_LbsAsyncWaiter();

	IMPORT_C void StartAndWait();
	IMPORT_C TInt Result() const;

private:
	CT_LbsAsyncWaiter();
	// from CActive
	virtual void RunL();
	virtual void DoCancel();

private:
	TInt iError;
	};

#endif // __CT_LBS_ASYNC_WAITER_H__
