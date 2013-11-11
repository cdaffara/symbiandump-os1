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
// Contains an active object that stops the active scheduler  
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef ACTIVESTOPPER_H_
#define ACTIVESTOPPER_H_

#include <e32base.h>

/**
 * CActiveStopper is an active object that stops the active scheduler in it's RunL.
 * 
 * Used to aid in testing of active objects
 * 
 * @test
 * @internalComponent
 */
class CActiveStopper : public CActive
{
public:
	
	CActiveStopper()
		: CActive(EPriorityNull)
		{
		CActiveScheduler::Add(this);
		// Set it up to run immediately
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* statusPtr = &iStatus;
		User::RequestComplete(statusPtr, KErrNone);
		}
	
	virtual ~CActiveStopper()
		{
		Deque();
		}
	
protected:
	
	virtual void RunL()
		{
		// Set it up to run immediately
		iStatus = KRequestPending;
		SetActive();
		TRequestStatus* statusPtr = &iStatus;
		User::RequestComplete(statusPtr, KErrNone);
		// Stop active scheduler
		CActiveScheduler::Stop();
		}
	
	virtual void DoCancel()
		{
		// Nothing to do
		}
	
	virtual TInt DoError(TInt aError)
		{
		// Just return the error to the scheduler
		return aError;
		}
};

#endif /*ACTIVESTOPPER_H_*/
