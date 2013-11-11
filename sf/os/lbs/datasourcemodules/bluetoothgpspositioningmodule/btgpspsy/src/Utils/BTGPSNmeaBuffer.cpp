// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
#include "BTGPSNmeaBuffer.h"
#include "BTGPSMessageDef.h"
#include "BTGPSPanic.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS

// Granularity

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::NewL
// -----------------------------------------------------------------------------
CBTGPSNmeaBuffer* CBTGPSNmeaBuffer::NewL(TInt aSize)
    {
    CBTGPSNmeaBuffer* self = new (ELeave) CBTGPSNmeaBuffer();
    CleanupStack::PushL(self);
    self->ConstructL(aSize);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::~CBTGPSNmeaBuffer
// -----------------------------------------------------------------------------
CBTGPSNmeaBuffer::~CBTGPSNmeaBuffer()
    {
    delete iBuffer;
    }

// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::ConstructL
// -----------------------------------------------------------------------------
void CBTGPSNmeaBuffer::ConstructL(TInt aSize)
    {
    iBuffer = (TUint8*)User::AllocZL(aSize);
    iSize = aSize;
    }

// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::CBTGPSNmeaBuffer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
CBTGPSNmeaBuffer::CBTGPSNmeaBuffer()
    {
    }

// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::ResetBuffer
// -----------------------------------------------------------------------------
void CBTGPSNmeaBuffer::ResetBuffer()
    {
    Mem::FillZ(iBuffer,iSize);
    iBottom = 0;
    }

// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::CurrentIndex
// -----------------------------------------------------------------------------
TInt CBTGPSNmeaBuffer::CurrentIndex() const
    {
    return iBottom;
    }

// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::AddSentences
// -----------------------------------------------------------------------------
void CBTGPSNmeaBuffer::AddSentences(const TDesC8& aNmea)
    {
    TPtrC8 srcDes(aNmea);
    TInt srcLen = srcDes.Length();
    
    while(srcLen>0)
        {
        TInt maxSize = iSize-iBottom;
        TPtr8 ptr(iBuffer+iBottom, maxSize);
        TInt cpLen = Min(maxSize,srcLen);
        ptr.Copy(srcDes.Left(cpLen));
        iBottom = (iBottom+cpLen)%iSize;
        srcDes.Set(srcDes.Right(srcLen-cpLen));
        srcLen = srcDes.Length();
        }
    }

// -----------------------------------------------------------------------------
// CBTGPSNmeaBuffer::ReadSentences
// -----------------------------------------------------------------------------
TInt CBTGPSNmeaBuffer::ReadSentences(
            TDes8& aDest, 
            TInt& aBeginning) const
    {
    __ASSERT_DEBUG(aBeginning<iSize, Panic(EPanicIndexOutOfRange));
    
    //Clear destination descriptor
    aDest.Zero();

    TBool started = EFalse;
    TBool ended = EFalse;
    
    if(aBeginning==iBottom)
        {
        return KErrEof;
        }
    
    if(aBeginning == KBTGPSNmeaIndexNotSet)
        {
        if(iBuffer[iBottom]!=0)
            {
            //Buffer has been rotated
            aBeginning = iBottom;
            }
        else
            {
            //Read from 0 to iBottom
            aBeginning = 0;
            if(iBottom == 0)
                {
                return KErrEof;
                }
            }
        }
        
    do
        {
        TUint8 nextChar = iBuffer[aBeginning];
        if(nextChar == KNmeaSentenceLead)
            {
            //Mark start of NMEA sentences
            started = ETrue;
            }
            
        if(started && aDest.Length()<aDest.MaxLength())
            {
            aDest.Append(nextChar);
            }
            
        aBeginning ++;
        aBeginning = aBeginning%iSize;

        if(started && nextChar == KNmeaSentenceTerminator2)
            {
            //Mark end of the sentence.
            ended = ETrue;
            }
        }while(aBeginning!=iBottom && !ended);
    
    
    if(aDest.Length()!=0)
        {
        return KErrNone;
        }
        
    return KErrEof;
    }
        

//  End of File



