// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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



//  INCLUDES
#include "ctlbssimulationpsyposnmeafilereader.h"
#include <e32cmn.h>

// CONSTANTS

// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CT_LbsSimulationPSYPosNmeaFileReader::CT_LbsSimulationPSYPosNmeaFileReader()
    {
    iReadBuffer.SetLength(0);
    iReadBufferCursor = 0;
    }
     
// Two-phased constructor.
CT_LbsSimulationPSYPosNmeaFileReader* CT_LbsSimulationPSYPosNmeaFileReader::NewLC(const TDesC& aFileName)
    {
    CT_LbsSimulationPSYPosNmeaFileReader* self = new (ELeave) CT_LbsSimulationPSYPosNmeaFileReader();
    CleanupStack::PushL(self); 
    self->ConstructL(aFileName);
    return self;
    }

// Two-phased constructor.
CT_LbsSimulationPSYPosNmeaFileReader* CT_LbsSimulationPSYPosNmeaFileReader::NewL(const TDesC& aFileName)
    {
    CT_LbsSimulationPSYPosNmeaFileReader* self = NewLC(aFileName);
    CleanupStack::Pop();
    return self;
    }

// EPOC default constructor can leave.
void CT_LbsSimulationPSYPosNmeaFileReader::ConstructL(const TDesC& aFileName)
    {
    User::LeaveIfError(iFileSession.Connect());
    User::LeaveIfError(iFile.Open(iFileSession, aFileName, EFileRead|EFileShareReadersOnly));
    }
            
// Destructor
CT_LbsSimulationPSYPosNmeaFileReader::~CT_LbsSimulationPSYPosNmeaFileReader()
    {
    iFile.Close();
    iFileSession.Close();
    }

TInt CT_LbsSimulationPSYPosNmeaFileReader::ReadNmeaSentence(TDes8& aInputBuffer)
    {
    aInputBuffer.SetLength(0);

    while (aInputBuffer.Length() < aInputBuffer.MaxLength())
        {
        if (iReadBufferCursor >= iReadBuffer.Length())
            {
            // Time to read a new choke of characters
            iReadBufferCursor = 0;
            TInt err = iFile.Read(iReadBuffer);
            if (err != KErrNone)
                {
                // Something went wrong
                return err;
                }
            if (iReadBuffer.Length() == 0)
                {
                // EOF encoutered
                return KErrEof;
                }
            }

        TInt maxRead = Min(iReadBuffer.Length() - iReadBufferCursor, 
            aInputBuffer.MaxLength() - aInputBuffer.Length());
        TPtrC8 readBuffer = iReadBuffer.Mid(iReadBufferCursor, maxRead);

        TInt newLineIndex = readBuffer.Locate('\n');
        if (newLineIndex == KErrNotFound)
            {
            // Append as much as possible
            aInputBuffer.Append(readBuffer);
            iReadBufferCursor += readBuffer.Length();
            if (aInputBuffer.Length() == aInputBuffer.MaxLength())
                {
                // Output parameter full - read past the rest of the line
                TBuf8<KLineReaderBufferSize> line = aInputBuffer.Right(1);
                while (line.Length() > 0 && line[line.Length() - 1] != '\n')
                    {
                    ReadNmeaSentence(line);
                    }
                }
            // else iReadBuffer has no more chars to read - loop another round
            }
        else
            {
            aInputBuffer.Append(readBuffer.Left(newLineIndex + 1));
            iReadBufferCursor += (newLineIndex + 1);
            break;
            }
        }

    return KErrNone;
    }


TInt CT_LbsSimulationPSYPosNmeaFileReader::ReadNmeaBurst(
    TDes8& aBuffer,
    const TDesC8& aDelimiter)
    {
    aBuffer.Append(iLastLine);
    iLastLine.Zero();
    TInt err = ReadNmeaSentence(iLastLine);
    if (err != KErrNone)
        {
        return err;
        }
    while (iLastLine.Find(aDelimiter) == KErrNotFound)
        {
        if (aBuffer.MaxLength() - aBuffer.Length() < iLastLine.Length())
            {
            return KErrBadDescriptor;
            }
        aBuffer.Append(iLastLine);
        err = ReadNmeaSentence(iLastLine);
        if (err != KErrNone)
            {
            return err;
            }
        }
    return KErrNone;
    }


TInt CT_LbsSimulationPSYPosNmeaFileReader::ReadFirstNmeaSentence(TDes8& aInputBuffer)
    {
    if (iReadBuffer.Length() != 0 || iReadBufferCursor != 0)
        {
        return KErrAccessDenied;
        }
    TInt err = ReadNmeaSentence(aInputBuffer);
    if (err != KErrNone)
        {
        return err;
        }
    iLastLine = aInputBuffer;
    return KErrNone;
    }

// End of File
