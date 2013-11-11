// Copyright (c) 1994-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// RWsBuffer class, handles buffering and flushing of window server commands
// 
//

#include <e32std.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"
#include "w32comm.h"

// Global functions

GLDEF_C void Assert(TW32Assert aAssert)
	{
	_LIT(KW32AssertCategory,"W32 Assert");
	User::Panic(KW32AssertCategory,aAssert);
	}

GLDEF_C void Panic(TW32Panic aPanic)
	{
	_LIT(KW32PanicCategory,"W32");
	User::Panic(KW32PanicCategory,aPanic);
	}

// Public functions

RWsBuffer::RWsBuffer(RWsSession *aSession) : iSession(aSession), iManager(NULL),
	#if defined(__AUTO_FLUSH)
		iAutoFlush(ETrue),
	#else
		iAutoFlush(EFalse),
	#endif
	iBuf(NULL,0,0), iNext(NULL), iPreviousHandle(0), iBufSize(0), iMaxBufSize(EMinBufferSize),
	#if defined(_DEBUG)
	iAppendDataLength(0),
	#endif
	iDirectAcessCount(0), iInvalidBitmapArray(EFalse), iWindowSizeCache(NULL)
	{
	}

TInt WsFbsDestroyCallBack(TAny* aBitmapHandle)
	{
	TInt* bitmapHandle=static_cast<TInt*>(aBitmapHandle);
	RWsBuffer::FlushAllBuffers(aBitmapHandle ? *bitmapHandle : 0);

	return(0);
	}

void RWsBuffer::SetCallBack()
	{
	for(RWsBuffer *buffer=(RWsBuffer *)Dll::Tls();buffer;buffer=buffer->iNext)
		if (buffer==this)
			return;	// Already linked
	iNext=(RWsBuffer *)Dll::Tls();
	Dll::SetTls(this);
	if (iNext==NULL)	// First connection so set callback
		RFbsSession::GetSession()->SetCallBack(TCallBack(WsFbsDestroyCallBack,NULL));
	}

void RWsBuffer::CancelCallBack()
	{
	RWsBuffer *buffer=(RWsBuffer *)Dll::Tls();
	if (buffer==this)
		{
		Dll::SetTls(iNext);
		if (iNext==NULL)
			RFbsSession::GetSession()->ResetCallBack();	// Last connection closing so cancel the callback
		}
	else
		{
		RWsBuffer *prev;
		while(buffer)
			{
			prev=buffer;
			buffer=buffer->iNext;
			if (buffer==this)
				{
				prev->iNext=iNext;
				break;
				}
			}
		}
	}

void RWsBuffer::Close()
	{
	User::Free((TAny *)iBuf.Ptr());
	if (iWindowSizeCache)
	    {
	    iWindowSizeCache->Close();
	    delete iWindowSizeCache;
	    iWindowSizeCache = NULL;
	    }
	}

void RWsBuffer::Destroy()
	{
	Flush();
	Close();
	delete this;
	}


TInt RWsBuffer::Flush(const TIpcArgs* aIpcArgs,TBool aRequestFinish)
	{
	iBitmapArray.Reset();
	iInvalidBitmapArray=EFalse;
	if (iBuf.Length()==0)
		{
		return(KErrNone);
		}
	TIpcArgs ipcArgs;
	if (aIpcArgs!=NULL)
		{
		ipcArgs=*aIpcArgs;
		// check that the caller hasn't used the first slot
		ipcArgs.Set(KBufferMessageSlot,TIpcArgs::ENothing);
		__ASSERT_ALWAYS(Mem::Compare(REINTERPRET_CAST(const TUint8*, &ipcArgs), sizeof(TIpcArgs), REINTERPRET_CAST(const TUint8*, aIpcArgs), sizeof(TIpcArgs))==0,Panic(EW32PanicUsingReservedIpcSlot));
		}
	ipcArgs.Set(KBufferMessageSlot,&iBuf);
	TInt ret;
	if(aRequestFinish)
		ret=iSession->DoFlush(ipcArgs);
	else
		ret=iSession->DoSyncMsgBuf(ipcArgs);
	iBuf.Zero();
	iPreviousHandle=0;
	return(ret);
	}

void RWsBuffer::FlushAllBuffers(TInt aBitmapHandle)
	{
	for(RWsBuffer *buffer=(RWsBuffer *)Dll::Tls();buffer;buffer=buffer->iNext)
		{
		if(!aBitmapHandle || buffer->iInvalidBitmapArray || (buffer->iBitmapArray.FindInOrder(aBitmapHandle)!=KErrNotFound))

			buffer->Flush();
		}
	}

inline void RWsBuffer::SetAndLimitMaxBufSize(TInt aMaxBufSize)
	{ // apply upper & lower limits to input buffer size
	if (aMaxBufSize < EMinBufferSize)
		{
		iMaxBufSize = EMinBufferSize;
		}
	else if	(aMaxBufSize > EMaxBufferSize)
		{
		iMaxBufSize = EMaxBufferSize;
		}
	else
		{
		iMaxBufSize = aMaxBufSize;
		}
	}

void RWsBuffer::SetBufferSizeL(TInt aBufSize)
	{
	SetAndLimitMaxBufSize(aBufSize);
	ReAllocBufferL(iMaxBufSize);
	}

void RWsBuffer::SetMaxBufferSizeL(TInt aMaxBufSize)
	{
	SetAndLimitMaxBufSize(aMaxBufSize);

	if (iMaxBufSize < iBufSize)
		{ // shrink to new maximum
		ReAllocBufferL(iMaxBufSize);
		}
	else
		{
		 // initial allocation should be (at least) a quarter of the requested size
		TInt minSize = Max( (iMaxBufSize + 3) >> 2, EMinBufferSize);
		if (minSize > iBufSize)
			{ // new or enlarged buffer
			ReAllocBufferL(minSize);
			}
		}

	__ASSERT_DEBUG((iBufSize >= EMinBufferSize) && (iBufSize <= iMaxBufSize), Assert(EW32AssertBufferLogic));
	}

// Flush() buffer, try to ReAlloc, Leave if the ReAlloc fails.
void RWsBuffer::ReAllocBufferL(TInt aNewSize)
	{
	if (aNewSize != iBufSize)
		{
		Flush();
		if (!ReAllocBuffer(aNewSize))
			{
			User::LeaveNoMemory();
			}
		}
	}

TBool RWsBuffer::ReAllocBuffer(TInt aNewSize)
	{
	TUint8* ptr = const_cast<TUint8*>(iBuf.Ptr());
	__ASSERT_DEBUG((iBufSize == 0) || (ptr != NULL), Assert(EW32AssertBufferLogic));
	const TInt len = iBuf.Length();
	TUint8* newPtr = static_cast<TUint8*>(User::ReAlloc(ptr, aNewSize));
	if (newPtr != NULL)
		{ // realloc was successful
		iBuf.Set(newPtr, len, aNewSize);
		iBufSize = aNewSize;
		return ETrue;
		}
	return EFalse;
	}

/* Expand the buffer, to allow new drawing command to fit.
 
 Called either when trying to store additional commands to queue for Wserv, or
 the trying to send a command bigger than the current buffer size.

 Failure to expand the buffer is a minor problem in the first case but a big 
 problem in the second.

 @param aRequiredSpace Size of buffer increase required.
 @param aMsgSize If expanding the buffer fails then needs this value to know whether Flush() is good enough.
 @return ETrue if there is enough space, EFalse if not.
 */
void RWsBuffer::GrowBuffer(TInt aRequiredSpace, TInt aMsgSize)
	{
	__ASSERT_DEBUG(iBufSize < iMaxBufSize, Assert(EW32AssertBufferLogic));
	// maximum size will be big enough?
	__ASSERT_ALWAYS(aMsgSize <= iMaxBufSize, Panic(EW32PanicDataExceedsBufferLength));
	
	 // double or quad the current size, then limit it
	TInt newSize = Min((iBufSize >= aRequiredSpace) ? iBufSize << 1 : iBufSize << 2, iMaxBufSize);

	if (!ReAllocBuffer(newSize))
		{ // OOM error
		Flush();
		if (aMsgSize > iBufSize)
			{ // message is too big for buffer
			Panic(EW32PanicDataExceedsBufferLength);
			}
		}
	}


TBool RWsBuffer::SetAutoFlush(TBool aState)
	{
	TBool old;

	old=iAutoFlush;
#if defined(__AUTO_FLUSH)
	if (aState)
#else
	iAutoFlush=aState;
	if (iAutoFlush)
#endif
		Flush();
	return(old);
	}

TInt RWsBuffer::DoWrite(TInt aHandle, TUint aOpcode, TBool aFlush, const TIpcArgs* aIpcArgs, const TAny* aData, TInt aLength, const TAny* aData2, TInt aLength2)
	{
	__ASSERT_DEBUG(((TUint32) aOpcode) < 0x8000, Assert(EW32AssertIllegalOpcode));
	__ASSERT_DEBUG((aLength&0x3) == 0, Assert(EW32AssertOddLengthData));
	TInt xtra(0);
	if (aLength2>0)
		xtra = PadValue(aLength2);		// Round data upto a multiple of 4

	const TInt msgSize = aLength + aLength2 + xtra + static_cast<TInt>(sizeof(TWsCmdHeader));
	TInt available = iBuf.MaxLength() - iBuf.Length();
	if (msgSize > available)
		{
		if (iBufSize >= iMaxBufSize)
			{ // buffer is maximum size already
			Flush();
			}
		else
			{ // try to grow buffer
			 if ( (iBuf.Length() + msgSize) > iMaxBufSize)
				{ // growing alone will not make enough extra space
				Flush();
				available = iBufSize;
				}

			const TInt requiredSpace = msgSize - available;
			if (requiredSpace > 0)
				{
				GrowBuffer(requiredSpace, msgSize);
				}
			}
		}

	TWsCmdHeader cmdHeader;
	cmdHeader.iBase.iOpcode = (TInt16)aOpcode;
	cmdHeader.iBase.iCmdLength = (TInt16)(aLength + aLength2 + xtra);

	// For performance reasons we only pass in the handle if it is different
	// from the previous command
	if (aHandle == iPreviousHandle)
		{
		iBuf.Append((TUint8 *)&cmdHeader.iBase,sizeof(cmdHeader.iBase));
		}
	else
		{
		iPreviousHandle = aHandle;
		cmdHeader.iBase.iOpcode|=EWsOpcodeHandle;
		cmdHeader.iDestHandle = aHandle;
		iBuf.Append((TUint8 *)&cmdHeader,sizeof(cmdHeader));
		}

	if (aLength)
		{
		iBuf.Append((TUint8 *)aData, aLength);
		}
	if (aLength2>0 && aData2!=NULL)
		{
		iBuf.Append((TUint8 *)aData2, aLength2);
		iBuf.AppendFill(0,xtra);
		}
#if defined(_DEBUG)
	else if (aLength2>0 && aData2==NULL)
		{
		iAppendDataLength = aLength2;
		}
#endif
	if (aFlush)
		{
		return Flush(aIpcArgs);
		}
	return KErrNone;
	}

void RWsBuffer::Write(TInt handle,TUint opcode)
	{
	DoWrite(handle, opcode, iAutoFlush, NULL);
	}

/**
Writes data sent in aData of length aLength1 for the specifed aOpcode
into wserv buffer. It also takes an TIpcArgs by which you can send additional 
data. But one thing needs to be noted that if aIpcArgs has some content then 
this function flushes the wserv buffer.

@param aHandle aHandle of class derived from MWsClientClass
@param aOpcode Opcode for the current command
@param aData Data to be added to the buffer
@param aLength Length of the data to be added to buffer
@param aIpcArgs Additional data sent from client to server. It has default argument NULL.
				And if some data is sent in aIpcArgs, it flushes wserv buffer
*/
void RWsBuffer::Write(TInt aHandle, TUint aOpcode, const TAny *aData, TInt aLength, const TIpcArgs* aIpcArgs/*=NULL*/)
	{
	TBool flush = (aIpcArgs != NULL ? ETrue : iAutoFlush);	// If aIpcArgs contains data then we do explicit flush
	DoWrite(aHandle, aOpcode, flush, aIpcArgs, aData, aLength);
	}

/**
Writes data sent in aData and aData2 of lengths aLength1 and aLength2 
for the specifed aOpcode into wserv buffer. It also takes an TIpcArgs by which 
you can send additional data. But one thing needs to be noted that if aIpcArgs 
has some content then this function flushes the wserv buffer.

@param aHandle Handle of class derived from MWsClientClass
@param aOpcode Opcode for the current command
@param aData Data to be added to the buffer
@param aLength Length of the data to be added to buffer
@param aData2 second Data to be added to the buffer
@param aLength2 Length of the second data to be added to buffer
@param aIpcArgs Additional data sent from client to server. It has default argument NULL.
				And if some data is sent in aIpcArgs, it flushes wserv buffer
*/
void RWsBuffer::Write(TInt aHandle, TUint aOpcode, const TAny *aData, TInt aLength, const TAny *aData2, TInt aLength2, const TIpcArgs* aIpcArgs/*=NULL*/)
	{
	__ASSERT_DEBUG(!((aIpcArgs != NULL) &&  (aLength2 > 0 && aData2 == NULL)), Assert(EW32AssertBufferLogic));
	TBool flush = iAutoFlush;
	if (aLength2 > 0 && aData2 == NULL)
		{
		flush = EFalse;		// if just length2 is sent then we should not flush
		}
	else if (aIpcArgs != NULL)
		{
		flush = ETrue;		// If aIpcArgs contains data then we do explicit flush
		}
	DoWrite(aHandle, aOpcode, flush, aIpcArgs, aData, aLength, aData2, aLength2);
	}

/**
Appends data directly to wserv buffer for the current command. So this function 
should be used after adding the current command. 

@param aData Data to be added to the buffer
@param aLength Length of the data to be added to buffer. Make sure that its length
		is less than availabe buffer.
@param aFinished EFalse, adds data to buffer and disables flushing even if auto flush is on, 
				 basically this notfies that more data is pending to be added.
				 ETrue, adds data to buffer and resume normal service for flushing
				 ie. Signals that this is the last bit of data to be added

Note: When data is added using this API, it pads out buffer to multiple of 4 bytes  
*/
void RWsBuffer::AppendData(const TAny *aData,TInt aLength,TBool aFinished)
	{
	__ASSERT_ALWAYS(iBuf.MaxLength()-iBuf.Length()>=PaddedValue(aLength),Assert(EW32AssertBufferLogic));
#if defined(_DEBUG)
	// Check if this function is called only after setting iAppendDataLength
	__ASSERT_DEBUG(iAppendDataLength > 0, Assert(EW32AssertBufferLogic));
	// Check if length passed in is less then iAppendDataLength
	__ASSERT_DEBUG(iAppendDataLength >= aLength, Assert(EW32AssertBufferLogic));
	if (aFinished)
		iAppendDataLength = 0; 
	else
		iAppendDataLength -= aLength;
#endif
	iBuf.Append((TUint8*)(aData),aLength);
	iBuf.AppendFill(0,PadValue(iBuf.Length()));		// Padout out buffer to multiple of 4 bytes
	if (aFinished && iAutoFlush)
		Flush(NULL);
	}

TInt RWsBuffer::WriteReply(TInt handle,TUint opcode,const TIpcArgs* aIpcArgs)
	{
	return DoWrite(handle, opcode, ETrue, aIpcArgs);
	}

TInt RWsBuffer::WriteReply(TInt handle,TUint opcode,const TAny *pData, TInt length,const TIpcArgs* aIpcArgs)
	{
	return DoWrite(handle, opcode, ETrue, aIpcArgs, pData, length);
	}
	
TInt RWsBuffer::WriteReply(TInt handle,TUint opcode,const TAny *pData,TInt length,const TAny *pData2,TInt length2,const TIpcArgs* aIpcArgs)
	{
	return DoWrite(handle, opcode, ETrue, aIpcArgs, pData, length, pData2, length2);
	}

TInt RWsBuffer::WriteReplyP(TInt aHandle, TUint aOpcode, const TWriteDescriptorType& aReplyBuffer)
	{
	TIpcArgs ipcArgs;
	aReplyBuffer.SetDescriptorOnIpcArgs(ipcArgs);
	return DoWrite(aHandle, aOpcode, ETrue, &ipcArgs);
	}

TInt RWsBuffer::WriteReplyP(TInt aHandle,TUint aOpcode,const TAny *aData,TInt aLength,const TWriteDescriptorType& aReplyBuffer)
	{
	TIpcArgs ipcArgs;
	aReplyBuffer.SetDescriptorOnIpcArgs(ipcArgs);
	return DoWrite(aHandle, aOpcode, ETrue, &ipcArgs, aData, aLength);
	}

TInt RWsBuffer::WriteReplyP(TInt aHandle,TUint aOpcode,const TAny *aData1,TInt aLengthData1,const TAny *aData2,TInt aLengthData2,const TWriteDescriptorType& aReplyBuffer)
	{
	TIpcArgs ipcArgs;
	aReplyBuffer.SetDescriptorOnIpcArgs(ipcArgs);
	return DoWrite(aHandle, aOpcode, ETrue, &ipcArgs, aData1, aLengthData1, aData2, aLengthData2);
	}

TInt RWsBuffer::WriteReplyWs(TUint opcode)
//
// Do a WriteReply using the sessions handle
//
	{
	return(iSession->WriteReply(opcode));
	}

TInt RWsBuffer::WriteReplyWs(const TAny *pData, TInt aLength, TUint aOpcode)
//
// Do a WriteReply using the sessions handle
//
	{
	return(iSession->WriteReply(pData,aLength,aOpcode));
	}

TInt RWsBuffer::WriteReplyWs(const TAny *pData, TInt aLength, const TAny *pData2, TInt aLength2, TUint aOpcode)
//
// Do a WriteReply using the sessions handle
//
	{
	return(iSession->WriteReply(pData,aLength,pData2,aLength2,aOpcode));
	}

TInt RWsBuffer::WriteReplyByProvidingRemoteReadAccess(TInt aHandle,TUint aOpcode,const TAny *aData, TInt aLength,const TReadDescriptorType& aRemoteReadBuffer)
	{
	TIpcArgs ipcArgs;
	aRemoteReadBuffer.SetDescriptorOnIpcArgs(ipcArgs);
	return DoWrite(aHandle, aOpcode, ETrue, &ipcArgs, aData, aLength);
	}

void RWsBuffer::AddToBitmapArray(TInt aBitmapHandle)
	{
	if(aBitmapHandle && !iInvalidBitmapArray)
		{
		if(iBitmapArray.InsertInOrder(aBitmapHandle)==KErrNoMemory)

			iInvalidBitmapArray=ETrue;
		}
	}
	
void RWsBuffer::SetWsGraphicManager(CWsGraphic::CManager* aManager)
	{
	__ASSERT_DEBUG(!WsGraphicManager(),Panic(EW32PanicGraphicInternal));
	iManager = aManager;
	}

CWsGraphic::CManager* RWsBuffer::WsGraphicManager()
	{
	for(RWsBuffer *buffer=(RWsBuffer *)Dll::Tls();buffer;buffer=buffer->iNext)
		if (buffer->iManager)
			return buffer->iManager;	
	return NULL; // does not yet exist
	}

void RWsBuffer::AsyncRequest(TInt aHandle, TUint aOpcode, TRequestStatus& aStatus)
	{
	aStatus = KRequestPending;
	__ASSERT_DEBUG((aOpcode&EWservMessAsynchronousService)==0, Assert(EW32AssertIllegalOpcode));
	Flush(); 

	const TInt function = EWservMessAsynchronousService | aOpcode;
	TIpcArgs ipcArgs(aHandle);

	iSession->SendReceive(function, ipcArgs, aStatus);
	}

void RWsBuffer::EnableWindowSizeCacheL()
    {
    if (iWindowSizeCache == NULL)
        {
        iWindowSizeCache = new (ELeave) RHashMap<TInt, TWindowSizeCacheEntry>();        
        }
    }
