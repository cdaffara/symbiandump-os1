// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "tsu_3gplibrary_parse_and_check.h"

_LIT( KAvcBaseline, "Baseline" );
_LIT( KAvcMain, "Main" );
_LIT( KAvcExtended, "Extended" );
_LIT( KAvcHigh, "High" );

const static TDesC* KAvcProfileNames[] = {
        &KAvcBaseline,
        &KAvcMain,
        &KAvcExtended,
        &KAvcHigh
    };

const TUint16 KAudioModeSet = 0x81ff;

C3GPLibParseAndCheckTypes::C3GPLibParseAndCheckTypes()
    {
    }

TVerdict C3GPLibParseAndCheckTypes::doTestStepPreambleL()
    {
    // ensure test always starts with clean results
    SetTestStepResult(EPass);
    SetTestStepError(KErrNone);
    
    #ifdef __WINSCW__
    _LIT(KTestFileName, "filepath");
    #else
    _LIT(KTestFileName, "filepathHw");
    #endif
    
    TPtrC inputFilePath;
    if (!GetStringFromConfig(ConfigSection(), KTestFileName, inputFilePath))
        {
        ERR_PRINTF1(_L("Error to read data from ini file."));
        SetTestStepResult(ETestSuiteError);
        return TestStepResult();
        }
    
    if (inputFilePath.Length() == 0)
        {
        ERR_PRINTF1(_L("Error to read input file path from ini file."));
        SetTestStepResult(ETestSuiteError);
        return TestStepResult();
        }
    
    TInt temp;
    if (!GetIntFromConfig(ConfigSection(), _L("audioType"), temp))
        {
        ERR_PRINTF1(_L("expected audio type not specified"));
        SetTestStepResult(ETestSuiteError);
        return TestStepResult();
        }
    else 
        {   
        iExpectedAudioType = (T3GPAudioType)temp;       
        }
    
    if (!GetIntFromConfig(ConfigSection(), _L("videoType"), temp))
        {
        ERR_PRINTF1(_L("expected video type not specified"));
        SetTestStepResult(ETestSuiteError);
        return TestStepResult();
        }
    else 
        {
        iExpectedVideoType = (T3GPVideoType)temp;       
        }
    
    
    iInputFilePath.CreateL(inputFilePath);
    
    iParser = C3GPParse::NewL();
    
    return TestStepResult();
    }

TVerdict C3GPLibParseAndCheckTypes::doTestStepL()
    {
    // ensure test always starts with clean results
    SetTestStepResult(EPass);
    SetTestStepError(KErrNone);
    
    INFO_PRINTF2(_L("ParseFileL: file=%S"), &iInputFilePath);               
    TRAPD(err, ParseAndCheckFileL(iInputFilePath));
    INFO_PRINTF2(_L("ParseFileL returns: err = %d"), err);
    if( err != KErrNone )
        {
        SetTestStepError( err );
        SetTestStepResult( EFail );
        iParser->Complete();
        }
    
    
    
    return TestStepResult();
    }

TVerdict C3GPLibParseAndCheckTypes::doTestStepPostambleL()
    {
    iInputFilePath.Close();
            
    delete iParser;
    
    return TestStepResult();
    }

void C3GPLibParseAndCheckTypes::ParseAndCheckFileL(const TDesC& aInputFile)
    {
    INFO_PRINTF1(_L("C3GPLibParseFileMainHigh::ParseFileL START"));  
    ASSERT(iParser);
    TInt err = 0;
    
    err = iParser->Open(aInputFile);
    
    if (err != KErrNone)
        {
        ERR_PRINTF2(_L("C3GPParse::Open() returns %d"), err);
        User::Leave(err);
        }
    
    //
    // Retrieve Video Properties
    //  
    T3GPVideoPropertiesBase* videoProperties = NULL;
    CleanupStack::PushL(videoProperties);
    
    RBuf8 videoDecoderSpecificInfo;
    CleanupClosePushL(videoDecoderSpecificInfo);
    
    TUint videoLengthInMs = 0;
        
    err = ParseVideoProperties(*iParser, videoDecoderSpecificInfo, videoProperties, videoLengthInMs);
    if ( err != KErrNone )
        {
        ERR_PRINTF2(_L("ParseVideoProperties failed: err = %d"), err);
        User::Leave(err);
        }
    
    //
    // Retrieve Audio Properties
    //
    T3GPAudioPropertiesBase* audioProperties = NULL;
    CleanupStack::PushL(audioProperties);   

    RBuf8 audioDecoderSpecificInfo;
    CleanupClosePushL(audioDecoderSpecificInfo);
    
    TUint audioLengthInMs = 0;
    err = ParseAudioProperties(*iParser, audioDecoderSpecificInfo, audioProperties, audioLengthInMs);
    if( err != KErrNone )
        {
        ERR_PRINTF2(_L("ParseAudioProperties failed: err = %d"), err);      
        User::Leave(err);
        }

    if (!videoProperties && !audioProperties)
        {
        ERR_PRINTF1(_L("File contains neither video nor audio data"));
        User::Leave(KErrNotFound);      
        }
    
    //
    // Check that types were as expected
    //
    if( audioProperties->iType != iExpectedAudioType )
        {
        INFO_PRINTF3(_L("Audio type check failed (expected %d, got %d)"), iExpectedAudioType, audioProperties->iType );
        SetTestStepResult( EFail );
        }
    
    if( videoProperties->iType != iExpectedVideoType )
        {
        INFO_PRINTF3(_L("Video type check failed (expected %d, got %d)"), iExpectedVideoType, videoProperties->iType );
        SetTestStepResult( EFail );
        }
    
    
    //
    // Clean up
    //
    CleanupStack::PopAndDestroy(4);  
                                    // audioDecoderSpecificInfo
                                    // audioProperties 
                                    // videoDecoderSpecificInfo 
                                    // videoProperties
    
    err = iParser->Complete();
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("aParser->Complete() failed"));
        User::Leave(err);
        }
    
    INFO_PRINTF1(_L("C3GPLibParseFileMainHigh::ParseFileL END"));
    }

TInt C3GPLibParseAndCheckTypes::ParseVideoProperties(C3GPParse& aParser,
                              RBuf8& aDecoderSpecificInfo,              
                              T3GPVideoPropertiesBase*& aProperties,
                              TUint& aLengthInMs)
    {
    ASSERT(iParser);
    
    T3GPVideoType type; 
    TReal frameRate;
    TUint avgBitRate;
    TSize size;
    TUint timeScale;    
    
    TInt err = aParser.GetVideoProperties(type, aLengthInMs, frameRate, avgBitRate, size, timeScale);
    if ( err != KErrNone )
        {
        ERR_PRINTF2(_L("aParser->GetVideoProperties failed: %d"), err);
        return err;
        }

    switch(type)
        {
        case E3GPMpeg4Video:
            {
            INFO_PRINTF1(_L("Video Type: Mpeg4"));          
            err = GetVideoDecoderSpecificInfo(aDecoderSpecificInfo);        
            if (err == KErrNone)
                {
                aProperties = new T3GPVideoPropertiesMpeg4Video(timeScale, size,
                        64000, avgBitRate, aDecoderSpecificInfo);
                if (!aProperties)
                    {
                    ERR_PRINTF1(_L("T3GPVideoPropertiesMpeg4Video allocation failed"));
                    err = KErrNoMemory;
                    }
                }
            break;          
            }
            
        case E3GPH263Profile0:
        case E3GPH263Profile3:
            {
            INFO_PRINTF1(_L("Video Type: H263"));           
            T3GPVideoPropertiesH263::TProfile profile = T3GPVideoPropertiesH263::EProfile0;
            if (type == E3GPH263Profile3)
                {
                profile = T3GPVideoPropertiesH263::EProfile3;
                }
            
            TInt videoLevel;
            err = iParser->GetH263VideoLevel(videoLevel);
            if( err != KErrNone )
                {
                ERR_PRINTF1(_L("aParser->GetH263VideoLevel() failed"));
                }
            else
                {           
                aProperties = new T3GPVideoPropertiesH263(timeScale, size, videoLevel, profile);
                if( !aProperties )
                    {
                    ERR_PRINTF1(_L("T3GPVideoPropertiesH263 allocation failed"));
                    err = KErrNoMemory;
                    }
                }
            break;          
            }
            
            
        case E3GPAvcProfileBaseline:
        case E3GPAvcProfileMain:
        case E3GPAvcProfileExtended:
        case E3GPAvcProfileHigh:
            {
            err = GetVideoDecoderSpecificInfo(aDecoderSpecificInfo);
            if (err == KErrNone)
                {
                INFO_PRINTF2(_L("Video Type: Avc Profile %S"), KAvcProfileNames[type-E3GPAvcProfileBaseline] );
                aProperties = new T3GPVideoPropertiesAvc(timeScale, size, aDecoderSpecificInfo);
                if (!aProperties)
                    {
                    ERR_PRINTF1(_L("T3GPVideoPropertiesAvc allocation failed"));
                    err = KErrNoMemory;
                    }
                else
                    {
                    // T3GPVideoPropertiesAvc defaults the video type to AVC baseline profile.
                    // Need to override that here because we want to check for the specific
                    // profile in this test.
                    aProperties->iType = type;
                    }
                }
            break;
            }
        
        case E3GPNoVideo:
            INFO_PRINTF1(_L("Video Type: None"));
            break;
            
        default:
            err = KErrNotSupported;
            break;
        }
    
    if( err == KErrNone )
        {
        INFO_PRINTF1(_L("**********"));
        INFO_PRINTF2(_L("Length In Miliseconds = %d"), aLengthInMs);
        INFO_PRINTF2(_L("Frame Rate = %f"), frameRate);
        INFO_PRINTF2(_L("Average Bit Rate = %d"), avgBitRate);
        INFO_PRINTF2(_L("Frame Width = %d"), size.iWidth);
        INFO_PRINTF2(_L("Frame Height = %d"), size.iHeight);
        INFO_PRINTF2(_L("Time Scale = %d"), timeScale);
        INFO_PRINTF1(_L("**********"));
        }
    return err;
    }

TInt C3GPLibParseAndCheckTypes::ParseAudioProperties(C3GPParse& aParser, 
                                                   RBuf8& aAudioDecoderSpecificInfo,
                                                   T3GPAudioPropertiesBase*& aAudioProperties,
                                                   TUint& aLength)
    {
    ASSERT(iParser);
    
    T3GPAudioType type; 
    TInt audioFPS; 
    TUint audioAvgBitRate; 
    TUint timeScale;    
    
    TInt err = aParser.GetAudioProperties(type, aLength, audioFPS, audioAvgBitRate, timeScale);
    if( err != KErrNone )
        {
        ERR_PRINTF2(_L("GetAudioProperties() failed: %d"), err);
        return err;
        }

    switch(type)
        {
        case E3GPMpeg4Audio:
            {
            INFO_PRINTF1(_L("Audio Type: Mpeg4"));
            err = GetAudioDecoderSpecificInfo(aAudioDecoderSpecificInfo);
            if (err == KErrNone)
                {
                aAudioProperties = new T3GPAudioPropertiesMpeg4Audio(timeScale, aAudioDecoderSpecificInfo);
                if( !aAudioProperties )
                    {
                    ERR_PRINTF1(_L("T3GPAudioPropertiesMpeg4Audio allocation failed"));
                    err = KErrNoMemory;
                    }
                }
            break;
            }
            
        case E3GPQcelp13K:
            {
            INFO_PRINTF1(_L("Audio Type: Qcelp13K"));
            T3GPQcelpStorageMode mode;
            err = iParser->GetQcelpStorageMode(mode);
            if (err != KErrNone)
                {
                ERR_PRINTF1(_L("GetQcelpStorageMode failed"));
                }
            else 
                {
                if( mode == E3GPMP4AudioDescriptionBox)
                    {
                    err = GetAudioDecoderSpecificInfo(aAudioDecoderSpecificInfo);
                    aAudioProperties = new T3GPAudioPropertiesQcelp(timeScale, audioFPS, aAudioDecoderSpecificInfo);
                    }
                else
                    {
                    aAudioProperties = new T3GPAudioPropertiesQcelp(timeScale, audioFPS);
                    }
                
                if( !aAudioProperties )
                    {
                    ERR_PRINTF1(_L("T3GPAudioPropertiesQcelp allocation failed"));
                    err = KErrNoMemory;
                    }
                }
            break;
            }
            
        case E3GPAmrNB:
            {
            INFO_PRINTF1(_L("Audio Type: AMR NB"));
            aAudioProperties = new T3GPAudioPropertiesAmr(timeScale, audioFPS, KAudioModeSet, T3GPAudioPropertiesAmr::EAmrNB);
            if( !aAudioProperties )
                {
                ERR_PRINTF1(_L("T3GPAudioPropertiesAmr allocation failed"));
                err = KErrNoMemory;
                }
            break;
            }
            
        case E3GPAmrWB:
            {
            INFO_PRINTF1(_L("Audio Type: AMR WB"));
            aAudioProperties = new T3GPAudioPropertiesAmr(timeScale, audioFPS, KAudioModeSet, T3GPAudioPropertiesAmr::EAmrWB);
            if( !aAudioProperties )
                {
                ERR_PRINTF1(_L("T3GPAudioPropertiesAmr allocation failed"));
                err = KErrNoMemory;
                }
            break;
            }
            
        case E3GPNoAudio:
            INFO_PRINTF1(_L("Audio Type: None"));           
            break;
            
        default:
            INFO_PRINTF1(_L("Audio Type: Unrecognized!"));
            err = KErrNotSupported;
            break;
        }
    
    return err;
    }

TInt C3GPLibParseAndCheckTypes::GetAudioDecoderSpecificInfo(RBuf8& aBuffer)
    {
    ASSERT(iParser);
    
    TInt size;
    TInt err = iParser->GetAudioDecoderSpecificInfoSize(size);
    if( err != KErrNone )
        {
        ERR_PRINTF2(_L("GetAudioDecoderSpecificInfoSize() failed: %d"), err);
        return err;
        }

    err = aBuffer.Create(size);
    if( err != KErrNone )
        {
        ERR_PRINTF1(_L("Create buffer failed"));
        return err;
        }

    err = iParser->GetAudioDecoderSpecificInfo(aBuffer);
    if( err != KErrNone )
        {
        ERR_PRINTF2(_L("GetAudioDecoderSpecificInfo() failed: %d"), err);
        }
    
    return err;
    }

TInt C3GPLibParseAndCheckTypes::GetVideoDecoderSpecificInfo(RBuf8& aBuffer)
    {
    TInt size;
    TInt err = iParser->GetVideoDecoderSpecificInfoSize(size);
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("GetVideoDecoderSpecificInfoSize() failed"));
        return err;
        }
    
    err = aBuffer.Create(size);
    if (err != KErrNone)
        {
        ERR_PRINTF1(_L("Create buffer failed"));
        return err;
        }

    err = iParser->GetVideoDecoderSpecificInfo(aBuffer);
    if( err != KErrNone )
        {
        ERR_PRINTF1(_L("GetVideoDecoderSpecificInfo() failed"));
        }
    
    return err;
    }
