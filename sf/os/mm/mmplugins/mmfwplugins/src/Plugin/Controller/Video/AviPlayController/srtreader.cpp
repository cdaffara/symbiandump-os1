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

#include <e32base.h>
#include <bautils.h> 

#include "srtreader.h"

/** Default constructor */
CSrtReader::CSrtReader()
    {
    }

/** Allocates and constructs a srt subtitle source 
    
@param aFilePath the SRT subtitle file path
@return A pointer to the new object. 
@leave If an error occurs, the method will leave with a system-wide error code. 
*/    
CSrtReader* CSrtReader::NewL(const TDesC& aFilePath)
    {
    CSrtReader* self = new(ELeave) CSrtReader();
    CleanupStack::PushL(self);
    self->ConstructL(aFilePath);
    CleanupStack::Pop(self);
    return self;
    }

/** Initialises the srt reader
    
@leave If an error occurs, the method will leave with a system-wide error code.
*/
void CSrtReader::ConstructL(const TDesC& aFilePath)
    {
    if (aFilePath.Length() < 1)
        {
        User::Leave(KErrArgument);
        }
    
    User::LeaveIfError(iFsSession.Connect());
    
    if (BaflUtils::FileExists(iFsSession, aFilePath))
        {
        User::LeaveIfError(iSource.Open(iFsSession, aFilePath, EFileRead));
        }
    else
        {
        User::Leave(KErrNotFound);
        }
    } 

/** Destructor */
CSrtReader::~CSrtReader()
    {
    iSource.Close();
    iFsSession.Close();
    }

void CSrtReader::GetSupportedSubtitleLanguagesL(RArray<TLanguage>& /*aLanguages*/)
    {
    User::Leave(KErrNotSupported);    
    }
    
TLanguage CSrtReader::SubtitleLanguageL()
    {
    User::Leave(KErrNotSupported);
    return ELangEnglish;      
    }
    
void CSrtReader::SetSubtitleLanguageL(TLanguage /*aLanguage*/)
    {
    User::Leave(KErrNotSupported);  
    }

TInt CSrtReader::SetPosition(TInt aPosition)
    {
    return iSource.Seek(ESeekStart, aPosition);
    }
    
TInt CSrtReader::GetBuffer(TDes8& aBuffer)
    {
    TInt err = KErrNone;
    TInt bufLen = aBuffer.MaxLength();
    
    if (bufLen <= 0)
        {
        return KErrArgument;
        }
    
    err = iSource.Read(aBuffer);
    
    if (KErrNone == err)
        {
        if (0 == aBuffer.Length())
            {
            err = KErrEof;
            }        
        }
    
    return err;
    }



