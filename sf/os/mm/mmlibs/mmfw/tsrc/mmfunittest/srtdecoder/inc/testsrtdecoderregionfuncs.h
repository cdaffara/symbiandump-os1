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

#ifndef TESTSRTDECODERREGIONFUNCS_H
#define TESTSRTDECODERREGIONFUNCS_H

#include <testframework.h>
#include "testsrtdecoder.h"

class CSrtReader;
class CSrtSubtitleDecoder;

/* 
Test functions of calculating region, setting video position, starting and stopping decoding.
MM-MMF-SUBTITLE-SRTDECODER-U-0001-HP
*/
NONSHARABLE_CLASS (RTestSrtDecoderStep0001) : public RTestSrtDecoderStep
	{
public:
	RTestSrtDecoderStep0001();
    
private:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();

    virtual TVerdict TestCalculateSubtitleRegion();
	virtual TVerdict TestSetVideoPosition();
	virtual TVerdict TestStartStop();
	};

/* 
Test functions of calculating invalid regions, setting invalid video position.
MM-MMF-SUBTITLE-SRTDECODER-U-0102-HP
*/
NONSHARABLE_CLASS (RTestSrtDecoderStep0102) : public RTestSrtDecoderStep
	{
public:
	RTestSrtDecoderStep0102();
	
private:
    // from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	
    TVerdict TestCalculateSubtitleRegion();
	TVerdict TestStartStop();
	};

/* 
Test functions of calculating invalid regions, setting invalid video position.
MM-MMF-SUBTITLE-SRTDECODER-U-0104-HP
*/
NONSHARABLE_CLASS (RTestSrtDecoderStep0104) : public RTestSrtDecoderStep
	{
public:
	RTestSrtDecoderStep0104();
	
private:
    // from RTestStep;
	TVerdict DoTestStepL();
	};

#endif // TESTSRTDECODERREGIONFUNCS_H
