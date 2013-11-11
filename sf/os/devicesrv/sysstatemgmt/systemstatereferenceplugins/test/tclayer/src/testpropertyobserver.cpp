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
// Contains wrapper helper functions for testing property observer class 
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#include "testpropertyobserver.h"

/**
 * Constructs a new CTestPropertyObserver using the current process SID as category
 * and KTestPropertyKey as key
 * 
 * @internalComponent
 * @test
 */
CTestPropertyObserver::CTestPropertyObserver()
: CSsmPropertyObserver(RProcess().SecureId(), KTestPropertyKey)
	{
	
	}

/**
 * Frees any resources allocated to this object
 * 
 * @internalComponent
 * @test
 */
CTestPropertyObserver::~CTestPropertyObserver()
	{
	// Nothing to do
	}

/**
 * Sets the next property changed return value to aReturn.
 * 
 * @internalComponent
 * @test
 */
void CTestPropertyObserver::SetNextPropertyChangedReturn(TBool aReturn)
	{
	iNextPropertyChangedReturn = aReturn;
	}

/**
 * Returns the last property changed value that this object saw
 * 
 * @internalComponent
 * @test
 */
TInt CTestPropertyObserver::GetLastPropertyChangedValue()
	{
	return iLastPropertyChangedValue;
	}

/**
 * Gets the count of number of times property changed has been called on this object
 * 
 * @internalComponent
 * @test
 */
TInt CTestPropertyObserver::GetPropertyChangedCount()
	{
	return iPropertyChangedCount;
	}
	
/**
 * Updates the test values for this test observer
 * 
 * @internalComponent
 * @test
 */
TBool CTestPropertyObserver::PropertyChangedL(TInt aValue)
	{
	++iPropertyChangedCount;
	iLastPropertyChangedValue = aValue;
	return iNextPropertyChangedReturn;
	}
	
/**
 * Runs the base class RunL and then stops the active scheduler
 * 
 * @internalComponent
 * @test
 */
void CTestPropertyObserver::RunL()
	{
	CSsmPropertyObserver::RunL();
	CActiveScheduler::Stop();
	}

