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

#ifndef TESTSRTREADER_H
#define TESTSRTREADER_H

#include <testframework.h>
#include "testsrtdecoder.h"

/* 
Test GetBuffer with a valid source
MM-MMF-SUBTITLE-SRTREADER-U-0001-HP
*/
NONSHARABLE_CLASS(RTestSrtReaderStep0001) : public RTestSrtDecoderStep
	{
public:
	RTestSrtReaderStep0001();

private:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	
private:	
    RFs iFsSession;
	};

/* 
Test constructor without source
MM-MMF-SUBTITLE-SRTREADER-U-0100-HP
*/
NONSHARABLE_CLASS(RTestSrtReaderStep0100) : public RTestStep
	{
public:
	RTestSrtReaderStep0100();

private:
	// from RTestStep;
	TVerdict DoTestStepL();
	};
	
#endif // TESTSRTREADER_H
