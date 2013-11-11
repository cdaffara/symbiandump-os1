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

#ifndef SUBTITLESOURCE_H
#define SUBTITLESOURCE_H

/**
@file
@internalTechnology 
*/

/** The generic interface for the subtitle source */ 
class MSubtitleSource
    {
public:
    /** Write subtitle source data to a buffer 
    
    @param aBuffer the buffer to be populated by srt subtitle source data
    @return A system-wide error code
    */
    virtual TInt GetBuffer(TDes8& aBuffer) = 0;
    
    /** Sets the the current subtitle source position
    
    @param aPosition Offset from the start location
    @return A system-wide error code
    */
    virtual TInt SetPosition(TInt aPosition) = 0;

    /** Get the current subtitle language. 
    
    @return The current subtitle stream language 
    @leave If an error occurs, the method will leave with a system-wide error code.
    */
    virtual TLanguage SubtitleLanguageL() = 0;
        
    /** Return all languages supported in the current video stream
    
    @return All currently supported languages
    @leave If an error occurs, the method will leave with a system-wide error code.
    */
    virtual void GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aLanguages) = 0;
    
    /** Set the subtitle language
    
    @param aLanguage the language to be used by the decoder
    @leave If an error occurs, the method will leave with a system-wide error code.
    */
    virtual void SetSubtitleLanguageL(TLanguage aLanguage) = 0;
    };

#endif //SUBTITLESOURCE_H

