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

#ifndef TESTSRTDECODER_H
#define TESTSRTDECODER_H

#include <fbs.h> 
#include <w32std.h>
#include <testframework.h>

class CSrtReader;
class CSrtSubtitleDecoder;

// Generic test step class with utilities
class RTestSrtDecoderStep : public RTestStep
    {
protected:
	static TBool CompareFilesL(RFs &aFs, const TDesC& aFilePath1, const TDesC& aFilePath2);
	void InitializeTestStepL(TBool aCreateSrtReaderOnly, const TDesC& aSrtFilePath);
	void UnInitializeTestStep();
	TBool IsFontAvailableL(const TDesC& aTypefaceName);
	TBool CompareBmpFilesL(TInt aStartIndex, TInt aEndIndex, const TDesC& aSrcFilePathSpec, const TDesC& aDestFilePathSpec);
	void DeleteTempFiles(TInt aStartIndex, TInt aEndIndex, const TDesC& aFilePathSpec);
	
protected:	
	CSrtReader* iSrtReader;
	CSrtSubtitleDecoder* iSrtDecoder;
    RFbsSession iRbsSession;
	
private:
	TBool iCreateSrtReaderOnly;
    };

/* 
Test decoding a valid srt file from various positions
MM-MMF-SUBTITLE-SRTDECODER-U-0002-HP
*/
NONSHARABLE_CLASS (RTestSrtDecoderStep0002) : public RTestSrtDecoderStep
	{
public:
    RTestSrtDecoderStep0002();
    
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	
private:
    void InitWservL();
    void UninitWserv();
    void TestGetNextFrameL();
	void TestGetNextFrameByPositionsL(const TTimeIntervalMicroSeconds& aPosition, TInt aExpectedNumOfFrames, TBool aSaveBitmap);
	void PrepGc();
	void RetireGc();
	void DrawBitmap(CFbsBitmap& aBitmap);
	
private:
    RWsSession iWs;
    CWsScreenDevice* iScreen;
    RWindow* iWindow;
    RWindowGroup iRootWindow;
    CWindowGc *iGc;
    CActiveScheduler* iActiveScheduler;
	TBool iActiveSchedulerStarted;
	};
	
/* 
Test decoding an invalid srt file from beginning
MM-MMF-SUBTITLE-SRTDECODER-U-0101-HP 
*/

NONSHARABLE_CLASS (RTestSrtDecoderStep0101) : public RTestSrtDecoderStep
	{
public:
    RTestSrtDecoderStep0101();

private:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
		
private:
	void TestGetNextFrameL();
	};
	
/* 
Out of memory test for srt decoder
MM-MMF-SUBTITLE-SRTDECODER-U-0103-HP
*/
NONSHARABLE_CLASS(RTestSrtDecoderStep0103) : public RTestStep
	{
public:
	RTestSrtDecoderStep0103();

protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();

private:
	void TestGetNextFrameL(CFbsBitmap& aBitmap);
	};

/* 
Start called twice panic test
MM-MMF-SUBTITLE-SRTDECODER-U-0105-HP
*/
NONSHARABLE_CLASS(RTestSrtDecoderStep0105) : public RTestSrtDecoderStep
	{
public:
	RTestSrtDecoderStep0105();

protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStepPreambleL();
	TVerdict DoTestStepPostambleL();
	};

#endif // TESTSRTDECODER_H

