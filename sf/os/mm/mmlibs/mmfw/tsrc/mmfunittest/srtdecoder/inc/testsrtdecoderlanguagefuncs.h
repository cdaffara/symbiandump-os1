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

#ifndef TESTSRTDECODERLANGUAGEFUNCS_H
#define TESTSRTDECODERLANGUAGEFUNCS_H

#include <testframework.h>
#include "testsrtdecoder.h"

class CSrtReader;
class CSrtSubtitleDecoder;

/* 
Test language functions
MM-MMF-SUBTITLE-SRTDECODER-U-0100-HP
*/
NONSHARABLE_CLASS (RTestSrtDecoderStep0100) : public RTestSrtDecoderStep
	{
public:
	RTestSrtDecoderStep0100();

private:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();

	TVerdict TestSubtitleLanguage();
	TVerdict TestSetSubtitleLanguage();
	TVerdict TestGetSupportedSubtitleLanguages();
	};

#endif // TESTSRTDECODERLANGUAGEFUNCS_H
