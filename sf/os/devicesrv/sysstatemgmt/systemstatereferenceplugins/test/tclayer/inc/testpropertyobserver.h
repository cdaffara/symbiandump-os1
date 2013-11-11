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
// Contains implementation of a test property observer
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/

#ifndef TESTPROPERTYOBSERVER_H_
#define TESTPROPERTYOBSERVER_H_

#include <ssm/ssmpropertyobserver.h>

// Forward declare for friendship
class CCLayerTestSsmPropertyObserver;

/**
 * Key for test property
 * @internalComponent
 * @test
 */
static const TUint KTestPropertyKey = 1234;

/**
 * Test Property Observer class
 * 
 * @internalComponent
 * @test
 */
class CTestPropertyObserver : public CSsmPropertyObserver
{
public:
	CTestPropertyObserver();
	virtual ~CTestPropertyObserver();

	void SetNextPropertyChangedReturn(TBool aReturn);
	TInt GetLastPropertyChangedValue();
	TInt GetPropertyChangedCount();
	
	// From CSsmPropertyObserver
	virtual TBool PropertyChangedL(TInt aValue);
	
	friend class CCLayerTestSsmPropertyObserver;
	
protected:
	// From CActive
	virtual void RunL();
	
private:
	TBool iNextPropertyChangedReturn;
	TInt iLastPropertyChangedValue;
	TInt iPropertyChangedCount;
};

#endif /*TESTPROPERTYOBSERVER_H_*/
