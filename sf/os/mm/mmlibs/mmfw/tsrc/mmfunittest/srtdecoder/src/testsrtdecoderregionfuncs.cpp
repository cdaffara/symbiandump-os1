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

#include "srtdecoder.h"
#include "srtreader.h"
#include "testsrtdecoderregionfuncs.h"

_LIT( KSampleSubtitleSRTFilepath1, "c:\\mm\\subtitle1.srt" );

// Implementation of RTestSrtDecoderStep0001

RTestSrtDecoderStep0001::RTestSrtDecoderStep0001()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0001-HP");
	}
	
TVerdict RTestSrtDecoderStep0001::DoTestStepPreambleL()
    {
    InitializeTestStepL(EFalse, KSampleSubtitleSRTFilepath1);
    
    return EPass;
    }
	
TVerdict RTestSrtDecoderStep0001::DoTestStepPostambleL()
    {
    UnInitializeTestStep();
    
    return EPass;
    }
	
TVerdict RTestSrtDecoderStep0001::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = TestCalculateSubtitleRegion();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0001::TestCalculateSubtitleRegion failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
        
    result = TestSetVideoPosition();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0001::TestSetVideoPosition failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
        
    result = TestStartStop();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0001::TestStartStop failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
     
    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}


TVerdict RTestSrtDecoderStep0001::TestCalculateSubtitleRegion()
    {
    TVerdict result = EPass;
    const TInt KSrtSubtitleRegionYFactor = 33;
    TRect inputRegion;
    inputRegion.iTl.iX = 1;
    inputRegion.iTl.iY = 1;
    inputRegion.iBr.iX = 240;
    inputRegion.iBr.iY = 160;
    TInt deltaValue = 24;
    
    TRect outputRegion;
    TRect expectedRegion;
    
    while((inputRegion.iBr.iX > inputRegion.iTl.iX) && (inputRegion.iBr.iY > inputRegion.iTl.iY))
        {
        TInt error = iSrtDecoder->CalculateSubtitleRegion(inputRegion, outputRegion);
        if (KErrNone != error)
            {
            return EFail;
            }
    
        expectedRegion.SetRect(
            inputRegion.iTl.iX, 
            inputRegion.iTl.iY + inputRegion.Height() * (100 - KSrtSubtitleRegionYFactor) / 100,
            inputRegion.iBr.iX,
            inputRegion.iBr.iY);
        
        if (expectedRegion != outputRegion)
            {
            ERR_PRINTF5(_L("Error - MSubtitleDecoder::CalculateSubtitleRegion failed to handle rect(%d, %d, %d, %d). "), 
                inputRegion.iTl.iX, inputRegion.iTl.iY, inputRegion.iBr.iX, inputRegion.iBr.iY);
            return EFail;
            }
        
        inputRegion.iTl.iX += deltaValue;
        inputRegion.iTl.iY += deltaValue;
        inputRegion.iBr.iX -= deltaValue;
        inputRegion.iBr.iY -= deltaValue;
        }
    
    return result;
    }

TVerdict RTestSrtDecoderStep0001::TestSetVideoPosition()
    {
    TVerdict result = EPass;
    
    TInt64 val = 0;
    
    for (TInt i = 0; i < 20; i++)
        {
        val += 10000000;
        iSrtDecoder->SetVideoPosition(val);
        }
     
    return result;
    }
    
TVerdict RTestSrtDecoderStep0001::TestStartStop()
    {
    TVerdict result = EPass;
    
    for (TInt i = 0; i < 10; i++)
        {
        iSrtDecoder->Start();
        iSrtDecoder->Stop();
        }
    
    return result;
    }
    
// Implementation of RTestSrtDecoderStep0102
	
RTestSrtDecoderStep0102::RTestSrtDecoderStep0102()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0102-HP");
	}

TVerdict RTestSrtDecoderStep0102::DoTestStepPreambleL()
    {
    InitializeTestStepL(EFalse, KSampleSubtitleSRTFilepath1);
    
    return EPass;
    }
	
TVerdict RTestSrtDecoderStep0102::DoTestStepPostambleL()
    {
    UnInitializeTestStep();
    
    return EPass;
    }
    
TVerdict RTestSrtDecoderStep0102::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = EPass;
	
    result = TestCalculateSubtitleRegion();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0102::TestCalculateSubtitleRegion failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
        
    result = TestStartStop();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0102::TestStartStop failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
     
    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}

TVerdict RTestSrtDecoderStep0102::TestCalculateSubtitleRegion()
    {
    TInt error = KErrNone;
    TInt expectedErr = KErrArgument;
    TRect inputRegion;
    TRect outputRegion;
    const TInt KTestSubtitleRegionValueNum = 4;
    const TInt KTestSubtitleInvalidRegionTypeNum = 5;
    const TInt KTestSubtitleRegionValues[KTestSubtitleRegionValueNum] = {10, 20, 20, 200};
    TInt testSubtitleRegionFactors[KTestSubtitleRegionValueNum] = {0, 0, 0, 0};
    const TInt KTestSubtitleRegionFactorIndex[KTestSubtitleInvalidRegionTypeNum][KTestSubtitleRegionValueNum] = 
        {
            // test inverted rect
            {2, 3, 0, 1},
            // test inverted rect
            {2, 3, 1, 0},
            // test a vertical line
            {1, 0, 2, 3},
            // test a horizontal line
            {0, 1, 3, 2},
            // test a point 
            {0, 0, 0, 0}
        };
    TInt i = 0;
    
    // test 0 values
    for (i = 0; i < KTestSubtitleRegionValueNum; i++)
        {
        inputRegion.SetRect(
            KTestSubtitleRegionValues[0] * testSubtitleRegionFactors[0], 
            KTestSubtitleRegionValues[1] * testSubtitleRegionFactors[1], 
            KTestSubtitleRegionValues[2] * testSubtitleRegionFactors[2], 
            KTestSubtitleRegionValues[3] * testSubtitleRegionFactors[3]);
        error = iSrtDecoder->CalculateSubtitleRegion(inputRegion, outputRegion);
        
        if (expectedErr != error)
            {
            ERR_PRINTF5(_L("Error - MSubtitleDecoder::CalculateSubtitleRegion failed to handle invalid rect(%d, %d, %d, %d). "), 
                inputRegion.iTl.iX, inputRegion.iTl.iY, inputRegion.iBr.iX, inputRegion.iBr.iY);
            return EFail;
            }
            
        // initialize factors
        testSubtitleRegionFactors[i] = 1;
        }
        
    // test minus values
    for (i = 0; i < KTestSubtitleRegionValueNum; i++)
        {
        // in each loop, the factors will have different number of minus value.
        testSubtitleRegionFactors[i] = -1;
        inputRegion.SetRect(
            KTestSubtitleRegionValues[0] * testSubtitleRegionFactors[0], 
            KTestSubtitleRegionValues[1] * testSubtitleRegionFactors[1], 
            KTestSubtitleRegionValues[2] * testSubtitleRegionFactors[2], 
            KTestSubtitleRegionValues[3] * testSubtitleRegionFactors[3]);
        error = iSrtDecoder->CalculateSubtitleRegion(inputRegion, outputRegion);
        
        if (expectedErr != error)
            {
            ERR_PRINTF5(_L("Error - MSubtitleDecoder::CalculateSubtitleRegion failed to handle invalid rect(%d, %d, %d, %d). "), 
                inputRegion.iTl.iX, inputRegion.iTl.iY, inputRegion.iBr.iX, inputRegion.iBr.iY);
            return EFail;
            }
        }
    
    // Test various invalid rectangles
    for (i = 0; i < KTestSubtitleInvalidRegionTypeNum; i++)
        {
        inputRegion.SetRect(
            KTestSubtitleRegionValues[KTestSubtitleRegionFactorIndex[i][0]], 
            KTestSubtitleRegionValues[KTestSubtitleRegionFactorIndex[i][1]], 
            KTestSubtitleRegionValues[KTestSubtitleRegionFactorIndex[i][2]], 
            KTestSubtitleRegionValues[KTestSubtitleRegionFactorIndex[i][3]]);
        error = iSrtDecoder->CalculateSubtitleRegion(inputRegion, outputRegion);
        if (expectedErr != error)
            {
            ERR_PRINTF5(_L("Error - MSubtitleDecoder::CalculateSubtitleRegion failed to handle invalid rect(%d, %d, %d, %d). "), 
                inputRegion.iTl.iX, inputRegion.iTl.iY, inputRegion.iBr.iX, inputRegion.iBr.iY);
            return EFail;
            }
        }
        
    return EPass;
    }
    
TVerdict RTestSrtDecoderStep0102::TestStartStop()
    {
    TVerdict result = EPass;
    
    // Do not call Start before Stop because it will panic
    // Call Stop() multiple times with and without calling Start() first
    for (TInt i = 0; i < 10; i++)
        {
        if ((0 == i) || (i % 4))
            {
            iSrtDecoder->Stop();
            }
        else
            {
            iSrtDecoder->Start();
            }
        }
        
    return result;
    }



// Implementation of RTestSrtDecoderStep0104
	
RTestSrtDecoderStep0104::RTestSrtDecoderStep0104()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0104-HP");
	}

TVerdict RTestSrtDecoderStep0104::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = EPass;
	
	iSrtReader = CSrtReader::NewL(KSampleSubtitleSRTFilepath1);
    
    User::LeaveIfError(iRbsSession.Connect());
    CleanupStack::PushL(&iRbsSession);
    iSrtDecoder = CSrtSubtitleDecoder::NewL(*iSrtReader);
	
    iSrtDecoder->SetVideoPosition(-100);
     
    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}








