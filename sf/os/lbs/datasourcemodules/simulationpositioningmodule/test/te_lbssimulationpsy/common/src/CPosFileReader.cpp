// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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



// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>

#include "CPosFileReader.h"

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// Two-phased constructor.
CPosFileReader* CPosFileReader::NewL()
    {
    CPosFileReader* self = CPosFileReader::NewLC();
    CleanupStack::Pop();
    return self;
    }

// Two-phased constructor.
CPosFileReader* CPosFileReader::NewLC()
    {
    CPosFileReader* self = new (ELeave) CPosFileReader;
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosFileReader::CPosFileReader()
    {
    }

// EPOC default constructor can leave.
void CPosFileReader::ConstructL()
    {     
    }

// Destructor
CPosFileReader::~CPosFileReader()
    {
    CloseResources();
    }

// ---------------------------------------------------------
// CPosFileReader::PrepareForRead
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosFileReader::PrepareForRead(const TDesC& aFileName)
    {    
    TInt err = Open(aFileName, EFileShareExclusive|EFileStream|EFileRead);
    if (!err)
        {
        iStream.Attach(iFile);
        }
    return err;
    }

// ---------------------------------------------------------
// CPosFileReader::Read
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosFileReader::ReadL(CDesC8Array*& aArray, TInt aItemsToRead, TBool aNmeaMode)
    {                    
    TInt err = KErrNone;        
    TBuf8<80> dataInfo;        
    TChar c;
    if (aNmeaMode)
        {
        c = (0x000A);
        }
    else
        {
        c = ';';
        }
    //TChar c(';');
    while (err != KErrEof && aArray->Count() != aItemsToRead)
    	{    	
        //TRAP(err, (iStream.ReadL(dataInfo, TChar(0x000A))));              
        TRAP(err, (iStream.ReadL(dataInfo, c)));              
        dataInfo.Trim();
        if (err != KErrNone && err != KErrEof)
            {
            User::Leave(err);
            }
        else if (dataInfo.Length() > 0)
            {
            aArray->AppendL(dataInfo);
            }    
    	}  
    if (aArray->Count() != aItemsToRead)
        {
        User::Leave(KErrCorrupt);                        
        }    
    }

// ---------------------------------------------------------
// CPosFileReader::Open
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
TInt CPosFileReader::Open(
    const TDesC& aFilename,
    const TInt aFileMode)
    {
    TInt err = KErrNone;
    if ((err = iFsSession.Connect()) != KErrNone)
        {
        return err;
        }
    if ((err = iFile.Open(iFsSession, aFilename, aFileMode)) != KErrNone)
        {
        iFsSession.Close();
        return err;
        }       
    return err;
    }

// ---------------------------------------------------------
// CPosFileReader::Close
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosFileReader::CloseResources()
    {    
    iFile.Close();
    iStream.Close();
    iFsSession.Close();
    }

// ---------------------------------------------------------
// CPosFileReader::Open
//
// (other items were commented in a header).
// ---------------------------------------------------------
//
void CPosFileReader::UpdateFileName()
    {    
    }

//  End of File
