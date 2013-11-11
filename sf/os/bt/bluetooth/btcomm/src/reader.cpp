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
// reader.cpp - all the active reader code
// 
//

#include <cs_port.h>
#include "btcomm.h"
#include "btcommactive.h"
#include "btcommutil.h"

CBTPortReader *CBTPortReader::NewL(CBTPortProxy *aParent)
/**
	Create new CBTPortReader.
	This function creates a new CBTPortReader active object.
**/
	{
	CBTPortReader *cc;
	cc=new (ELeave) CBTPortReader();
	CleanupStack::PushL(cc);
	cc->InitL(aParent);
	CleanupStack::Pop();
	CActiveScheduler::Add(cc);
	return cc;
	}

void CBTPortReader::InitL(CBTPortProxy *aParent)
/**
	CBTPortReader InitL.
	This function sets up the local read buffer
	that is the first port of call for data read
	from the socket.  The size of the local read 
	buffer should be the max size available for
	socket datagrams.
**/
	{
	iPortProxy=aParent;
	iLocalReadBuffer=HBufC8::NewMaxL(KBTCOMMRecvBufferLength);
	iLocalReadBufferPtr.Set(iLocalReadBuffer->Des());
	}

CBTPortReader::~CBTPortReader()
/**
	CBTPortReader destructor.	
**/
	{
	delete iLocalReadBuffer;
	Cancel();
	}

void CBTPortReader::RunL()
/**
	CBTPortReader RunL.
	Active status cleared.  Put the read data into the circular
	buffer.  After that we can call the DoReadCompleted 
	function on the corresponding port proxy object.
**/
	{
	CBTPortBuffer& circBuf=*(iPortProxy->iCircularReadBuf);

	TInt ret=iStatus.Int();
	if (ret==KErrNone)
		{
			TUint8 *ptr=CONST_CAST(TUint8*,iLocalReadBufferPtr.Ptr());
			TInt receivedBytes=iLocalReadBufferPtr.Length();

			// add newly arrived data in the circular buffer
#ifdef _DEBUG
			TInt consumed=
#endif
				circBuf.Add(ptr,receivedBytes);
			
			__ASSERT_DEBUG(consumed==receivedBytes,BTCommUtil:: \
				Panic(EBTCommPortReaderPotentialLossOfIncomingData));
		}

	iPortProxy->DoReadCompleted(ret);
	}

void CBTPortReader::QueueRead()
/**
	CBTPortReader QueueRead.
	This function is invoked to queue a read on the socket.
	We just keep queueing reads on the socket here with the 
	resulting data being fed into the circular read buffer.
**/
	{
	if (IsActive())
		{
		//__DEBUGGER(); //FIXME
		return;
		}
	else
		{	
		// queue read to socket
		iLocalReadBufferPtr.SetLength(0);
		iPortProxy->iSocket.RecvOneOrMore(iLocalReadBufferPtr,0,iStatus,iPlen);
		SetActive();
		}
	}

void CBTPortReader::StartReading()
	{
	iKeepReading=ETrue;
	QueueRead();
	}

void CBTPortReader::StopReading()
	{
	iKeepReading=EFalse;
	}

TBool CBTPortReader::IsReading()
	{ 
	return iKeepReading;
	}

void CBTPortReader::DoCancel()
/**
	CBTPortReader DoCancel.
**/
	{ 
	if ( iPortProxy->iSocket.SubSessionHandle())
		{
		iPortProxy->iSocket.CancelRead();
		}
	}

CBTPortReader::CBTPortReader() 
	: CActive (EPriorityHigh), 
	  iLocalReadBufferPtr(NULL,0,0)
	{
	}

//********************* READ CIRCULAR BUFFER CODE **********************

CBTPortBuffer *CBTPortBuffer::NewL(CBTPortProxy* aParent,TInt aBufSize)
	{
	CBTPortBuffer *buf;
	buf=new (ELeave) CBTPortBuffer(aBufSize);
	CleanupStack::PushL(buf);
	buf->ConstructL(aParent);
	CleanupStack::Pop();
	return buf;
	}

void CBTPortBuffer::ConstructL(CBTPortProxy* aParent)
/**
	Sets up the circular buffer.
**/
	{
	iPortProxy=aParent;
	SetLengthL(iCircBufSize);
	}

CBTPortBuffer::CBTPortBuffer(TInt aBufSize) 
	: CCirBufBase(sizeof(TUint8)), 
	  iCircBufSize(aBufSize)
	{
	iCircBufSize=aBufSize;
	}

CBTPortBuffer::~CBTPortBuffer()
	{
	}

TInt CBTPortBuffer::Add(const TUint8* aPtr,TInt aCount)
	{
	return DoAdd(aPtr,aCount);
	}

TInt CBTPortBuffer::Remove(TUint8* aPtr,TInt aCount)
	{
	return DoRemove(aPtr,aCount);
	}

TInt CBTPortBuffer::ScanForTerminator(TReadTerminator& aTerminator)
/**
	This function returns the number of bytes to be read of the circ buffer or KErrNotFound.
	If the terminator sequence was not found then KErrNotFound is returned to 
	indicate this, the  returned number of bytes does include the terminator 
	sequence held in aTerminator.
*/
	{
	// go through the circbuf trying to find a terminator char.
	TInt byteCount=1;
	TInt charsFound=KErrNotFound; //ret val from the sequence scan
	
	TUint8* ptr=iTail; //somehow inefficient for now
	TText8 nextChar;

	// init scanning
	aTerminator.iNextCharToFindIdx=0; //point to the first char to be scanned

	FOREVER
		{	
		if(ptr>=iPtrE) 
			{// roll over
			ptr=iPtr;
			}
		
		nextChar=*ptr;
		
		charsFound=ScanForTerminatorSequence(aTerminator,nextChar);
		if(charsFound>KErrNone)
			{// charsFound will only be positive once			
			__ASSERT_DEBUG(charsFound==aTerminator.iTerminatorCount, \
		BTCommUtil::Panic(EBTCommPortBufferTerminatorCharNoOtherThanExpected));
			
			break;
			}
		ptr++; // go for the next one
		if(byteCount==iCount)
			{
			break; //we scanned the whole buf
			}
		byteCount++; // record one more char that will definately be searched
		}
	
	 // either returns the no of bytes read or KErrNotFound
	if(charsFound>=KErrNone)
		{
		__ASSERT_DEBUG(byteCount<=iCount,BTCommUtil::Panic(EBTCommPortBufferTerminatedCharsFoundBogus));
		return byteCount;
		}
	return KErrNotFound;
	}


TInt CBTPortBuffer::ScanForTerminatorSequence(TReadTerminator& aTerminator,
											  TText8 aChar)
/**
	Will return the number of terminator chars found or KErrNotFound.
	The invariant is that the number of chars returned should be equal to
	the TReadTerminator::iTerminatorCount.
	The control and drive of the 'find the terminator sequence' state machine
	is kept in this method.

	To drive this state machine, a client must keep feeding it with aChar until
	it returns a positive int or the client's buffer has been exhausted.
*/
	{
	//check for the expected character
	if(aChar==aTerminator.iTerminatorChars[aTerminator.iNextCharToFindIdx])
		{// the expected character was found
		// check to see if we found the whole sequence
		if(aTerminator.iNextCharToFindIdx==(aTerminator.iTerminatorCount-1))
			{// yes this was the last expected char in the sequence
			TInt ret=aTerminator.iNextCharToFindIdx; 
			aTerminator.iNextCharToFindIdx=0; //set it back to the first char in the seq
			return ret+1; // found so many, return with success
			}
		// we didn't but still we're doing ok, so prepare for the next one
		aTerminator.iNextCharToFindIdx++;
		return KErrNotFound; //yet
		}

	// the current char wasn't in the terminator sequence or it was 
	// in an irrelevant position so reset the state machine in order to start again
	aTerminator.iNextCharToFindIdx=0;		
	return KErrNotFound; 
	}
