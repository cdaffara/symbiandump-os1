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
// CDummyAnswer.cpp
// 
//

#ifndef __CDUMMYANSWER_H__
#define __CDUMMYANSWER_H__

#include <e32base.h>
#include <e32cons.h>
#include <e32def.h>

#include "CISVAPIAsync.h"

/**
An active object that is activated by the call being picked up. Counts the
call's duration.
*/
class CDummyAnswer : public CActive
	{
public:
	static CDummyAnswer* NewL(MExecAsync* aController);
	~CDummyAnswer();

	void StartCount(TTimeIntervalMicroSeconds32 aDelay);

private:
	CDummyAnswer(MExecAsync* aController);
	void ConstructL();
	void ResetFirstTime();

	void ExampleComplete();

	void RunL();
	void DoCancel();

private:
	/**
	Set to ETrue the first time the example is run or when an active object
	request is cancelled.
	*/
	TBool iFirstTime;
	/**
	Pointer to console to which output is printed.
	*/
	CConsoleBase* iConsole;
	/**
	Timer for timing a call's duration.
	*/
	RTimer iTimer;
	/**
	Pointer to an MExecAsync object that owns this object.
	*/
	MExecAsync* iController;
	/**
	The duration of the current call (in seconds).
	*/
	TInt iCount;
	};

#endif // __CDUMMYANSWER_H__
