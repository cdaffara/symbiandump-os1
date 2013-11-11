// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__W32COMM_H__)
#define __W32COMM_H__


const TUint KBufferMessageSlot			=0;
const TUint KReplyBufferMessageSlot		=1;
const TUint KRemoteBufferMessageSlot	=2;

const TUint KAsyncMessageSlotAnimDllHandle	=0;
const TUint KAsyncMessageSlotAnimHandle		=1;

class TDescriptorType
	{
public:
	enum TBufferType
		{
		EDes8,
		EDes16,
		EDesC8,
		EDesC16,
		};
public:
	virtual void SetDescriptorOnIpcArgs(TIpcArgs& aIpcArgs) const=0;
protected:
	inline TDescriptorType(TInt aArgument, TBufferType aBufferType) : iArgument(aArgument), iBufferType(aBufferType) {}
protected:
	TInt iArgument;
	TBufferType iBufferType;
	};

class TReadDescriptorType : public TDescriptorType
	{
public:
	inline TReadDescriptorType(const TDesC8* aBuffer) : TDescriptorType((TInt)aBuffer, EDesC8)
		{
		}
	inline TReadDescriptorType(const TDesC16* aBuffer) : TDescriptorType((TInt)aBuffer, EDesC16)
		{
		}
public: // from TDescriptorType
	void SetDescriptorOnIpcArgs(TIpcArgs& aIpcArgs) const;
	};

class TWriteDescriptorType : public TDescriptorType
	{
public:
	inline TWriteDescriptorType(TDes8* aBuffer) : TDescriptorType((TInt)aBuffer, EDes8)
		{
		}
	inline TWriteDescriptorType(TDes16* aBuffer) : TDescriptorType((TInt)aBuffer, EDes16)
		{
		}
public: // from TDescriptorType
	void SetDescriptorOnIpcArgs(TIpcArgs& aIpcArgs) const;
	};



#endif
