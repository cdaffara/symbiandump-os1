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

#ifndef SRTREADER_H
#define SRTREADER_H

#include <e32cmn.h> 
#include <f32file.h>
#include <e32lang.h> 

#include "subtitlesource.h"

/** The implementation of SRT subtitle source */ 
NONSHARABLE_CLASS(CSrtReader) : public CBase, public MSubtitleSource
{
public:
    /** Allocates and constructs the object */
    static CSrtReader* NewL(const TDesC& aFilePath);
    
    /** Destructor */
    ~CSrtReader();

    /** Implementation of MSubtitleSource */
    TInt GetBuffer(TDes8& aBuffer);
    TInt SetPosition(TInt aPosition);
    TLanguage SubtitleLanguageL();
    void GetSupportedSubtitleLanguagesL(RArray<TLanguage>& aLanguages);
    void SetSubtitleLanguageL(TLanguage aLanguage);

private:
    /** Default constructor */
    CSrtReader();
    
    /** Initialises the srt reader */
    void ConstructL(const TDesC& aFilePath);
    
private:
    /** SRT file */
    RFile iSource;
    
    /** File session */
    RFs iFsSession;
};

#endif //__SRTREADER_H__

