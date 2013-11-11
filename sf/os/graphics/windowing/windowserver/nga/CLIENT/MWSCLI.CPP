// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Common client side class
// 
//

#include <e32std.h>
#include "../SERVER/w32cmd.h"
#include "CLIENT.H"


MWsClientClass::MWsClientClass() : iWsHandle(0), iBuffer(NULL)
	{
	}

MWsClientClass::MWsClientClass(RWsBuffer *aBuffer) : iWsHandle(0), iBuffer(aBuffer)
	{
	}

/**
Writes data sent in aData1 of length aLength1 for the specifed aOpcode
into wserv buffer. It also takes aLength2 paratmeter, which is the length of second data 
that will be added using AppendData. This Function checks if current command and aLength2 
can fit in the remaining buffer, if not then it does flush of previous command so that
the current command can be set along with data of aLength2. 

@param aData1 Data to be added to the buffer
@param aLength1 Length of the data to be added to buffer
@param aLength2 Length of second data that will(and must) be added using AppendData()
@param aOpcode Opcode for the current command
*/
void MWsClientClass::Write(const TAny *aData1,TInt aLength1,TInt aLength2,TUint aOpcode) const
	{
	Write(aData1,aLength1,NULL,aLength2,aOpcode);
	}

/**
Writes data sent in aData1 and aData2 of lengths aLength1 and aLength2 
for the specifed aOpcode into wserv buffer. It also takes an TIpcArgs by which 
you can send additional data. But one thing needs to be noted that if aIpcArgs 
has some content then this function flushes the wserv buffer 

@param aData1 Data to be added to the buffer
@param aLength1 Length of the data to be added to buffer
@param aData2 second Data to be added to the buffer
@param aLength2 second Length of second data then can be added using AppendData()
@param aOpcode Opcode for the current command
@param aIpcArgs Additional data sent from client to server. It has default argument NULL.
				And if some data is sent in aIpcArgs, it flushes wserv buffer.

Note:Only second, third and fourth slot of IPC agrs can be used to send data 
as first slot is used up for normal wserv buffer
*/
void MWsClientClass::Write(const TAny *aData1, TInt aLength1,const TAny *aData2, TInt aLength2,TUint aOpcode,const TIpcArgs* aIpcArgs/*=NULL*/) const
	{
	iBuffer->Write(iWsHandle,aOpcode,aData1,aLength1,aData2,aLength2,aIpcArgs);
	}

/**
Writes data sent in aData1 of length aLength1 for the specifed aOpcode 
into wserv buffer. It also takes an TIpcArgs by which you can send additional data. 
But one thing needs to be noted that if aIpcArgs has some content then this 
function flushes the wserv buffer 

@param aData1 Data to be added to the buffer
@param aLength1 Length of the data to be added to buffer
@param aOpcode Opcode for the current command
@param aIpcArgs Additional data sent from client to server. It has default argument NULL.
				And if some data is sent in aIpcArgs, it flushes wserv buffer

Note:Only second, third and fourth slot of IPC agrs can be used to send data 
as first slot is used up for normal wserv buffer				
*/
void MWsClientClass::Write(const TAny *aData, TInt aLength, TUint aOpcode, const TIpcArgs* aIpcArgs/*=NULL*/) const
	{
	iBuffer->Write(iWsHandle,aOpcode,aData,aLength,aIpcArgs);
	}

void MWsClientClass::Write(TUint aOpcode) const
	{
	iBuffer->Write(iWsHandle,aOpcode);
	}

void MWsClientClass::WriteInt(TInt aInt, TUint aOpcode) const
	{
	iBuffer->Write(iWsHandle,aOpcode,&aInt,sizeof(TInt));
	}

void MWsClientClass::WriteRect(const TRect &aRect, TUint aOpcode) const
	{
	Write(&aRect,sizeof(aRect),aOpcode);
	}

void MWsClientClass::WritePoint(const TPoint &aPoint, TUint aOpcode) const
	{
	Write(&aPoint,sizeof(aPoint),aOpcode);
	}

void MWsClientClass::WriteSize(const TSize &aSize, TUint aOpcode) const
	{
	Write(&aSize,sizeof(aSize),aOpcode);
	}

/**
Appends data directly to wserv buffer for the current command. So this function 
should be used after adding the current command.
Typically this function is used in conjunction with  
MWsClientClass::Write(const TAny *aData1,TInt aLength1,TInt aLength2,TUint aOpcode)
Please see its description for more details.  

@param aData Data to be added to the buffer
@param aLength Length of the data to be added to buffer. Make sure that its length
		is less than availabe buffer.
@param aFinished EFalse, adds data to buffer and disables flushing(notfies that more data is pending to be added)
				 ETrue, adds data to buffer and enables flushing(Signals that this is the last bit of data to be added)

Notes:
1. The total length of all data added by using this API(by one or more calls) must be less then(See note 3)or 
same as aLength2 passed in earlier call to MWsClientClass::Write(const TAny *aData1,TInt aLength1,TInt aLength2,TUint aOpcode)
2. When data is added using this API, it pads out buffer to multiple of 4 bytes so that 
all strings added to the buffer will begin on a 4 byte boundary.
3. If data added is less then the length passed to above Write API it works, but wastes buffer spaces. 
*/
void MWsClientClass::AppendData(const TAny *aData,TInt aLength,TBool aFinished)
	{
	iBuffer->AppendData(aData,aLength,aFinished);
	}

TInt MWsClientClass::WriteReply(TUint aOpcode,const TIpcArgs* aIpcArgs) const
	{
	return(iBuffer->WriteReply(iWsHandle,aOpcode,aIpcArgs));
	}

TInt MWsClientClass::WriteReplyInt(TInt aInt, TUint aOpcode,const TIpcArgs* aIpcArgs) const
	{
	return(iBuffer->WriteReply(iWsHandle,aOpcode,&aInt,sizeof(TInt),aIpcArgs));
	}

TInt MWsClientClass::WriteReply(const TAny *aData1, TInt aLength1,TUint aOpcode,const TIpcArgs* aIpcArgs) const
	{
	return(iBuffer->WriteReply(iWsHandle,aOpcode,aData1,aLength1,aIpcArgs));
	}

TInt MWsClientClass::WriteReply(const TAny *aData1, TInt aLength1,const TAny *aData2, TInt aLength2,TUint aOpcode,const TIpcArgs* aIpcArgs) const
	{
	return(iBuffer->WriteReply(iWsHandle,aOpcode,aData1,aLength1,aData2,aLength2,aIpcArgs));
	}

TInt MWsClientClass::WriteReplyP(const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const
	{
	return(iBuffer->WriteReplyP(iWsHandle,aOpcode,aReplyPackage));
	}

TInt MWsClientClass::WriteReplyIntP(TInt aInt, const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const
	{
	return(iBuffer->WriteReplyP(iWsHandle,aOpcode,&aInt,sizeof(aInt),aReplyPackage));
	}

TInt MWsClientClass::WriteReplyP(const TAny *aData1, TInt aLength1, const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const
	{
	return(iBuffer->WriteReplyP(iWsHandle,aOpcode,aData1,aLength1,aReplyPackage));
	}

TInt MWsClientClass::WriteReplyP(const TAny *aData1, TInt aLength1,const TAny *aData2, TInt aLength2, const TWriteDescriptorType& aReplyPackage,TUint aOpcode) const
	{
	return(iBuffer->WriteReplyP(iWsHandle,aOpcode,aData1,aLength1,aData2,aLength2,aReplyPackage));
	}

TInt MWsClientClass::WriteReplyByProvidingRemoteReadAccess(const TAny* aBuf, TInt aBufLen,const TReadDescriptorType& aRemoteReadBuffer, TUint aOpcode) const
	{
	return(iBuffer->WriteReplyByProvidingRemoteReadAccess(iWsHandle,aOpcode,aBuf,aBufLen,aRemoteReadBuffer));
	}

void MWsClientClass::AddToBitmapArray(const TInt aBitmapHandle)const
	{
	iBuffer->AddToBitmapArray(aBitmapHandle);
	}

void MWsClientClass::AsyncRequest(TRequestStatus& aStatus, TUint aOpcode) const
	{
	iBuffer->AsyncRequest(iWsHandle, aOpcode, aStatus);
	}

TBool MWsClientClass::WindowSizeCacheEnabled() const
    {
    return iBuffer->WindowSizeCacheEnabled();
    }

void MWsClientClass::MarkWindowSizeCacheDirty()
    {
    iBuffer->MarkWindowSizeCacheDirty(iWsHandle);
    }

void MWsClientClass::RefreshWindowSizeCache(const TSize& aNewSize) const
    {
    iBuffer->RefreshWindowSizeCache(iWsHandle,aNewSize);
    }

TInt MWsClientClass::CachedWindowSize(TSize& aSize) const
    {
    return iBuffer->CachedWindowSize(iWsHandle, aSize);
    }

void MWsClientClass::DestroyWindowSizeCacheEntry()
    {
    iBuffer->DestroyWindowSizeCacheEntry(iWsHandle);
    }
