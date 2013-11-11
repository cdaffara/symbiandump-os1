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
#include "EPos_CPosFileHandler.h"

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CPosFileHandler::CPosFileHandler()
    {
    }

// EPOC default constructor can leave.
void CPosFileHandler::ConstructL(const TDesC& aFileName)
    {
    iFileName = aFileName.AllocL();
    User::LeaveIfError(iFsSession.Connect());
    User::LeaveIfError(iFile.Open(iFsSession, aFileName,
        EFileShareReadersOnly));
    iStream.Attach(iFile);
    }

// Two-phased constructor.
CPosFileHandler* CPosFileHandler::NewL(const TDesC& aFileName)
    {
    CPosFileHandler* self = new (ELeave) CPosFileHandler;
    CleanupStack::PushL(self);
    self->ConstructL(aFileName);
    CleanupStack::Pop(self);
    return self;
    }

// Destructor
CPosFileHandler::~CPosFileHandler()
    {
    iStream.Close();
    iFile.Close();
    iFsSession.Close();
    delete iFileName;
    }

// ----------------------------------------------------------------------------
// CPosFileHandler::Read
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CPosFileHandler::Read(TDes8& aDes)
    {
    const TChar c(0x000A) ;
    TRAPD(err, (iStream.ReadL(aDes, c)));

    if (err == KErrEof)
        {
        TInt err2 = ReadFromStart();
        if (err2 != KErrNone)
            {
            err = err2;
            }
        }
    return err;
    }

// ----------------------------------------------------------------------------
// CPosFileHandler::ReadL
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
void CPosFileHandler::ReadL(CDesC8Array& aArray)
    {
    TInt err = KErrNone;
    TBuf8<KMaxSentenceLength> dataInfo;
    const TChar endoflinechar(';');
	TStreamPos offset;
    while (err != KErrEof)
        {
		//Store the current offset in the stream
        offset = iStream.Source()->TellL(MStreamBuf::ERead);
		
        TRAP(err, (iStream.ReadL(dataInfo, endoflinechar)));
		if(err == KErrEof)
        	{
        	//End of File has been reached without finding an endoflinechar, so attempt to
        	// extract the remaining data of the file
        	TInt length = iStream.Source()->SizeL();
        	TInt readLength = length - (offset.Offset());
			
        	//Reset the stream back to the end of the previous successful data extraction 
        	iStream.Source()->SeekL(MStreamBuf::ERead, offset);
        	//Read the remaining data from the stream
        	TRAP(err, (iStream.ReadL(dataInfo, readLength)));
        	if(err == KErrNone)
        		{
        		//Managed to read the remaining data.  Ensure the error is EOF to exit the loop 
        		err = KErrEof;
        		}
        	}
        if (err != KErrNone && err != KErrEof)
            {
            User::Leave(err);
            }
        dataInfo.Trim();
        if ((err == KErrNone || err == KErrEof) && dataInfo.Length() > 0)
            {
            aArray.AppendL(dataInfo);
            }
        }
    if (aArray.Count() != KNoOfSimulatedDataItems)
        {
        User::Leave(KErrCorrupt);
        }
    }

// ----------------------------------------------------------------------------
// CPosFileHandler::ReadFromStart
//
// (other items were commented in a header).
// ----------------------------------------------------------------------------
//
TInt CPosFileHandler::ReadFromStart()
    {
    // Close stream and file and open them again.
    iStream.Close();
    iFile.Close();
    TInt err = iFile.Open(iFsSession, *iFileName, EFileShareReadersOnly);
    iStream.Attach(iFile);
    return err;
    }

//  End of File
