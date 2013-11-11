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

#include "srtreader.h"
#include "testsrtreader.h"

_LIT( KSampleSubtitleSRTFilepath, "c:\\mm\\subtitle1.srt" );
_LIT( KSampleSubtitleTempFilepath, "c:\\mm\\subtitle1t.srt" );
_LIT( KSampleSubtitleBogusFilepath, "c:\\mm\\bogussubtitle.srt" );

// Implementation of RTestSrtReaderStep0001

RTestSrtReaderStep0001::RTestSrtReaderStep0001()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTREADER-U-0001-HP");
	}


TVerdict RTestSrtReaderStep0001::DoTestStepPreambleL()
    {
    __MM_HEAP_MARK;
    User::LeaveIfError(iFsSession.Connect());
    
    return EPass;
    }

TVerdict RTestSrtReaderStep0001::DoTestStepPostambleL()
    {
    if (BaflUtils::FileExists(iFsSession, KSampleSubtitleTempFilepath))
        {
        BaflUtils::DeleteFile(iFsSession, KSampleSubtitleTempFilepath);
        }
    iFsSession.Close();
    
    __MM_HEAP_MARKEND;
    return EPass;
    }
    
TVerdict RTestSrtReaderStep0001::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TVerdict result = EPass;
	
    for(TInt bufSize = 0; bufSize <= 640 ; bufSize += 32)
        {
        RFile resultSource;
        TInt err = resultSource.Create(iFsSession, KSampleSubtitleTempFilepath, EFileRead | EFileWrite);
        if (KErrAlreadyExists == err)
            {
            User::LeaveIfError(resultSource.Open(iFsSession, KSampleSubtitleTempFilepath, EFileRead | EFileWrite));
            }
        else
            {
            User::LeaveIfError(err);
            }
        
        CleanupClosePushL(resultSource);
        
        HBufC8 *buf = HBufC8::NewL(bufSize);
        CleanupStack::PushL(buf);
        
        CSrtReader* srtReader = CSrtReader::NewL(KSampleSubtitleSRTFilepath);
        CleanupStack::PushL(srtReader);

        TInt error = KErrNone;
        
        while(KErrNone == error)
            {
            TPtr8 ptr8 = buf->Des();
            error = srtReader->GetBuffer(ptr8);
            
            if (ptr8.Length() > 0)
                {
                User::LeaveIfError(resultSource.Write(ptr8));
                }
            }
         
        CleanupStack::PopAndDestroy(srtReader);
        CleanupStack::PopAndDestroy(buf);
        CleanupStack::PopAndDestroy(&resultSource);
        
        if (bufSize == 0)
            {
            if (error != KErrArgument)
                {
                ERR_PRINTF3(_L("Error - CSrtReader::GetBuffer returned %d when the buffer is set to %d byte"), error, bufSize);
                result = EFail;
                }
            }
        else
            {
            if (error != KErrEof)
                {
                ERR_PRINTF3(_L("Error - CSrtReader::GetBuffer returned %d when the buffer is set to %d bytes"), error, bufSize);
                result = EFail;
                }
            else
                {
                // compare KSampleSubtitleSRTFilepath with KSampleSubtitleTempFilepath
                if (!RTestSrtDecoderStep::CompareFilesL(iFsSession, KSampleSubtitleSRTFilepath, KSampleSubtitleTempFilepath))
                    {
                    ERR_PRINTF1(_L("Error - the result file does not match the original file. "));
                    result = EFail;
                    }
                }
            }
        
        if (BaflUtils::FileExists(iFsSession, KSampleSubtitleTempFilepath))
            {
            User::LeaveIfError(BaflUtils::DeleteFile(iFsSession, KSampleSubtitleTempFilepath));  
            }
        }

    INFO_PRINTF1(_L("Exit DoTestStepL"));
	return result;
	}


// Implementation of RTestSrtReaderStep0100

RTestSrtReaderStep0100::RTestSrtReaderStep0100()
	{
	iTestStepName = _L("MM-MMF-SUBTITLE-SRTREADER-U-0100-HP");
	}
	
TVerdict RTestSrtReaderStep0100::DoTestStepL()
	{
	INFO_PRINTF1(_L("Enter DoTestStepL"));
	TInt expectedErr = KErrNotFound;
	
	__MM_HEAP_MARK
	
	TVerdict result = EPass;
	
	TRAPD(err, CSrtReader* srtReader = CSrtReader::NewL(KSampleSubtitleBogusFilepath));
	
    if (err != expectedErr)
        {
        ERR_PRINTF2(_L("Error - CSrtReader constructor returned %d for a bogus srt source. "), err);
        result = EFail;
        }

    __MM_HEAP_MARKEND;
    
    INFO_PRINTF1(_L("Exit DoTestStepL"));
    
	return result;
	}



