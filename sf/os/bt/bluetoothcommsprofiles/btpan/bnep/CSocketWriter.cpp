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

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include "CBnepLink.h"
#include "CSocketWriter.h"
#include "bneputils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

CSocketWriter::CSocketWriter (RInternalSocket& aSocket, CBnepLink& aOwner)
        : CActive(EPriorityHigh), iSocket(aSocket), iBnepLink(aOwner)
    {
    LOG_FUNC
    CActiveScheduler::Add(this); 
    }

/**
   Cancels any pending writes.
   @internalComponent
*/
CSocketWriter::~CSocketWriter()
    {
    LOG_FUNC
    Cancel();
    
    iPendingWriteData.Free();
    }

/**
   Create the socket writer
   @internalComponent
*/ 
CSocketWriter* CSocketWriter::NewL (RInternalSocket& aSocket, CBnepLink& aOwner)
    {
    LOG_STATIC_FUNC
    CSocketWriter* self = new(ELeave) CSocketWriter(aSocket, aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self; 
    }

/**
   Attempt to write a buffer.
   @return KErrNone if successful or a system-wide error code
   @internalComponent
*/
TInt CSocketWriter::Write (RMBufChain& aChain)
    {
    LOG_FUNC
    if (aChain.Length() > KBnepMTU)
        {
        LOG(_L8("Length greater than expected MTU!"));
        aChain.Free();
        return KErrTooBig;
        }
    if (IsActive())
        {
        if(iQueueSize < KMaxSocketWriterQueueSize)
            {
            iQueue.Append(aChain);
            iQueueSize++;
            return KErrNone;
            }
        else
            {
            LOG(_L8("Bnep::ESocketWriterQueueOverflow"));
            aChain.Free();
            return KErrOverflow;
            }
        }
    LOG1(_L8("Length %d"), aChain.Length());
    
    __ASSERT_DEBUG(iPendingWriteData.IsEmpty(), BnepUtils::Panic(Bnep::ESocketWriterQueueOverflow));
    iPendingWriteData.Assign(aChain);
    iSocket.Write(iPendingWriteData, iStatus);

    SetActive();
    return KErrNone; 
    }

/**
   Does nothing
   @internalComponent
*/ 
void CSocketWriter::ConstructL ()
    {
    LOG_FUNC
    }

/**
   Checks iQueue for outstanding buffers and writes the first
   if it exists.
   @internalComponent
*/ 
void CSocketWriter::RunL ()
    {
    LOG_FUNC
	// Check if the completed write was a success.
	if(iStatus.Int() != KErrNone)
		{
		// The write failed - clear the pending write data.
	    LOG1(_L8("CSocketWriter::RunL completed with error: %d"), iStatus.Int());
		iPendingWriteData.Free();
		}

    __ASSERT_DEBUG(iPendingWriteData.IsEmpty(), BnepUtils::Panic(Bnep::ESocketWriterQueueOverflow));

    if(!iQueue.IsEmpty())
        {
        RMBufChain item;
        //Check on return value of iQueue.Remove() is unnecessary, since we've checked iQueue.IsEmpty()
        static_cast<void>(iQueue.Remove(item));
        iQueueSize--;
        Write(item);
        }
    else
        {
        iQueueSize = 0; // To make sure that we are always in sync
        }
    }

/**
   Cancel write.
   @internalComponent
*/
void CSocketWriter::DoCancel ()
    {
    LOG_FUNC
    iSocket.CancelWrite();
    
    // Free the pending data buffer to aviod asserting when the next Write operation
    // is performed.
    
    iPendingWriteData.Free();
    }
