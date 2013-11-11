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

#include "tsu_mmf_subtitlegraphicsuite.h"

#include "tsu_mmf_subtitlegraphicclientneg.h"
#include "tsu_mmf_subtitlegraphicclientpos.h"


/**
NewTestSuite is exported at ordinal 1
this provides the interface to allow schedule test
to create instances of this test suite
*/
EXPORT_C CTestSuite* NewTestSuiteL()
    {
    CTestSubtitleGraphicSuite* result = new (ELeave) CTestSubtitleGraphicSuite();
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/** Destructor */
CTestSubtitleGraphicSuite::~CTestSubtitleGraphicSuite()
	{
	}

/**
make a version string available for test system
@return version string
*/
TPtrC CTestSubtitleGraphicSuite::GetVersion() const
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
Constructor for test suite
this creates all the test steps and 
stores them inside CTestSubtitleGraphicSuite
*/
void CTestSubtitleGraphicSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_SUBTITLEGRAPHIC");

	// POSITIVE TESTS	
	AddTestStepL(RSubtitleGraphicTestStepDrawFrame::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0001-HP")));
	AddTestStepL(RSubtitleGraphicTestStepSwapFrame::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0002-HP")));
	AddTestStepL(RSubtitleGraphicTestStepGetUid::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0003-HP")));
	AddTestStepL(RSubtitleGraphicTestStepClear::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0004-HP")));


	// NEGATIVE TESTS
	AddTestStepL(RSubtitleGraphicTestNegDrawFrame::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0101-HP")));
	AddTestStepL(RSubtitleGraphicTestNegDrawFrame2::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0102-HP")));
	AddTestStepL(RSubtitleGraphicTestNegInitialize::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0103-HP")));
	AddTestStepL(RSubtitleGraphicTestNegSwapFrame1::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0104-HP")));
	AddTestStepL(RSubtitleGraphicTestNegSwapFrame2::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0105-HP")));
	AddTestStepL(RSubtitleGraphicTestNegDuration::NewL(_L("MM-MMF-SUBTITLE-CRP-U-0106-HP")));
	}
