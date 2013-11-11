/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/
//
// ActiveYield.h
//
/** 
 @file
 @internalTechnology
 @test
*/

#ifndef __LBS_GETAFIX_TEST_ACTIVE_YIELDER_H__
#define __LBS_GETAFIX_TEST_ACTIVE_YIELDER_H__

#include <e32base.h>
#include <e32std.h>

class CTestStep;

#include <test/testexecutestepbase.h>

IMPORT_C void CheckForObserverEventTestsL(TTimeIntervalMicroSeconds32 aTimeout, CTestStep& aStep);
IMPORT_C void ReturnToTestStep();
	
#endif // __LBS_GETAFIX_TEST_ACTIVE_YIELDER_H__

