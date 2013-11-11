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
// This contains RootServer Test cases from section 8
// 
//

// EPOC includes
#include <e32base.h>

// Test system includes
//#include <networking/log.h>
//#include <networking/teststep.h>
//#include <networking/testsuite.h>

#include "TestStepRootServer.h"
#include "RootServerTestSection8.h"
#include "c32cmiutils.h"

/** Calls utility function to convert aPriorityName to a TThreadPriority enumerator.
If either the conversion fails or the resulting priority does not match aExpectedPriority
then it sets aTestStepResult() to EFail.
*/
void CRootServerTest8_1::TestConvertPriorityValidL(const TDesC8& aPriorityName, TThreadPriority aExpectedPriority, TVerdict& aVerdict)
	{
	TThreadPriority priority;
	TInt result = C32CmiUtils::ThreadPriorityNameToEnum(aPriorityName, priority);
	if ((result != KErrNone) || (priority != aExpectedPriority))
		{
		INFO_PRINTF3(_L("TestConvertPriorityValid: %S failed to convert to %d"), &aPriorityName, aExpectedPriority);
		aVerdict = EFail;
		}
	}

/** Calls utility function to convert aPriorityName to a TThreadPriority enumerator.
Assumes aPriorityName is an invalid priority name: if the conversion unexpectedly succeeds,
then it sets aTestStepResult() to EFail.
*/
void CRootServerTest8_1::TestConvertPriorityInvalidL(const TDesC8& aPriorityName, TVerdict& aVerdict)
	{
	TThreadPriority priority;
	TInt result = C32CmiUtils::ThreadPriorityNameToEnum(aPriorityName, priority);
	if (result != KErrCorrupt)
		{
		INFO_PRINTF3(_L("TestConvertPriorityInvalid: %S converted to %d when it should have been invalid"), &aPriorityName, priority);
		aVerdict = EFail;
		}
	}

// Test step 8.1 - Unit testing conversion of thread priority names to enums
CRootServerTest8_1::CRootServerTest8_1(CTE_RootServerServer *aSuite) : CTestStepRootServer(aSuite)
{
	// store the name of this test case
	SetTestStepName(_L("Test8.1"));
}

TVerdict CRootServerTest8_1::doTestStepL( void )
	{
	INFO_PRINTF1(_L("Test Purpose: Unit testing conversion of thread priority names to enums"));
	TVerdict verdict = EPass;

	// test relative thread priorities
	TestConvertPriorityValidL(_L8("EPriorityMuchLess"),	EPriorityMuchLess,	verdict);
	TestConvertPriorityValidL(_L8("EPriorityLess"),		EPriorityLess,		verdict);
	TestConvertPriorityValidL(_L8("EPriorityNormal"),	EPriorityNormal,	verdict);
	TestConvertPriorityValidL(_L8("EPriorityMore"),		EPriorityMore,		verdict);
	TestConvertPriorityValidL(_L8("EPriorityMuchMore"),	EPriorityMuchMore,	verdict);
	TestConvertPriorityValidL(_L8("EPriorityRealTime"),	EPriorityRealTime,	verdict);
	// test absolute thread priorities
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteVeryLow"),			EPriorityAbsoluteVeryLow,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteLowNormal"),		EPriorityAbsoluteLowNormal,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteLow"),				EPriorityAbsoluteLow,				verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteBackgroundNormal"),	EPriorityAbsoluteBackgroundNormal,	verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteBackground"),		EPriorityAbsoluteBackground,		verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteForegroundNormal"),	EPriorityAbsoluteForegroundNormal,	verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteForeground"),		EPriorityAbsoluteForeground,		verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteHighNormal"),		EPriorityAbsoluteHighNormal,		verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime1"),		EPriorityAbsoluteRealTime1,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime2"),		EPriorityAbsoluteRealTime2,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteHigh"),				EPriorityAbsoluteHigh,				verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime3"),		EPriorityAbsoluteRealTime3,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime4"),		EPriorityAbsoluteRealTime4,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime5"),		EPriorityAbsoluteRealTime5,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime6"),		EPriorityAbsoluteRealTime6,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime7"),		EPriorityAbsoluteRealTime7,			verdict);
	TestConvertPriorityValidL(_L8("EPriorityAbsoluteRealTime8"),		EPriorityAbsoluteRealTime8,			verdict);
	// test a handful of valid names that differ in case
	TestConvertPriorityValidL(_L8("EPrIOritymORE"),					EPriorityMore,				verdict);
	TestConvertPriorityValidL(_L8("EPRIORITYMUCHMORE"),				EPriorityMuchMore,			verdict);
	TestConvertPriorityValidL(_L8("epriorityabsoluterealtime2"),	EPriorityAbsoluteRealTime2,	verdict);

	// test a handful of valid numerical values
	TestConvertPriorityValidL(_L8("10"),	EPriorityMore,				verdict);
	TestConvertPriorityValidL(_L8("500"),	EPriorityAbsoluteHigh,		verdict);
	TestConvertPriorityValidL(_L8("820"),	EPriorityAbsoluteRealTime2,	verdict);
	// confirm any old number is returned as an integer, unchecked:
	TestConvertPriorityValidL(_L8("666"),	(TThreadPriority)666,		verdict);

	// test a handful of invalid enumerations
	TestConvertPriorityInvalidL(_L8("EPriorityNormal0"),	verdict);
	TestConvertPriorityInvalidL(_L8("0EPriorityNormal"),	verdict);
	TestConvertPriorityInvalidL(_L8("EPriorityNormal "),	verdict);
	TestConvertPriorityInvalidL(_L8(" EPriorityNormal"),	verdict);
	TestConvertPriorityInvalidL(_L8("EPriorityStandard"),	verdict);
	TestConvertPriorityInvalidL(_L8("Nonsense"), 			verdict);
	TestConvertPriorityInvalidL(_L8(" 810 "),	 			verdict);

	return verdict;

	}
