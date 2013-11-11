/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: This is the header file for CT_LbsAsyncWaiter 
*
*/

/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __CT_ASYNC_WAITER_H__
#define __CT_ASYNC_WAITER_H__

//epoc includes
#include <e32base.h>

// Implements functions related to active objects
class CT_AsyncWaiter : public CTimer
	{
public:
	IMPORT_C static CT_AsyncWaiter* NewL();
	~CT_AsyncWaiter();

	IMPORT_C void StartAndWait();
	IMPORT_C void StartAndWaitTimer(const TTimeIntervalMicroSeconds32& aTime);
	IMPORT_C TInt Result() const;

private:
	CT_AsyncWaiter();
	void ConstructL();
	// from CActive
	virtual void RunL();
	virtual void DoCancel();

private:
	TInt iError;
	};

#endif // __CT_ASYNC_WAITER_H__
