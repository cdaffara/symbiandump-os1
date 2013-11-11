// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


//-----------------------------------------------------------------------------------
#include "lbssuplsocketlistener.h"

/*
 CSuplSocketListener: active object wrapping comms read requests
*/
CSuplSocketListener::CSuplSocketListener(RSocket& aSocket, RBuf8& aBuffer) :
						CActive(EPriorityStandard),
						iSocket(aSocket),
						iBuffer(aBuffer),
						iFragmentLength(0),
						iIsFirstFragment(ETrue),
						iLengthReadSoFar(0),
						iSuplMessageLength(0)
    {
    }

CSuplSocketListener* CSuplSocketListener::NewL(RSocket& aSocket, RBuf8& aBuffer)
    {
    CSuplSocketListener* self = NewLC(aSocket, aBuffer);
    CleanupStack::Pop();
    return self;
    }

CSuplSocketListener* CSuplSocketListener::NewLC(RSocket& aSocket, RBuf8& aBuffer)
    {
    CSuplSocketListener* self = new(ELeave) CSuplSocketListener(aSocket, aBuffer);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }

void CSuplSocketListener::ConstructL()
    { 
    CActiveScheduler::Add(this);
    }
    
CSuplSocketListener::~CSuplSocketListener()
{
	Cancel();
}

/*
 Cancel asychronous read request
*/
void CSuplSocketListener::DoCancel()
    {
    iSocket.CancelRead();
    User::RequestComplete(iUserStatus,KErrCancel);
    }

/*
Read an entire SUPL message from the socket
*/
void CSuplSocketListener::ReadMessage(TRequestStatus& aStatus)
{
	iUserStatus = &aStatus;
    aStatus = KRequestPending;
    
    // Ensure buffer length is maximum
    //
    if (iBuffer.MaxLength() < KSuplMaxMessageLength)
    	{
    	TInt err = iBuffer.ReAlloc(KSuplMaxMessageLength);
    	if(KErrNone != err)
    		{
    		User::RequestComplete(iUserStatus,err);
		return;
    		}
    	}
    
    iBuffer.SetLength(KSuplMaxMessageLength);
    	
  	iIsFirstFragment = ETrue;
	iLengthReadSoFar = 0;
	iSuplMessageLength = 0;

	IssueRead(iBuffer);
}

/*
 Read a Fragment of data from a stream socket
*/
void CSuplSocketListener::IssueRead(TDes8& aBuffer)
    {
    if (!IsActive())
        {
        iSocket.RecvOneOrMore(aBuffer, 0, iStatus, iFragmentLength);
        SetActive();
        }
    }
    

/*
 Active object request complete handler
*/
void CSuplSocketListener::RunL()
    {
    
    TBool err = EFalse;
    
    if (iStatus == KErrNone)
        // A message Fragment has been read from socket
        {
        
        if (iIsFirstFragment)
        	{
        	if (iFragmentLength() != 0)
        		{
        		// First two bytes of the first Fragment
        		// contain the length of the SUPL message.
       			SuplMessageLength();
       			iIsFirstFragment = EFalse;
        		}
        	else
        		{
        	    User::RequestComplete(iUserStatus,KErrGeneral);
        	    err = ETrue;
        		}
       		}
       	
       	// If no error's so far, read another chunch of the message
       	if (err == EFalse)
       		{	
	      	iLengthReadSoFar += iFragmentLength();
       	
	       	if (iLengthReadSoFar == iSuplMessageLength)
	       		{
	       		// Whole message has been read.
	       		// Complete call.
	       		iBuffer.SetLength(iSuplMessageLength);
	       		User::RequestComplete(iUserStatus,KErrNone);
	       		}
	       	else if(iLengthReadSoFar > iSuplMessageLength)
	       		{
	   			// This should not happened. Complete caller with error:
	   			User::RequestComplete(iUserStatus,KErrGeneral);
	       		}
	       	else
	       		{
	       		// Read data into new buffer fragment
	        	// after the last position read
	        	TPtr8 bufferFragment = iBuffer.MidTPtr(iLengthReadSoFar);
	        	IssueRead(bufferFragment);
	       		}
       		}
        }
    else
        {
        // Error reading socket. Report error to user.
        User::RequestComplete(iUserStatus,iStatus.Int());
        }   
    }

void CSuplSocketListener::SuplMessageLength()
{
	// Get the first two bytes of iBuffer as a TInt and put it in iSuplMessageLength
	iSuplMessageLength = STATIC_CAST(TUint8,iBuffer[0]);
	iSuplMessageLength = iSuplMessageLength << 8;
	iSuplMessageLength |= STATIC_CAST(TUint8,iBuffer[1]);
}
    
