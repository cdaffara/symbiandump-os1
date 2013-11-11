// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

void MWsClientClass::Write(const TAny *aData1, TInt aLength1,const TAny *aData2, TInt aLength2,TUint aOpcode,const TIpcArgs* /*aIpcArgs=NULL*/) const
	{
	iBuffer->Write(iWsHandle,aOpcode,aData1,aLength1,aData2,aLength2);
	}

void MWsClientClass::Write(const TAny *aData1, TInt aLength1,TUint aOpcode,const TIpcArgs* /*aIpcArgs=NULL*/) const
	{
	iBuffer->Write(iWsHandle,aOpcode,aData1,aLength1);
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

void MWsClientClass::AppendData(const TAny */*aData*/,TInt /*aLength*/,TBool /*aFinished*/)
	{
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

	
