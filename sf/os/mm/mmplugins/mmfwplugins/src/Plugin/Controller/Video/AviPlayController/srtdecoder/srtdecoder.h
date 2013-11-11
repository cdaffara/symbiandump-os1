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

/**
 @file
 @internalTechnology 
*/

#ifndef SRTDECODER_H
#define SRTDECODER_H

#include <e32lang.h>
#include <e32std.h>
#include <fbs.h>

#include "subtitledecoder.h"

/** the size of the buffer for SRT reader */
const TInt KSubtitleDecoderBufferLength = 256;

/** The font used in decoding text to bitmap */
_LIT(KSrtTargetTypefaceName, "NewTimes");

/** Define the height of the type face */
const TInt KSrtTargetTypefaceHeightInTwips = 200;

class MSubtitleDecoder;
class CSrtFrame;

/** The implementation of SRT subtitle decoder */ 
NONSHARABLE_CLASS(CSrtSubtitleDecoder) : public CBase, public MSubtitleDecoder
{
public:
    /** Allocates and constructs a srt subtitle decoder */
    IMPORT_C static CSrtSubtitleDecoder* NewL(MSubtitleSource& aSubtitleSource);
    
    IMPORT_C virtual ~CSrtSubtitleDecoder();

    /** Implementation of MSubtitleDecoder */
    IMPORT_C TInt CalculateSubtitleRegion(const TRect& aScreenSize, TRect& aSubtitleRegion);
    IMPORT_C void GetNextFrameL(
        CFbsBitmap& aSubtitleFrame, 
        TRect& aDirtyRegion, 
        TTimeIntervalMicroSeconds& aDisplayTime, 
        TTimeIntervalMicroSeconds& aDisplayDuration);
    IMPORT_C TLanguage SubtitleLanguageL();
    IMPORT_C void GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aLanguages);
    IMPORT_C void SetSubtitleLanguageL(TLanguage aLanguage);
    IMPORT_C void SetVideoPosition(const TTimeIntervalMicroSeconds& aPosition);
    IMPORT_C void Start();
    IMPORT_C void Stop();
    
private:
    CSrtSubtitleDecoder(); 
    
    /** Constructor with a reference to the subtitle source */
    CSrtSubtitleDecoder(MSubtitleSource& aSubtitleSource);
    
    void ConstructL();
    
    /** Searches for the information of the first SRT frame separator (\r\n\r\n or \n\n) */
    TInt GetFrameSeparatorInfo(const TDesC8& aDes, TInt& aSeparatorLength, TInt& aPosition);
    
    /** Populate a bitmap with the content of a SRT frame and update the dirty region */
    void DecodeSubtitleContentL(const TDesC8& aContent, CFbsBitmap& aSubtitleFrame, TRect& aDirtyRegion);
    
    /** Try to get the next valid frame */
    void GetNextValidFrameL(TBool aSearchByVideoPosition, TInt64 aVideoPosition);
    
    /** Calculate the number of characters in the next line */
    TInt CalculateCharsInNextLine(const CFont &aFont, const TDesC &aText, TInt aWidthInPixels, TInt& aNumOfCharsToSkip);

private:
    /** The reference of SRT subtitle source */
    MSubtitleSource& iSource;
    
    /** A buffer for reading SRT source, its content shall be copied to iDecodeBuffer */
    TBuf8<KSubtitleDecoderBufferLength> iBuffer;
    
    /** A buffer maintains the un-processed SRT frames, consumed by GetNextFrameL */
    RBuf8 iDecodeBuffer;
    
    /** The video stream position set by SetVideoPosition */
    TInt64 iStartVideoPosition;
    
    /** The most recent system time when subtitle rendering has started */
    TTime iStartSystemTime;
    
    /** A flag indicating if that subtitle rendering has started */  
    TBool iStarted;
    
    /** A value indicates the current position of iDecodeBuffer */
    TInt iDecodeBufferPosition;
    
    /** A parser for the SRT frame */
    CSrtFrame *iFrameParser;
    
    /** A flag indicating the first SRT source scanning after start */
    TBool iFirstSearch;
    
    /** The end time of the last valid frame */
    TInt64 iEndTimeOfLastValidFrame;
    
    /** True if EOF marked the end of the previous frame. */
    TBool iLastFrame;
};

#endif //__SRTDECODER_H__



