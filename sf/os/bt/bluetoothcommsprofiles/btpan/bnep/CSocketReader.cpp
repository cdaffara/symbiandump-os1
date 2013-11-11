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
#include <es_mbuf.h>
#include "CBnepLink.h"
#include "CSocketReader.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_PAN_BNEP);
#endif

// Class CSocketReader 

CSocketReader::CSocketReader (RInternalSocket& aSocket, CBnepLink& aOwner)
    : CActive(KSocketReaderPriority), iSocket(aSocket), iBnepLink(aOwner)
    {
    LOG_FUNC
    CActiveScheduler::Add(this); 
    }


/**
Cancels any pending reads.
@internalComponent
*/
CSocketReader::~CSocketReader()
    {
    LOG_FUNC
    iRecvMsg.Free();
    Cancel();
    }


/**
Does not create the socket reader's buffer, as this initialised 
as a side effect of the RInternalSocket::Read(), 
which has a hardcoded buffer size for Bluetooth - Maintainer BEWARE!!!!!.
@internalComponent
*/
void CSocketReader::ConstructL ()
    {
    LOG_FUNC
    }


/**
Cancel a read.
@internalComponent
*/
void CSocketReader::DoCancel ()
    {
    LOG_FUNC
    iSocket.CancelRead(); 
    }

    
/**
Create a socket reader.
@internalComponent
*/
CSocketReader* CSocketReader::NewL (RInternalSocket& aSocket, CBnepLink& aOwner)
    {
    LOG_STATIC_FUNC
    CSocketReader* self = new(ELeave) CSocketReader(aSocket, aOwner);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self; 
    }


/**
Queue a read.
@internalComponent
*/
void CSocketReader::Read ()
    {
    LOG_FUNC
    LOG1(_L8("Reader 0x%08x -- queueing read"), this);
    if (!IsActive())
        {
        // Ensure that the MBufChain that we are receiving into is available
        // by free-ing any data currently in it.
        iRecvMsg.Free();
        
        // As RMBufChain is essentially a pointer, the data returned must be consumed
        // later either by re-assigning or by free-ing it.  This is a bit unlike a
        // real RSocket, where the descriptor is simply overwritten.
        iSocket.Read(iRecvMsg, iStatus);
        SetActive(); 
        }
    else
        {
        LOG(_L8("Read queued but reader already active.  Discarding"));
        }
    }


/**
Read complete.
Pass the received data up to the BNEP link, and signal disconnections.

This method effectively triggers a long sequence of synchronous calls 
into BNEP that culminate either in the completion of packet processing 
or an asynchronous hiatus while waiting for the agent to respond to a 
request.
@internalComponent
*/
void CSocketReader::RunL ()
    {
    LOG_FUNC
    
    // Save iStatus value to allow us to queue a new read prior to finishing processing
    // this one
    TInt err = iStatus.Int();
    LOG2(_L8("Length %d error %d"),iRecvMsg.Length(), err);
    
    if (iRecvMsg.Length())	// send any data up
        {
        // This is what kicks off the long processing 
        // sequence that may culminate in a write.
        // Note that this call may cause calls back down to CSocketReader
        iBnepLink.ReadComplete(iRecvMsg);
        LOG1(_L8("After ReadComplete Length %d"),iRecvMsg.Length());
        }
    
    if (err != KErrNone)	// if the connection has failed for some reason
        {
        LOG(_L8("Attempting a remote device disconnect"));
        
        // Cancel any new read queued
        Cancel();
        iBnepLink.RemoteDeviceDisconnect(err); // then notify the upper layer of the disconnect
        }
    }
