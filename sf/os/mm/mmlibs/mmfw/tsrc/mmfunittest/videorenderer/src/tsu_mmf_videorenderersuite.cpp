// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsu_mmf_videorenderersuite.h"
#include "testbuffer.h"
#include "testrenderer.h"
#include "testrendererorder.h"
#include "testrendererneg.h"

/**
NewTestSuite is exported at ordinal 1
this provides the interface to allow schedule test
to create instances of this test suite
*/
EXPORT_C CTestSuite* NewTestSuiteL()
    {
    CTestVideoRendererSuite* result = new (ELeave) CTestVideoRendererSuite();
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/** Destructor */
CTestVideoRendererSuite::~CTestVideoRendererSuite()
	{
	}

/**
make a version string available for test system
@return version string
*/
TPtrC CTestVideoRendererSuite::GetVersion() const
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
Constructor for test suite
this creates all the test steps and 
stores them inside CTestVideoRendererSuite
*/
void CTestVideoRendererSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_VIDEORENDERER");

	// POSITIVE TEST
	AddTestStepL(RTestVideoFrameBufferStep::NewL());
	AddTestStepL(RTestRendererStep::NewL(_L("MM-MMF-VIDREND-U-0002"), EFalse));
	AddTestStepL(RTestRendererStep::NewL(_L("MM-MMF-VIDREND-U-0003"), ETrue));
	AddTestStepL(RTestRendererOOOStep::NewL(_L("MM-MMF-VIDREND-U-0004")));
	AddTestStepL(RTestRendererNonTimedOOOStep::NewL(_L("MM-MMF-VIDREND-U-0005")));
	AddTestStepL(RTestRendererSkipStep::NewL(_L("MM-MMF-VIDREND-U-0006"), ETrue));
	AddTestStepL(RTestRendererSkipStep::NewL(_L("MM-MMF-VIDREND-U-0007"), EFalse));
	AddTestStepL(RTestRendererOODStep::NewL(_L("MM-MMF-VIDREND-U-0008"), ETrue));
	AddTestStepL(RTestRendererOODStep::NewL(_L("MM-MMF-VIDREND-U-0009"), EFalse));
	AddTestStepL(RTestRendererReplaceStep::NewL(_L("MM-MMF-VIDREND-U-0010"), ETrue));
	AddTestStepL(RTestRendererReplaceStep::NewL(_L("MM-MMF-VIDREND-U-0011"), EFalse));
	AddTestStepL(RTestRendererSameTimeStep::NewL(_L("MM-MMF-VIDREND-U-0012")));
	AddTestStepL(RTestRendererSameTimeNonTimedStep::NewL(_L("MM-MMF-VIDREND-U-0013")));
	AddTestStepL(RTestRendererZeroTimeStep::NewL(_L("MM-MMF-VIDREND-U-0014")));
	AddTestStepL(RTestRendererZeroTimeNonTimedStep::NewL(_L("MM-MMF-VIDREND-U-0015")));

	// NEGATIVE TEST
	AddTestStepL(RTestRendererCreateSurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0100"), KErrNotSupported, EFbsBitmapColor4K, TSize(1024, 768), 1));
	AddTestStepL(RTestRendererCreateSurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0101"), KErrNoMemory, ERgb32bit888, TSize(10000, 10000), 1));
	AddTestStepL(RTestRendererCreateSurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0102"), KErrArgument, ERgb32bit888, TSize(0, 0), 1));
	AddTestStepL(RTestRendererCreateSurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0103"), KErrArgument, ERgb32bit888, TSize(-1, -1), 1));
	AddTestStepL(RTestRendererCreateSurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0104"), KErrNoMemory, ERgb32bit888, TSize(1024, 768), 100));
	AddTestStepL(RTestRendererCreateSurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0105"), KErrNotSupported, ERgb32bit888, TSize(1024, 768), -1));
	AddTestStepL(RTestRendererCreateSurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0106"), KErrNotSupported, ERgb32bit888, TSize(1024, 768), 0));
	// test case 107 is removed as not applicable
	AddTestStepL(RTestRendererUpdateTwiceStep::NewL(_L("MM-MMF-VIDREND-U-0108"), ETrue));
	AddTestStepL(RTestRendererUpdateTwiceStep::NewL(_L("MM-MMF-VIDREND-U-0109"), EFalse));
	AddTestStepL(RTestRendererReleaseTwiceStep::NewL(_L("MM-MMF-VIDREND-U-0110"), EFalse));
	AddTestStepL(RTestRendererDestroySurfaceStep::NewL(_L("MM-MMF-VIDREND-U-0111"), EFalse));
	AddTestStepL(RTestRendererUpdateAndDestroyStep::NewL(_L("MM-MMF-VIDREND-U-0112"), ETrue));
	AddTestStepL(RTestRendererUpdateAndDestroyStep::NewL(_L("MM-MMF-VIDREND-U-0113"), EFalse));
	AddTestStepL(RTestRendererUpdateAndDeleteStep::NewL(_L("MM-MMF-VIDREND-U-0114"), ETrue));
	AddTestStepL(RTestRendererUpdateAndDeleteStep::NewL(_L("MM-MMF-VIDREND-U-0115"), EFalse));
	AddTestStepL(RTestRendererUpdateInvalidStep::NewL(_L("MM-MMF-VIDREND-U-0116"), ETrue));
	AddTestStepL(RTestRendererUpdateInvalidStep::NewL(_L("MM-MMF-VIDREND-U-0117"), EFalse));
	AddTestStepL(RTestRendererCreateTwiceStep::NewL(_L("MM-MMF-VIDREND-U-0118"), EFalse));
	AddTestStepL(RTestRendererDestroyInvalidStep::NewL(_L("MM-MMF-VIDREND-U-0119"), EFalse));
	AddTestStepL(RTestResourceFileReaderStep::NewL(_L("MM-MMF-VIDREND-U-0120a"), _L("Z:\\resource\\videorenderer\\tsu_videorenderer1.rsc"), KErrNotSupported, KErrNone, 15000, 0));
	AddTestStepL(RTestResourceFileReaderStep::NewL(_L("MM-MMF-VIDREND-U-0120b"), _L("Z:\\resource\\videorenderer\\tsu_videorenderer2.rsc"), KErrNotSupported, KErrNone, 15000, 0));
	AddTestStepL(RTestResourceFileReaderStep::NewL(_L("MM-MMF-VIDREND-U-0120c"), _L("Z:\\resource\\videorenderer\\tsu_videorenderer3.rsc"), KErrNone, KErrNone, 15000, 0));
	AddTestStepL(RTestResourceFileReaderStep::NewL(_L("MM-MMF-VIDREND-U-0120d"), _L("Z:\\resource\\videorenderer\\tsu_videorenderer4.rsc"), KErrNone, KErrNone, 0, 0));
	// test case 121 is removed as not applicable

	// ALLOC TEST
//	AddTestStepL( CTestStepDevVideoPlayAllocFailNew::NewL( _L("MM-MMF-DEVVIDEO-U-0100-CP")));
	}
