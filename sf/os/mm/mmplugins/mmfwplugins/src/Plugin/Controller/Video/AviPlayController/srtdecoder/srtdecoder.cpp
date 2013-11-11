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

#include <bitdev.h> 
#include <fbs.h> 
#include <gdi.h> 
#include <utf.h> 

#include "subtitlesource.h"
#include "srtdecoder.h"
#include "srtframe.h"

// Define the separators for each subtitle frame in SRT file
_LIT8(KSrtFrameSeparator1, "\r\n\r\n" );
_LIT8(KSrtFrameSeparator2, "\n\n" );

// The length of the separators
const TInt KSrtFrameSeparator1Len = 4;
const TInt KSrtFrameSeparator2Len = 2;

// The margin size
const TInt KSrtBmpMarginWidth = 6;
const TInt KSrtBmpMarginHeight = 6;

// The width of subtitle region is the same as the width of the video region, while the height is 33 
// percent of the height of the video region. 
const TInt KSrtSubtitleRegionYFactor = 33;

// Define panic category
_LIT(KSrtSubtitleDecoderPanicCategory, "SrtSubDec");

// TSrtSubtitleDecoderPanics is an enumeration with the following entries:
// EBadCall indicates a bad call
enum TSrtSubtitleDecoderPanics
    {
    ESrtBadCall = 1,
    ESrtUnexpected,
    ESrtNotStarted,
    ESrtAlreadyStarted
    };


/** Constructor with a reference to the subtitle source 
    
@param aSubtitleSource a reference to the subtitle source
*/ 
CSrtSubtitleDecoder::CSrtSubtitleDecoder(MSubtitleSource& aSubtitleSource) : 
    iSource(aSubtitleSource)
    {
    }

EXPORT_C CSrtSubtitleDecoder::~CSrtSubtitleDecoder()
    {
    delete iFrameParser;
    iDecodeBuffer.Close();
    }

/** Allocates and constructs a srt subtitle decoder 
    
@param aSubtitleSource a reference to the subtitle source
@return A pointer to the new object. 
@leave If an error occurs, the method will leave with a system-wide error code. 
*/    
EXPORT_C CSrtSubtitleDecoder* CSrtSubtitleDecoder::NewL(MSubtitleSource& aSubtitleSource)
    {
    CSrtSubtitleDecoder* self = new(ELeave) CSrtSubtitleDecoder(aSubtitleSource);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/** Initialises the decoder
    
@leave If an error occurs, the method will leave with a system-wide error code.
*/
void CSrtSubtitleDecoder::ConstructL()
    {
    iFrameParser = CSrtFrame::NewL();
    iDecodeBuffer.Create(KSubtitleDecoderBufferLength);
    }
    
EXPORT_C TInt CSrtSubtitleDecoder::CalculateSubtitleRegion(const TRect& aScreenSize, TRect& aSubtitleRegion)
    {
    // TRect::IsEmpty is not used because the following test tests the minus values.
    if ((aScreenSize.iBr.iX < 0) || (aScreenSize.iBr.iY < 0) || 
        (aScreenSize.iTl.iX < 0) || (aScreenSize.iTl.iY < 0))
        {
        return KErrArgument;  
        }
    
    if ((aScreenSize.iBr.iX <= aScreenSize.iTl.iX) || (aScreenSize.iBr.iY <= aScreenSize.iTl.iY))
        {
        return KErrArgument; 
        }
    
    TPoint topleft;
    
    topleft.iX = aScreenSize.iTl.iX;
    topleft.iY = aScreenSize.iTl.iY + aScreenSize.Height() * (100 - KSrtSubtitleRegionYFactor) / 100; 
    
    aSubtitleRegion.SetRect(topleft, aScreenSize.iBr);
    
    return KErrNone;
    }

/** Calculate the number of the characters in the first line. The full words are supposed to be displayed 
unless the length of a word is bigger than one line space. 

@param aFont The font in current use.
@param aText The descriptor
@param aWidthInPixels The available width for character display.
@param aNumOfCharsToSkip The number of characters to skip, it can be 0, 1 (\n) or 2 (\r\n) 
@return The number of characters which will be able to be displayed without exceeding the specified width. 
The count starts from the beginning of the descriptor. 
*/   
TInt CSrtSubtitleDecoder::CalculateCharsInNextLine(const CFont &aFont, const TDesC &aText, TInt aWidthInPixels, TInt& aNumOfCharsToSkip)
    {
    const TChar KMMFSubtitleCharSpace(' ');
    const TChar KMMFSubtitleCharCR('\r');
    const TChar KMMFSubtitleCharLF('\n');
    TInt charsInLine = aFont.TextCount(aText, aWidthInPixels);
    aNumOfCharsToSkip = 0;
        
    if (charsInLine < aText.Length())
        {
        // For finding the last space of a descriptor which is not very big, using a simple loop 
        // shall be more efficient than using TLex
        for (TInt i = charsInLine; i > 0; i--)
            {
            if (KMMFSubtitleCharSpace == aText[i - 1])
                {
                charsInLine = i;
                break;
                }
            }
        }
    
    // Check \r\n or \n, the following loop is more efficient than TDesC::Find() in most cases 
    // because both \r\n and \n are handled in one loop. 
    for (TInt j = 0; j < charsInLine; j++)
        {
        if (KMMFSubtitleCharLF == aText[j])
            {
            if (j > 0)
                {
                // ignore \r if there is a \n
                if (KMMFSubtitleCharCR == aText[j - 1])
                    {
                    charsInLine = j;
                    aNumOfCharsToSkip = 2;
                    }
                else
                    {
                    charsInLine = j + 1;
                    aNumOfCharsToSkip = 1;
                    }
                }
            else
                {
                charsInLine = j + 1;
                aNumOfCharsToSkip = 1;
                }
            break;
            }
        }

    return charsInLine;
    }

/** Based on a descriptor containing a SRT subtitle frame, the function generates a single frame of 
subtitle animation for the current time and outputs the region of the subtitle frame that has been 
updated.

@param aContent The subtitle frame as a descriptor
@param aSubtitleFrame Reference to the current subtitle frame
@param aDirtyRegion The decoder outputs the region of the subtitle frame that has been updated. 
    i.e. the region that now contains new subtitle content.
@leave If an error occurs, the method will leave with a system-wide error code. 
*/
void CSrtSubtitleDecoder::DecodeSubtitleContentL(const TDesC8& aContent, CFbsBitmap& aSubtitleFrame, TRect& aDirtyRegion)
    {
    TSize bmpSize = aSubtitleFrame.SizeInPixels(); 
    TSize modifiedBmpSize;
    
    // the following value of "2" means two side of margin. 
    modifiedBmpSize.iHeight = bmpSize.iHeight - KSrtBmpMarginHeight * 2;
    modifiedBmpSize.iWidth = bmpSize.iWidth - KSrtBmpMarginWidth * 2;
    
    // test if the size of the bitmap is valid after taking margin into account   
    if ((modifiedBmpSize.iHeight > 0) && (modifiedBmpSize.iWidth > 0))
        {
        // prepare and populate the bitmap
        CFbsBitmapDevice* bmpDevice = CFbsBitmapDevice::NewL(&aSubtitleFrame);
        CleanupStack::PushL(bmpDevice);
        
        CFbsBitGc* bitmapContext = CFbsBitGc::NewL(); //Create a GC to draw to the bitmap
        bitmapContext->Activate(bmpDevice);
        
        CleanupStack::PushL(bitmapContext);
        
        bitmapContext->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        
        // Use a white color (R=255, G=255, B=255) with alpha value of 0. 
        TRgb transparentWhite(255, 255, 255, 0);
        bitmapContext->SetBrushColor(transparentWhite);
        
        bitmapContext->Clear(); 
        
        bitmapContext->SetDrawMode(CGraphicsContext::EDrawModePEN);
        
        bitmapContext->SetPenStyle(CGraphicsContext::ESolidPen);
        bitmapContext->SetPenColor(KRgbBlack);
        
        HBufC* content = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aContent);
        CleanupStack::PushL(content);
        
        // try to set up font
        CFont *font = NULL;
        TFontSpec fontSpec(KSrtTargetTypefaceName, KSrtTargetTypefaceHeightInTwips);
        User::LeaveIfError(bmpDevice->GetNearestFontToDesignHeightInTwips(font, fontSpec));
            
        bitmapContext->UseFont(font);

        TInt maxCharWidthInPixels = font->MaxCharWidthInPixels();
        TInt heightInPixels = font->HeightInPixels();
        TInt maxLines = modifiedBmpSize.iHeight / heightInPixels;    
        TInt maxCharsPerLine = modifiedBmpSize.iWidth / maxCharWidthInPixels; 
        TInt charsInLine = 0;
        TInt currentRow = 0;
        TPoint drawPosition;
        drawPosition.iX = KSrtBmpMarginWidth;
        drawPosition.iY = KSrtBmpMarginHeight + heightInPixels;
        TInt unvisitedChars = content->Length();
        TInt lineStartPos = 0;
        TInt numOfCharsToSkip = 0;
        
        if ((maxLines > 0) && (maxCharsPerLine > 0))
            {
            do
                {
                TPtrC ptrUnvisitedDesc = content->Right(unvisitedChars);                
                
                charsInLine = CalculateCharsInNextLine(*font, ptrUnvisitedDesc, modifiedBmpSize.iWidth, numOfCharsToSkip); 
                if (charsInLine > 0)
                    {
                    if (charsInLine > numOfCharsToSkip)
                        {
                        TPtrC ptrLine = content->Mid(lineStartPos, charsInLine - numOfCharsToSkip);
                        // try to draw text line by line
                        // Note that aPosition in DrawText indicates the bottom left of the first character. 
                        bitmapContext->DrawText(ptrLine, drawPosition);
                    
                        drawPosition.iY += heightInPixels; 
                        currentRow++;
                        }
                        
                    lineStartPos += charsInLine;
                    unvisitedChars -= charsInLine;
                    }
                }
            while(charsInLine > 0 && currentRow < maxLines);
            
            // update the dirty region
            aDirtyRegion.iTl.SetXY(KSrtBmpMarginWidth, KSrtBmpMarginHeight);

            TInt actualMaxCharsPerLine = 0;
            actualMaxCharsPerLine = Min(content->Length(), maxCharsPerLine);

            aDirtyRegion.iBr.SetXY(
                KSrtBmpMarginWidth + actualMaxCharsPerLine * maxCharWidthInPixels,
                KSrtBmpMarginHeight + currentRow * heightInPixels + font->FontMaxDescent());
            }
        else
            {
            // bitmap too small for the font
            User::Leave(KErrArgument);
            }

        bmpDevice->ReleaseFont(font);
        bitmapContext->DiscardFont();
        
        CleanupStack::PopAndDestroy(3, bmpDevice); // content, bitmapContext, bmpDevice
        }
    else
        {
        // bitmap too small or the length of content is 0
        User::Leave(KErrArgument);
        }
    }
    
/** Try to get the next valid frame 

@param aSearchByVideoPosition Indicate if search for the next valid frame matching aVideoPosition
@param aVideoPosition The video position in microsecond
@leave If an error occurs, the method will leave with a system-wide error code. 
*/
void CSrtSubtitleDecoder::GetNextValidFrameL(TBool aSearchByVideoPosition, TInt64 aVideoPosition)
    {
    TInt decoderBufferMaxLen = iDecodeBuffer.MaxLength();
    TInt errorFrame = KErrNone;
    TInt error = KErrNone;
    TInt sepLen = 0;
    TInt i = 0;
    TChar currentChar = 0;
    TBool frameFound = EFalse;
    
    if (iLastFrame)
    	{
    	User::Leave(KErrEof);
    	}
    
    while (!frameFound)
        {
        // if a frame is not available in iDecodeBuffer, fill iBuffer until CRLFCRLF is available in iBuffer
        TPtrC8 restOfData = iDecodeBuffer.Right(iDecodeBuffer.Length() - iDecodeBufferPosition);
        TInt sepPosition = 0;
        
        // try to extract a frame from iDecodeBuffer
        errorFrame = GetFrameSeparatorInfo(restOfData, sepLen, sepPosition);
        if (KErrNotFound != errorFrame)
            {
            User::LeaveIfError(errorFrame);
            }
            
        // if no frame can be extracted, read source until a frame is available or get an EOF error.
        while (KErrNotFound == errorFrame)
            {
            // move the unvisited data in decode buffer to the head before read from the source
            if (iDecodeBufferPosition != 0)
                {
                // move the unvisited data in decode buffer to the head
                if (iDecodeBufferPosition <= iDecodeBuffer.Length() - 1)
                    {
                    TPtrC8 newSubtitleFrame = iDecodeBuffer.Right(iDecodeBuffer.Length() - (iDecodeBufferPosition));
                    
                    // use iBuffer to hold the preserved data when rotating data in iDecodeBuffer
                    if (iBuffer.MaxLength() < newSubtitleFrame.Length())
                        {
                        // Some data in iBuffer make iDecodeBuffer hold at least one frame, and so
                        // iBuffer should always be larger than the preserved data in iDecodeBuffer
                        User::Panic(KSrtSubtitleDecoderPanicCategory, ESrtUnexpected);
                        }
                        
                    iBuffer.Copy(newSubtitleFrame);
                    iDecodeBuffer.Copy(iBuffer);
                    }

                iDecodeBufferPosition = 0;
                }
                
            error = iSource.GetBuffer(iBuffer);
            if ((KErrNone != error) && (KErrEof != error))
                {
                User::Leave(error);
                }
            
            if (iBuffer.Length() > 0)
                {
                TInt bufLen = iDecodeBuffer.Length();
                TInt newBufLen = iBuffer.Length();
                
                if (decoderBufferMaxLen < newBufLen + bufLen)
                    {
                    iDecodeBuffer.ReAllocL(newBufLen + bufLen);
                    decoderBufferMaxLen = iDecodeBuffer.MaxLength();
                    }
                iDecodeBuffer.Append(iBuffer);
                }
                
            if (KErrEof == error)
                {
                // reach the end of the subtitle source but cannot find new line separators.  Frame 
                // ends at EOF marker then.  No more frames can be read after this one.
                sepLen = 0;
                sepPosition = iDecodeBuffer.Length();
                errorFrame = KErrNone;
                if (iLastFrame)
                	{
                	User::Leave(KErrEof);
                	}
                iLastFrame = ETrue;
                }
            else
            	{
            	errorFrame = GetFrameSeparatorInfo(iDecodeBuffer, sepLen, sepPosition);
            	}
            
            if (KErrNotFound != errorFrame)
                {
                User::LeaveIfError(errorFrame);
                }
            }
        
        // extract a possible frame
        TPtrC8 subtitleFrameDescriptor = iDecodeBuffer.Mid(iDecodeBufferPosition, sepPosition);
        
        // Find the start of the next frame, and update the current position of the decode buffer by 
        // the following method: from the start of iDecodeBuffer, find the position of the first digit 
        // character after the first frame separator. 
        // Not the current position of the decode buffer is updated before parsing the frame, so the 
        // potential invalid frame will be skipped.  
        iDecodeBufferPosition += sepPosition + sepLen;
        TInt firstDigitPosFromCurrentPos = KErrNotFound;
        for (i = iDecodeBufferPosition; i < iDecodeBuffer.Length(); i++)
            {
            currentChar = iDecodeBuffer[i];
            if (currentChar.IsDigit())
                {
                firstDigitPosFromCurrentPos = i;
                break;
                }
            }
        if (KErrNotFound == firstDigitPosFromCurrentPos) 
            {
            iDecodeBufferPosition = iDecodeBuffer.Length() - 1;
            }
        else
            {
            iDecodeBufferPosition = firstDigitPosFromCurrentPos;
            }
        
        // parse the frame 
        TRAPD(parseError, iFrameParser->ParseL(subtitleFrameDescriptor, aSearchByVideoPosition, aVideoPosition));
        if (KErrNone == parseError)
            {
            frameFound = ETrue;
            if (!aSearchByVideoPosition)
                {
                // skip the past frames, such as 
                // 2\r\n00:00:02,000 --> 00:00:04,000\r\nhome\r\n\r\n
                // 1\r\n00:00:00,000 --> 00:00:02,000\r\nwelcome\r\n\r\n
                // 3\r\n00:00:04,000 --> 00:00:06,000\r\nagain
                if (iFrameParser->EndTime() < iEndTimeOfLastValidFrame)
                    {
                    frameFound = EFalse;
                    }  
                else
                    {
                    iEndTimeOfLastValidFrame = iFrameParser->EndTime();
                    }  
                }      
            else
                {
                iEndTimeOfLastValidFrame = iFrameParser->EndTime();
                }    
            }
        else
            {
            if (KErrArgument != parseError)
                {
                User::Leave(parseError);
                }
            }
        }
    }

/** The decoder generates a single frame of subtitle animation for the current time.   
The decoder maintains track of the video position. The video position can be updated 
via a call to MSubtitleDecoder::SetVideoPosition(). 

@param aSubtitleFrame Reference to the current subtitle frame
@param aDirtyRegion The decoder outputs the region of the subtitle frame that has been updated. 
    i.e. the region that now contains new subtitle content.
@param aDisplayTime The time in microseconds from the current stream position that the subtitle is due. 
    DevSubtitle will draw aSubtitleFrame after aDisplayTime has elapsed.
@param aDisplayDuration The time in microseconds that this frame should be displayed for. The CRP will clear 
    this frame after aDisplayFor microseconds
@leave If an error occurs, the method will leave with a system-wide error code.
*/   
EXPORT_C void CSrtSubtitleDecoder::GetNextFrameL(CFbsBitmap& aSubtitleFrame, TRect& aDirtyRegion, TTimeIntervalMicroSeconds& aDisplayTime, TTimeIntervalMicroSeconds& aDisplayDuration)
    {
    if (!iStarted)
        {
        User::Panic(KSrtSubtitleDecoderPanicCategory, ESrtNotStarted);
        }
        
    TTime currentTime;
    currentTime.UniversalTime();
    TTimeIntervalMicroSeconds elapsedTime = currentTime.MicroSecondsFrom(iStartSystemTime);
    TInt64 currentVideoPositionValue = elapsedTime.Int64() + iStartVideoPosition;
    
    if (iFirstSearch)
        {
        // skip the past frame
        GetNextValidFrameL(ETrue, currentVideoPositionValue);
            
        iFirstSearch = EFalse;
        }
    else
        {
        // get the next frame
        GetNextValidFrameL(EFalse, 0);
        }
    
    TInt64 elapsedDuration = 0;
    // if start time <= current video time, this frame needs to be started immediately.    
    if (iFrameParser->StartTime() <= currentVideoPositionValue)
        {
        aDisplayTime = 0;
        elapsedDuration = currentVideoPositionValue - iFrameParser->StartTime();
        }
    else
        {
        aDisplayTime = iFrameParser->StartTime() - currentVideoPositionValue;
        }
    
    // if end time < current video time, this duration becomes 0.
    if (iFrameParser->EndTime() <= currentVideoPositionValue)
        {
        aDisplayDuration = 0;
        }
    else
        {
        aDisplayDuration = iFrameParser->Duration() - elapsedDuration;
        }
            
    // decode the content into bitmap and get dirty region
    TPtrC8 ptrContent = iFrameParser->Content();
    DecodeSubtitleContentL(ptrContent, aSubtitleFrame, aDirtyRegion);
    }
    
EXPORT_C TLanguage CSrtSubtitleDecoder::SubtitleLanguageL()
    {
    return iSource.SubtitleLanguageL();
    }
    
EXPORT_C void CSrtSubtitleDecoder::GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aLanguages)
    {
    iSource.GetSupportedSubtitleLanguagesL(aLanguages);
    }
    
EXPORT_C void CSrtSubtitleDecoder::SetSubtitleLanguageL(TLanguage aLanguage)
    {
    iSource.SetSubtitleLanguageL(aLanguage);
    }
    
EXPORT_C void CSrtSubtitleDecoder::SetVideoPosition(const TTimeIntervalMicroSeconds& aPosition)
    {
    if (aPosition < 0)
        {
        User::Panic(KSrtSubtitleDecoderPanicCategory, ESrtBadCall);
        }
        
    iStartVideoPosition = aPosition.Int64();
    iLastFrame = EFalse;
    }
    
EXPORT_C void CSrtSubtitleDecoder::Start()
    {
    if (iStarted)
        {
        User::Panic(KSrtSubtitleDecoderPanicCategory, ESrtAlreadyStarted);
        }
    iSource.SetPosition(0);
    iStartSystemTime.UniversalTime();
    iStarted = ETrue;
    iDecodeBufferPosition = 0;
    iFirstSearch = ETrue;
    iEndTimeOfLastValidFrame = 0;
    iLastFrame = EFalse;
    }
    
EXPORT_C void CSrtSubtitleDecoder::Stop()
    {
    iSource.SetPosition(0);
    iDecodeBuffer.SetLength(0);
    iBuffer.SetLength(0);
    iStartVideoPosition = 0; 
    iStartSystemTime = 0;
    iStarted = EFalse;
    iDecodeBufferPosition = 0;
    iLastFrame = EFalse;
    }
    
/** Searches for the first occurrence of the SRT frame separator (\r\n\r\n or \n\n) within this 
    descriptor and return the length and the position of the separator.
        
@param aDes The buffer being searched
@param aSeparatorLength Output the length of the SRT frame separator
@param aPosition The offset of the SRT frame separator from the beginning of this descriptor's data.
@return A system-wide error code, KErrNotFound if the SRT frame separator cannot be found. 
*/
TInt CSrtSubtitleDecoder::GetFrameSeparatorInfo(const TDesC8& aDes, TInt& aSeparatorLength, TInt& aPosition)
    {
    TInt errCode = KErrNone;
    
    aPosition = aDes.Find(KSrtFrameSeparator1);
    if (KErrNotFound == aPosition)
        {
        aPosition = aDes.Find(KSrtFrameSeparator2);
        if (KErrNotFound == aPosition)
            {
            errCode = KErrNotFound;
            }
        else
            {
            aSeparatorLength = KSrtFrameSeparator2Len;
            }
        }
    else
        {
        aSeparatorLength = KSrtFrameSeparator1Len;
        }   
    
    return errCode;
    }




