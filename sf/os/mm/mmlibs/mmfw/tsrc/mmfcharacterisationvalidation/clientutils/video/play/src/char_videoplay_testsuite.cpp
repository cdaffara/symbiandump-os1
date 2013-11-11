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
//

#include <e32base.h>

#include "char_videoplay_testsuite.h"
#include "play_teststep.h"
#include "play_teststep_apr.h"


// Entry function - create a test suite object
EXPORT_C CTestSuiteCharVideoPlay* NewTestSuiteL() 
    { 
	return CTestSuiteCharVideoPlay::NewL();
    }
    
CTestSuiteCharVideoPlay* CTestSuiteCharVideoPlay::NewL() 
    { 
	CTestSuiteCharVideoPlay* self = new (ELeave) CTestSuiteCharVideoPlay;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
    }

CTestSuiteCharVideoPlay::CTestSuiteCharVideoPlay()
	{
	iSuiteName = _L("CHAR_VIDEOPLAY");
	}

CTestSuiteCharVideoPlay::~CTestSuiteCharVideoPlay()
	{
	}

void CTestSuiteCharVideoPlay::InitialiseL( void )
	{
	
#ifdef __WINS__
    // Check that the window server is ready for us
    // This is so that the call to CDirectScreenAccess::StartL does not leave with KErrNotReady
    if (!WaitUntilWindowServerReadyL())
        {
        INFO_PRINTF1(_L("TestSuite Error : ERROR Receiving Window Server Redraw event"));
        return;
        }
#endif // __WINS__

	AddTestStepL(RVPlyrOpenFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0001"), _L("SectionOne")));
	AddTestStepL(RVidPlyrOpnFileInvFmtTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0002"), _L("SectionOne"), _L("jpgfile"), KErrNotSupported));
	AddTestStepL(RVidPlyrOpnFileInvFmtTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0002a"), _L("SectionOne"), _L("textfile"), KErrNotSupported));
	AddTestStepL(RVidPlyrOpnFileInvFmtTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0002b"), _L("SectionOne"), _L("xvidmp3file"), KErrNotSupported));
	AddTestStepL(RVidPlyrOpnFileInvFmtTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0003"), _L("SectionOne"), _L("corruptfilename"), KErrNotSupported));				
	AddTestStepL(RVidPlyrOpnFileCntrlrUidTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0004"), _L("SectionOne")));
	AddTestStepL(RVidPlyrOpnFileInvCntrlrUidTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0005"), _L("SectionOne")));
	AddTestStepL(RVideoPlayTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0006"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPlayAgainTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0007"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPlayFileWithoutOpenTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0008"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPlayWithoutPrepareTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0009"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPlayFileStEndptsTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0010"), _L("SectionOne")));
	AddTestStepL(RVidPlyrStopVidPlayTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0011"), _L("SectionOne")));
	AddTestStepL(RVidPlyrStopVidPlayInPauseTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0012"), _L("SectionOne")));
	AddTestStepL(RVidPlyrStopAftrOpnFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0013"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPauseVidPlayFilPlngTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0014"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPauseVidPlayAftrOpnFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0015"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPauseVidPlayAftrStopTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0016"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPauseVidPlayB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0017"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVolAftrOpngVidFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0018"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVolDuringPlayTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0019"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVolInPauseTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0020"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVolInStopTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0021"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVolAftrOpngVidOnlyFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0022"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVolWithInvPmtsTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0023"), _L("SectionOne")));
	AddTestStepL(RVidPlyrMaxVolInDfrntStTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0024"), _L("SectionOne")));
	AddTestStepL(RVidPlyrAudEnbldInDfrntStTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0025"), _L("SectionOne")));
	AddTestStepL(RVidPlyrAudEnbldInPausedStTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0025a"), _L("SectionOne")));
	AddTestStepL(RVidPlyrAudEnbldBfrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0026"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetVolAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0027"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetVolB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0028"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetDrnAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0029"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetDrnPlyngTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0030"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetDrnPausedTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0031"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetDrnStpdTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0032"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetDrnB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0033"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPosAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0034"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPosInPlyngTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0035"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPosPausedTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0036"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPosInStpdStTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0037"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPosB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0038"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPosInvPmtsAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0039"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryAudTypVidFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0040"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryAudTypB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0044"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryAudTypVidOnlyFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0045"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryAudBitRateVidFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0046"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryAudBitRateB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0050"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryAudBitRateVidOnlyFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0051"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryVidBitRateVidFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0052"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryVidBitRateB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0056"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryVidFmtMimeTypTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0057"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryVidFrameSizTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0060"), _L("SectionOne")));
	AddTestStepL(RVidPlyrQryVidFrameSizB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0063"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVidFrameRateTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0064"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetVidFrameRateTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0065"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetVidFrameRateB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0069"), _L("SectionOne")));
	AddTestStepL(RVidPlyrRfrshFrmPlyngTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0070"), _L("SectionOne")));
	AddTestStepL(RVidPlyrRfrshFrmB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0072"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetCurrentFrmPlyngTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0073"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetCurrentFrmB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0075"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPrtyAftrOpnFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0076"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPrtyB4OpnFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0078"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetBalanceAftrOpnFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0081"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetBalanceB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0085"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetBalanceAftrOpngVidOnlyFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0086"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetBalanceWthoutSetAftrOpnFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0087"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetBalanceB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0091"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetBalanceAftrOpngVidOnlyFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0092"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetRotationAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0093"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetRotationB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0096"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetRotationInStopTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0096a"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetScaleFactorAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0097"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetScaleFactorInStopTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0100"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetScaleFactorWthOutSetAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0101"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetScalingFactorB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0102"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetCropRegionAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0103"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetCropRegionB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0104"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetCropRegionWthOutsetAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0105"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetCropRegionWthOutSetB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0106"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetNumMetaDataAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0107"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetNumberOfMetadataB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0111"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetMetaDataEntryAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0112"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetMetadataEntryB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0116"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetCntrlrImplmtnInfoAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0117"), _L("SectionOne")));
	AddTestStepL(RVidPlyrGetCntrlrInfoB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0121"), _L("SectionOne")));
	AddTestStepL(RVidPlyrStartDrctScrnAcsAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0122"), _L("SectionOne")));
	AddTestStepL(RVidPlyrStopNStartDrctScrnAcsB4OpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0123"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetDsplyWndowAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0124"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetDsplyWndowInvDimnAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0136"), _L("SectionOne")));
	
	// APR test cases 
	AddTestStepL(RTestVclntAprRegAftrOpenAndB4Play::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0125"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprRegBeforeOpen::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0127"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprRegWithUnSupptdEvntUid::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0128"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprRegMultpleTimes::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0129"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprCancelNtfnAftrOpen::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0130"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprCancelNtfnWthInvalidEvntUid::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0132"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprWillResumePlay::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0133"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprCancelNtfnWthRegnAftrOpen::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0134"), _L("SectionAPR")));
	AddTestStepL(RTestVclntAprCancelNtfnMultTimesAftrOpen::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0135"), _L("SectionAPR")));
	
	// Incongruence test cases
	AddTestStepL(RVPlyrCrtMltpleTimesTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0150"), _L("SectionOne")));
	AddTestStepL(RVidPlyrOpnFileMultTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0151"), _L("SectionOne")));
	AddTestStepL(RVidPlyrCloseFileWithoutopngTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0152"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPrepareMultTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0153"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPlayCorruptedFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0154"), _L("SectionOne")));
	AddTestStepL(RVidPlyrOpnFileWthOutExtnTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0155"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPlayFileInvStEndptsTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0156"), _L("SectionOne")));
	AddTestStepL(RVidPlyrStopPlayAfterComplnOfPlayTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0157"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetVidFrameRateInvValTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0158"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetPrtyWthInvValAftrOpnFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0159"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetBalanceWthInvValAftrOpnFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0160"), _L("SectionOne")));
	AddTestStepL(RVidPlyrPlayAudioFileTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0161"), _L("SectionOne")));	
	AddTestStepL(RVidPlyrSetNGetRotationWthInvValAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0162"), _L("SectionOne")));
	AddTestStepL(RVidPlyrSetNGetScaleFactorWthInvValAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0163"), _L("SectionOne")));	
	AddTestStepL(RVidPlyrSetNGetCropRegionWithInvValAftrOpngFilTest::NewL( _L("MM-MMF-VCLNT-CHRTZ-I-0164"), _L("SectionOne")));			
	}

#ifdef __WINS__
/**
  * Wait synchronously for the first WS redraw event...
*/  
TBool CTestSuiteCharVideoPlay::WaitUntilWindowServerReadyL()
    {
    InitWservL();

    TWsRedrawEvent event;
    TRequestStatus status;
    iWs.RedrawReady(&status);
    User::WaitForRequest(status);
    iWs.GetRedraw(event);

    DeInitWserv();

    return (status.Int() == KErrNone);
    }

/**
 * InitWservL
*/
void CTestSuiteCharVideoPlay::InitWservL()
    {
    TInt err = iWs.Connect();
    if (err != KErrNone)
        {
        // Access violation if ws is null
        INFO_PRINTF1(_L("TestSuite error: Cannot test, no window server available"));
        User::Leave(err);
        }

    iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
    User::LeaveIfError(iScreen->Construct()); // and complete its construction

    iRootWindow = RWindowGroup(iWs);
    User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

    iWindow = new(ELeave) RWindow(iWs);
    User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
    iWindow->SetExtent(TPoint(0,0), TSize(400,200));
    iWindow->SetVisible(ETrue);
    iWindow->Activate();
    iWs.Flush();
    }

/**
 * DeInitWserv
*/
void CTestSuiteCharVideoPlay::DeInitWserv()
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
    iWs.Flush();
    iWs.Close();
    }
#endif // __WINS__
