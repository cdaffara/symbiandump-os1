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

#ifndef SUBTITLEDECODER_H
#define SUBTITLEDECODER_H

#include "subtitlesource.h"

/** The generic interface for the subtitle decoder */ 
class MSubtitleDecoder
    {
public:
    /** The Decoder calculates the subtitle region of the display given. 
    
    @param aScreenSize The size of the video window
    @param aSubtitleRegion TRect reference for returning the subtitle region, coordinates 
        relative to the top right of the video window
    @return A system-wide error code
    */
    virtual TInt CalculateSubtitleRegion(const TRect& aScreenSize, TRect& aSubtitleRegion) = 0;
    
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
    virtual void GetNextFrameL(
        CFbsBitmap& aSubtitleFrame, 
        TRect& aDirtyRegion, 
        TTimeIntervalMicroSeconds& aDisplayTime, 
        TTimeIntervalMicroSeconds& aDisplayDuration) = 0;
    
    /** Get the current subtitle language. 
    
    @return The current subtitle stream language 
    @leave If an error occurs, the method will leave with a system-wide error code.
    */
    virtual TLanguage SubtitleLanguageL() = 0;
    
    /** Return all languages supported in the current video stream
    
    @@param aLanguages All currently supported languages
    @leave If an error occurs, the method will leave with a system-wide error code.
    */
    virtual void GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aLanguages) = 0;
    
    /** Set the subtitle language
    
    @param aLanguage the language to be used by the decoder
    @leave If an error occurs, the method will leave with a system-wide error code.
    */
    virtual void SetSubtitleLanguageL(TLanguage aLanguage) = 0;
    
    /** Set the current video stream position
    
    @param aCurrentTime The current video stream position    
    @return A system-wide error code
    */
    virtual void SetVideoPosition(const TTimeIntervalMicroSeconds& aPosition) = 0;
    
    /** Informs the decoder that subtitle rendering has started. */
    virtual void Start() = 0;
    
    /** Informs the decoder that the video stream has stopped playing. */
    virtual void Stop() = 0;
    };

#endif // SUBTITLEDECODER_H

