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

#include "testsrtdecoder.h"
#include "testsrtdecoderlanguagefuncs.h"
#include "testsrtdecoderregionfuncs.h"
#include "testsrtreader.h"
#include "tsu_mmf_subtitledecodersuite.h"

/**
NewTestSuite is exported at ordinal 1
this provides the interface to allow schedule test
to create instances of this test suite
*/
EXPORT_C CTestSuite* NewTestSuiteL()
    {
    CTestSubtitleDecoderSuite* instance = new (ELeave) CTestSubtitleDecoderSuite();
	CleanupStack::PushL(instance);
	instance->ConstructL();
	CleanupStack::Pop(); // result
	return instance;
    }

/** Destructor */
CTestSubtitleDecoderSuite::~CTestSubtitleDecoderSuite()
	{
	}

/**
make a version string available for test system
@return version string
*/
TPtrC CTestSubtitleDecoderSuite::GetVersion() const
	{
	_LIT(KTxtVersion,"1.0");
	return KTxtVersion();
	}

/**
Constructor for test suite
this creates all the test steps and 
stores them inside CTestSubtitleDecoderSuite
*/
void CTestSubtitleDecoderSuite::InitialiseL()
	{
	// store the name of this test suite 
	iSuiteName = _L("TSU_MMF_SUBTITLEDECODER");

	// POSITIVE TEST
    AddTestStepL(new (ELeave)RTestSrtReaderStep0001());
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0001());
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0002());

	// NEGATIVE TEST
    AddTestStepL(new (ELeave)RTestSrtReaderStep0100());
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0100());
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0101());
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0102());
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0104());
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0105());

	// OOM TEST
    AddTestStepL(new (ELeave)RTestSrtDecoderStep0103());
	}


