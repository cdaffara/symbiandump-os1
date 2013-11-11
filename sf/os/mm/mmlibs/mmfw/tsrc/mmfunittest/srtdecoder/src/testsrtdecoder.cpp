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

#include <bautils.h> 
#include <bitdev.h>
#include <gulutil.h>

#include "srtdecoder.h"
#include "srtreader.h"
#include "testsrtdecoder.h"

_LIT(KSampleSubtitleSRTFilepath1, "c:\\mm\\subtitle1.srt");
_LIT(KSampleSubtitleSRTFilepath2, "c:\\mm\\subtitle2.srt");

// subtitlebmp0.mbm -- subtitlebmp9.mbm: bitmaps for comparison
_LIT(KSubtitleTargetBitmapFilePathSpec, "c:\\mm\\subtitlebmp%d.mbm");
// subtitlecurbmp0.mbm -- subtitlecurbmp9.mbm: runtime bitmaps for comparison, 
// will be removed unless MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA is defined
_LIT(KSubtitleCurrentBitmapFilePathSpec, "c:\\mm\\subtitlecurbmp%d.mbm");

// defining MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA means re-creating the test data (bitmaps) for comparison, instead 
// of doing the comparison with the existing test data 
// at runtime with the spec of KSubtitleCurrentBitmapFilePathSpec  
#ifndef MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA
// #define MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA
#endif 

void RTestSrtDecoderStep::InitializeTestStepL(TBool aCreateSrtReaderOnly, const TDesC& aSrtFilePath)
    {
    __MM_HEAP_MARK;
    iSrtReader = CSrtReader::NewL(aSrtFilePath);
    
    iCreateSrtReaderOnly = aCreateSrtReaderOnly;
    if (!iCreateSrtReaderOnly)
        {
        User::LeaveIfError(iRbsSession.Connect());
        iSrtDecoder = CSrtSubtitleDecoder::NewL(*iSrtReader);
        }
    }
	
void RTestSrtDecoderStep::UnInitializeTestStep()
    {
    if (!iCreateSrtReaderOnly)
        {
        iRbsSession.Disconnect();
        delete iSrtDecoder;
        }
    delete iSrtReader;
    __MM_HEAP_MARKEND;
    }

// Implementation of the generic test step
TBool RTestSrtDecoderStep::CompareFilesL(RFs &aFs, const TDesC& aFilePath1, const TDesC& aFilePath2)
    {
    TBool identical = EFalse;
    RFile file1;
    RFile file2;
    TInt file1Size = 0;
    TInt file2Size = 0;
    
    if (aFilePath1 == aFilePath2)
        {
        identical = ETrue;
        }
    else
        {
        User::LeaveIfError(file1.Open(aFs, aFilePath1, EFileRead));
        CleanupClosePushL(file1);
        User::LeaveIfError(file2.Open(aFs, aFilePath2, EFileRead)); 
        CleanupClosePushL(file2);
        
        User::LeaveIfError(file1.Size(file1Size));
        User::LeaveIfError(file2.Size(file2Size));
        
        if (file1Size == file2Size)
            {
            TBuf8<256> buffer1;
            TBuf8<256> buffer2;
            
            identical = ETrue;
            while(ETrue)
                {
                User::LeaveIfError(file1.Read(buffer1));
                User::LeaveIfError(file2.Read(buffer2));
                
                if (buffer1 != buffer2)
                    {
                    identical = EFalse;
                    break;
                    }
                
                if (0 == buffer1.Length())
                    {
                    break;
                    }
                }
            }
        
        CleanupStack::PopAndDestroy(&file2);
        CleanupStack::PopAndDestroy(&file1);
        }
        
    return identical;     
    }
    
TBool RTestSrtDecoderStep::IsFontAvailableL(const TDesC& aTypefaceName)
    {
    TBool avail = EFalse;
    TSize bmpSize(100, 100);
    
    CFbsBitmap* sampleBitmap = new (ELeave) CFbsBitmap();
 	CleanupStack::PushL(sampleBitmap);
 	
 	User::LeaveIfError(sampleBitmap->Create(bmpSize, EColor16MA));
 	
    CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(sampleBitmap);
    CleanupStack::PushL(bmpDevice);
    
    if (0 != FontUtils::TypefaceAttributes(*bmpDevice, aTypefaceName))
        {
        const TInt KSubtitleFontMaxSpecHeights = 128;
        CArrayFix<TInt> *listHeights = new (ELeave) CArrayFixFlat<TInt>(KSubtitleFontMaxSpecHeights); 
        CleanupStack::PushL(listHeights);
        
        FontUtils::GetAvailableHeightsInTwipsL(*bmpDevice, aTypefaceName, *listHeights);
        for (TInt i = 0; i < listHeights->Count(); i++) 
            {
            if (KSrtTargetTypefaceHeightInTwips == (*listHeights)[i])
                {
                avail = ETrue;
                break;
                }
            }
        
        CleanupStack::PopAndDestroy(listHeights);
        }
    
    CleanupStack::PopAndDestroy(bmpDevice);
    CleanupStack::PopAndDestroy(sampleBitmap);
    
    return avail; 
    }
    
TBool RTestSrtDecoderStep::CompareBmpFilesL(TInt aStartIndex, TInt aEndIndex, const TDesC& aSrcFilePathSpec, const TDesC& aDestFilePathSpec)
    {
    TBool identical = ETrue;
    TBuf <KMaxFullName> bitmapFilename1;
    TBuf <KMaxFullName> bitmapFilename2;
    RFs rfs;
    User::LeaveIfError(rfs.Connect());
    CleanupClosePushL(rfs);
    for (TInt i = aStartIndex; i <= aEndIndex; i++)
        {
        bitmapFilename1.Format(aDestFilePathSpec, i);
        bitmapFilename2.Format(aSrcFilePathSpec, i);
        if (!CompareFilesL(rfs, bitmapFilename1, bitmapFilename2))
            {
            identical = EFalse;
            break;
            }
        }
    CleanupStack::PopAndDestroy(&rfs);

    return identical;
    }
    
void RTestSrtDecoderStep::DeleteTempFiles(TInt aStartIndex, TInt aEndIndex, const TDesC& aFilePathSpec)
    {
    TBuf <KMaxFullName> bitmapFilename;
    RFs rfs;
    
    if (KErrNone == rfs.Connect())
        {
        for (TInt i = aStartIndex ; i <= aEndIndex; i++)
            {
            bitmapFilename.Format(aFilePathSpec, i);
            BaflUtils::DeleteFile(rfs, bitmapFilename);
            }
        }
    }    
    
    
// Implementation of RTestSrtDecoderStep0002

RTestSrtDecoderStep0002::RTestSrtDecoderStep0002()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0002-HP");
	}

TVerdict RTestSrtDecoderStep0002::DoTestStepPreambleL()
    {
    InitializeTestStepL(EFalse, KSampleSubtitleSRTFilepath1);
    
    // Install the Active Scheduler
    iActiveScheduler = new(ELeave) CActiveScheduler;
    CActiveScheduler::Install(iActiveScheduler);
	iActiveSchedulerStarted = EFalse;
	
    InitWservL();
    
    return EPass;
    }

TVerdict RTestSrtDecoderStep0002::DoTestStepPostambleL()
    {
    UninitWserv();
    CActiveScheduler::Install(NULL);
	delete iActiveScheduler;
    iActiveScheduler = NULL;
    
    UnInitializeTestStep();
    
    return EPass;
    }

TVerdict RTestSrtDecoderStep0002::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = EPass;
 
    TRAPD(err, TestGetNextFrameL());
    if (KErrNone != err)
        {
        result = EFail;
        ERR_PRINTF2(_L("Error - RTestSrtDecoderStep0002::TestGetNextFrameL failed. error code %d. "), err);
        }
   
    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}
	
void RTestSrtDecoderStep0002::InitWservL()
    {
    TInt err = iWs.Connect();
    
    if (err != KErrNone)
        {
        // Access violation if ws is null
        ERR_PRINTF2(_L("Error - Failed to connect to RWsSession. error code %d. "), err);
        User::Leave(err);
        }

    iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction

    iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
    iWindow->SetExtent(TPoint(0,0), iScreen->SizeInPixels());
    iWindow->SetVisible(ETrue);
    iWindow->SetRequiredDisplayMode(EColor16MA);

    // Light Sky Blue 135-206-250
    TRgb backgroundColour = TRgb(135, 206, 250);
    iWindow->SetBackgroundColor(backgroundColour);
    
    iGc = new(ELeave) CWindowGc(iScreen);
	User::LeaveIfError(iGc->Construct());
	
    iWindow->Activate();
    iWs.Flush();
    }
    
void RTestSrtDecoderStep0002::UninitWserv()
    {
    if (iWindow)
        {
        iWindow->Close();
        delete iWindow;
        iWindow = NULL;
        }
    
    iRootWindow.Close();
    delete iScreen;
    iScreen = NULL;
    
    delete iGc;
    iGc = NULL;
    
    iWs.Flush();
    iWs.Close();
    }
    
void RTestSrtDecoderStep0002::PrepGc()
	{
	iGc->Activate(*iWindow);
	iWindow->Invalidate();
	iWindow->BeginRedraw();

	iGc->Clear(TRect(iScreen->SizeInPixels()));
	iWs.Flush();
	}

void RTestSrtDecoderStep0002::RetireGc()
	{
	iGc->Deactivate();
	iWindow->EndRedraw();
	iWs.Flush();
	}
	
void RTestSrtDecoderStep0002::DrawBitmap(CFbsBitmap& aBitmap)
	{	
	PrepGc();
	
    TSize size = iScreen->SizeInPixels();
    TInt width = size.iWidth;
    TInt height = size.iHeight;
    TPoint pos(0, 0);

    // Draw a square border
    iGc->SetPenColor(TRgb(255,0,0));
    iGc->DrawLine(TPoint(0,0),TPoint(0,height-1));
    iGc->DrawLine (TPoint (0, height-1), TPoint (width-1, height-1));
    iGc->DrawLine(TPoint(width-1,height-1),TPoint(width-1,0));
    iGc->DrawLine (TPoint (width-1, 0), TPoint (0, 0));

    // Draw a line between the corners of the window
    iGc->DrawLine(TPoint(0,0),TPoint(width, height));
    iGc->DrawLine (TPoint (0, height), TPoint (width, 0));
	
	// Draw bitmap
	iGc->BitBlt(pos, &aBitmap);
	
	RetireGc();
	}

void RTestSrtDecoderStep0002::TestGetNextFrameL()
    {
    // start/stop for multiple times is also tested.
    const TInt64 KSrtMicroSecondsInAMilliSecond = 1000;
    const TInt64 KSrtMicroSecondsInASecond = KSrtMicroSecondsInAMilliSecond * 1000;
    const TInt64 KSrtMicroSecondsInAMinute = KSrtMicroSecondsInASecond * 60;
    const TInt64 KSrtMicroSecondsInAnHour = KSrtMicroSecondsInAMinute * 60;
    const TInt KSrtSetPosTestCount = 6;
    const TInt KSrtNumOfFrames = 10;
    
    TTimeIntervalMicroSeconds videoPos[KSrtSetPosTestCount] = 
        {
        0,
        KSrtMicroSecondsInAMinute,
        1 * KSrtMicroSecondsInAMinute + 2 * KSrtMicroSecondsInASecond + 1 * KSrtMicroSecondsInAMilliSecond,
        1 * KSrtMicroSecondsInAMinute + 6 * KSrtMicroSecondsInASecond + 20 * KSrtMicroSecondsInAMilliSecond,
        1 * KSrtMicroSecondsInAnHour + 1 * KSrtMicroSecondsInAMinute + 20 * KSrtMicroSecondsInASecond + 1 * KSrtMicroSecondsInAMilliSecond,
        2 * KSrtMicroSecondsInAnHour
        };
        
    TInt expectedNumOfFrames[KSrtSetPosTestCount] = 
        {
        10,
        8,
        6,
        6,
        2,
        0
        };
     
    // check if the required Font is available before do the bitmap file comparison
    TInt requiredFontAvailable = IsFontAvailableL(KSrtTargetTypefaceName);
    if (!requiredFontAvailable)
        {
#ifdef MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA 
        INFO_PRINTF1(_L("Required font is not available, cannot save bitmap data. "));
        User::Leave(KErrGeneral);
#else
        INFO_PRINTF1(_L("Required font is not available, no bitmap comparison. "));
#endif //MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA 
        }
     
    for (TInt i = 0; i < KSrtSetPosTestCount; i++)
		{
#ifdef __WINSCW__
		TBool compareBitmap = (0 == i) && requiredFontAvailable;
#else
		TBool compareBitmap = EFalse;
#endif //__WINSCW__
        
        TestGetNextFrameByPositionsL(videoPos[i], expectedNumOfFrames[i], compareBitmap);
        
        // verify the bitmaps
        if (compareBitmap)
            {
#ifndef MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA
            TBool compResult = CompareBmpFilesL(KSrtNumOfFrames - expectedNumOfFrames[i], KSrtNumOfFrames - 1, KSubtitleCurrentBitmapFilePathSpec, KSubtitleTargetBitmapFilePathSpec);

            DeleteTempFiles(KSrtNumOfFrames - expectedNumOfFrames[i], KSrtNumOfFrames - 1, KSubtitleCurrentBitmapFilePathSpec);
        
            if (!compResult)
                {
                INFO_PRINTF1(_L("At least one bitmap file does not match the expected one. "));
                User::Leave(KErrGeneral);
                }
#endif //MMF_SUBTITLE_SUPPORT_TEST_SAVEDATA 
            }
        }
    }

void RTestSrtDecoderStep0002::TestGetNextFrameByPositionsL(const TTimeIntervalMicroSeconds& aPosition, TInt aExpectedNumOfFrames, TBool aSaveBitmap)
    {
    TRect dirtyRegion;
    TTimeIntervalMicroSeconds displayTime = 0;
    TTimeIntervalMicroSeconds displayDuration = 0;
    TInt64 tDisplayTime = 0;
    TInt64 tDisplayDuration = 0;
    TSize bmpSize(600, 300);
    TInt numOfFrames = 0;
 	TInt err = KErrNone;
 	TBuf <KMaxFullName> bitmapFilename;
 	
 	CFbsBitmap* sampleBitmap = new (ELeave) CFbsBitmap();
 	CleanupStack::PushL(sampleBitmap);

    User::LeaveIfError(sampleBitmap->Create(bmpSize, EColor16MA));

    iSrtDecoder->SetVideoPosition(aPosition);
    iSrtDecoder->Start();

    while (KErrNone == err)
        {
        TRAP(err, iSrtDecoder->GetNextFrameL(*sampleBitmap, dirtyRegion, displayTime, displayDuration));
        if (KErrNone == err)
            {
            // show bitmap
            DrawBitmap(*sampleBitmap);
            
            // save bitmap for the possible comparison
            if (aSaveBitmap)
                {
                bitmapFilename.Format(KSubtitleCurrentBitmapFilePathSpec, numOfFrames);
                User::LeaveIfError(sampleBitmap->Save(bitmapFilename));
                }
                         
            if ((dirtyRegion.iTl.iX >= dirtyRegion.iBr.iX) || 
                (dirtyRegion.iTl.iY >= dirtyRegion.iBr.iY))
                {
                INFO_PRINTF2(_L("Invalid dirty region received. (frame %d)"), numOfFrames);
                User::Leave(KErrGeneral);
                }
            
            tDisplayTime = displayTime.Int64()/1000;
            tDisplayDuration = displayDuration.Int64()/1000;
            INFO_PRINTF4(_L("Frame[%d]: displayTime: %dms, displayDuration: %dms."), 
                numOfFrames, 
                I64LOW(tDisplayTime), 
                I64LOW(tDisplayDuration));
                
            numOfFrames++;
            }
        }
    
    CleanupStack::PopAndDestroy(sampleBitmap);
    
    iSrtDecoder->Stop();
    
    if (KErrEof != err)
        {
        User::Leave(err);
        }
        
    if (aExpectedNumOfFrames != numOfFrames)
        {
        INFO_PRINTF2(_L("The number of frame (%d) is unexpected. "), numOfFrames);
        User::Leave(KErrGeneral);
        }
    }
    
    
// Implementation of RTestSrtDecoderStep0101

RTestSrtDecoderStep0101::RTestSrtDecoderStep0101()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0101-HP");
	}

TVerdict RTestSrtDecoderStep0101::DoTestStepPreambleL()
    {
    InitializeTestStepL(EFalse, KSampleSubtitleSRTFilepath2);
    
    return EPass;
    }
    
TVerdict RTestSrtDecoderStep0101::DoTestStepPostambleL()
    {
    UnInitializeTestStep();
    
    return EPass;
    }
    
TVerdict RTestSrtDecoderStep0101::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = EPass;
 
    TRAPD(err, TestGetNextFrameL());
    if (KErrNone != err)
        {
        result = EFail;
        ERR_PRINTF2(_L("Error - RTestSrtDecoderStep0101::TestGetNextFrameL failed. error code %d. "), err);
        INFO_PRINTF1(_L("Exit CSrtDecoder"));
	    return result;
        }

    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}

void RTestSrtDecoderStep0101::TestGetNextFrameL()
    {
    TInt numOfValidFrame = 0;
    const TInt KSrtCase0101ExpectedValidFrame = 6;
    TRect dirtyRegion;
    TTimeIntervalMicroSeconds displayTime = 0;
    TTimeIntervalMicroSeconds displayDuration = 0;
    TSize bmpSize(320, 120);
    
    CFbsBitmap* sampleBitmap = new (ELeave) CFbsBitmap();
 	CleanupStack::PushL(sampleBitmap);
 	
 	TInt err = sampleBitmap->Create(
		bmpSize,
        EColor16MA
        );
    User::LeaveIfError(err);
     
    iSrtDecoder->SetVideoPosition(0);
    iSrtDecoder->Start();
    
    while (KErrNone == err)
        {
        TRAP(err, iSrtDecoder->GetNextFrameL(*sampleBitmap, dirtyRegion, displayTime, displayDuration));
        if (KErrNone == err)
            {
            numOfValidFrame++;
            }
        else if (KErrArgument == err)
            {
            err = KErrNone;
            }
        }
    
    CleanupStack::PopAndDestroy(sampleBitmap);
    
    iSrtDecoder->Stop();
    
    if (err != KErrEof)
        {
        User::Leave(err);
        }
        
    if (KSrtCase0101ExpectedValidFrame != numOfValidFrame)
        {
        INFO_PRINTF2(_L("The number of valid frame (%d) is unexpected. "), numOfValidFrame);
        User::Leave(KErrGeneral);
        }
    }

// Implementation of RTestSrtDecoderStep0103

RTestSrtDecoderStep0103::RTestSrtDecoderStep0103()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0103-HP");
	}

TVerdict RTestSrtDecoderStep0103::DoTestStepPreambleL()
    {
    User::LeaveIfError(RFbsSession::Connect());
    
    return EPass;
    }
    
TVerdict RTestSrtDecoderStep0103::DoTestStepPostambleL()
    {
    RFbsSession::Disconnect();
    
    return EPass;
    }
     
TVerdict RTestSrtDecoderStep0103::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = EFail;
	TSize bmpSize(120, 320);

    CFbsBitmap* sampleBitmap = new (ELeave) CFbsBitmap();
 	CleanupStack::PushL(sampleBitmap);
 	User::LeaveIfError(sampleBitmap->Create(bmpSize, EColor16MA));
	
    for (TInt failRate = 1; ; ++failRate)
        {
        __UHEAP_SETFAIL(RHeap::EFailNext, failRate);
    	__UHEAP_MARK;

    	TRAPD(error, TestGetNextFrameL(*sampleBitmap));
    	RDebug::Printf("Ending iteration %d.  Result = %d. Failures = %d", failRate, error, __UHEAP_CHECKFAILURE);
    	
        __UHEAP_MARKEND;

    	if ((error != KErrNone) && (error != KErrNoMemory))
    	    {
    	    ERR_PRINTF3(_L("RTestSrtDecoderStep0103: TESTS FAILED TO COMPLETE (failRate=%i) error code: %d\n"), failRate, error);
    		break;
    	    }    	
    	
        TAny* const pointer = User::Alloc(1);
    	User::Free(pointer);
    	if (!pointer)
    		{
    		result = EPass;
    		break;
    		}
        }

    __UHEAP_RESET; 
    
    CleanupStack::PopAndDestroy(sampleBitmap);
    
    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}
	
void RTestSrtDecoderStep0103::TestGetNextFrameL(CFbsBitmap& aBitmap)
    {
    TRect dirtyRegion;
    CSrtReader *srtReader = CSrtReader::NewL(KSampleSubtitleSRTFilepath1);
    CleanupStack::PushL(srtReader);
    
    CSrtSubtitleDecoder *srtDecoder = CSrtSubtitleDecoder::NewL(*srtReader);
    CleanupStack::PushL(srtDecoder);
    
    TInt err = KErrNone;
    TTimeIntervalMicroSeconds displayTime = 0;
    TTimeIntervalMicroSeconds displayDuration = 0;
 	
 	srtDecoder->SetVideoPosition(0);
    srtDecoder->Start();
    
    while (KErrNone == err)
        {
        TRAP(err, srtDecoder->GetNextFrameL(aBitmap, dirtyRegion, displayTime, displayDuration));
        }

    if (err != KErrEof)
        {
        User::LeaveIfError(err);
        }
    
    srtDecoder->Stop();
    
    CleanupStack::PopAndDestroy(srtDecoder);
    CleanupStack::PopAndDestroy(srtReader);
    }
    
RTestSrtDecoderStep0105::RTestSrtDecoderStep0105()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0105-HP");
	}

TVerdict RTestSrtDecoderStep0105::DoTestStepL()
	{
	iSrtDecoder->Start();
	iSrtDecoder->Start();
	
	ERR_PRINTF1(_L("Panic expected, so failing"));
	return EFail;
	}

TVerdict RTestSrtDecoderStep0105::DoTestStepPreambleL()
	{
	InitializeTestStepL(EFalse, KSampleSubtitleSRTFilepath1);
	return EPass;
	}

TVerdict RTestSrtDecoderStep0105::DoTestStepPostambleL()
	{
	UnInitializeTestStep();
	return EPass;
	}
