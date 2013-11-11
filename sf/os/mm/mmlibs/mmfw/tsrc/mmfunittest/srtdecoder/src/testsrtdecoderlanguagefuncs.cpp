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
#include "testsrtdecoderlanguagefuncs.h"

_LIT( KSampleSubtitleSRTFilepath1, "c:\\mm\\subtitle1.srt" );

RTestSrtDecoderStep0100::RTestSrtDecoderStep0100()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTDECODER-U-0100-HP");
	}
	
TVerdict RTestSrtDecoderStep0100::DoTestStepPreambleL()
    {
    InitializeTestStepL(EFalse, KSampleSubtitleSRTFilepath1);
    
    return EPass;
    }

TVerdict RTestSrtDecoderStep0100::DoTestStepPostambleL()
    {
    UnInitializeTestStep();
    
    return EPass;
    }

TVerdict RTestSrtDecoderStep0100::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = EPass;
	
	result = TestSubtitleLanguage();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0100::TestGetSubtitleLanguage failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
        
    result = TestSetSubtitleLanguage();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0100::TestSetSubtitleLanguage failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
        
    result = TestGetSupportedSubtitleLanguages();
    if (EPass != result)
        {
        ERR_PRINTF1(_L("Error - RTestSrtDecoderStep0100::TestGetSubtitleLanguageSupport failed. "));
        INFO_PRINTF1(_L("Exit DoTestStepL"));
	    return result;
        }
       
    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}


TVerdict RTestSrtDecoderStep0100::TestSubtitleLanguage()
    {
    TInt err = KErrNone;
    TVerdict result = EPass;
    
    TRAP(err, iSrtDecoder->SubtitleLanguageL());
    if (err != KErrNotSupported)
        {
        ERR_PRINTF2(_L("Error - MSubtitleDecoder::SubtitleLanguageL returned %d. "), err);
        result = EFail;
        }
    
    return result;
    }

TVerdict RTestSrtDecoderStep0100::TestSetSubtitleLanguage()
    {
    TInt err = KErrNone;
    TLanguage leng = ELangEnglish;
    TVerdict result = EPass;
    
    TRAP(err, iSrtDecoder->SetSubtitleLanguageL(leng));
    if (err != KErrNotSupported)
        {
        ERR_PRINTF2(_L("Error - MSubtitleDecoder::SetSubtitleLanguageL returned %d. "), err);
        result = EFail;
        }
    
    return result;
    }
	
TVerdict RTestSrtDecoderStep0100::TestGetSupportedSubtitleLanguages()
    {
    TVerdict result = EPass;
    TInt err = KErrNone;
    RArray<TLanguage> data;
    
    TRAP(err, iSrtDecoder->GetSupportedSubtitleLanguagesL(data));
    if (err != KErrNotSupported)
        {
        ERR_PRINTF2(_L("Error - MSubtitleDecoder::GetSubtitleLanguageSupportL returned %d. "), err);
        result = EFail;
        }
    
    return result;
    }
    

