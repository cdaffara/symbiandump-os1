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

#include "tsu_mmf_devsubtitle_suite.h"

#include "tsu_mmf_devsubtitle_neg.h"
#include "tsu_mmf_devsubtitle_pos.h"


/**
NewTestSuite is exported at ordinal 1
this provides the interface to allow schedule test
to create instances of this test suite
*/
EXPORT_C CTestSuite* NewTestSuiteL()
    {
    CTestDevSubtitleSuite* result = new (ELeave) CTestDevSubtitleSuite();
	CleanupStack::PushL(result);
	result->ConstructL();
	CleanupStack::Pop(); // result
	return result;
    }

/** Destructor */
CTestDevSubtitleSuite::~CTestDevSubtitleSuite()
	{
	}

/**
make a version string available for test system
@return version string
*/
TPtrC CTestDevSubtitleSuite::GetVersion() const
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
Constructor for test suite
this creates all the test steps and 
stores them inside CTestDevSubtitleSuite
*/
void CTestDevSubtitleSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_DEVSUBTITLE");

	// POSITIVE TESTS	
	AddTestStepL(RDevSubtitleTestStepPos0001::NewL(_L("MM-MMF-DEVSUBTITLE-U-0001-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0002::NewL(_L("MM-MMF-DEVSUBTITLE-U-0002-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0003::NewL(_L("MM-MMF-DEVSUBTITLE-U-0003-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0004::NewL(_L("MM-MMF-DEVSUBTITLE-U-0004-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0005::NewL(_L("MM-MMF-DEVSUBTITLE-U-0005-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0007::NewL(_L("MM-MMF-DEVSUBTITLE-U-0007-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0009::NewL(_L("MM-MMF-DEVSUBTITLE-U-0009-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0010::NewL(_L("MM-MMF-DEVSUBTITLE-U-0010-HP")));
	AddTestStepL(RDevSubtitleTestStepPos0011::NewL(_L("MM-MMF-DEVSUBTITLE-U-0011-HP")));

	// NEGATIVE TESTS
	AddTestStepL(RDevSubtitleLanguage::NewL(_L("MM-MMF-DEVSUBTITLE-U-0101-HP")));
	AddTestStepL(RDevSubtitleAddConfigTwice::NewL(_L("MM-MMF-DEVSUBTITLE-U-0102-HP")));
	AddTestStepL(RDevSubtitleGetCrpNegative::NewL(_L("MM-MMF-DEVSUBTITLE-U-0103-HP")));
	AddTestStepL(RDevSubtitleLanguageNoLoad::NewL(_L("MM-MMF-DEVSUBTITLE-U-0104-HP")));
	AddTestStepL(RDevSubtitleRemoveNoAdd::NewL(_L("MM-MMF-DEVSUBTITLE-U-0105-HP")));
	AddTestStepL(RDevSubtitleNoSelect::NewL(_L("MM-MMF-DEVSUBTITLE-U-0106-HP")));
	AddTestStepL(RDevSubtitleBogusDecoder::NewL(_L("MM-MMF-DEVSUBTITLE-U-0107-HP")));
	AddTestStepL(RDevSubtitleInvalidStatePos::NewL(_L("MM-MMF-DEVSUBTITLE-U-0108-HP")));
	AddTestStepL(RDevSubtitleInvalidStateStart::NewL(_L("MM-MMF-DEVSUBTITLE-U-0109-HP")));
	AddTestStepL(RDevSubtitleStartTwice::NewL(_L("MM-MMF-DEVSUBTITLE-U-0110-HP")));
	AddTestStepL(RDevSubtitleInvalidStateStop::NewL(_L("MM-MMF-DEVSUBTITLE-U-0111-HP")));
	AddTestStepL(RDevSubtitleInvalidConfig::NewL(_L("MM-MMF-DEVSUBTITLE-U-0112-HP")));
	AddTestStepL(RDevSubtitleGetCrpAfterRemove::NewL(_L("MM-MMF-DEVSUBTITLE-U-0113-HP")));
	}
