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
 @internalComponent - Internal Symbian test code
*/

#ifndef TESTHARNESSCALLBACK_H
#define TESTHARNESSCALLBACK_H

/** Interface used to provide callbacks when testing using CTExtendedBitmapGc. 
An object that implements MTestHarnessCallBack is provided as a parameter when
creating a CTExtendedBitmapGc object, allowing CTExtendedBitmapGc to call back
with logging requests and test status updates.
 */
class MTestHarnessCallBack
	{
public:
	virtual CTestExecuteLogger& Logger() = 0;
	virtual void TestTrue(TBool aCondition) = 0;
	virtual void TestComplete() = 0;
	};

#endif // TESTHARNESSCALLBACK_H
